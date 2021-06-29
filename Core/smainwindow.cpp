#include "smainwindow.h"
#include "sactionforjs.h"
#include "sproject.h"
#include "tool/sprojecttreewidget.h"

int SMainWindow::m_logBeginID = 0;
SSafeData<QList<QString>> SMainWindow::m_logs;

SMainWindow::SMainWindow(const QString &strPluginRoot, QWidget *parent)
    : QMainWindow(parent)
{
    QDir::setCurrent(QApplication::applicationDirPath());
    QCoreApplication::setOrganizationName("YC");
    QCoreApplication::setApplicationName("GMDTTS");
    QSettings::setDefaultFormat(QSettings::NativeFormat);
    qInstallMessageHandler(msgHandler);
    createAction();
    createMenu();
    createToolbar();
    loadPlugins(strPluginRoot);
    setWindowTitle("GMDTTS");
    setWindowIcon(QIcon(":/icon/logo"));
    m_processes[P_HELP] = new QProcess(this);
    showMaximized();
}

SMainWindow::~SMainWindow()
{
    slotCloseProject(true);
}

QMenu *SMainWindow::menu(SMainWindow::MenuT index)
{
    return m_menus[index];
}

QToolBar *SMainWindow::toolbar(SMainWindow::ToolBarT index)
{
    return m_toolbars[index];
}

QMap<QString, QMap<QString, SPluginInterface *> > &SMainWindow::mutablePluginInterfaces()
{
    return m_pluginInterfaces;
}

const QMap<QString, QMap<QString, SPluginInterface *> > &SMainWindow::pluginInterfaces() const
{
    return m_pluginInterfaces;
}

SProject *SMainWindow::project()
{
    return m_project;
}

void SMainWindow::msgHandler(QtMsgType type,
                             const QMessageLogContext &content,
                             const QString &msg)
{
    QString strMsg;
    strMsg = QTime::currentTime().toString("hhmmss");
    switch (type) {
    case QtDebugMsg:
        strMsg +=  "d:";
        break;
    case QtWarningMsg:

        strMsg +=  QString("w:%1 line %2 %3").arg(content.function).arg(content.line).arg(content.category) ;
        break;
    case QtCriticalMsg:
        strMsg +=  "c:";
        break;
    case QtFatalMsg:
        strMsg +=  "f:";
    }
    strMsg += msg;// + "\n";

    m_logs.lockForWrite();
    if(m_logs->size() > 1000){
        m_logs->erase(m_logs->begin(), m_logs->begin() + 500);
        m_logBeginID += 500;
    }
    m_logs->append(strMsg);
    m_logs.unlock();
    //ts<<strMsg;
    fprintf(stderr,"%s\n",strMsg.toLocal8Bit().constData());
    fflush(stderr);
    if(type == QtFatalMsg)abort();
}

QStringList SMainWindow::logs(int &nBeginID)
{
    QStringList lstLog;
    if(nBeginID > (m_logBeginID + m_logs->size() - 1)){
        return lstLog;
    }
    int nIndex;
    m_logs.lockForRead();
    if(m_logBeginID > nBeginID)
        nIndex = 0;
    else
        nIndex = nBeginID - m_logBeginID;
    nBeginID = m_logBeginID + m_logs->size();
    for(auto iterMsg = m_logs->begin() + nIndex; iterMsg != m_logs->end(); iterMsg++)
        lstLog.append(*iterMsg);
    m_logs.unlock();
    return lstLog;
}

void SMainWindow::loadPlugins(const QString &strPluginDir)
{
    QDir pluginsDir(strPluginDir);

    foreach (QString fileName, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        loadPlugins(strPluginDir + "/" + fileName);
    }
    SPluginInterface* pInterface;
    QStringList lstKeys,lstKey;

    pluginsDir.setNameFilters(QStringList() << "*.dll"<< "*.a" << "*.so");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        if ((pInterface =
             qobject_cast<SPluginInterface *>(loader.instance())) != NULL){
            lstKeys = pInterface->keys();//获取插件内的所有功能
            foreach (QString strKey, lstKeys) {
                strKey.replace(':', "_");
                lstKey = strKey.split('@');
                if(lstKey.size() < 2
                        || lstKey[0].isEmpty()
                        || lstKey[1].isEmpty())
                    continue;
                m_pluginInterfaces[lstKey[0]][lstKey[1]] = pInterface;//保存功能的创造指针
            }
        }else{
            qDebug() << loader.errorString();
        }
    }
}

