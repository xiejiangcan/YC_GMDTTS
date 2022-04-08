#include "susbcanwidget.h"

#include "tool/usbinterface.h"
#include "tool/ycanhandle.h"

static QStringList _baudList = QStringList() << "5Kbps"
                                             << "10kbps"
                                             << "20kbps"
                                             << "40kbps"
                                             << "50kbps"
                                             << "80kbps"
                                             << "100kbps"
                                             << "125kbps"
                                             << "200kbps"
                                             << "250kbps"
                                             << "400kbps"
                                             << "500kbps"
                                             << "666kbps"
                                             << "800kbps"
                                             << "1000kbps";

static const GUID GUID_DEVINTERFACE_LIST[] =
{
    { 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
    //{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
    //{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
    //{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } },
    //{ 0xaa87d8ea, 0xe78f, 0x466d, { 0x90, 0x40, 0x02, 0x25, 0x4a, 0x61, 0xe3, 0x09 } }
};

SUsbCanWidget::SUsbCanWidget(SMainWindow *mainWindow, QWidget *parent)
    : SWidget(mainWindow, parent),
      m_switchBtn(new QPushButton(QStringLiteral("连接"), this)),
      m_handle(YCanHandle::getInstance())
{
    for(int i = 0; i < G_LENGTH; ++i){
        m_groupBox[i] = new QGroupBox(this);
    }
    for(int i = 0; i < C_LENGTH; ++i){
        m_comboBox[i] = new QComboBox(this);
    }
    for(int i = 0; i < B_LENGTH; ++i){
        m_radioBtns[i] = new QRadioButton(this);
    }

    initWidget();
    registerDevice();

    connect(m_switchBtn, &QPushButton::clicked, this, &SUsbCanWidget::slotBtnClicked);
    connect(m_handle, &YCanHandle::signCanMessage,
            this, &SUsbCanWidget::slotCanMessage);
}

SUsbCanWidget::~SUsbCanWidget()
{
    if(m_thread.isRunning()){
        m_thread.stop();
        m_thread.wait();
        m_thread.quit();
    }
}

void SUsbCanWidget::initWidget()
{
    m_deviceList = canDeviceList();

    m_radioBtns[B_TYPE1]->setText(QStringLiteral("USBCAN I"));
    m_radioBtns[B_TYPE2]->setText(QStringLiteral("USBCAN II"));
    m_radioBtns[B_CHA1]->setText(QStringLiteral("Channel1"));
    m_radioBtns[B_CHA2]->setText(QStringLiteral("Channel2"));
    m_comboBox[C_BAUD]->addItems(_baudList);
    m_comboBox[C_DEVLST]->addItems(m_deviceList);

    this->setLayout(new QVBoxLayout);

    QGroupBox* group = nullptr;

    group = m_groupBox[G_DEVICES];
    group->setTitle(tr("DEVICE ID"));
    group->setLayout(new QVBoxLayout);
    group->layout()->addWidget(m_comboBox[C_DEVLST]);
    this->layout()->addWidget(group);

    group = m_groupBox[G_DEVTYPE];
    group->setTitle(tr("Device Type"));
    group->setLayout(new QVBoxLayout);
    group->layout()->addWidget(m_radioBtns[B_TYPE1]);
    group->layout()->addWidget(m_radioBtns[B_TYPE2]);
    this->layout()->addWidget(group);

    group = m_groupBox[G_CHANNEL];
    group->setTitle(tr("CAN Channel"));
    group->setLayout(new QVBoxLayout);
    group->layout()->addWidget(m_radioBtns[B_CHA1]);
    group->layout()->addWidget(m_radioBtns[B_CHA2]);
    this->layout()->addWidget(group);

    group = m_groupBox[G_BAUD];
    group->setTitle(tr("CAN BAUD"));
    group->setLayout(new QVBoxLayout);
    group->layout()->addWidget(m_comboBox[C_BAUD]);
    this->layout()->addWidget(group);

    this->layout()->addWidget(m_switchBtn);
}

void SUsbCanWidget::registerDevice()
{
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
    {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];

        hDevNotify = RegisterDeviceNotification((HANDLE)this->winId(),
                                                &NotifacationFiler,
                                                DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!hDevNotify)
        {
            qDebug() << QStringLiteral("注册失败");
        }
    }
    return;
}

