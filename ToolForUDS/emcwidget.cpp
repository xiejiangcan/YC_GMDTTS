#include "emcwidget.h"
#include "ui_emcwidget.h"

#define ISAMONG(D,L,H) ((D>=L)&&(D<=H))

QMap<QString, QPair<double, double>> _EMC_VALUE_RANGE = {
    {QStringLiteral("电源"), {1, 10}},
    {QStringLiteral("KEYON唤醒源"), {1, 10}},
    {QStringLiteral("CAN唤醒源"), {1, 10}},
    {QStringLiteral("ETH_V3P3"), {1, 2}},
    {QStringLiteral("V3P3_UP"), {1, 2}},
    {QStringLiteral("VDD_1V25"), {1, 2}},
    {QStringLiteral("VDD_CNN_0V9"), {1, 1}},
    {QStringLiteral("VDD_0V9"), {1, 1}},
    {QStringLiteral("VDDQ_DDR_1V1"), {1, 1}},
    {QStringLiteral("VDDA_1V8"), {1, 1}},
    {QStringLiteral("VDD_DDR_1V0"), {1, 1}},
    {QStringLiteral("VDD_CORE_A0"), {1, 1}},
    {QStringLiteral("V3P3_J2"), {1, 10}},
    {QStringLiteral("SYS_IO_1P8V"), {1, 10}},
    {QStringLiteral("板上温度"), {0, 125}},
    {QStringLiteral("J2工作状态"), {1, 10}},
    {QStringLiteral("CAN2"), {1, 10}},
    {QStringLiteral("J2温度"), {1, 10}},
    {QStringLiteral("CPU"), {1, 100}},
    {QStringLiteral("BPU"), {1, 100}}
};

EmcWidget::EmcWidget(SMainWindow *mainWindow, QWidget *parent) :
    SWidget(mainWindow, parent),
    ui(new Ui::EmcWidget)
{
    ui->setupUi(this);

    initConfig();
}

EmcWidget::~EmcWidget()
{
    delete ui;
    if(m_thread.isRunning()){
        m_thread.stop();
        m_thread.wait();
        m_thread.quit();
    }
}

void EmcWidget::setSObject(SObject *obj)
{
    SWidget::setSObject(obj);

    QString title = QString("***********************************%1***********************************")
            .arg(obj->objectName());
    ui->LB_title->setText(title);
    ui->LB_Filepath->setText(obj->property(EMC_FILEPATH).toString());

    m_saveTimer = startTimer(obj->property(EMC_DATAFREQ).toUInt());
    m_devInd = obj->property(EMC_DEVIND).toUInt();
    m_devChan = obj->property(EMC_DEVCHAN).toUInt();
    QString strDevInd = QString("DevInd: %1").arg(m_devInd);
    QString strDevChan = QString("DevChan: %1").arg(m_devChan);
    ui->LB_devInd->setText(strDevInd);
    ui->LB_devChan->setText(strDevChan);
}

void EmcWidget::propertyOfSObjectChanged(SObject *obj, const QString &strPropName, const SObject::PropertyT &propChangedBy)
{
    Q_UNUSED(obj)
    Q_UNUSED(propChangedBy)
    QObject::blockSignals(true);
    if(strPropName == STR_SIGNALOUT){
        auto& mapMapping = mapping();
        if(mapMapping.contains(STR_SIGNALOUT)
                && mapMapping[STR_SIGNALOUT][STR_TYPE].toString() == STR_PROP){
            m_thread.setUserFunction(controlThread);
            m_thread.setUserParam(this);
            m_thread.start();
        }
    }
    QObject::blockSignals(false);
}

QString EmcWidget::keyString()
{
    return SWidget::key(EMC_WIDGET);
}

void EmcWidget::initSObject(SObject *obj)
{
    obj->setObjectName(EMC_WIDGET);
    obj->setProperty(EMC_DATAFREQ, 1000);
    obj->setProperty(EMC_FILEPATH, STR_DEFAULT);
    obj->setProperty(EMC_DEVIND, 0);
    obj->setProperty(EMC_DEVCHAN, 0);
    addSpecialProperty(obj, STR_DATASOURCE, "buff.receive_buff", STR_ROLE_MAPPING);
    addSpecialProperty(obj, STR_SIGNALOUT, "can.can_openstate", STR_ROLE_MAPPING);
    addSpecialProperty(obj, STR_RESULTOUT, "emcResult.emc_result", STR_ROLE_MAPPING);
}

QString EmcWidget::LinkFailString()
{
    QString result = QDateTime::currentDateTime().toString("hh:mm:ss") + "\t";
    result += QString("CAN Link failed\n");
    return result;
}

