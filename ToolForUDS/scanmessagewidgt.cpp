#include "scanmessagewidgt.h"
#include "tool/usbinterface.h"
#include "tool/ycanhandle.h"

SCanMessageWidgt::SCanMessageWidgt(SMainWindow *mainWindow, QWidget *parent)
    : SWidget(mainWindow, parent),
      m_table(new QTableView(this)),
      m_model(new CanMessageModel(this)),
      m_filterBtn(new QPushButton(tr("Filter"), this)),
      m_topWidget(new QWidget(this)),
      m_proxyModel(new CanMessageProxyModel(this))
{
    m_checkBox[C_HEX] = new QCheckBox("isHex", this);
    m_checkBox[C_AUTO] = new QCheckBox("AutoScrollBottom", this);
    for(int i = 0; i < L_LENGTH; ++i){
        m_lineEdit[i] = new QLineEdit(this);
    }
    for(int i = 0; i < G_LENGTH; ++i){
        m_groupBox[i] = new QGroupBox(this);
    }

    initWidget();

    connect(this, &SCanMessageWidgt::signUpdateTable, this, [&](){
        m_table->scrollToBottom();
    }, Qt::QueuedConnection);

    connect(m_filterBtn, &QPushButton::clicked,
            this, &SCanMessageWidgt::slotFilterTable);

    connect(m_checkBox[C_HEX], &QCheckBox::stateChanged,
            this, &SCanMessageWidgt::slotIsHexStateChanged);
}

SCanMessageWidgt::~SCanMessageWidgt()
{
    if(m_thread.isRunning()){
        m_thread.stop();
        m_thread.wait();
        m_thread.quit();
    }
}

void SCanMessageWidgt::initWidget()
{
    this->setLayout(new QVBoxLayout);

    m_topWidget->setLayout(new QHBoxLayout);

    m_groupBox[G_TIME]->setLayout(new QHBoxLayout);
    m_groupBox[G_TIME]->setTitle(tr("Time filter"));
    m_groupBox[G_TIME]->layout()->addWidget(m_lineEdit[L_TIMEL]);
    m_groupBox[G_TIME]->layout()->addWidget(new QLabel(" - "));
    m_groupBox[G_TIME]->layout()->addWidget(m_lineEdit[L_TIMEH]);

    m_groupBox[G_CANID]->setLayout(new QHBoxLayout);
    m_groupBox[G_CANID]->setTitle(tr("CAN ID filter"));
    m_groupBox[G_CANID]->layout()->addWidget(m_lineEdit[L_CANIDL]);
    m_groupBox[G_CANID]->layout()->addWidget(new QLabel(" - "));
    m_groupBox[G_CANID]->layout()->addWidget(m_lineEdit[L_CANIDH]);

    m_topWidget->layout()->addWidget(m_checkBox[C_HEX]);
    m_topWidget->layout()->addWidget(m_checkBox[C_AUTO]);
    m_topWidget->layout()->addWidget(m_groupBox[G_TIME]);
    m_topWidget->layout()->addWidget(m_groupBox[G_CANID]);
    m_topWidget->layout()->addWidget(m_filterBtn);

    this->layout()->addWidget(m_topWidget);
    this->layout()->addWidget(m_table);

    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_proxyModel->setSourceModel(m_model);
    m_table->setModel(m_proxyModel);
}

void SCanMessageWidgt::slotFilterTable()
{
    QString time_l = m_lineEdit[L_TIMEL]->text();
    QString time_h = m_lineEdit[L_TIMEH]->text();
    QString canID_l = m_lineEdit[L_CANIDL]->text();
    QString canID_h = m_lineEdit[L_CANIDH]->text();

    m_proxyModel->setTimeRange(time_l, time_h);
    m_proxyModel->setCanIDRange(canID_l, canID_h);

    m_model->updateModel();
}

void SCanMessageWidgt::slotIsHexStateChanged(int state)
{
    m_model->setIsHex(state == Qt::Checked);
    m_proxyModel->setIsHex(state == Qt::Checked);
    m_model->updateModel();
}

void SCanMessageWidgt::setSObject(SObject *obj)
{
    SWidget::setSObject(obj);

    m_checkBox[C_HEX]->setChecked(obj->property(IS_HEX).toBool());
    m_model->setIsHex(obj->property(IS_HEX).toBool());
    m_proxyModel->setIsHex(obj->property(IS_HEX).toBool());
    m_checkBox[C_AUTO]->setChecked(obj->property(IS_AUTO).toBool());
}

void SCanMessageWidgt::propertyOfSObjectChanged(SObject *obj, const QString &strPropName,
                                                const SObject::PropertyT &propChangedBy)
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

QString SCanMessageWidgt::keyString()
{
    return SWidget::key(CAN_MESSAGE);
}

void SCanMessageWidgt::initSObject(SObject *obj)
{
    obj->setObjectName(CAN_MESSAGE);
    obj->setProperty(IS_AUTO, true);
    obj->setProperty(IS_HEX, true);
    addSpecialProperty(obj, STR_DATASOURCE, "buff.receive_buff", STR_ROLE_MAPPING);
}

Q_DECLARE_METATYPE(CAN_MESSAGE_PACKAGE)
int SCanMessageWidgt::controlThread(void *pParam, const bool &bRunning)
{
    SCanMessageWidgt* pWidget = (SCanMessageWidgt*)pParam;
    if(pWidget){
        QVariantMap ParamInfo;
        SObject* signalObj = nullptr;
        uint dataSourceVer = 0;
        QVariant data;
        while(bRunning){
            if(!pWidget->isMapped(STR_DATASOURCE)){
                break;
            }
            ParamInfo = pWidget->mapping()[STR_DATASOURCE];
            signalObj =(SObject*)ParamInfo[STR_VALUE].value<void*>();
            if(signalObj == nullptr){
                break;
            }
            QByteArray signalProp = ParamInfo[STR_PROP].toString().toUtf8();
            if(signalProp.isEmpty())
                break;
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
                    pWidget->m_model->insertData(canObj);
                }
                if(pWidget->m_checkBox[C_AUTO]->isChecked()){
                    emit pWidget->signUpdateTable();
                }
            }

            QThread::msleep(1);
        }
    }
    return 0;
}

