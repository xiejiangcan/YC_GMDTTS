#ifndef SMAINWINDOW_H
#define SMAINWINDOW_H

#include "core_global.h"
#include "sobject.h"
#include "tool/ssafedata.h"

class CORE_EXPORT SMainWindow : public QMainWindow
{
    Q_OBJECT
    friend class SProject;
public:
    enum ActrionT{ACT_NEW_INS, ACT_NEW_COMP, ACT_OPEN, ACT_SAVE, ACT_SAVEAS, ACT_EXIT,
                  ACT_MINS, ACT_LANGUAGE, ACT_ABOUT, ACT_CLOSE, ACT_HELP, ACT_VIEW_TOOLBAR,
                  ACT_LENGTH};
    enum MenuT{M_FILE , M_VIEW, M_HELP, M_RECENT_PRO, M_LENGTH};
    enum ToolBarT{TB_FILE, TB_USER, TB_LENGTH};
    enum ProcessT{P_HELP, P_LENGTH};
public:
    SMainWindow(const QString& strPluginRoot = "../",
                QWidget *parent = nullptr);
    virtual ~SMainWindow();

    // gettings
    QMenu* menu(MenuT index);
    QToolBar* toolbar(ToolBarT index);
    QMap<QString, QMap<QString, SPluginInterface*>>& mutablePluginInterfaces();
    const QMap<QString, QMap<QString, SPluginInterface*>>& pluginInterfaces() const;
    SProject* project();

    // static function
    static void msgHandler(QtMsgType type, const QMessageLogContext& content,
                          const QString& msg);
    static QStringList logs(int& nBeginID);

    // function
    void loadPlugins(const QString& strPluginDir);
    QObject* createPlugin(const QString& strType,
                          const QString& strKey,
                          QObject* parent = NULL);
    SObject *createSObjectForPlugin(const QString& strType,
                                    const QString& strKey,
                                    SObject* parent = NULL);

protected:
    // init
    virtual void createAction();
    virtual void createMenu();
    virtual void createToolbar();

    // event
    void closeEvent(QCloseEvent* event) override;

public slots:
    void slotNewProject();
    void slotOpenProject(const QString& strFileName = "");
    void slotOpenProject(const SObject* projObj);
    void slotCloseProject(bool bReleaseOld = true);
    void slotSaveAs();
    void slotSave();
    void slotAboutSw();
    void slotHelp();
    void slotManageProject();
    void slotUpdateFileMenu();
    void slotTriRecentProj();
    void slotShowToolBar(bool show);

private:
    static int                          m_logBeginID;
    static SSafeData<QList<QString>>    m_logs;

    QAction*    m_actions[ACT_LENGTH];
    QMenu*      m_menus[M_LENGTH];
    QToolBar*   m_toolbars[TB_LENGTH];
    QProcess*   m_processes[P_LENGTH];
    SProject*   m_project = nullptr;
    QMap<QString, QMap<QString, SPluginInterface*>> m_pluginInterfaces;
};

#endif // SMAINWINDOW_H
