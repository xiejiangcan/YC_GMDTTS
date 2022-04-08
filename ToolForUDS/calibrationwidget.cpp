#include "calibrationwidget.h"
#include "ui_calibrationwidget.h"

#include "tool/generatekeyexlmpl.h"

CalibrationWidget::CalibrationWidget(SMainWindow *mainWindow, QWidget *parent) :
    SWidget(mainWindow, parent),
    ui(new Ui::CalibrationWidget),
    m_msgModel(new CanMessageModel),
    m_server(new UdsServer(this))
{
    ui->setupUi(this);

    m_curState = INIT_STATE;

    // init table view
    ui->tableView->setModel(m_msgModel);
    ui->tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    // init server
    m_server->setCanID(0x782, 0x78A);
    m_server->setTimeout(1000);
    m_server->setRecvParams(1024, 0x14);

    connect(&m_heartTime, &QTimer::timeout,
            this, &CalibrationWidget::slotTimeout);

    connect(m_server, &UdsServer::signCompleted,
            this, &CalibrationWidget::slotServerFinished);

    connect(m_server, &UdsServer::signalShowMsg, this, &CalibrationWidget::slotShowMsg);
}

CalibrationWidget::~CalibrationWidget()
{
    delete ui;
    if(m_thread.isRunning()){
        m_thread.stop();
        m_thread.wait();
        m_thread.quit();
    }
}

void CalibrationWidget::setSObject(SObject *obj)
{
    SWidget::setSObject(obj);

}

Q_DECLARE_METATYPE(CAN_OBJ)
void CalibrationWidget::propertyOfSObjectChanged(SObject *obj, const QString &strPropName, const SObject::PropertyT &propChangedBy)
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

QString CalibrationWidget::keyString()
{
    return SWidget::key(CAL_WIDGET);
}

void CalibrationWidget::initSObject(SObject *obj)
{
    obj->setSObjectName(CAL_WIDGET);
    //obj->setProperty(SERVER_FILE, STR_DEFAULT);
    obj->setProperty(MAP_CAN_ID, STR_DEFAULT);
    obj->setProperty(LAST_CAN_ID, STR_DEFAULT);
    obj->setProperty(LAST_DATA, STR_DEFAULT);

    addSpecialProperty(obj, STR_DATASOURCE, "buff.receive_buff", STR_ROLE_MAPPING);
    addSpecialProperty(obj, STR_DATAOUT, "buff.send_buff_0", STR_ROLE_MAPPING);
}