QObject *SMainWindow::createPlugin(const QString &strType, const QString &strKey, QObject *parent)
{
    QString strID = strType + STR_CUT_CHAR + strKey;
    if(strType == STR_ACTION
            && strID == SActionForJS::key())
        return new SActionForJS(this, parent);
    auto iterType = m_pluginInterfaces.find(strType);
    if(iterType == m_pluginInterfaces.end())
        return NULL;
    auto iterInterface = iterType->find(strKey);
    if(iterInterface == iterType->end())
        return NULL;
    return iterInterface.value()->create(strID, this, parent);
}

SObject *SMainWindow::createSObjectForPlugin(const QString &strType, const QString &strKey, SObject *parent)
{
    SObject* pPlugin;
    QString strID = strType + STR_CUT_CHAR + strKey;

    if(strType == STR_ACTION
            && strID == SActionForJS::key()){
        pPlugin = new SObject(parent);
        SActionForJS::initailizeSObject(pPlugin);
        pPlugin->setProperty(STR_PLUGIN, strKey);
        return pPlugin;
    }
    auto iterType = m_pluginInterfaces.find(strType);
    if(iterType == m_pluginInterfaces.end())
        return NULL;
    auto iterInterface = iterType->find(strKey);
    if(iterInterface == iterType->end())
        return NULL;
    pPlugin = iterInterface.value()->sobject(strID, parent);
    pPlugin->setProperty(STR_PLUGIN, strKey);
    return pPlugin;
}

void SMainWindow::createAction()
{
    m_actions[ACT_NEW_INS] = new QAction(QIcon(":/icon/new"), tr("&New"), this);
    connect(m_actions[ACT_NEW_INS], SIGNAL(triggered()), this, SLOT(slotNewProject()));

    m_actions[ACT_OPEN] = new QAction(QIcon(":/icon/open"), tr("&Open"), this);
    connect(m_actions[ACT_OPEN], SIGNAL(triggered()), this, SLOT(slotOpenProject()));

    m_actions[ACT_CLOSE] = new QAction(QIcon(":/icon/close"), tr("&Close"), this);
    connect(m_actions[ACT_CLOSE], SIGNAL(triggered()), this, SLOT(slotCloseProject()));
    m_actions[ACT_CLOSE]->setEnabled(false);

    m_actions[ACT_SAVE] = new QAction(QIcon(":/icon/save"), tr("&Save"), this);
    m_actions[ACT_SAVE]->setEnabled(false);
    connect(m_actions[ACT_SAVE], SIGNAL(triggered()), this, SLOT(slotSave()));

    m_actions[ACT_SAVEAS] = new QAction(tr("Save &As"), this);
    m_actions[ACT_SAVEAS]->setEnabled(false);
    connect(m_actions[ACT_SAVEAS], SIGNAL(triggered()), this, SLOT(slotSaveAs()));

    m_actions[ACT_EXIT] = new QAction(QIcon(":/icon/exit"), tr("&Exit"), this);
    connect(m_actions[ACT_EXIT], SIGNAL(triggered()), this, SLOT(close()));

    m_actions[ACT_MINS] = new QAction(QIcon(":/icon/manage"), tr("&Manage"), this);
    m_actions[ACT_MINS]->setEnabled(false);
    connect(m_actions[ACT_MINS], SIGNAL(triggered()), this, SLOT(slotManageProject()));

    m_actions[ACT_HELP] = new QAction(QIcon(":/icon/info"), tr("&Help"), this);
    connect(m_actions[ACT_HELP], SIGNAL(triggered()), this, SLOT(slotHelp()));

    m_actions[ACT_ABOUT] = new QAction(QIcon(":/icon/info"), tr("&About"), this);
    connect(m_actions[ACT_ABOUT], SIGNAL(triggered()), this, SLOT(slotAboutSw()));

    m_actions[ACT_VIEW_TOOLBAR] = new QAction(tr("&Toolbar"), this);
    m_actions[ACT_VIEW_TOOLBAR]->setCheckable(true);
    m_actions[ACT_VIEW_TOOLBAR]->setChecked(true);
    connect(m_actions[ACT_VIEW_TOOLBAR], SIGNAL(triggered(bool)), this, SLOT(slotShowToolBar(bool)));
}

