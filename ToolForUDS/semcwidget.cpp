#include "semcwidget.h"
#include "ui_semcwidget.h"

#define ISAMONG(D,L,H) ((D>=L)&&(D<=H))

QMap<QString, QPair<double, double>> _EMC_VALUE_RANGE = {
    {QStringLiteral("电源"), {6, 100}},
    {QStringLiteral("KEYON唤醒源"), {6, 100}},
    {QStringLiteral("CAN唤醒源"), {6, 100}},
    {QStringLiteral("ETH_V3P3"), {2, 4}},
    {QStringLiteral("V3P3_UP"), {2, 4}},
    {QStringLiteral("VDD_1V25"), {0.75, 2}},
    {QStringLiteral("VDD_CNN_0V9"), {0.5, 2}},
    {QStringLiteral("VDD_0V9"), {0.5, 2}},
    {QStringLiteral("VDDQ_DDR_1V1"), {0.75, 2}},
    {QStringLiteral("VDDA_1V8"), {1, 8}},
    {QStringLiteral("VDD_DDR_1V0"), {0.5, 2}},
    {QStringLiteral("VDD_CORE_A0"), {0.5, 10}},
    {QStringLiteral("V3P3_J2"), {1, 10}},
    {QStringLiteral("SYS_IO_1P8V"), {1, 10}},
    {QStringLiteral("板上温度"), {0, 125}},
    {QStringLiteral("J2工作状态"), {0, 0.5}},
    {QStringLiteral("CAN2"), {0.5, 1.5}},
    {QStringLiteral("CAN3"), {0.5, 1.5}},
    {QStringLiteral("J2温度"), {0, 125}},
    {QStringLiteral("CPU"), {1, 100}},
    {QStringLiteral("BPU"), {1, 100}}
};

SEmcWidget::SEmcWidget(SMainWindow *mainWindow, QWidget *parent) :
    SWidget(mainWindow, parent),
    ui(new Ui::SEmcWidget),
    m_timer(new QTimer(this)),
    m_timeOutFlag(true)
{
    ui->setupUi(this);

    initConfig();

    connect(m_timer, &QTimer::timeout, this, &SEmcWidget::slotTimeOut);
}

SEmcWidget::~SEmcWidget()
{
    delete ui;
    if(m_thread.isRunning()){
        m_thread.stop();
        m_thread.wait();
        m_thread.quit();
    }
}

void SEmcWidget::setSObject(SObject *obj)
{
    SWidget::setSObject(obj);

    QString title = QString("***********************************%1***********************************")
            .arg(obj->objectName());
    ui->LB_title->setText(title);
    ui->LB_Filepath->setText(obj->property(EMC_FILEPATH).toString());

    uint time = obj->property(EMC_DATAFREQ).toUInt();
    m_timer->start(time);
    m_devInd = obj->property(EMC_DEVIND).toUInt();
    m_devChan = obj->property(EMC_DEVCHAN).toUInt();
    QString strDevInd = QString("DevInd: %1").arg(m_devInd);
    QString strDevChan = QString("DevChan: %1").arg(m_devChan);
    ui->LB_devInd->setText(strDevInd);
    ui->LB_devChan->setText(strDevChan);

    if(isMapped(STR_DATASOURCE)){
        QVariantMap ParamInfo = mapping()[STR_DATASOURCE];
        SObject* signalObj =(SObject*)ParamInfo[STR_VALUE].value<void*>();
        if(signalObj){
            connect(signalObj, &SObject::signalNotifed,
                    this, &SEmcWidget::slotProcess);
        }
    }
}

void SEmcWidget::propertyOfSObjectChanged(SObject *obj, const QString &strPropName, const SObject::PropertyT &propChangedBy)
{
    Q_UNUSED(obj)
    Q_UNUSED(propChangedBy)
    QObject::blockSignals(true);
//    if(strPropName == STR_SIGNALOUT){
//        auto& mapMapping = mapping();
//        if(mapMapping.contains(STR_SIGNALOUT)
//                && mapMapping[STR_SIGNALOUT][STR_TYPE].toString() == STR_PROP){
//            m_thread.setUserFunction(controlThread);
//            m_thread.setUserParam(this);
//            m_thread.start();
//        }
//    }
    QObject::blockSignals(false);
}

QString SEmcWidget::keyString()
{
    return SWidget::key(EMC_WIDGET);
}