QString EmcWidget::InitFileString()
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
    result += QString("CPU_USED_RATE\t");
    result += QString("BPU_USED_RATE\n");

    return result;
}

QString EmcWidget::DataToString(const S4_VEH_RX_DATA &data)
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
    result += QString::number(data.emcTmsg4_25B.CAN_STAT_OK) + "\t";
    result += QString::number(data.emcTmsg4_25B.CPU_USED_RATE) + "\t";
    result += QString::number(data.emcTmsg4_25B.BPU_USED_RATE) + "\n";

    return result;
}

Q_DECLARE_METATYPE(CAN_MESSAGE_PACKAGE)
int EmcWidget::controlThread(void *pParam, const bool &bRunning)
{
    EmcWidget* pWidget = (EmcWidget*)pParam;
    if(pWidget){
        QVariantMap ParamInfo;
        SObject* signalObj = nullptr;
        QByteArray signalProp;
        uint uVersion, oldVersion = 0;
        QVariant data;
        while(bRunning){
            if(pWidget->isMapped(STR_DATASOURCE))
            {
                ParamInfo = pWidget->mapping()[STR_DATASOURCE];
                signalObj =(SObject*)ParamInfo[STR_VALUE].value<void*>();
                if(signalObj != nullptr)
                {
                    signalProp = ParamInfo[STR_PROP].toString().toUtf8();
                    if(!signalProp.isEmpty())
                    {
                        uVersion = signalObj->propertyInfo()[signalProp].m_version;
                        if(uVersion != oldVersion)
                        {
                            if(signalObj->lock().tryLockForRead())
                            {
                                data = signalObj->property(signalProp.data());
                                signalObj->lock().unlock();
                            }
                            if(data.isValid() && !data.isNull())
                            {
                                CAN_MESSAGE_PACKAGE canObj = data.value<CAN_MESSAGE_PACKAGE>();
                                oldVersion = uVersion;
                                pWidget->analysisData(canObj);
                            }
                        }
                    }
                }
            }

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
                        if(uVersion != oldVersion)
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
                                oldVersion = uVersion;
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

void EmcWidget::timerEvent(QTimerEvent *evt)
{
    if(evt->timerId() != m_saveTimer)
        return;
    if(!m_saveFlag)
        m_saveFlag = true;
}

void EmcWidget::analysisData(const CAN_OBJ &source)
{
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

    if(isMapped(STR_RESULTOUT)){
        auto mapResult = mapping()[STR_RESULTOUT];
        if(!mapResult.isEmpty()){
            SObject *resultObj = (SObject*)mapResult[STR_VALUE].value<void*>();
            if(resultObj){
                QString propName = mapResult[STR_PROP].toString();
                if(!propName.isEmpty()
                        && resultObj->lock().tryLockForWrite()){
                    resultObj->setProperty(propName, QVariant::fromValue(m_result));
                    resultObj->lock().unlock();
                }
            }
        }
    }

    auto data = m_tool.GetData();
    if(m_saveFlag){
        if(!mFileTool.IsOpen()){
            mFileTool.OpenFile();
        }
        mFileTool.WriteData(DataToString(data));
        m_saveFlag = false;
    }

}

Q_DECLARE_METATYPE(CAN_OBJ)
void EmcWidget::analysisData(const CAN_MESSAGE_PACKAGE &source)
{
    if(source.devInd == m_devInd && source.devChan == m_devChan){
        analysisData(source.canObj.value<CAN_OBJ>());
    }
}

void EmcWidget::fileOperation(bool open)
{
    if(open){
        mFileTool.OpenFile();
    }else{
        mFileTool.CloseFile();
        QString filepath = ui->LB_Filepath->text();
        QString devName = mapping()[STR_DATASOURCE][STR_PROP].toString();
        QString filename = devName
                + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss_")
                + QString(".tsf");
        mFileTool.SaveFile(filepath + "/" + filename);
    }
}

void EmcWidget::initConfig()
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
    m_result[QStringLiteral("J2温度")]["label"] = QVariant::fromValue(ui->LB_Temp_J2);
    m_result[QStringLiteral("CPU")]["label"] = QVariant::fromValue(ui->LB_CPU);
    m_result[QStringLiteral("BPU")]["label"] = QVariant::fromValue(ui->LB_BPU);

    for(auto iter = _EMC_VALUE_RANGE.begin();
        iter != _EMC_VALUE_RANGE.end(); ++iter){
        m_result[iter.key()]["range"] = QVariant::fromValue(iter.value());
    }
}

void EmcWidget::on_btn_file_clicked()
{
    auto filepath = QFileDialog::getExistingDirectory((QWidget*)this, tr("Save File"), "");
    ui->LB_Filepath->setText(filepath);
    this->sobject()->setPropertyS(EMC_FILEPATH, filepath);
}
