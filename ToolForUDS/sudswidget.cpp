#include "sudswidget.h"
#include "tool/usbinterface.h"
#include "tool/ycanhandle.h"

SUdsWidget::SUdsWidget(SMainWindow *mainWindow, QWidget *parent)
    : SWidget(mainWindow, parent),
      m_serverTree(new UdsServerTree),
      m_canIDMapModel(new UdsCanIdMapModel),
      m_msgModel(new CanMessageModel),
      m_heartBeat(new QCheckBox(this)),
      m_server(new UdsServer(this))
{
    for(int i = 0; i < T_LENGTH; ++i){
        m_tables[i] = new QTableView();
    }
    for(int i = 0; i < E_LENGTH; ++i){
        m_lEdits[i] = new QLineEdit(this);
    }
    for(int i = 0; i < B_LENGTH; ++i){
        m_pButtons[i] = new QPushButton(this);
    }
    for(int i = 0; i < G_LENGTH; ++i){
        m_groupBoxs[i] = new QGroupBox(this);
    }
    for(int i = 0; i < L_LENGTH; ++i){
        m_labels[i] = new QLabel(this);
    }

    initWidget();

    connect(m_pButtons[B_SEND], &QPushButton::clicked,
            this, &SUdsWidget::slotSendClicked);
    connect(m_pButtons[B_ADD], &QPushButton::clicked,
            this, &SUdsWidget::slotAddClicked);

    connect(m_serverTree, &UdsServerTree::signServerChanged,
            this, &SUdsWidget::slotServerChanged);
    connect(m_server, &UdsServer::signCompleted,
            this, &SUdsWidget::slotServerFinished);

    connect(m_lEdits[E_FCBS], &QLineEdit::editingFinished,
            this, &SUdsWidget::slotChangedUdsParams);
    connect(m_lEdits[E_FCST], &QLineEdit::editingFinished,
            this, &SUdsWidget::slotChangedUdsParams);
    connect(m_lEdits[E_P2TIME], &QLineEdit::editingFinished,
            this, &SUdsWidget::slotChangedUdsParams);
    connect(m_lEdits[E_SDATA], &QLineEdit::textChanged,
            this, [&](){
        m_labels[L_ADATA_V]->setText(m_labels[L_SERPARAM_V]->text() + m_lEdits[E_SDATA]->text());
    });

    connect(m_heartBeat, &QCheckBox::stateChanged,
            this, [&](int state){
        if(state == Qt::Checked){
            m_time.start(3000);
        }else{
            m_time.stop();
        }
    });
    connect(&m_time, &QTimer::timeout,
            this, &SUdsWidget::slotTimeout);

    connect(this, &SUdsWidget::signPackAna, m_server, &UdsServer::slotPackAna);
    connect(m_server, &UdsServer::signalShowMsg, this, &SUdsWidget::slotShowMsg);
}

SUdsWidget::~SUdsWidget()
{
    //this->sobject()->setPropertyEx(SERVER_FILE, m_serverTree->lastFileName());
    QSettings settings;
    settings.setValue("/config/value/mapCanID", m_canIDMapModel->getModel());
    settings.setValue("/config/ui/lastCanID", m_lEdits[E_CANID]->text());
    settings.setValue("/config/ui/lastData", m_lEdits[E_SDATA]->text());
    settings.setValue("/config/ui/FCBS", m_lEdits[E_FCBS]->text());
    settings.setValue("/config/ui/FCST", m_lEdits[E_FCST]->text());
    settings.setValue("/config/ui/P2", m_lEdits[E_P2TIME]->text());
    if(m_thread.isRunning()){
        m_thread.stop();
        m_thread.wait();
        m_thread.quit();
    }
}