void SMainWindow::createMenu()
{
    QMenu* pMenu/*,*pSubMenu*/;
    QMenuBar* pMenuBar = menuBar();

    // File
    pMenu = pMenuBar->addMenu(tr("&File"));
    pMenu->addAction(m_actions[ACT_NEW_INS]);
    pMenu->addAction(m_actions[ACT_OPEN]);
    pMenu->addAction(m_actions[ACT_MINS]);
    pMenu->addAction(m_actions[ACT_SAVE]);
    pMenu->addAction(m_actions[ACT_SAVEAS]);
    pMenu->addAction(m_actions[ACT_CLOSE]);
    m_menus[M_RECENT_PRO] = pMenu->addMenu(tr("&Recent projects"));
    slotUpdateFileMenu();
    pMenu->addAction(m_actions[ACT_EXIT]);
    m_menus[M_FILE] = pMenu;

    // View
    pMenu = pMenuBar->addMenu(tr("&View"));
    pMenu->addAction(m_actions[ACT_VIEW_TOOLBAR]);
    m_menus[M_VIEW] = pMenu;

    // Help
    pMenu = pMenuBar->addMenu(tr("&Help"));
    pMenu->addAction(m_actions[ACT_HELP]);
    pMenu->addAction(m_actions[ACT_ABOUT]);
    m_menus[M_HELP] = pMenu;
}

void SMainWindow::createToolbar()
{
    m_toolbars[TB_FILE] = new QToolBar(tr("&File"), this);
    m_toolbars[TB_FILE]->addAction(m_actions[ACT_OPEN]);
    m_toolbars[TB_FILE]->addAction(m_actions[ACT_MINS]);
    m_toolbars[TB_FILE]->addAction(m_actions[ACT_SAVE]);
    m_toolbars[TB_FILE]->addAction(m_actions[ACT_CLOSE]);
    addToolBar(Qt::TopToolBarArea, m_toolbars[TB_FILE]);
}

void SMainWindow::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::warning(this, tr("GMDTTS"),
                            tr("Are you sure to exit the GMDTTS?"),
                            QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes)
        event->accept();
    else event->ignore();
}

void SMainWindow::slotNewProject()
{
    QDialog* dlg = new QDialog(this);
    QHBoxLayout* pLayout = new QHBoxLayout(dlg);
    SProjectTreeWidget* projTree = new SProjectTreeWidget(dlg);
    projTree->setMainWindow(this);
    projTree->slotNewProject();
    pLayout->addWidget(projTree);
    dlg->setLayout(pLayout);
    dlg->setAttribute (Qt::WA_DeleteOnClose);
    dlg->setWindowTitle(tr("New project"));
    dlg->show();
}

void SMainWindow::slotOpenProject(const QString &strFileName)
{
    QString strFileName2;
    strFileName2 = strFileName;
    if(strFileName.isEmpty())
        strFileName2 =  QFileDialog::getOpenFileName(this, "Open GMDTTS File",
                                                     "../examples", "project file(*.sproj)");
    else
        strFileName2 = strFileName;
    if(strFileName2.isEmpty())
        return;
    slotCloseProject(true);
    SProject* proj = new SProject(this);
    if(!proj->openFile(strFileName2)){
        m_project = NULL;
        delete proj;
        return;
    }
    m_project = proj;
    m_actions[ACT_CLOSE]->setEnabled(true);
    m_actions[ACT_SAVE]->setEnabled(true);
    m_actions[ACT_SAVEAS]->setEnabled(true);
    m_actions[ACT_MINS]->setEnabled(true);

    QString strPros;
    QSettings setting;
    setting.beginGroup(MAIN_WND_NAME);
    QStringList lstPro = setting.value(RECENT_PROJECTS).toString().split(',');
    int nIndex = lstPro.indexOf(strFileName2);
    if(nIndex >= 0)
        lstPro.removeAt(nIndex);
    while (lstPro.size() > 5) {
        lstPro.removeLast();
    }
    strPros = strFileName2;
    if(lstPro.size() > 0
            && !lstPro[0].isEmpty()){
        foreach (QString strPro, lstPro) {
            strPros += "," + strPro;
        }
    }
    setting.setValue(RECENT_PROJECTS, strPros);
    setting.endGroup();
    slotUpdateFileMenu();
}