void SEmcWidget::initSObject(SObject *obj)
{
    obj->setObjectName(EMC_WIDGET);
    obj->setProperty(EMC_DATAFREQ, 1000);
    obj->setProperty(EMC_FILEPATH, STR_DEFAULT);
    obj->setProperty(EMC_DEVIND, 0);
    obj->setProperty(EMC_DEVCHAN, 0);
    addSpecialProperty(obj, STR_DATASOURCE, "buff.receive_buff", STR_ROLE_MAPPING);
    addSpecialProperty(obj, STR_SIGNALOUT, "can.can_openstate", STR_ROLE_MAPPING);
    addSpecialProperty(obj, STR_RESULTOUT, "emcResult.emc_result_", STR_ROLE_MAPPING);
}

QString SEmcWidget::LinkFailString()
{
    QString result = QDateTime::currentDateTime().toString("hh:mm:ss") + "\t";
    result += QString("CAN Link failed\n");
    return result;
}

QString SEmcWidget::InitFileString()
{
    QString result = QString("time\t");
    result += QString("Index\t");
    result += QString("VBAT_P_F\t");
    result += QString("IGN\t");
    result += QString("CAN_WAKE_UP\t");
    result += QString("V3P3_ETH\t");
    result += QString("MUC_V3P3_UP\t");
    result += QString("MCU_VDD_1V25_UP\t");
    result += QString("J2A_VDD_CNN_0V9\t");
    result += QString("J2A_VDD_0V9\t");
    result += QString("J2A_VDDQ_DDR_1V1\t");
    result += QString("J2A_VDDA_1V8\t");
    result += QString("J2A_VDD_DDR_1V0\t");
    result += QString("J2A_VDD_CORE\t");
    result += QString("V3P3_J2\t");
    result += QString("J2_TEMP_ADC\t");
    result += QString("SYS_IO_1P8V\t");
    result += QString("NTC_temp\t");
    result += QString("J2_state\t");
    result += QString("CAN2_state\t");
    result += QString("CAN3_state\t");
    result += QString("CPU_USED_RATE\t");
    result += QString("BPU_USED_RATE\n");

    return result;
}

QString SEmcWidget::DataToString(const S4_VEH_RX_DATA &data)
{
    QString result = QDateTime::currentDateTime().toString("hh:mm:ss") + "\t";

    result += QString::number(data.emcTmsg1_258.VBAT_P_F_ADC) + "\t";
    result += QString::number(data.emcTmsg1_258.IGN_ADC) + "\t";
    result += QString::number(data.emcTmsg1_258.CAN_WAKE_UP_ADC) + "\t";
    result += QString::number(data.emcTmsg1_258.V3P3_ETH_ADC) + "\t";
    result += QString::number(data.emcTmsg1_258.V3P3_UP_ADC) + "\t";

    result += QString::number(data.emcTmsg2_259.VDD_1V25_UP_ADC) + "\t";
    result += QString::number(data.emcTmsg2_259.J2_CNN_0V9_ADC) + "\t";
    result += QString::number(data.emcTmsg2_259.J2_VDD_0V9_ADC) + "\t";
    result += QString::number(data.emcTmsg2_259.J2_VDDQ_DDR_1V1_AI_ADC) + "\t";
    result += QString::number(data.emcTmsg2_259.J2_VDDA_1V8_AI_ADC) + "\t";

    result += QString::number(data.emcTmsg3_25A.J2A_VDD_DDR_1V0_ADC) + "\t";
    result += QString::number(data.emcTmsg3_25A.J2A_VDD_CORE_AO_AI_ADC) + "\t";
    result += QString::number(data.emcTmsg3_25A.V3P3_J2_ADC) + "\t";
    result += QString::number(data.emcTmsg3_25A.J2_TEMP_ADC) + "\t";
    result += QString::number(data.emcTmsg3_25A.SYS_IO_1P8V_ADC) + "\t";
    result += QString::number(data.emcTmsg3_25A.NTC_temp_ADC) + "\t";

    result += QString::number(data.emcTmsg4_25B.J2_STAT_OK) + "\t";
    result += QString::number(data.emcTmsg5_262.CAN2_STAT_OK) + "\t";
    result += QString::number(data.emcTmsg6_26C.CAN3_STAT_OK) + "\t";
    result += QString::number(data.emcTmsg4_25B.CPU_USED_RATE) + "\t";
    result += QString::number(data.emcTmsg4_25B.BPU_USED_RATE) + "\n";

    return result;
}