bool SUsbCanWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    MSG *msg = reinterpret_cast<MSG *>(message);
    int msgType = msg->message;

    if(WM_DEVICECHANGE == msgType){
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        switch (msg->wParam) {
        case DBT_DEVICEARRIVAL:
        {
            if(DBT_DEVTYP_DEVICEINTERFACE == lpdb->dbch_devicetype){
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  =
                        (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
                QString name = QString::fromWCharArray(pDevInf->dbcc_name);
                if(name.contains("VID_0C66") &&
                        name.contains("PID_000C")){
                    m_deviceList = canDeviceList();
                    m_handle->setDeviceSize(m_deviceList.size());
                    m_comboBox[C_DEVLST]->clear();
                    m_comboBox[C_DEVLST]->addItems(m_deviceList);
                }
            }
        }
            break;
        case DBT_DEVICEREMOVECOMPLETE:
        {
            if (DBT_DEVTYP_DEVICEINTERFACE == lpdb->dbch_devicetype) {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  =
                        (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
                QString name = QString::fromWCharArray(pDevInf->dbcc_name);
                if(name.contains("VID_0C66") &&
                        name.contains("PID_000C")){
                    m_deviceList = canDeviceList();
                    m_handle->setDeviceSize(m_deviceList.size());
                    m_comboBox[C_DEVLST]->clear();
                    m_comboBox[C_DEVLST]->addItems(m_deviceList);
                }
            }
        }
            break;
        }
    }
    return false;
}

Q_DECLARE_METATYPE(CAN_OBJ)
int SUsbCanWidget::controlThread(void *pParam, const bool &bRunning)
{
    SUsbCanWidget* pWidget = (SUsbCanWidget*)pParam;
    QVector<uint> mVersionCMD;
    CAN_OBJ sendObj;
    QString nameCMD;
    if(pWidget){
        QByteArray DataDAQ;
        while(bRunning){
            SObject* pObjBuff = pWidget->sobject()->findChild<SObject*>(MSG_BUFF);
            if(pObjBuff){
                int nSumBuff = pObjBuff->property(SEND_BUFF_NUM).toInt();
                while(mVersionCMD.size()!= nSumBuff){
                    if(mVersionCMD.size()> nSumBuff)
                        mVersionCMD.removeLast();
                    else
                        mVersionCMD.append(0);
                }
                for(int i = 0; i < nSumBuff; ++i){
                    nameCMD = SEND_BUFF_ + QString::number(i);
                    if(pObjBuff->propertyInfo().contains(nameCMD)){
                        uint tempVersionCMD = pObjBuff->propertyInfo()[nameCMD].m_version;
                        if(mVersionCMD[i] != tempVersionCMD){
                            QVariant value = pObjBuff->property(nameCMD.toLatin1().data());
                            if(value.isValid() && !value.isNull()){
                                int devIndex = pWidget->m_comboBox[C_DEVLST]->currentIndex();
                                int chanIndex = pWidget->m_radioBtns[B_CHA1]->isChecked() ? 0 : 1;
                                sendObj = value.value<CAN_OBJ>();
                                pWidget->m_handle->SendDataToChan(sendObj, devIndex, chanIndex);
                            }
                            mVersionCMD[i] = tempVersionCMD;
                        }
                    }
                }
            }
            QThread::msleep(1);
        }
    }
    return 0;
}

Q_DECLARE_METATYPE(CAN_MESSAGE_PACKAGE)
void SUsbCanWidget::slotCanMessage(const CAN_MESSAGE_PACKAGE &buf)
{
    SObject* pBuff = this->sobject()->findChild<SObject*>(MSG_BUFF);
    if(pBuff){
        emit pBuff->signalNotifed(SIGNAL_CAN_MESSAGE, QVariant::fromValue(buf));
    }
}

void SUsbCanWidget::slotBtnClicked()
{
    if(m_isOpen){
        m_isOpen = false;
        m_handle->CloseAll();
        m_switchBtn->setText(QStringLiteral("连接"));
    }else{
        m_isOpen = true;
        m_handle->setDeviceType(m_radioBtns[B_TYPE1]->isChecked() ? USBCAN1 : USBCAN2);
        m_handle->setBaudType((BaudType)m_comboBox[C_BAUD]->currentIndex());
        if(!m_handle->OpenAll())
            return;
        m_switchBtn->setText(QStringLiteral("断开"));
    }
    m_groupBox[G_BAUD]->setEnabled(!m_isOpen);
    m_groupBox[G_CHANNEL]->setEnabled(!m_isOpen);
    m_groupBox[G_DEVTYPE]->setEnabled(!m_isOpen);

    this->setProperty(CAN_DEVICE_TYPE, m_radioBtns[B_TYPE1]->isChecked() ? 1 : 2);
    this->setProperty(CAN_CHANNEL, m_radioBtns[B_CHA1]->isChecked() ? 1 : 2);
    this->setProperty(CAN_BAUD, m_comboBox[C_BAUD]->currentIndex());
    sobject()->setPropertyEx(CAN_OPENSTATE, m_isOpen);
}

void SUsbCanWidget::setSObject(SObject *obj)
{
    SWidget::setSObject(obj);

    if(obj->property(CAN_DEVICE_TYPE).toInt() == 2){
        m_radioBtns[B_TYPE1]->setChecked(false);
        m_radioBtns[B_TYPE2]->setChecked(true);
    }else{
        m_radioBtns[B_TYPE1]->setChecked(true);
        m_radioBtns[B_TYPE2]->setChecked(false);
    }
    if(obj->property(CAN_CHANNEL).toInt() == 2){
        m_radioBtns[B_CHA1]->setChecked(false);
        m_radioBtns[B_CHA2]->setChecked(true);
    }else{
        m_radioBtns[B_CHA1]->setChecked(true);
        m_radioBtns[B_CHA2]->setChecked(false);
    }

    uint index = obj->property(CAN_BAUD).toUInt();
    if(index < m_comboBox[C_BAUD]->count())
        m_comboBox[C_BAUD]->setCurrentIndex(index);

    m_handle->setDeviceSize(m_deviceList.size());

    m_thread.setUserFunction(controlThread);
    m_thread.setUserParam(this);
    m_thread.start();
}

void SUsbCanWidget::propertyOfSObjectChanged(SObject *obj,
                                             const QString &strPropName,
                                             const SObject::PropertyT &propChangedBy)
{
    Q_UNUSED(propChangedBy)
    QVariant varValue = obj->property(strPropName.toUtf8().data());
    QObject::blockSignals(true);
    // TODO
    QObject::blockSignals(false);
}

QString SUsbCanWidget::keyString()
{
    return SWidget::key(USB_CAN);
}

void SUsbCanWidget::initSObject(SObject *obj)
{
    obj->setObjectName(USB_CAN);
    obj->setProperty(STR_ID, "can");
    obj->setProperty(CAN_DEVICE_TYPE, 2);
    obj->setProperty(CAN_CHANNEL, 2);
    obj->setProperty(CAN_BAUD, 11);
    obj->setProperty(CAN_OPENSTATE, false);

    SObject* pChildBuff = new SObject(obj);
    pChildBuff->setSObjectName(MSG_BUFF);
    pChildBuff->setProperty(STR_ID, "buff");
    pChildBuff->setProperty(SEND_BUFF_NUM, 8);
}