Q_DECLARE_METATYPE(CAN_MESSAGE_PACKAGE)
int CalibrationWidget::controlThread(void *pParam, const bool &bRunning)
{
    CalibrationWidget* pWidget = (CalibrationWidget*)pParam;
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

void CalibrationWidget::DealExtendResponse(QString response)
{
    if(response.startsWith("50 03"))
    {
        m_curState = GETSN_STATE;
        m_heartTime.start(3000);
        SendUDSMsg("22 F1 8C");
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::DealGetSNResponse(QString response)
{
    if(response.startsWith("62 F1 8C"))
    {
        m_curState = GETKEY_STATE;
        qDebug() << "DealGetSNResponse : " << response;
        // 提取SN信息，最后四位
        QStringList list = response.split(' ');
        for(int i = list.size() - 5, j = 0; j < 4; i++, j++)
        {
            bool isOk;
            m_SN[j] = list[i].toUShort(&isOk, 16);
        }

        SendUDSMsg("27 01");
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::DealGetKeyResponse(QString response)
{
    if(response.startsWith("67 01"))
    {
        m_curState = IDENTIFY_STATE;

        // 提取KEY信息，标志位后面的四位
        QStringList list = response.split(' ');
        for(int i = 2, j = 0; j < 4; i++, j++)
        {
            bool isOk;
            m_Key[j] = list[i].toUShort(&isOk, 16);
        }

        unsigned int realLength = 4;
        VKeyGenResultEx res = GenerateKeyExOpt(m_Key, 4, 1, m_SN, 4, m_Res, 4, realLength);
        if(res != KGRE_Ok || realLength != 4)
        {
            m_curState = INIT_STATE;
            ui->btn_step1->setEnabled(true);
            ui->btn_step2->setEnabled(true);
            ui->btn_step3->setEnabled(true);
            m_heartTime.stop();
            QMessageBox::warning(this, tr("Warning"),
                                 tr("calulate the key failed!"));
            return;
        }

        QString msg = QString("27 02 %1 %2 %3 %4").arg(m_Res[0], 2, 16, QChar('0'))
                .arg(m_Res[1], 2, 16, QChar('0'))
                .arg(m_Res[2], 2, 16, QChar('0'))
                .arg(m_Res[3], 2, 16, QChar('0'));
        qDebug() << msg;
        SendUDSMsg(msg);
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::DealIdentityResponse(QString response)
{
    if(response.startsWith("67 02"))
    {
        m_curState = SEND1D06_STATE;
        SendUDSMsg("2E 1D 06 0F A0 00 96 00 96 02 76 02 76 00 A0 01 00");
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::Deal1D06Response(QString response)
{
    if(response.startsWith("6E 1D"))
    {
        m_curState = SEND31010459_STATE;
        SendUDSMsg("31 01 04 59");
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::Deal3101Response(QString response)
{
    if(response.startsWith("71 01"))
    {
        m_curState = SEND31030459_STATE;
        // step1 finished
        ui->btn_step1->setEnabled(true);
        ui->btn_step2->setEnabled(true);
        ui->btn_step3->setEnabled(true);
        QMessageBox::information(this, tr("information"),
                                 QString("Step1 finished successfully"));
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::Deal3103Response(QString response)
{
    if(response.startsWith("71 03"))
    {
        // step2 finished
        ui->btn_step1->setEnabled(true);
        ui->btn_step2->setEnabled(true);
        ui->btn_step3->setEnabled(true);

        if(response.startsWith("71 03 04 59 02"))
        {
            m_curState = SEND31020459_STATE;
            QMessageBox::information(this, tr("information"),
                                     QString("Step2 finished successfully"));
        }

    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::Deal3102Response(QString response)
{
    if(response.startsWith("71 02"))
    {
        m_curState = SEND221D07_STATE;

        SendUDSMsg("22 1D 07");
    }
    else if(response.startsWith("71 03"))
    {
        qDebug() << "ignore the 71 03 message";
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::Deal221DResponse(QString response)
{
    if(response.startsWith("62 1D"))
    {
        m_curState = SEND14FFFFFF_STATE;
        SendUDSMsg("14 FF FF FF");
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::Deal14FFResponse(QString response)
{
    if(response.startsWith("54"))
    {
        m_curState = SEND190209_STATE;
        SendUDSMsg("19 02 09");
    }
    else
    {
        StageFailed(response);
    }
}

void CalibrationWidget::Deal1902Response(QString response)
{
    if(response.size() > 0)
    {
        m_curState = END_STATE;
        ui->btn_step1->setEnabled(true);
        ui->btn_step2->setEnabled(true);
        ui->btn_step3->setEnabled(true);
        QMessageBox::information(this, tr("information"),
                                 tr("Calibration finished successfully!"));
    }
}

void CalibrationWidget::SendUDSMsg(QString msg)
{
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

    m_server->setContent(msg);
    m_server->process(ObjDataOut, PropDataOut);
}

void CalibrationWidget::StageFailed(QString msg)
{
    ui->btn_step1->setEnabled(true);
    ui->btn_step2->setEnabled(true);
    ui->btn_step3->setEnabled(true);
    m_heartTime.stop();
    QMessageBox::warning(this, tr("Warning"),
                         QString("Calibration failed at %1 [%2]").arg((int)m_curState).arg(msg));

    m_curState = INIT_STATE;
}

void CalibrationWidget::slotServerFinished()
{
    QString result;
    if(m_server->getStatus() == UdsServer::N_OK){
        result = m_server->getBackPack().toUpper();
    }else{
        m_curState = INIT_STATE;
        ui->btn_step1->setEnabled(true);
        ui->btn_step2->setEnabled(true);
        ui->btn_step3->setEnabled(true);
        m_heartTime.stop();
        QMessageBox::information(this, tr("information"), tr("UDS send failed!"));
        return;
    }

    switch (m_curState) {
    case EXTENT_STATE:
        DealExtendResponse(result);
        break;
    case GETSN_STATE:
        DealGetSNResponse(result);
        break;
    case GETKEY_STATE:
        DealGetKeyResponse(result);
        break;
    case IDENTIFY_STATE:
        DealIdentityResponse(result);
        break;
    case SEND1D06_STATE:
        Deal1D06Response(result);
        break;
    case SEND31010459_STATE:
        Deal3101Response(result);
        break;
    case SEND31030459_STATE:
        Deal3103Response(result);
        break;
    case SEND31020459_STATE:
        Deal3102Response(result);
        break;
    case SEND221D07_STATE:
        Deal221DResponse(result);
        break;
    case SEND14FFFFFF_STATE:
        Deal14FFResponse(result);
        break;
    case SEND190209_STATE:
        Deal1902Response(result);
        break;
    case END_STATE:
        break;
    default:
        break;
    }
}

void CalibrationWidget::slotTimeout()
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

void CalibrationWidget::slotShowMsg(const CAN_MESSAGE_PACKAGE &pack)
{
    CAN_MESSAGE_PACKAGE obj = pack;
    obj.type = QString("RXD");
    m_msgModel->insertData(obj);
}

void CalibrationWidget::on_btn_step1_clicked()
{
    m_curState = EXTENT_STATE;
    SendUDSMsg("10 03");

    ui->btn_step1->setEnabled(false);
    ui->btn_step2->setEnabled(false);
    ui->btn_step3->setEnabled(false);
}

void CalibrationWidget::on_btn_step2_clicked()
{
    if(SEND31030459_STATE != m_curState)
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Please complete STEP1!"));
        return;
    }

    SendUDSMsg("31 03 04 59");
    ui->btn_step1->setEnabled(false);
    ui->btn_step2->setEnabled(false);
    ui->btn_step3->setEnabled(false);
}

void CalibrationWidget::on_btn_step3_clicked()
{
    if(SEND31020459_STATE != m_curState)
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Please complete STEP1 and STEP2!"));
        return;
    }

    SendUDSMsg("31 02 04 59");
    ui->btn_step1->setEnabled(false);
    ui->btn_step2->setEnabled(false);
    ui->btn_step3->setEnabled(false);
}

void CalibrationWidget::on_btn_save_clicked()
{
    QSettings settings;
    auto filepath = QFileDialog::getSaveFileName((QWidget*)this, tr("Save File"), settings.value("/calibration/lastSavePath").toString(), tr("Text (*.txt)"));
    settings.setValue("/calibration/lastSavePath", filepath);

    if(!m_msgModel->saveToFile(filepath))
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Save to File Failed!"));
        return;
    }
}