void SMainWindow::slotOpenProject(const SObject *projObj)
{
    slotCloseProject(true);
    m_project = new SProject(this, projObj->clone());
    m_actions[ACT_CLOSE]->setEnabled(true);
    m_actions[ACT_SAVE]->setEnabled(true);
    m_actions[ACT_SAVEAS]->setEnabled(true);
    m_actions[ACT_MINS]->setEnabled(true);
}

void SMainWindow::slotCloseProject(bool bReleaseOld)
{
    if(m_project != NULL
            && bReleaseOld)
        delete m_project;
    m_project = NULL;
    m_actions[ACT_CLOSE]->setEnabled(false);
    m_actions[ACT_SAVE]->setEnabled(false);
    m_actions[ACT_SAVEAS]->setEnabled(false);
    m_actions[ACT_MINS]->setEnabled(false);
    showNormal();
    setWindowTitle("GMDTTS");
}

void SMainWindow::slotSaveAs()
{
    QString strFileName = QFileDialog::getSaveFileName(this, tr("Save as"),
                                                       QString(), tr("SRLS project file(*.sproj)"));
    if(strFileName.isEmpty())
        return;
    SObject* pObj = m_project->projectRoot();
    pObj->setProperty(STR_PATH,strFileName);
    pObj->saveFile(strFileName);
}

void SMainWindow::slotSave()
{
    if(m_project == NULL)
        return;
    SObject* pObj = m_project->projectRoot();
    pObj->saveFile(pObj->property(STR_PATH).toString());
}

void SMainWindow::slotAboutSw()
{
    QMessageBox::about(this, tr("About GMDTTS"), tr("General Motors Diagnostic Testing Tools(GMDTTS) 1.0.0.0\n"
                                                   "Copyrights 2020-2021 YuChi domaincompute company(Hefei) Co.Ltd. "
                                                   "All rights reserved."));
}

void SMainWindow::slotHelp()
{
    if (m_processes[P_HELP]->state() != QProcess::Running) {
        QString app;
#if !defined(Q_OS_MAC)
        app += QLatin1String("./assistant");
#else
        app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#endif

        QStringList args;

        m_processes[P_HELP]->start(app, args);

        if (!m_processes[P_HELP]->waitForStarted()) {
            QMessageBox::critical(0, QObject::tr("Simple Text Viewer"),
                                  QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
        }
    }
}

void SMainWindow::slotManageProject()
{
    QDialog* dlg = new QDialog(this);
    QHBoxLayout* pLayout = new QHBoxLayout(dlg);
    SProjectTreeWidget* projTree = new SProjectTreeWidget(dlg);
    projTree->setMainWindow(this);
    if(m_project)
        projTree->setProjectObj(m_project->projectRoot()->clone());
    pLayout->addWidget(projTree);
    dlg->setLayout(pLayout);
    dlg->setAttribute (Qt::WA_DeleteOnClose);
    dlg->setWindowTitle(tr("Manage project"));
    dlg->show();
}

void SMainWindow::slotUpdateFileMenu()
{
    auto lstActions = m_menus[M_RECENT_PRO]->actions();
    foreach (QAction* pAct, lstActions) {
        delete pAct;
    }
    QAction* pItem;
    QStringList lstPro;
    QSettings setting;
    setting.beginGroup(MAIN_WND_NAME);
    lstPro = setting.value(RECENT_PROJECTS).toString().split(',');
    setting.endGroup();
    foreach (QString strPro, lstPro) {
        if(strPro.isEmpty())
            continue;
        pItem = m_menus[M_RECENT_PRO]->addAction(strPro);
        connect(pItem, SIGNAL(triggered()), SLOT(slotTriRecentProj()));
    }
}

void SMainWindow::slotTriRecentProj()
{
    QAction* pAct = qobject_cast<QAction*>(sender());
    if(pAct == nullptr)
        return;
    slotOpenProject(pAct->text());
}

void SMainWindow::slotShowToolBar(bool show)
{
    auto lstToolbars = findChildren<QToolBar*>();
    foreach (auto tb, lstToolbars) {
        tb->setVisible(show);
    }
}