void SUdsWidget::initWidget()
{
    QMap<QString, QVariant> mapID;
    QSettings settings;
    mapID = settings.value("/config/value/mapCanID").toMap();
    m_lEdits[E_CANID]->setText(settings.value("/config/ui/lastCanID").toString());
    m_lEdits[E_SDATA]->setText(settings.value("/config/ui/lastData").toString());
    m_lEdits[E_FCBS]->setText(settings.value("/config/ui/FCBS", "0").toString());
    m_lEdits[E_FCST]->setText(settings.value("/config/ui/FCST", "0").toString());
    m_lEdits[E_P2TIME]->setText(settings.value("/config/ui/P2", "1000").toString());
    m_canIDMapModel->initData(mapID);
    slotChangedUdsParams();

    m_heartBeat->setText(QStringLiteral("MCU HeartBeat"));
    m_labels[L_FCBS]->setText(QStringLiteral("PC BS："));
    m_labels[L_FCST]->setText(QStringLiteral("PC ST："));
    m_labels[L_P2TIME]->setText(QStringLiteral("P2 Time："));
    m_labels[L_SERPARAM]->setText(QStringLiteral("服务参数："));
    m_labels[L_CANID]->setText(QStringLiteral("CAN ID："));
    m_labels[L_SDATA]->setText(QStringLiteral("发送数据："));
    m_labels[L_ADATA]->setText(QStringLiteral("合成数据："));
    m_labels[L_BDATA]->setText(QStringLiteral("回报数据："));
    m_pButtons[B_ADD]->setText(QStringLiteral("添加"));
    m_pButtons[B_SEND]->setText(QStringLiteral("发送"));

    m_tables[T_MAP]->setModel(m_canIDMapModel);
    m_tables[T_MAP]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tables[T_MAP]->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_tables[T_MAP]->setSelectionMode(QAbstractItemView::SingleSelection);

    m_tables[T_DATA]->setModel(m_msgModel);
    m_tables[T_DATA]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tables[T_DATA]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tables[T_DATA]->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_tables[T_DATA]->setSelectionMode(QAbstractItemView::SingleSelection);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout;

    QWidget* configWidget = new QWidget(this);
    QHBoxLayout* configLayout = new QHBoxLayout;

    configLayout->addWidget(m_heartBeat);
    configLayout->addItem(new QSpacerItem(100,20));
    configLayout->addWidget(m_labels[L_FCBS]);
    configLayout->addWidget(m_lEdits[E_FCBS]);
    configLayout->addWidget(m_labels[L_FCST]);
    configLayout->addWidget(m_lEdits[E_FCST]);
    configLayout->addWidget(m_labels[L_P2TIME]);
    configLayout->addWidget(m_lEdits[E_P2TIME]);
    configWidget->setLayout(configLayout);
    configWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    mainLayout->addWidget(configWidget);
    mainLayout->addWidget(m_tables[T_DATA]);
    mainWidget->setLayout(mainLayout);

    splitter->addWidget(mainWidget);

    QWidget* rightWidget = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout;

    QGroupBox* groupBox = m_groupBoxs[G_MAPTABLE];
    groupBox->setLayout(new QVBoxLayout);
    groupBox->setTitle(QStringLiteral("Server ID Mapping Table"));
    groupBox->layout()->addWidget(m_tables[T_MAP]);
    //QHBoxLayout* layout = new QHBoxLayout(groupBox);
    //layout->addSpacing(40);
    //layout->addWidget(m_pButtons[B_ADD]);
    groupBox->layout()->addWidget(m_pButtons[B_ADD]);
    rightLayout->addWidget(groupBox);

    groupBox = m_groupBoxs[G_SERTREE];
    groupBox->setLayout(new QVBoxLayout);
    groupBox->setTitle(QStringLiteral("UDS Server Tree"));
    groupBox->layout()->addWidget(m_serverTree);
    rightLayout->addWidget(groupBox);

    groupBox = m_groupBoxs[G_SENDCONF];
    QGridLayout* gridLayout1 = new QGridLayout;
    groupBox->setLayout(gridLayout1);
    groupBox->setTitle(QStringLiteral("UDS Send Config"));
    gridLayout1->addWidget(m_labels[L_SERPARAM], 0, 0);
    gridLayout1->addWidget(m_labels[L_SERPARAM_V], 0, 1);
    gridLayout1->addWidget(m_labels[L_CANID], 1, 0);
    gridLayout1->addWidget(m_lEdits[E_CANID], 1, 1);
    gridLayout1->addWidget(m_labels[L_SDATA], 2, 0);
    gridLayout1->addWidget(m_lEdits[E_SDATA], 2, 1);
    gridLayout1->addWidget(m_labels[L_ADATA], 3, 0);
    gridLayout1->addWidget(m_labels[L_ADATA_V], 3, 1);
    rightLayout->addWidget(groupBox);

    groupBox = m_groupBoxs[G_BACK];
    QGridLayout* gridLayout2 = new QGridLayout;
    groupBox->setLayout(gridLayout2);
    groupBox->setTitle(QStringLiteral("UDS Back Message"));
    gridLayout2->addWidget(m_labels[L_BDATA], 0, 0);
    gridLayout2->addWidget(m_labels[L_BDATA_V], 0, 1);
    rightLayout->addWidget(groupBox);

    rightLayout->addWidget(m_pButtons[B_SEND]);
    rightWidget->setLayout(rightLayout);

    splitter->addWidget(rightWidget);

    this->setLayout(new QVBoxLayout);
    this->layout()->addWidget(splitter);
}