Q_DECLARE_METATYPE(CAN_MESSAGE_PACKAGE)
int SEmcWidget::controlThread(void *pParam, const bool &bRunning)
{
    SEmcWidget* pWidget = (SEmcWidget*)pParam;
    if(pWidget){
        QVariantMap ParamInfo;
        SObject* signalObj = nullptr;
        QByteArray signalProp;
        uint uVersion;
        uint signalVer = 0, dataVer = 0;
        QVariant data;
        while(bRunning){
//            if(pWidget->isMapped(STR_DATASOURCE))
//            {
//                ParamInfo = pWidget->mapping()[STR_DATASOURCE];
//                signalObj =(SObject*)ParamInfo[STR_VALUE].value<void*>();
//                if(signalObj != nullptr)
//                {
//                    signalProp = ParamInfo[STR_PROP].toString().toUtf8();
//                    if(!signalProp.isEmpty())
//                    {
//                        uVersion = signalObj->propertyInfo()[signalProp].m_version;
//                        if(uVersion != dataVer)
//                        {
//                            if(signalObj->lock().tryLockForRead())
//                            {
//                                data = signalObj->property(signalProp.data());
//                                signalObj->lock().unlock();
//                            }
//                            if(data.isValid() && !data.isNull())
//                            {
//                                CAN_MESSAGE_PACKAGE canObj = data.value<CAN_MESSAGE_PACKAGE>();
//                                dataVer = uVersion;
//                                pWidget->analysisData(canObj);
//                            }
//                        }
//                    }
//                }
//            }

            if(pWidget->isMapped(STR_SIGNALOUT))
            {
                ParamInfo = pWidget->mapping()[STR_SIGNALOUT];
                signalObj =(SObject*)ParamInfo[STR_VALUE].value<void*>();
                if(signalObj != nullptr)
                {
                    signalProp = ParamInfo[STR_PROP].toString().toUtf8();
                    if(!signalProp.isEmpty())
                    {
                        uVersion = signalObj->propertyInfo()[signalProp].m_version;
                        if(uVersion != signalVer)
                        {
                            if(signalObj->lock().tryLockForRead())
                            {
                                auto propLst = signalObj->dynamicPropertyNames();
                                data = signalObj->property(signalProp.data());
                                signalObj->lock().unlock();
                            }
                            if(data.isValid() && !data.isNull())
                            {
                                bool isRunning = data.toBool();
                                signalVer = uVersion;
                                pWidget->fileOperation(isRunning);
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

void SEmcWidget::analysisData(const CAN_OBJ &source)
{
    m_timeOutFlag = false;
    m_changeFlag = true;
    m_tool.AddNewMessage(source.ID, source.Data);
    auto map = m_tool.GetMap();
    for(auto iter = map.begin(); iter != map.end(); ++iter){
        QString keyName = iter.key();
        double value = iter.value().toDouble();
        QPair<double,double> range = m_result[keyName]["range"]
                .value<QPair<double,double>>();
        if(ISAMONG(value, range.first, range.second)){
            m_result[keyName]["state"] = true;
        }else{
            if(m_result[keyName]["state"].toBool()){
                uint count = m_result[keyName]["error"].toUInt();
                m_result[keyName]["error"] = count + 1;
            }
            m_result[keyName]["state"] = false;
        }
        QLabel* pLabel = m_result[keyName]["label"].value<QLabel*>();
        pLabel->setNum(value);
    }

    if(m_tool.IsDataFull() && isMapped(STR_RESULTOUT)){
        auto mapResult = mapping()[STR_RESULTOUT];
        if(!mapResult.isEmpty()){
            SObject *resultObj = (SObject*)mapResult[STR_VALUE].value<void*>();
            if(resultObj){
                QString result = getResultStr();
                QString propName = mapResult[STR_PROP].toString();
                if(!propName.isEmpty()){
                    resultObj->setPropertyS(propName, result);
                }
            }
        }
    }

    auto data = m_tool.GetData();
    if(m_saveFlag){
        if(!mFileTool.IsOpen()){
            mFileTool.OpenFile(this->sobject()->objectName());
            mFileTool.WriteData(InitFileString());
        }
        mFileTool.WriteData(DataToString(data));
        m_saveFlag = false;
    }

}

Q_DECLARE_METATYPE(CAN_OBJ)
void SEmcWidget::analysisData(const CAN_MESSAGE_PACKAGE &source)
{
    if(source.devInd == m_devInd && source.devChan == m_devChan){
        analysisData(source.canObj.value<CAN_OBJ>());
    }
}

void SEmcWidget::fileOperation(bool open)
{
    if(open){
        mFileTool.OpenFile(this->sobject()->objectName());
        mFileTool.WriteData(InitFileString());
    }else{
        mFileTool.CloseFile();
        QString filepath = ui->LB_Filepath->text();
        if(filepath.isEmpty()){
            filepath = QApplication::applicationDirPath() + "/data";
        }
        QString devName = this->sobject()->objectName();
        QString filename = devName
                + QDateTime::currentDateTime().toString(" yyyyMMdd-hhmmss_")
                + QString(".tsf");
        if(!mFileTool.SaveFile(filepath + "/" + filename))
            qDebug() << "save file failed";
    }
}

void SEmcWidget::initConfig()
{
    m_result[QStringLiteral("电源")]["label"] = QVariant::fromValue(ui->LB_Power);
    m_result[QStringLiteral("KEYON唤醒源")]["label"] = QVariant::fromValue(ui->LB_KEYON);
    m_result[QStringLiteral("CAN唤醒源")]["label"] = QVariant::fromValue(ui->LB_CAN);
    m_result[QStringLiteral("ETH_V3P3")]["label"] = QVariant::fromValue(ui->LB_ETH);
    m_result[QStringLiteral("V3P3_UP")]["label"] = QVariant::fromValue(ui->LB_MCU_UP);
    m_result[QStringLiteral("VDD_1V25")]["label"] = QVariant::fromValue(ui->LB_VDD);
    m_result[QStringLiteral("VDD_CNN_0V9")]["label"] = QVariant::fromValue(ui->LB_J2_CNN);
    m_result[QStringLiteral("VDD_0V9")]["label"] = QVariant::fromValue(ui->LB_J2_VDD);
    m_result[QStringLiteral("VDDQ_DDR_1V1")]["label"] = QVariant::fromValue(ui->LB_J2_VDDQ);
    m_result[QStringLiteral("VDDA_1V8")]["label"] = QVariant::fromValue(ui->LB_J2_VDDA);
    m_result[QStringLiteral("VDD_DDR_1V0")]["label"] = QVariant::fromValue(ui->LB_J2_DDR);
    m_result[QStringLiteral("VDD_CORE_A0")]["label"] = QVariant::fromValue(ui->LB_J2_CORE);
    m_result[QStringLiteral("V3P3_J2")]["label"] = QVariant::fromValue(ui->LB_J2);
    m_result[QStringLiteral("SYS_IO_1P8V")]["label"] = QVariant::fromValue(ui->LB_SYS);
    m_result[QStringLiteral("板上温度")]["label"] = QVariant::fromValue(ui->LB_Temp);
    m_result[QStringLiteral("J2工作状态")]["label"] = QVariant::fromValue(ui->LB_J2_Status);
    m_result[QStringLiteral("CAN2")]["label"] = QVariant::fromValue(ui->LB_CAN2_Status);
    m_result[QStringLiteral("CAN3")]["label"] = QVariant::fromValue(ui->LB_CAN3_Status);
    m_result[QStringLiteral("J2温度")]["label"] = QVariant::fromValue(ui->LB_Temp_J2);
    m_result[QStringLiteral("CPU")]["label"] = QVariant::fromValue(ui->LB_CPU);
    m_result[QStringLiteral("BPU")]["label"] = QVariant::fromValue(ui->LB_BPU);

    for(auto iter = _EMC_VALUE_RANGE.begin();
        iter != _EMC_VALUE_RANGE.end(); ++iter){
        m_result[iter.key()]["range"] = QVariant::fromValue(iter.value());
    }
}

QString SEmcWidget::getResultStr()
{
    QString str = sobject()->objectName() + "@";
    for(auto iter = m_result.begin(); iter != m_result.end(); ++iter){
        str += (iter.key() + QString(":"));
        for(auto item = iter.value().begin(); item != iter.value().end(); ++item){
            str += (item.key() + QString("-"));
            str += (QString::number(item.value().toDouble()) + QString("&"));
        }
        str += QString(";");
    }
    return str;
}

QString SEmcWidget::getTimeoutStr()
{
    QString str = sobject()->objectName() + "@";
    return str;
}

void SEmcWidget::on_btn_file_clicked()
{
    auto filepath = QFileDialog::getExistingDirectory((QWidget*)this, tr("Save File"), "");
    ui->LB_Filepath->setText(filepath);
    this->sobject()->setPropertyS(EMC_FILEPATH, filepath);
}

void SEmcWidget::slotTimeOut()
{
    if(!m_saveFlag)
        m_saveFlag = true;
    if(m_timeOutFlag && m_changeFlag){
        if(isMapped(STR_RESULTOUT)){
            auto mapResult = mapping()[STR_RESULTOUT];
            if(!mapResult.isEmpty()){
                SObject *resultObj = (SObject*)mapResult[STR_VALUE].value<void*>();
                if(resultObj){
                    QString result = getTimeoutStr();
                    QString propName = mapResult[STR_PROP].toString();
                    if(!propName.isEmpty()){
                        resultObj->setPropertyS(propName, result);
                    }
                }
            }
        }
        m_changeFlag = false;
    }
    m_timeOutFlag = true;
}

void SEmcWidget::slotProcess(uint signType, QVariant data)
{
    if(signType != SIGNAL_CAN_MESSAGE)
        return;
    CAN_MESSAGE_PACKAGE canObj = data.value<CAN_MESSAGE_PACKAGE>();
    canObj.type = QString("RXD");
    analysisData(canObj);
}

