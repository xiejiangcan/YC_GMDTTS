#ifndef SEMCWIDGET_H
#define SEMCWIDGET_H

#include "ToolForUDS_global.h"

#include "tool/yfileoperation.h"
#include "tool/decodetools.h"
#include "tool/ycanhandle.h"

namespace Ui {
class SEmcWidget;
}

class TOOLFORUDS_EXPORT SEmcWidget : public SWidget
{
    Q_OBJECT

public:
    SEmcWidget(SMainWindow *mainWindow, QWidget *parent = 0);
    ~SEmcWidget();

    // swidget
    void setSObject(SObject *obj) override;
    void propertyOfSObjectChanged(SObject *obj, const QString &strPropName,
                                  const SObject::PropertyT &propChangedBy) override;

    // static
    static QString keyString();
    static void initSObject(SObject* obj);

    static QString LinkFailString();
    static QString InitFileString();
    static QString DataToString(const S4_VEH_RX_DATA& data);

protected:
    SThread m_thread;
    static int controlThread(void *pParam, const bool &bRunning);

    void analysisData(const CAN_OBJ &source);
    void analysisData(const CAN_MESSAGE_PACKAGE& source);
    void fileOperation(bool open);
    void initConfig();

    QString getResultStr();
    QString getTimeoutStr();

private slots:
    void on_btn_file_clicked();
    void slotTimeOut();

private:
    Ui::SEmcWidget *ui;
    DecodeTools     m_tool;
    YFileOperation  mFileTool;
    QTimer*         m_timer;

    QMap<QString, QVariantMap>  m_result;
    bool                        m_saveFlag;
    bool                        m_timeOutFlag;
    bool                        m_changeFlag;

    uint                        m_devInd;
    uint                        m_devChan;
};

#endif // SEMCWIDGET_H