void SUdsWidget::setSObject(SObject *obj)
{
    SWidget::setSObject(obj);

    //m_serverTree->openFile(obj->property(SERVER_FILE).toString());
    m_canIDMapModel->initData(obj->property(MAP_CAN_ID).toMap());
    m_lEdits[E_CANID]->setText(obj->property(LAST_CAN_ID).toString());
    m_lEdits[E_SDATA]->setText(obj->property(LAST_DATA).toString());
}

void SUdsWidget::propertyOfSObjectChanged(SObject *obj, const QString &strPropName, const SObject::PropertyT &propChangedBy)
{
    Q_UNUSED(propChangedBy)
    QObject::blockSignals(true);
    if(strPropName == STR_DATASOURCE){
        auto& mapMapping = mapping();
        if(mapMapping.contains(STR_DATASOURCE)
                && mapMapping[STR_DATASOURCE][STR_TYPE].toString() == STR_PROP){
            m_thread.setUserFunction(controlThread);
            m_thread.setUserParam(this);
            m_thread.start();
        }
    }
    QObject::blockSignals(false);
}

QString SUdsWidget::keyString()
{
    return SWidget::key(UDS_WIDGET);
}

void SUdsWidget::initSObject(SObject *obj)
{
    obj->setSObjectName(UDS_WIDGET);
    //obj->setProperty(SERVER_FILE, STR_DEFAULT);
    obj->setProperty(MAP_CAN_ID, STR_DEFAULT);
    obj->setProperty(LAST_CAN_ID, STR_DEFAULT);
    obj->setProperty(LAST_DATA, STR_DEFAULT);

    addSpecialProperty(obj, STR_DATASOURCE, "buff.receive_buff", STR_ROLE_MAPPING);
    addSpecialProperty(obj, STR_DATAOUT, "buff.send_buff_0", STR_ROLE_MAPPING);
}

Q_DECLARE_METATYPE(CAN_MESSAGE_PACKAGE)
int SUdsWidget::controlThread(void *pParam, const bool &bRunning)
{
    SUdsWidget* pWidget = (SUdsWidget*)pParam;
    QVariantMap ParamInfo;
    SObject* signalObj = nullptr;
    uint dataSourceVer = 0;
    QVariant data;
    uint mVersionCMD = 0;
    if(pWidget){
        while(bRunning){
            if(pWidget->isMapped(STR_DATASOURCE)){
                ParamInfo = pWidget->mapping()[STR_DATASOURCE];
                signalObj =(SObject*)ParamInfo[STR_VALUE].value<void*>();
                if(signalObj != nullptr){
                    QByteArray signalProp = ParamInfo[STR_PROP].toString().toUtf8();
                    if(!signalProp.isEmpty()){
                        QString propName = QString(signalProp);
                        uint uVersion = signalObj->propertyInfo()[propName].m_version;
                        if(uVersion != dataSourceVer){
                            if(signalObj->lock().tryLockForRead()){
                                auto propLst = signalObj->dynamicPropertyNames();
                                data = signalObj->property(propName.toLatin1().data());
                                signalObj->lock().unlock();
                            }
                            if(data.isValid() && !data.isNull()){
                                CAN_MESSAGE_PACKAGE canObj = data.value<CAN_MESSAGE_PACKAGE>();
                                dataSourceVer = uVersion;
                                canObj.type = QString("RXD");
                                pWidget->m_msgModel->insertData(canObj);
                                emit pWidget->signPackAna(canObj);
                            }
                        }
                    }
                }
            }

            if(pWidget->isMapped(STR_DATAOUT)){
                ParamInfo = pWidget->mapping()[STR_DATAOUT];
                signalObj =(SObject*)ParamInfo[STR_VALUE].value<void*>();
                if(signalObj != nullptr){
                    QByteArray signalProp = ParamInfo[STR_PROP].toString().toUtf8();
                    if(!signalProp.isEmpty()){
                        if(signalObj->propertyInfo().contains(signalProp)){
                            uint tempVersionCMD = signalObj->propertyInfo()[signalProp].m_version;
                            if(mVersionCMD != tempVersionCMD){
                                QVariant value = signalObj->property(signalProp.data());
                                if(value.isValid() && !value.isNull()){
                                    CAN_MESSAGE_PACKAGE canObj;
                                    canObj.canObj = value;
                                    canObj.type = QString("TXD");
                                    pWidget->m_msgModel->insertData(canObj);
                                }
                                mVersionCMD = tempVersionCMD;
                            }
                        }
                    }
                }
            }
            QThread::msleep(1);
        }
    }
    return 0;
}

