#ifndef SPROJECT_H
#define SPROJECT_H

#include "core_global.h"
#include "sobject.h"
#include "tool/ssafedata.h"
#include "saction.h"
#include "sthread.h"
#include "scommunication.h"

#define DIALOG_DEFAULT_POS 16777215

class CORE_EXPORT SProject : public QObject
{
    Q_OBJECT
public:
    struct DataT{
        SObject* object; /**< TODO */
        QString property; /**< TODO */
        QString attribute; /**< TODO */
        QString type; /**< TODO */
    };
    enum TimerID{TID_TIMER_ACTION, TID_LENGTH};
    enum ThreadT{T_ACTION, T_LENGTH};
    typedef  QMap<QString, SCommunication*> Communications;

public:
    SProject(SMainWindow* mainWindow = nullptr,
             SObject *root = nullptr);
    ~SProject();

    // gettings
    SObject* projectRoot();
    Communications& mutableCommunications();
    const Communications& communications() const;
    QScriptEngine& scriptEngine();

    QVariant resource(const QString& strID);
    QVariantMap resourceByID(const QString& strID);
    bool _data(const QString& strID, DataT& dat);
    QObject* _ui(const QString& strID);
    SObject* _globalData();

    // settings
    void setProjectRoot(SObject* project);

    // function
    bool openFile(const QString& strFileName);
    void addResource(QObject* pSrcObj, const char* szIDName,
                     const QString& strType, const QVariant& value,
                     bool bInstance = true);
    bool copyProperty(const QString& strFrom, const QString& strTo);
    bool addTimerAction(const QString& strID);
    void removeTimerAction(const QString& strID);
    bool addThreadAction(const QString& strID);
    void removeThreadACtion(const QString& strID);
    bool popWidget(const QString& strID, bool bMode = false,
                   int x = DIALOG_DEFAULT_POS, int y = DIALOG_DEFAULT_POS,
                   Qt::WindowFlags flags = Qt::Dialog);
    void showWidget(const QString& strID, bool bMode = false,
                    int x = DIALOG_DEFAULT_POS,int y = DIALOG_DEFAULT_POS,
                    Qt::WindowFlags flags=Qt::Dialog);

    // static function
    static int actionThread(void* pParam, const bool& bRunning);

protected:
    void timerEvent(QTimerEvent *event) override;

public slots:
    QScriptValue ui(const QString& strID);
    QScriptValue data(const QString& strID);
    QScriptValue js(const QString& strCode);
    QScriptValue globalData();
    QScriptValue communication(const QString& strID);

private:
    void constructSubmenu(SObject* src,
                          QMenu* pMenuParent,
                          QToolBar* pToolBar,
                          int& toolbarItemCnt,
                          const QString& strParentPath);
    QWidget* constructWidget(SObject* src, QWidget* pParent);
    QLayout* constructLayout(const QString & strName, QWidget* pParent = 0);

private:
    QScriptEngine   m_scriptEngine;
    int             m_timerID[TID_LENGTH];
    SThread         m_threads[T_LENGTH];
    SMainWindow*    m_mainWindow = nullptr;
    SObject*        m_projectRoot = nullptr;
    QWidget*        m_central = nullptr;
    QStatusBar*     m_statusbar = nullptr;

    QMap<QString, QMenu*>       m_menus;
    QMap<QString, QAction*>     m_actions;
    QMap<QString, QToolBar*>    m_toolbars;
    QMap<QString, QDockWidget*> m_dockWidgets;
    QMap<QString, QDialog*>     m_popWidgets;
    QMap<QString, QVariantMap>  m_resources;
    Communications              m_communications;

    SSafeData<QMap<QString, SAction*>>  m_timerActions;
    SSafeData<QMap<QString, SAction*>>  m_threadActions;
};

#endif // SPROJECT_H
