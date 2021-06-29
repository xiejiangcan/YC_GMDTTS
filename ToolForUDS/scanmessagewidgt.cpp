#include "scanmessagewidgt.h"
#include "tool/usbinterface.h"

SCanMessageWidgt::SCanMessageWidgt(SMainWindow *mainWindow, QWidget *parent)
    : SWidget(mainWindow, parent),
      m_table(new QTableView(this)),
      m_model(new CanMessageModel(this)),
      m_filterBtn(new QPushButton(tr("Filter"), this)),
      m_topWidget(new QWidget(this)),
      m_proxyModel(new QSortFilterProxyModel(this))
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
}

SCanMessageWidgt::~SCanMessageWidgt()
{
    if(m_timerID != -1){
        killTimer(m_timerID);
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

void SCanMessageWidgt::setSObject(SObject *obj)
{
    SWidget::setSObject(obj);

    m_checkBox[C_HEX]->setChecked(obj->property(IS_HEX).toBool());
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
            if(m_timerID == -1)
                m_timerID = startTimer(50);
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
    addSpecialProperty(obj, STR_DATASOURCE, STR_DEFAULT, STR_ROLE_MAPPING);
}

void SCanMessageWidgt::timerEvent(QTimerEvent *evt)
{
    if(evt->timerId() == m_timerID){
        analyzeData();
    }
}

Q_DECLARE_METATYPE(CAN_OBJ)

void SCanMessageWidgt::analyzeData()
{
    if(!isMapped(STR_DATASOURCE)){
        return;
    }
    QStringList deviceList = canDeviceList();
    SObject* signalObj = nullptr;
    auto ParamInfo = mapping()["DataSource"];
    signalObj =(SObject*)ParamInfo["value"].value<void*>();
    if(signalObj == nullptr){
        return;
    }
    while(m_versions.size() != deviceList.size()){
        if(m_versions.size() > deviceList.size()){
            m_versions.removeLast();
        }else{
            m_versions.append(1);
        }
    }
    QByteArray signalProp = ParamInfo["property"].toString().toUtf8();
    if(signalProp.isEmpty())
        return;
    for(int i = 0; i < deviceList.size(); ++i){
        QString propName = QString(signalProp) + QString::number(i);
        uint uVersion = signalObj->propertyInfo()[propName].m_version;
        if(uVersion != m_versions[i]){
            signalObj->lock().lockForRead();
            CAN_OBJ canObj = signalObj->property(propName.toLatin1().data()).value<CAN_OBJ>();
            m_versions[i] = uVersion;
            signalObj->lock().unlock();
            m_model->insertData(canObj);
            if(m_checkBox[C_AUTO]->isChecked()){
                m_table->scrollToBottom();
            }
        }
    }
}
