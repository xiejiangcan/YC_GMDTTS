#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include "ToolForUDS_global.h"

#include "tool/ycanhandle.h"
#include "tool/udsservertree.h"
#include "tool/udsserver.h"
#include "model/udscanidmapmodel.h"
#include "model/canmessagemodel.h"

namespace Ui {
class CalibrationWidget;
}

enum CALIBRATION_STATE
{
    // step1
    INIT_STATE = 0,
    EXTENT_STATE,
    GETSN_STATE,
    GETKEY_STATE,
    IDENTIFY_STATE,
    SEND1D06_STATE,
    SEND31010459_STATE,
    // step2
    SEND31030459_STATE,
    // step3
    SEND31020459_STATE,
    SEND221D07_STATE,
    SEND14FFFFFF_STATE,
    SEND190209_STATE,
    END_STATE,
};

class TOOLFORUDS_EXPORT CalibrationWidget : public SWidget
{
    Q_OBJECT

public:
    CalibrationWidget(SMainWindow *mainWindow, QWidget *parent = 0);
    ~CalibrationWidget();

    void setSObject(SObject *obj) override;
    void propertyOfSObjectChanged(SObject *obj,
                                  const QString &strPropName,
                                  const SObject::PropertyT &propChangedBy) override;

    // static
    static QString keyString();
    static void initSObject(SObject* obj);

protected:
    SThread m_thread;
    static int controlThread(void *pParam, const bool &bRunning);

    void initWidget();

    void DealExtendResponse(QString response);
    void DealGetSNResponse(QString response);
    void DealGetKeyResponse(QString response);
    void DealIdentityResponse(QString response);
    void Deal1D06Response(QString response);
    void Deal3101Response(QString response);
    void Deal3103Response(QString response);
    void Deal3102Response(QString response);
    void Deal221DResponse(QString response);
    void Deal14FFResponse(QString response);
    void Deal1902Response(QString response);

    void SendUDSMsg(QString msg);
    void StageFailed(QString msg);

signals:
    void signPackAna(const CAN_MESSAGE_PACKAGE &pack);

protected slots:
    void slotServerFinished();
    void slotTimeout();
    void slotShowMsg(const CAN_MESSAGE_PACKAGE &pack);

private slots:
    void on_btn_step1_clicked();

    void on_btn_step2_clicked();

    void on_btn_step3_clicked();

    void on_btn_save_clicked();

private:
    Ui::CalibrationWidget *ui;
    CanMessageModel*    m_msgModel = nullptr;
    UdsServer*          m_server = nullptr;

    QTimer              m_heartTime;
    CALIBRATION_STATE   m_curState;

    unsigned char       m_SN[4];
    unsigned char       m_Key[4];
    unsigned char       m_Res[4];
};

#endif // CALIBRATIONWIDGET_H
