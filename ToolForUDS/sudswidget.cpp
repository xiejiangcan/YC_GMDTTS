#include "sudswidget.h"
#include "tool/usbinterface.h"

SUdsWidget::SUdsWidget(SMainWindow *mainWindow, QWidget *parent)
    : SWidget(mainWindow, parent),
      m_serverTree(new UdsServerTree),
      m_canIDMapModel(new UdsCanIdMapModel),
      m_heartBeat(new QCheckBox(this)),
      m_server(new UdsServer(this))
{
    for(int i = 0; i < T_LENGTH; ++i){
        m_tables[i] = new QTableView(this);
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
}

SUdsWidget::~SUdsWidget()
{
    //this->sobject()->setPropertyEx(SERVER_FILE, m_serverTree->lastFileName());
    if(m_thread.isRunning()){
        m_thread.stop();
        m_thread.wait();
        m_thread.quit();
    }
}

void SUdsWidget::initWidget()
{
    m_heartBeat->setText(QStringLiteral("MCU HeartBeat"));
    m_labels[L_PCBS]->setText(QStringLiteral("PC BS："));
    m_labels[L_PCST]->setText(QStringLiteral("PC ST："));
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

    this->setLayout(new QHBoxLayout);
    this->layout()->addWidget(m_tables[T_DATA]);

    QVBoxLayout* configLayout = new QVBoxLayout;

    QGroupBox* groupBox = m_groupBoxs[G_CONFIG];
    QGridLayout* gridLayout0 = new QGridLayout;
    groupBox->setLayout(gridLayout0);
    groupBox->setTitle(QStringLiteral("UDS Config"));
    gridLayout0->addWidget(m_heartBeat, 0, 0, 0, -1);
    gridLayout0->addWidget(m_labels[L_PCBS], 1, 0);
    gridLayout0->addWidget(m_labels[L_PCST], 2, 0);
    gridLayout0->addWidget(m_labels[L_P2TIME], 3, 0);
    gridLayout0->addWidget(m_lEdits[E_PCBS], 1, 1);
    gridLayout0->addWidget(m_lEdits[E_PCST], 2, 1);
    gridLayout0->addWidget(m_lEdits[E_P2TIME], 3, 1);
    configLayout->addWidget(groupBox);

    groupBox = m_groupBoxs[G_HANDLST];
    groupBox->setLayout(new QVBoxLayout);
    groupBox->setTitle(QStringLiteral("Device list"));
    configLayout->addWidget(groupBox);

    this->layout()->addItem(configLayout);

    QVBoxLayout* rightLayout = new QVBoxLayout;

    groupBox = m_groupBoxs[G_MAPTABLE];
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

    this->layout()->addItem(rightLayout);
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

    addSpecialProperty(obj, STR_DATASOURCE, STR_DEFAULT, STR_ROLE_MAPPING);
    addSpecialProperty(obj, STR_DATAOUT, STR_DEFAULT, STR_ROLE_MAPPING);
    addSpecialProperty(obj, STR_PARAMOUT, STR_DEFAULT, STR_ROLE_MAPPING);
}

Q_DECLARE_METATYPE(CAN_MESSAGE_PACKAGE)
int SUdsWidget::controlThread(void *pParam, const bool &bRunning)
{
    SUdsWidget* pWidget = (SUdsWidget*)pParam;
    QVariantMap ParamInfo;
    SObject* signalObj = nullptr;
    uint paramOutVer = 0;
    uint dataSourceVer = 0;
    QVariant data;
    QStringList deviceList;
    if(pWidget){
        while(bRunning){
            // deviceList
            if(pWidget->isMapped(STR_PARAMOUT)){
                ParamInfo = pWidget->mapping()[STR_PARAMOUT];
                signalObj =(SObject*)ParamInfo[STR_VALUE].value<void*>();
                if(signalObj != nullptr){
                    QByteArray signalProp = ParamInfo[STR_PROP].toString().toUtf8();
                    if(!signalProp.isEmpty()){
                        uint uVersion = signalObj->propertyInfo()[signalProp].m_version;
                        if(uVersion != paramOutVer){
                            if(signalObj->lock().tryLockForRead()){
                                data = signalObj->property(signalProp.data());
                                signalObj->lock().unlock();
                                if(data.isValid() && !data.isNull()){
                                    deviceList = data.toStringList();
                                    paramOutVer = uVersion;
                                }
                            }
                        }
                    }
                }
            }
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
            for(int i = 0; i < deviceList.size(); ++i){
                QString propName = QString(signalProp);
                uint uVersion = signalObj->propertyInfo()[propName].m_version;
                if(uVersion != dataSourceVer){
                    signalObj->lock().lockForRead();
                    CAN_MESSAGE_PACKAGE msg = signalObj->property(propName.toLatin1().data()).value<CAN_MESSAGE_PACKAGE>();
                    dataSourceVer = uVersion;
                    signalObj->lock().unlock();
                    pWidget->m_server->slotPackAna(msg);
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
    m_server->setContent(m_labels[L_SDATA]->text());
    m_server->process(ObjDataOut, PropDataOut);
}

void SUdsWidget::slotAddClicked()
{
    m_canIDMapModel->insertNewData();
}