Q_DECLARE_METATYPE(CAN_OBJ)
void SUdsWidget::slotSendClicked()
{
    QString strCanID = m_lEdits[E_CANID]->text();
    if(strCanID.isEmpty()){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Please input the CanID!"));
        return;
    }
    if(!m_canIDMapModel->getModel().contains(strCanID)){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Please set the CanID Mapping!"));
        return;
    }

    if(!isMapped(STR_DATAOUT)){
        QMessageBox::warning(this, tr("Warning"),
                             tr("UDS Widget Write EmptyMap!"));
        return;
    }
    auto mapDataOut = mapping()[STR_DATAOUT];
    SObject* ObjDataOut =(SObject*)mapDataOut[STR_VALUE].value<void*>();
    if(!ObjDataOut){
        QMessageBox::warning(this, tr("Warning"),
                             tr("UDS Widget Write Out NULL!"));
        return;
    }
    QByteArray PropDataOut =mapDataOut[STR_PROP].toString().toUtf8();
    if(PropDataOut.isEmpty()){
        QMessageBox::warning(this, tr("Warning"),
                             tr("UDS Widget Write Out property Empty!"));
        return;
    }

    this->sobject()->setPropertyEx(MAP_CAN_ID, m_canIDMapModel->getModel());
    this->sobject()->setPropertyEx(LAST_CAN_ID, m_lEdits[E_CANID]->text());
    this->sobject()->setPropertyEx(LAST_DATA, m_lEdits[E_SDATA]->text());

    m_server->setCanID(strCanID, m_canIDMapModel->getModel()[strCanID].toString());
    m_server->setContent(m_labels[L_ADATA_V]->text());
    m_server->process(ObjDataOut, PropDataOut);

    m_pButtons[B_SEND]->setEnabled(false);
}

void SUdsWidget::slotAddClicked()
{
    m_canIDMapModel->insertNewData();
}

void SUdsWidget::slotServerChanged(const QString &server)
{
    m_labels[L_SERPARAM_V]->setText(server);
    m_labels[L_ADATA_V]->setText(server + m_lEdits[E_SDATA]->text());
}

void SUdsWidget::slotServerFinished()
{
    if(m_server->getStatus() == UdsServer::N_OK){
        m_labels[L_BDATA_V]->setText(m_server->getBackPack());
        //QMessageBox::information(this, tr("information"), tr("UDS send successfully!"));
    }else{
        QMessageBox::information(this, tr("information"), tr("UDS send failed!"));
    }
    m_pButtons[B_SEND]->setEnabled(true);
}

void SUdsWidget::slotChangedUdsParams()
{
    QString strP2 = m_lEdits[E_P2TIME]->text();
    QString strFCBS = m_lEdits[E_FCBS]->text();
    QString strFCST = m_lEdits[E_FCST]->text();
    if(strP2.isEmpty() ||
            strFCBS.isEmpty() ||
            strFCST.isEmpty()){
        QMessageBox::warning(this, tr("Warning"), tr("UDS config can not be empty!"));
        return;
    }
    m_server->setTimeout(strP2.toUInt());
    m_server->setRecvParams(strFCBS.toUInt(), strFCST.toUInt());
}

void SUdsWidget::slotTimeout()
{
    CAN_OBJ obj;
    obj.ID = 0x782;
    obj.DataLen = 8;
    obj.Data[0] = 0x02;
    obj.Data[1] = 0x3e;
    obj.Data[2] = 0x00;

    if(!isMapped(STR_DATAOUT)){
        QMessageBox::warning(this, tr("Warning"),
                             tr("UDS Widget Write EmptyMap!"));
        return;
    }
    auto mapDataOut = mapping()[STR_DATAOUT];
    SObject* ObjDataOut =(SObject*)mapDataOut[STR_VALUE].value<void*>();
    if(!ObjDataOut){
        QMessageBox::warning(this, tr("Warning"),
                             tr("UDS Widget Write Out NULL!"));
        return;
    }
    QByteArray PropDataOut =mapDataOut[STR_PROP].toString().toUtf8();
    if(PropDataOut.isEmpty()){
        QMessageBox::warning(this, tr("Warning"),
                             tr("UDS Widget Write Out property Empty!"));
        return;
    }

    ObjDataOut->setPropertyS(PropDataOut, QVariant::fromValue(obj));
}

//Q_DECLARE_METATYPE(CAN_MESSAGE_PACKAGE)
void SUdsWidget::slotShowMsg(const CAN_MESSAGE_PACKAGE &pack)
{
    CAN_MESSAGE_PACKAGE obj = pack;
    obj.type = QString("RXD");
    m_msgModel->insertData(obj);
}
