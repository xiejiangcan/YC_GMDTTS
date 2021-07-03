#ifndef EMCWIDGET_H
#define EMCWIDGET_H

#include "ToolForUDS_global.h"

#include "tool/yfileoperation.h"
#include "tool/decodetools.h"
#include "tool/ycanhandle.h"

namespace Ui {
class EmcWidget;
}

class TOOLFORUDS_EXPORT EmcWidget : public SWidget
{
    Q_OBJECT

public:
    EmcWidget(SMainWindow *mainWindow, QWidget *parent = 0);
    ~EmcWidget();

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
    void timerEvent(QTimerEvent* evt) override;

    void analysisData(const CAN_OBJ &source);
    void analysisData(const CAN_MESSAGE_PACKAGE& source);
    void fileOperation(bool open);
    void initConfig();

private slots:
    void on_btn_file_clicked();

private:
    Ui::EmcWidget   *ui;
    DecodeTools     m_tool;
    YFileOperation  mFileTool;

    QMap<QString, QVariantMap>  m_result;
    int                         m_saveTimer;
    bool                        m_saveFlag;

    uint                        m_devInd;
    uint                        m_devChan;
};

#endif // EMCWIDGET_H
