#include "sproject.h"
#include "smainwindow.h"
#include "sactionforjs.h"
#include "swidget.h"

SProject::SProject(SMainWindow *mainWindow, SObject *root)
    : QObject(mainWindow),
      m_mainWindow(mainWindow)
{
    mainWindow->m_project = this;
    m_scriptEngine.globalObject().
            setProperty(STR_TYPE_W, m_scriptEngine.newQObject(root));
    m_scriptEngine.globalObject().
            setProperty(STR_TYPE_P, m_scriptEngine.newQObject(this));
    setProjectRoot(root);
}

SProject::~SProject()
{
    QString strInitAction;
    if(m_projectRoot){
        strInitAction = m_projectRoot
                ->findChild<SObject*>(STR_PROROOT)
                ->property(STR_ACT_REL).toString();
        if(!strInitAction.isEmpty()){
            SAction* pInitAct;
            QVariant varAct = resource(STR_ACTION + STR_CUT_CHAR + strInitAction);
            if(varAct.isValid() &&
                    (pInitAct = (SAction*)varAct.value<void*>()) != nullptr){
                pInitAct->call();
                delete pInitAct;
            }
        }
    }

    m_threads[T_ACTION].stop();
    m_threads[T_ACTION].wait();
    for(auto iterItem = m_toolbars.begin();
        iterItem != m_toolbars.end();
        ++iterItem){
        delete iterItem.value();
    }
    for(auto iterItem = m_menus.begin();
        iterItem != m_menus.end();
        ++iterItem){
        delete iterItem.value();
    }
    for(auto iterItem = m_dockWidgets.begin();
        iterItem != m_dockWidgets.end();
        ++iterItem){
        delete iterItem.value();
    }
    for(auto iterItem = m_popWidgets.begin();
        iterItem != m_popWidgets.end();
        ++iterItem){
        delete iterItem.value();
    }
    for(auto iterItem = m_communications.begin();
        iterItem != m_communications.end();
        ++iterItem){
        delete iterItem.value();
    }
    if(m_central){
        delete m_central;
    }
    if(m_statusbar){
        delete m_statusbar;
    }
    if(m_projectRoot){
        delete m_projectRoot;
    }
}

SObject *SProject::projectRoot()
{
    return m_projectRoot;
}

SProject::Communications &SProject::mutableCommunications()
{
    return m_communications;
}

const SProject::Communications &SProject::communications() const
{
    return m_communications;
}

QScriptEngine &SProject::scriptEngine()
{
    return m_scriptEngine;
}

QVariant SProject::resource(const QString &strID)
{
    SObject* pObj[5];
    QStringList lstID = strID.split(CUT_CHAR);
    QVariant varValue;
    if(lstID.size() < 2
            || (pObj[0] = m_projectRoot->findChild<SObject*>(STR_PROROOT)) == nullptr
            || (pObj[1] = pObj[0]->findChild<SObject*>(STR_RES)) == nullptr){
        return varValue;
    }

    if(lstID[0] == STR_ICON){
        if((pObj[2] = pObj[1]->findChild<SObject*>(STR_ICON)) == nullptr){
            return varValue;
        }
        return pObj[2]->property(lstID[1].toUtf8().data());
    }else if(lstID[0] == STR_ACTION){
        if((pObj[2] = pObj[1]->findChild<SObject*>(STR_ACTION)) == nullptr
                || (pObj[3] = pObj[2]->findChild<SObject*>(lstID[1])) == nullptr){
            return varValue;
        }
        SAction* pAction;
        SMainWindow* pMainW = m_mainWindow;
        int nIndex;
        QVariantList lstParam;
        if((pAction = qobject_cast<SAction*>(
                pMainW->createPlugin(STR_ACTION,
                        pObj[3]->property(STR_PLUGIN).toString(), this)))
                == nullptr){
            pAction = new SAction(pMainW, this);
        }
        pAction->setSObject(pObj[3]);

        if(lstID.size() > 2){
            pAction->setFuncName(lstID[2]);
        }
        for(nIndex = 5; nIndex <= lstID.size(); nIndex+=2){
            lstParam.append(SObject::stringToVariant(lstID[nIndex-2],
                            lstID[nIndex-1]));
        }
        if(!lstParam.isEmpty()){
            pAction->setParams(lstParam);
        }
        return QVariant::fromValue((void*)pAction);
    }
    return varValue;
}

QVariantMap SProject::resourceByID(const QString &strID)
{
    if(strID.isEmpty()){
        return QVariantMap();
    }
    QStringList lstIDInfo = strID.split('.');
    auto iterRes = m_resources.find(lstIDInfo[0]);
    if(iterRes == m_resources.end()){
        return QVariantMap();
    }
    if(lstIDInfo.size() > 1){
        QVariantMap mapRes = iterRes.value();
        if(mapRes[STR_TYPE] == STR_SOBJ){
            mapRes[STR_TYPE] = STR_PROP;
            mapRes[STR_PROP] = lstIDInfo[1];
            return mapRes;
        }else{
            return QVariantMap();
        }
    }
    return iterRes.value();
}

bool SProject::_data(const QString &strID, SProject::DataT &dat)
{
    QStringList lstPath;
    SObject* pChild;
    int nIndex, nPathLen;
    QString strProp, strAttr;

    dat.object = nullptr;
    dat.type = dat.attribute = dat.property = "";
    if(strID.isEmpty()){
        return false;
    }
    lstPath = strID.split(CUT_CHAR);
    if(lstPath[0] != STR_OBJ
            && lstPath[0] != STR_PROP
            && lstPath[0] != STR_ATTR){
        lstPath.push_front(STR_PROP);
    }

    if(lstPath.size() < 2)
        return false;
    if(lstPath[0] == STR_OBJ){
        nPathLen = lstPath.size();
    }else if(lstPath[0] == STR_PROP){
        if(lstPath.size() < 3)
            return false;
        nPathLen = lstPath.size() - 1;
        strProp = lstPath.last();
    }else if(lstPath[0] == STR_ATTR){
        if(lstPath.size() < 4)
            return false;
        nPathLen = lstPath.size() - 2;
        strProp = lstPath[lstPath.size() - 2];
        strAttr = lstPath.last();
    }else{
        return false;
    }

    if(m_projectRoot->objectName() != lstPath[1])
        return false;
    pChild = m_projectRoot;
    for(nIndex = 2; nIndex < nPathLen; nIndex++) {
        pChild = pChild->findChild<SObject*>(lstPath[nIndex]);
        if(pChild == NULL)
            return false;
    }
    dat.object = pChild;
    if(lstPath[0] == STR_OBJ){
        dat.type = lstPath[0];
        return true;
    }
    if(!pChild->dynamicPropertyNames().contains(strProp.toUtf8())){
        dat.object = NULL;
        return false;
    }
    dat.property = strProp;
    if(lstPath[0] == STR_PROP){
        dat.type = lstPath[0];
        return true;
    }
    if(!pChild->propertyInfo()[strProp].m_attribute.contains(strAttr)){
        dat.object = NULL;
        dat.property = STR_DEFAULT;
        return false;
    }
    dat.attribute = strAttr;
    dat.type = lstPath[0];
    return true;
}

QObject *SProject::_ui(const QString &strID)
{
    QStringList lstPath;
    int nIndex;
    QObject* pTarget = NULL;

    lstPath = strID.split(CUT_CHAR);
    if(lstPath.size() < 1)
        return NULL;
    if(lstPath[0] == STR_ACTION){
        if(lstPath.size() < 3)
            return NULL;
        QString strPath = lstPath[1];
        for(nIndex = 2; nIndex < lstPath.size(); nIndex++){
            strPath.append(STR_CUT_CHAR + lstPath[nIndex]);
        }
        if(m_actions.contains(strPath))
            return m_actions[strPath];
        return NULL;
    }else if(lstPath[0] == STR_CENTRAL){
        nIndex = 1;
        pTarget = ((SMainWindow*)parent())->centralWidget();
    }else if(lstPath[0] == STR_DOCK){
        if(lstPath.size() < 2)
            return NULL;
        nIndex = 2;
        if(m_dockWidgets.contains(lstPath[1]))
            pTarget = m_dockWidgets[lstPath[1]];
    }else if(lstPath[0] == STR_MENU){
        if(lstPath.size() < 2)
            return NULL;
        nIndex = 2;
        if(m_menus.contains(lstPath[1]))
            pTarget = m_menus[lstPath[1]];
    }else if(lstPath[0] == STR_POPWIDGET){
        if(lstPath.size() < 2)
            return NULL;
        nIndex = 2;
        if(m_popWidgets.contains(lstPath[1]))
            pTarget = m_popWidgets[lstPath[1]];
    }
    QScrollArea* pScroll;
    QString strUID;
    for(; pTarget != NULL && nIndex < lstPath.size(); nIndex++) {
        if((pScroll = qobject_cast<QScrollArea*>(pTarget)) != NULL)
            pTarget = pScroll->widget();
        auto lstChild = pTarget->children();
        pTarget = NULL;
        foreach (QObject* pObj, lstChild) {
            //strUID = pObj->objectName();
            strUID = pObj->property(STR_NAME).toString();
            if(pObj->isWidgetType()
                    && strUID == lstPath[nIndex]){
                pTarget = pObj;
                break;
            }
        }
    }
    return pTarget;
}

SObject *SProject::_globalData()
{
    return m_projectRoot->findChild<SObject*>(STR_PROROOT)->findChild<SObject*>(STR_DATA);
}

void SProject::setProjectRoot(SObject *project)
{
    if(m_projectRoot != NULL
            || project == NULL)
        return;
    if(project->property(STR_TYPE).toString() != STR_PROROOT)
        return;
    SObject* pObjs[10];
    if((pObjs[0] = project->findChild<SObject*>(STR_PROROOT)) == NULL)
        return;
    QString strResID;
    m_scriptEngine.globalObject().setProperty(STR_TYPE_O, m_scriptEngine.newQObject(project));
    SMainWindow* pMainW = (SMainWindow*)parent();
    pMainW->setWindowTitle("GMDTTS : " + pObjs[0]->property(STR_NAME).toString());
    m_projectRoot = project;

    //给STR_PROROOT及其子对象(sobject类型)分配ID，并填充 m_resources 的键为ID，ID的 type 为 STR_SOBJ, value 为 对象地址
    analyseObjectID(project, m_resources, STR_SOBJ);
    // resources
    if((pObjs[1] = pObjs[0]->findChild<SObject*>(STR_RES)) == NULL){
        pObjs[1] = new SObject(pObjs[0]);
        pObjs[1]->setObjectName(STR_RES);
    }
    // icon
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_ICON)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_ICON);
    }
    auto lstIcon = pObjs[2]->children();
    for(auto iterIcon = lstIcon.begin(); iterIcon != lstIcon.end(); iterIcon++){
        pObjs[3] = (SObject*)(*iterIcon);
        strResID = STR_ICON + STR_CUT_CHAR + pObjs[3]->objectName();
        m_resources[strResID][STR_TYPE] = STR_SOBJ;
        m_resources[strResID][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);
    }
    // action
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_ACTION)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_ACTION);
    }
    auto lstAction = pObjs[2]->children();
    for(auto iterAction = lstAction.begin(); iterAction != lstAction.end(); iterAction++){
        pObjs[3] = (SObject*)(*iterAction);
        strResID = STR_ACTION + STR_CUT_CHAR + pObjs[3]->objectName();
        m_resources[strResID][STR_TYPE] = STR_SOBJ;
        m_resources[strResID][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);
    }

    // communication
    if((pObjs[1] = pObjs[0]->findChild<SObject*>(STR_COM)) == NULL){
        pObjs[1] = new SObject(pObjs[0]);
        pObjs[1]->setObjectName(STR_COM);
    }
    SCommunication* pComm;
    QString strCommName;
    auto lstComm = pObjs[1]->children();
    for(auto iterComm = lstComm.begin(); iterComm != lstComm.end(); iterComm++){
        pObjs[2] = (SObject*)(*iterComm);
        strCommName = pObjs[2]->objectName();
        pComm = qobject_cast<SCommunication*>(
                    pMainW->createPlugin(
                        STR_COM,
                        pObjs[2]->property(STR_PLUGIN).toString(), this));
        if(pComm == NULL)
            continue;
        pComm->setSObject(pObjs[2]);
        addResource(pObjs[2], STR_ID, STR_COM, QVariant::fromValue((void*)pComm));
    }

    // ui
    if((pObjs[1] = pObjs[0]->findChild<SObject*>(STR_UI)) == NULL){
        pObjs[1] = new SObject(pObjs[0]);
        pObjs[1]->setObjectName(STR_UI);
    }
    bool bShowToolbar = pObjs[1]->property(STR_TOOLBAR_VISIBLE).toBool();

    // menu & toolbar
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_MENU)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_MENU);
    }
    int toolbarItemCnt;
    SObject* pAMenuObj;
    QMenu* pMenu;
    QToolBar* pToolBar;
    QMenuBar *pMenuBar = pMainW->menuBar();

    auto lstMenuChild = pObjs[2]->children();
    for(auto iterChild = lstMenuChild.begin(); iterChild != lstMenuChild.end(); iterChild++){
        pAMenuObj = (SObject*)(*iterChild);
        pMenu = new QMenu(pAMenuObj->objectName(), pMainW);
        pMenu->setProperty(STR_NAME, pAMenuObj->objectName());
        pToolBar = new QToolBar(pAMenuObj->objectName(), pMainW);
        toolbarItemCnt = 0;
        constructSubmenu(pAMenuObj, pMenu, pToolBar, toolbarItemCnt, pAMenuObj->objectName());
        if(toolbarItemCnt > 0){
            pMainW->addToolBar(Qt::TopToolBarArea, pToolBar);
            m_toolbars[pAMenuObj->objectName()] = pToolBar;
            if(!bShowToolbar){
                pToolBar->hide();
            }
        }else
            delete pToolBar;
        m_menus[pAMenuObj->objectName()] = pMenu;
        pMenuBar->insertMenu(pMainW->menu(SMainWindow::M_HELP)->menuAction(), pMenu);
        addResource(pObjs[2], STR_ID, STR_MENU, QVariant::fromValue((void*)pMenu));
    }

    if(bShowToolbar){
        pMainW->toolbar(SMainWindow::TB_FILE)->show();
    }else{
        pMainW->toolbar(SMainWindow::TB_FILE)->hide();
    }
    // central pane
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_CENTRAL)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_CENTRAL);
    }
    if(!pObjs[2]->children().isEmpty()){
        pObjs[3] = (SObject*)pObjs[2]->children().first(); //仅加载第一个窗口
        m_central = constructWidget(pObjs[3], pMainW);
        pMainW->setCentralWidget(m_central);
        //m_central->show();
    }
    // dock pane
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_DOCK)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_DOCK);
    }
    int nArea;
    QString strTitle;
    QDockWidget* pNewDockW,*pBeforeDock,*pFirstDock;
    QWidget* pWidget;
    QMap<QString, QMap<int, QDockWidget*> > mapWidgets;
    auto lstDockChild = pObjs[2]->children();
    QMenu* pViewMenu = pMainW->menu(SMainWindow::M_VIEW);
    for(auto iterChild = lstDockChild.begin(); iterChild != lstDockChild.end(); iterChild++){
        pObjs[3] = (SObject*)(*iterChild);
        pNewDockW = new QDockWidget(pObjs[3]->objectName(), pMainW);
        pNewDockW->setProperty(STR_NAME, pObjs[3]->objectName());
        pWidget = constructWidget(pObjs[3], pNewDockW);
        if(pWidget == NULL)
            continue;
        pWidget->setProperty(STR_NAME, STR_WIDGET);
        pNewDockW->setWidget(pWidget);
        pNewDockW->setProperty(STR_PTR_W, QVariant::fromValue(pWidget));
        pNewDockW->setFeatures(QDockWidget::DockWidgetMovable | pNewDockW->features());
        if(!pObjs[3]->property("_ui_closable").toBool()){
            pNewDockW->setFeatures(pNewDockW->features() & (~QDockWidget::DockWidgetClosable));
        }
        pNewDockW->setFloating(pObjs[3]->property("_ui_floating").toBool());
        strTitle = pObjs[3]->property(STR_UI_TITLE).toString();
        if(strTitle.isEmpty()){
            pNewDockW->setTitleBarWidget(NULL);
        }else{
            pNewDockW->setWindowTitle(strTitle);
        }
        m_dockWidgets[pObjs[3]->objectName()] = pNewDockW;

        addResource(pObjs[3], STR_ID, STR_DOCK, QVariant::fromValue((void*)pNewDockW));
        mapWidgets[pObjs[3]->property("_ui_area").toString()][pObjs[3]->property("_ui_index").toInt()]
                = pNewDockW;
        if(!pObjs[3]->property(STR_UI_VISIBLE).toBool()){
            pNewDockW->hide();
        }else{
            pNewDockW->show();
        }
    }
    for(auto iterDocks = mapWidgets.begin(); iterDocks != mapWidgets.end(); iterDocks++){
        if(iterDocks.key() == STR_RIGHT){
            nArea = Qt::RightDockWidgetArea;
        }else if(iterDocks.key() == "top"){
            nArea = Qt::TopDockWidgetArea;
        }else if(iterDocks.key() == "bottom"){
            nArea = Qt::BottomDockWidgetArea;
        }else{
            nArea = Qt::LeftDockWidgetArea;
        }
        pBeforeDock = pFirstDock = NULL;
        for(auto iterDock = iterDocks.value().begin(); iterDock != iterDocks.value().end(); iterDock++){
            pNewDockW = iterDock.value();
            pMainW->addDockWidget((Qt::DockWidgetArea)nArea, pNewDockW);
            if(pBeforeDock != NULL)
                pMainW->tabifyDockWidget(pBeforeDock, pNewDockW);
            pBeforeDock = pNewDockW;
            if(pFirstDock == NULL)
                pFirstDock = pNewDockW;
            pViewMenu->addAction(pNewDockW->toggleViewAction());
        }
        if(pFirstDock == NULL)
            pFirstDock->raise();
    }

    // status bar
    if((pObjs[2] = pObjs[1]->findChild<SObject*>("statusbar")) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName("statusbar");
    }

    QMap<bool, QMap<int, QWidget*> > mapStatusItems;
    QStatusBar* pStatusBar = pMainW->statusBar();
    if(pStatusBar == NULL){
        pStatusBar = new QStatusBar(pMainW);
    }
    auto lstStatusItem = pObjs[2]->children();
    for(auto iterChild = lstStatusItem.begin(); iterChild != lstStatusItem.end(); iterChild++){
        pObjs[3] = (SObject*)(*iterChild);
        pWidget = constructWidget(pObjs[3], pStatusBar);
        pWidget->setProperty(STR_NAME, pObjs[3]->objectName());
        pWidget->setProperty("__stretch", pObjs[3]->property("_ui_stretch").toInt());
        mapStatusItems[pObjs[3]->property("_ui_left").toBool()][pObjs[3]->property("_ui_index").toInt()]
                = pWidget;
    }
    for(auto iterItem1 = mapStatusItems.begin(); iterItem1 != mapStatusItems.end(); iterItem1++){
        for(auto iterItem2 = iterItem1->begin(); iterItem2 != iterItem1->end(); iterItem2++){
            if(iterItem1.key()){
                pStatusBar->addWidget(iterItem2.value(),
                                               iterItem2.value()->property("__stretch").toInt());
            }else{
                pStatusBar->addPermanentWidget(iterItem2.value(),
                                               iterItem2.value()->property("__stretch").toInt());
            }
        }
    }
    if(!pObjs[1]->dynamicPropertyNames().contains("show_statusbar")){
        pObjs[1]->setProperty("show_statusbar",true);
    }
    if(pObjs[1]->property("show_statusbar").toBool())
        pStatusBar->show();
    else
        pStatusBar->hide();
    if(mapStatusItems.isEmpty()){
        delete pStatusBar;
        pStatusBar = NULL;
    }
    m_statusbar = pStatusBar;
    addResource(pObjs[2], STR_ID, "statusbar", QVariant::fromValue((void*)pStatusBar));
    // m_STR_PROROOT = STR_PROROOT;
    m_projectRoot->setParent(this);
    project->setAsRoot(true);

    // regesiter all of the resources to the javascript engeene
    QString strResType;
    for (auto iterRes = m_resources.begin(); iterRes != m_resources.end(); iterRes++) {
        strResType = iterRes.value()[STR_TYPE].toString();
        m_scriptEngine.globalObject().setProperty(iterRes.key(),
                                                  m_scriptEngine.newQObject((QObject*)iterRes.value()[STR_VALUE].value<void*>()));
        qDebug() << __FUNCTION__ << __LINE__ << iterRes.key() << strResType << (QObject*)iterRes.value()[STR_VALUE].value<void*>();
    }

    //show
    QStringList lstPos = pObjs[1]->property(STR_UI_SHOW).toString().split(',');
    if(lstPos.size() >= 4){
        pMainW->setGeometry(lstPos[0].toInt(), lstPos[1].toInt(), lstPos[2].toInt(), lstPos[3].toInt());
    }else if(lstPos.size() >= 2){
        pMainW->resize(lstPos[0].toInt(), lstPos[1].toInt());
    }else if(lstPos.size() >= 1){
        if(lstPos[0] == STR_MAX){
            pMainW->showMaximized();
        }else if(lstPos[0] == STR_MIN){
            pMainW->showMinimized();
        }else if(lstPos[0] == STR_FULL){
            pMainW->showFullScreen();
        }else{
            pMainW->showNormal();
        }
    }else{
        pMainW->showNormal();
    }


    // app icon
    QVariant varAppIcon;
    QString strAppIcon = pObjs[0]->property(STR_ICON).toString();
    if(!strAppIcon.isEmpty()
            && (varAppIcon = resource(STR_ICON + STR_CUT_CHAR + strAppIcon)).isValid()){
        pMainW->setWindowIcon(QIcon(varAppIcon.value<QPixmap>()));
    }

    // timer
    uint nTimerInterval = 1;
    nTimerInterval = pObjs[0]->property(STR_TIMER_INTER).toUInt();
    if(nTimerInterval < 1)
        nTimerInterval = 1;
    pObjs[0]->setProperty(STR_TIMER_COUNT, 0);
    m_timerID[TID_TIMER_ACTION] = startTimer((int)nTimerInterval);

    // action thread
    m_threads[T_ACTION].setUserFunction(actionThread);
    m_threads[T_ACTION].setUserParam(this);
    m_threads[T_ACTION].start();

    // init action
    QString strInitAction;
    strInitAction = pObjs[0]->property(STR_ACT_INIT).toString();
    if(!strInitAction.isEmpty()){
        SAction* pInitAct;
        QVariant varAct = resource(STR_ACTION + STR_CUT_CHAR + strInitAction);
        if(varAct.isValid()
                && (pInitAct = (SAction*)varAct.value<void*>()) != NULL){
            pInitAct->call();
            delete pInitAct;
        }
    }
}

bool SProject::openFile(const QString &strFileName)
{
    SObject *pIns = new SObject();
    if(!pIns->openFile(strFileName)){
        delete pIns;
        return false;
    }
    pIns->setProperty("path", strFileName);
    setProjectRoot(pIns);
    return true;
}

void SProject::addResource(QObject *pSrcObj,
                           const char *szIDName,
                           const QString &strType,
                           const QVariant &value,
                           bool bInstance)
{
    QString strID = assignID(pSrcObj, szIDName, strType, m_resources, bInstance);
    m_resources[strID][STR_TYPE] = strType;
    m_resources[strID][STR_VALUE] = value;
}

bool SProject::copyProperty(const QString &strFrom, const QString &strTo)
{
    DataT dtFrom,dtTo;
    auto fFind = [&](DataT& dat, const QString &strPath)->bool{
        _data(strPath, dat);
        if(dat.type != "property"){
            QVariantMap mapInfo;
            mapInfo = resourceByID(strPath);
            if(mapInfo.isEmpty()
                    || mapInfo[STR_TYPE] != "property")
                return false;
            dat.object = (SObject*)mapInfo[STR_VALUE].value<void*>();
            dat.property = mapInfo["property"].toString();
        }
        return true;
    };

    if(fFind(dtFrom, strFrom)
            && fFind(dtTo, strTo)){
        dtTo.object->setProperty(
                    dtTo.property.toUtf8().data(),
                    dtFrom.object->property(dtFrom.property.toUtf8().data()));
        return true;
    }else
        return false;
}

bool SProject::addTimerAction(const QString &strID)
{
    bool bRes = false;
    if(!strID.isEmpty()){
        m_timerActions.lockForWrite();
        if(!m_timerActions->contains(strID)){
            SAction* pAct;
            QVariant varAct;
            if((varAct = resource("action@"+strID)).isValid()
                    && (pAct = (SAction*)varAct.value<void*>()) != NULL){
                m_timerActions->insert(strID, pAct);
                bRes = true;
            }
        }else
            bRes = true;
        m_timerActions.unlock();
    }
    return bRes;
}

void SProject::removeTimerAction(const QString &strID)
{
    if(!strID.isEmpty()){
        m_timerActions.lockForWrite();
        auto iterAct = m_timerActions->find(strID);
        if(iterAct != m_timerActions->end()){
            delete iterAct.value();
            m_timerActions->erase(iterAct);
        }
        m_timerActions.unlock();
    }
}

bool SProject::addThreadAction(const QString &strID)
{
    bool bRes = false;
    if(!strID.isEmpty()){
        m_threadActions.lockForWrite();
        if(!m_threadActions->contains(strID)){
            SAction* pAct;
            QVariant varAct;
            if((varAct = resource("action@"+strID)).isValid()
                    && (pAct = (SAction*)varAct.value<void*>()) != NULL){
                m_threadActions->insert(strID, pAct);
                bRes = true;
            }
        }else
            bRes = true;
        m_threadActions.unlock();
    }
    return bRes;
}

void SProject::removeThreadACtion(const QString &strID)
{
    if(!strID.isEmpty()){
        m_threadActions.lockForWrite();
        auto iterAct = m_threadActions->find(strID);
        if(iterAct != m_threadActions->end()){
            delete iterAct.value();
            m_threadActions->erase(iterAct);
        }
        m_threadActions.unlock();
    }
}

bool SProject::popWidget(const QString &strID,
                         bool bMode,
                         int x, int y,
                         Qt::WindowFlags flags)
{
    SObject* pObjs[5];
    QDialog* pDialog = NULL;

    if(m_popWidgets.contains(strID)){
        pDialog = m_popWidgets[strID];
    }else if((pObjs[0] = m_projectRoot->findChild<SObject*>(STR_PROROOT)) != NULL
            && (pObjs[1] = pObjs[0]->findChild<SObject*>("pop_widget")) != NULL
            && (pObjs[2] = pObjs[1]->findChild<SObject*>(strID)) != NULL){
        pDialog = (QDialog*)constructWidget(pObjs[2], (SMainWindow*)parent());
        m_popWidgets[strID] = pDialog;
        //init action
        QString strInitAction;
        strInitAction = pObjs[2]->property("ui_init_action").toString();
        if(!strInitAction.isEmpty()){
            SAction* pInitAct;
            QVariant varAct = resource("action@"+strInitAction);
            if(varAct.isValid()
                    && (pInitAct = (SAction*)varAct.value<void*>()) != NULL){
                pInitAct->call();
                delete pInitAct;
            }
        }
    }
    if(pDialog){
        if(bMode){
            pDialog->exec();
        }else{
            //Qt::WindowFlags flags=Qt::Dialog;
            flags |=Qt::CustomizeWindowHint;
            pDialog->setWindowFlags(flags);
            if((x!=16777215) && (y!=16777215))
                pDialog->move(x,y);
            pDialog->show();
        }
        return true;
    }
    return false;
}

void SProject::showWidget(const QString &strID,
                          bool bMode, int x, int y,
                          Qt::WindowFlags flags)
{
    static bool bShow=false;
    if(bShow){
        if(m_popWidgets.contains(strID))
            m_popWidgets[strID]->hide();
        bShow=false;
    }else{
        popWidget(strID, bMode, x, y, flags);
        bShow=true;
    }
}

int SProject::actionThread(void *pParam, const bool &bRunning)
{
    //int nIndex;
    SAction* pAction;
    SActionForJS* pJsAct;
    //SObject* pObjs[5];
    QScriptEngine scriptEngine;
    SProject* pProj = (SProject*)pParam;
    scriptEngine.globalObject().setProperty("_w", scriptEngine.newQObject(pProj->parent()));
    scriptEngine.globalObject().setProperty("_p", scriptEngine.newQObject(pProj));
    scriptEngine.globalObject().setProperty("_obj", scriptEngine.newQObject(pProj->projectRoot()));

    while(bRunning){
        pProj->m_threadActions.lockForWrite();
        if(pProj->m_threadActions->isEmpty()){
            pProj->m_threadActions.unlock();
            QThread::msleep(100);
            continue;
        }
        for(auto iterAction = pProj->m_threadActions->begin();
            iterAction != pProj->m_threadActions->end();
            iterAction++){
            pAction = iterAction.value();
            if((pJsAct = qobject_cast<SActionForJS*>(pAction)) != NULL)
                pJsAct->call(/*&scriptEngine*/);
            else
                pAction->call();
            //emit iterAction.value()->triggered();
        }
        pProj->m_threadActions.unlock();
    }
    return 0;
}

void SProject::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timerID[TID_TIMER_ACTION]){
        SObject* pIns = m_projectRoot->findChild<SObject*>(STR_PROROOT);
        pIns->setProperty("timer_counter", pIns->property("timer_counter").toUInt() + 1);
        m_timerActions.lockForRead();
        for(auto iterAction = m_timerActions->begin(); iterAction != m_timerActions->end(); iterAction++){
            emit iterAction.value()->triggered();
        }
        m_timerActions.unlock();
    }
}

QScriptValue SProject::ui(const QString &strID)
{
    QObject * pChild =  _ui(strID);
    if(pChild == NULL)
        return QScriptValue();
    return m_scriptEngine.newQObject(pChild);
}

QScriptValue SProject::data(const QString &strID)
{
    DataT dat;
    if(!_data("object@" + strID, dat))
        return QScriptValue();
    return m_scriptEngine.newQObject(dat.object);
}

QScriptValue SProject::js(const QString &strCode)
{
    QScriptValue val = m_scriptEngine.evaluate(strCode);
    if (m_scriptEngine.hasUncaughtException()){
       qDebug()<<m_scriptEngine.uncaughtException().toString();
       qDebug()<<m_scriptEngine.uncaughtExceptionBacktrace().join("\n");
    }
    return val;
}

QScriptValue SProject::globalData()
{
    return m_scriptEngine.newQObject(_globalData());
}

QScriptValue SProject::communication(const QString &strID)
{
    if(m_communications.contains(strID))
        return m_scriptEngine.newQObject(m_communications[strID]);
    return QScriptValue();
}

void SProject::constructSubmenu(SObject *src, QMenu *pMenuParent, QToolBar *pToolBar, int &toolbarItemCnt, const QString &strParentPath)
{
    QMenu* pMenu;
    SObject* pAMenuObj;
    SAction* pAction;
    QString strName,strResID;
    auto lstChild = src->children();
    for(auto iterChild = lstChild.begin(); iterChild != lstChild.end(); iterChild++){
        pAMenuObj = (SObject*)(*iterChild);
        strName = pAMenuObj->objectName();
        if(pAMenuObj->children().isEmpty()){
            QVariant varAction;
            QString strAction = pAMenuObj->property(STR_ACTION).toString();
            if(strAction.isEmpty()
                    || (varAction = resource(QStringLiteral(STR_ACTION) + STR_CUT_CHAR + strAction)).isNull()){
                pAction = new SAction((SMainWindow*)parent(), this);
            }else{
                pAction = (SAction*)varAction.value<void*>();
            }
            pAction->setText(strName);
            if(!pAMenuObj->property(STR_ENABLE).toBool())
                pAction->setEnabled(false);
            if(pAMenuObj->property(STR_TOOLBAR).toBool()){
                pToolBar->addAction(pAction);
                toolbarItemCnt++;
            }
            pMenuParent->addAction(pAction);
            m_actions[strParentPath + STR_CUT_CHAR + strName] = pAction;
            addResource(pAMenuObj, STR_ID, STR_ACTION, QVariant::fromValue((void*)pAction));
        }else{
            pMenu = new QMenu(pAMenuObj->objectName(), pMenuParent);
            pMenu->setProperty(STR_NAME, pAMenuObj->objectName());
            constructSubmenu(pAMenuObj, pMenu, pToolBar, toolbarItemCnt, strParentPath + STR_CUT_CHAR + pAMenuObj->objectName());
            pMenuParent->addMenu(pMenu);
            addResource(pAMenuObj, STR_ID, STR_MENU, QVariant::fromValue((void*)pMenu));
        }
    }
}

QWidget *SProject::constructWidget(SObject *src, QWidget *pParent)
{
    SWidget* pSWidget;
    QWidget* pNewWidget;
    SMainWindow* pMainW = (SMainWindow*)parent();
    QString strType,strClass;
    strType = src->property(STR_INS_TYPE).toString();

    if(strType.compare(STR_INSTANCE, Qt::CaseInsensitive) == 0){
        if((pSWidget = qobject_cast<SWidget*>(
                pMainW->createPlugin(STR_WIDGET, src->property(STR_PLUGIN).toString(), pParent))) == NULL)
            return NULL;
        pSWidget->setSObject(src); // 启动插件功能
        pSWidget->setProperty(STR_NAME, src->objectName());
        pNewWidget = pSWidget;
        addResource(src, STR_ID, STR_WIDGET, QVariant::fromValue((void*)pNewWidget));
    }else if(strType.compare(STR_CONTAINER, Qt::CaseInsensitive) == 0){
        QWidget* pLayoutWidget;
        QWidget* pChildW[5];
        SObject* pObj[5];
        QVector<QWidget*> vecChild;

        strClass = src->property(STR_UI_CLASS).toString();

        // construct container
        if(strClass == STR_GROUP){
            QString strAlign;
            QGroupBox* pGroupBox = new QGroupBox(src->property(STR_UI_TITLE).toString(), pParent);
            strAlign = src->property(STR_UI_ALIGN).toString();
            if(strAlign == STR_ALIGN_LEFT){
                pGroupBox->setAlignment(Qt::AlignLeft); //标题位置
            }else if(strAlign == STR_ALIGN_RIGHT){
                pGroupBox->setAlignment(Qt::AlignRight);
            }else if(strAlign == STR_ALIGN_CENTER){
                pGroupBox->setAlignment(Qt::AlignHCenter);
            }
            pLayoutWidget = pNewWidget = pGroupBox;
        }else if(strClass == STR_SCROLL){
            QScrollArea* pScrollW = new QScrollArea(pParent);
            QWidget* pScrollContainer = new QWidget(pScrollW);
            pScrollW->setWidget(pScrollContainer);
            pScrollW->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            pScrollW->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            pScrollW->setWidgetResizable(true);
            pScrollW->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            pScrollW->setWidget(pScrollContainer);
            pNewWidget = pScrollW;
            pLayoutWidget = pScrollContainer;
        }else if(strClass == STR_WIDGET){
            pLayoutWidget = pNewWidget = new QWidget(pParent);
        }else if(strClass == STR_DIALOG){
            pLayoutWidget = pNewWidget = new QDialog(pParent);
            pNewWidget->setWindowTitle(src->property(STR_UI_TITLE).toString());
        }else if(strClass == STR_SPLIT){
            QSplitter* pSplitter = new QSplitter(pParent);
            pSplitter->setOrientation(src->property(STR_UI_ORIENTATION).toString() == STR_HORIZONTAL ? Qt::Horizontal : Qt::Vertical);
            pLayoutWidget = pNewWidget = pSplitter;
        }else if(strClass == STR_TAB){
            QString strAlign;
            QTabWidget* pTabW = new QTabWidget(pParent);
            pTabW->setTabsClosable(src->property(STR_UI_CLOSE).toBool());
            pTabW->setMovable(true);
            strAlign = src->property(STR_UI_ALIGN).toString();
            if(strAlign == STR_ALIGN_NORTH){
                pTabW->setTabPosition(QTabWidget::North);
            }else if(strAlign == STR_ALIGN_SOUTH){
                pTabW->setTabPosition(QTabWidget::South);
            }else if(strAlign == STR_ALIGN_WEST){
                pTabW->setTabPosition(QTabWidget::West);
            }else if(strAlign == STR_ALIGN_EAST){
                pTabW->setTabPosition(QTabWidget::East);
            }
            pLayoutWidget = pNewWidget = pTabW;
        }
        pNewWidget->setProperty(STR_NAME, src->objectName());

        addResource(src, STR_ID, STR_CONTAINER, QVariant::fromValue((void*)pNewWidget));
         // construct children
        auto lstChild = src->children();
        for(auto iterChild = lstChild.begin(); iterChild != lstChild.end(); iterChild++){
            pObj[0] = (SObject*)(*iterChild);
            if((pChildW[0] = constructWidget(pObj[0], pLayoutWidget)) == NULL)
                continue;
            pChildW[0]->setProperty(STR_NAME, pObj[0]->objectName());
            pChildW[0]->setProperty(STR_PTR_OBJ, QVariant::fromValue((void*)pObj[0]));
            vecChild.append(pChildW[0]);
        }

        // layout param
        QStringList lstPos,lstLayoutParam[3];
        QString strLayout;
        QList<int> lstColStretch,lstRowStretch;
        strLayout = src->property(STR_UI_LAYOUT).toString();
        lstLayoutParam[0] = src->property(STR_UI_LAYOUT_P).toString().split(';');

        foreach (QString strLevel0, lstLayoutParam[0]) {
            if(strLevel0.isEmpty())
                continue;
            lstLayoutParam[1] = strLevel0.split(':');
            if(lstLayoutParam[1].size() < 2)
                continue;
            lstLayoutParam[2] = lstLayoutParam[1][1].split(',');
            if(lstLayoutParam[1][0] == STR_LAYOUT_COL){
                foreach (QString strLevel2, lstLayoutParam[2]) {
                    lstColStretch.append(strLevel2.toInt());
                }
            }else if(lstLayoutParam[1][0] == STR_LAYOUT_ROW){
                foreach (QString strLevel2, lstLayoutParam[2]) {
                    lstRowStretch.append(strLevel2.toInt());
                }
            }
        }

        if(strClass == STR_GROUP
                || strClass == STR_SCROLL
                || strClass == STR_WIDGET
                || strClass == STR_DIALOG){
            // layout
            if(strLayout.compare(STR_LAYOUT_GRID, Qt::CaseInsensitive) == 0){
                QGridLayout* pLayout = new QGridLayout(pLayoutWidget);
                //pLayout->setSpacing(5);
                for(auto iterChild = vecChild.begin(); iterChild != vecChild.end(); iterChild++){
                    pChildW[0] = *iterChild;
                    pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                    lstPos = pObj[0]->property(STR_UI_LAYOUT).toString().split(',');
                    while(lstPos.size() < 5)
                        lstPos << "0";
                    // pChildW[0]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                    pLayout->addWidget(pChildW[0], lstPos[0].toInt(), lstPos[1].toInt(), lstPos[2].toInt(),
                            lstPos[3].toInt(), (Qt::AlignmentFlag) lstPos[4].toInt());
                }
                foreach (int nItem, lstColStretch) {
                    pLayout->setColumnStretch(nItem, 1);
                }
                foreach (int nItem, lstRowStretch) {
                    pLayout->setRowStretch(nItem, 1);
                }
//                pLayout->setMargin(0);
//                pLayout->setSpacing(0);
            }else if(strLayout == STR_LAYOUT_VBOX){
                QVBoxLayout* pLayout = new QVBoxLayout(pLayoutWidget);
                QList<QWidget*> lstChild;
                int nIndex,nIndex2;
                for(auto iterChild = vecChild.begin(); iterChild != vecChild.end(); iterChild++){
                    pChildW[0] = *iterChild;
                    pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                    lstPos = pObj[0]->property(STR_UI_LAYOUT).toString().split(',');
                    if(lstPos.size() < 1)
                        nIndex = 0;
                    else
                        nIndex = lstPos[0].toInt();
                    auto iterChild2 = lstChild.begin();
                    for(; iterChild2 != lstChild.end(); iterChild2++){
                        pChildW[1] = *iterChild2;
                        pObj[1] = (SObject*)pChildW[1]->property(STR_PTR_OBJ).value<void*>();
                        lstPos = pObj[1]->property(STR_UI_LAYOUT).toString().split(',');
                        if(lstPos.size() < 1)
                            nIndex2 = 0;
                        else
                            nIndex2 = lstPos[0].toInt();
                        if(nIndex < nIndex2){
                            lstChild.insert(iterChild2, pChildW[0]);
                            break;
                        }
                    }
                    if(iterChild2 == lstChild.end())
                        lstChild.append(pChildW[0]);
                    pLayout->setMargin(0);
                    pLayout->setSpacing(0);
                }
                for(auto iterChild = lstChild.begin(); iterChild != lstChild.end(); iterChild++){
                    pChildW[0] = *iterChild;
                    pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                    lstPos = pObj[0]->property(STR_UI_LAYOUT).toString().split(',');
                    if(lstPos.size() < 2)
                        nIndex = 0;
                    else
                        nIndex = lstPos[1].toInt();
                    if(lstPos.size() < 3)
                        nIndex2 = 0;
                    else
                        nIndex2 = lstPos[2].toInt();
                    pLayout->addWidget(pChildW[0], nIndex, (Qt::AlignmentFlag)nIndex2);
                }
                foreach (int nItem, lstRowStretch) {
                    pLayout->setStretch(nItem, 1);
                }
            }else if(strLayout.compare(STR_LAYOUT_HBOX, Qt::CaseInsensitive) == 0){
                QHBoxLayout* pLayout = new QHBoxLayout(pLayoutWidget);
                QList<QWidget*> lstChild;
                int nIndex,nIndex2;
                for(auto iterChild = vecChild.begin(); iterChild != vecChild.end(); iterChild++){
                    pChildW[0] = *iterChild;
                    pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                    lstPos = pObj[0]->property(STR_UI_LAYOUT).toString().split(',');
                    if(lstPos.size() < 1)
                        nIndex = 0;
                    else
                        nIndex = lstPos[0].toInt();
                    auto iterChild2 = lstChild.begin();
                    for(; iterChild2 != lstChild.end(); iterChild2++){
                        pChildW[1] = *iterChild2;
                        pObj[1] = (SObject*)pChildW[1]->property(STR_PTR_OBJ).value<void*>();
                        lstPos = pObj[1]->property(STR_UI_LAYOUT).toString().split(',');
                        if(lstPos.size() < 1)
                            nIndex2 = 0;
                        else
                            nIndex2 = lstPos[0].toInt();
                        if(nIndex < nIndex2){
                            lstChild.insert(iterChild2, pChildW[0]);
                            break;
                        }
                    }
                    if(iterChild2 == lstChild.end())
                        lstChild.append(pChildW[0]);
                }
                for(auto iterChild = lstChild.begin(); iterChild != lstChild.end(); iterChild++){
                    pChildW[0] = *iterChild;
                    pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                    lstPos = pObj[0]->property(STR_UI_LAYOUT).toString().split(',');
                    if(lstPos.size() < 2)
                        nIndex = 0;
                    else
                        nIndex = lstPos[1].toInt();
                    if(lstPos.size() < 3)
                        nIndex2 = 0;
                    else
                        nIndex2 = lstPos[2].toInt();
                    pLayout->addWidget(pChildW[0], nIndex, (Qt::AlignmentFlag)nIndex2);
                }
                foreach (int nItem, lstColStretch) {
                    pLayout->setStretch(nItem, 1);
                }
            }else if(strLayout.compare(STR_LAYOUT_FORM, Qt::CaseInsensitive) == 0){
                QFormLayout* pLayout = new QFormLayout(pLayoutWidget);
                QMap<int, QMap<int, QWidget*> > mapChild;
                int nIndex,nIndex2;
                for(auto iterChild = vecChild.begin(); iterChild != vecChild.end(); iterChild++){
                    pChildW[0] = *iterChild;
                    pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                    lstPos = pObj[0]->property(STR_UI_LAYOUT).toString().split(',');
                    if(lstPos.size() < 1)
                        nIndex = 0;
                    else
                        nIndex = lstPos[0].toInt();
                    if(lstPos.size() < 2)
                        nIndex2 = 0;
                    else
                        nIndex2 = lstPos[1].toInt();
                    mapChild[nIndex][nIndex2] = pChildW[0];
                }
                for(auto iterChildren = mapChild.begin(); iterChildren != mapChild.end(); iterChildren++){
                    pChildW[0] = pChildW[1] = NULL;
                    if(iterChildren->size() < 2){
                        if(iterChildren->begin().key() == 0){
                            pChildW[0] = iterChildren->begin().value();
                        }else{
                            pChildW[1] = iterChildren->begin().value();
                        }
                    }else{
                        pChildW[0] = iterChildren->begin().value();
                        pChildW[1] = (iterChildren->begin() + 1).value();
                    }
                    pLayout->insertRow(iterChildren.key(), pChildW[0], pChildW[1]);
                }
            }else if(strLayout.compare(STR_LAYOUT_STACK, Qt::CaseInsensitive) == 0){
                QStackedLayout* pLayout = new QStackedLayout(pLayoutWidget);
                QList<QWidget*> lstChild;
                int nIndex,nIndex2;
                for(auto iterChild = vecChild.begin(); iterChild != vecChild.end(); iterChild++){
                    pChildW[0] = *iterChild;
                    pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                    nIndex = pObj[0]->property(STR_UI_LAYOUT).toString().toInt();
                    auto iterChild2 = lstChild.begin();
                    for(; iterChild2 != lstChild.end(); iterChild2++){
                        pChildW[1] = *iterChild2;
                        pObj[1] = (SObject*)pChildW[1]->property(STR_PTR_OBJ).value<void*>();
                        nIndex2 = pObj[1]->property(STR_UI_LAYOUT).toString().toInt();
                        if(nIndex < nIndex2){
                            lstChild.insert(iterChild2, pChildW[0]);
                            break;
                        }
                    }
                    if(iterChild2 == lstChild.end())
                        lstChild.append(pChildW[0]);
                }
                for(auto iterChild = lstChild.begin(); iterChild != lstChild.end(); iterChild++){
                    pLayout->addWidget(*iterChild);
                }
            }else if(strLayout.compare(STR_LAYOUT_GEOMETRY, Qt::CaseInsensitive) == 0){
                for(auto iterChild = vecChild.begin(); iterChild != vecChild.end(); iterChild++){
                    pChildW[0] = *iterChild;
                    pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                    lstPos = pObj[0]->property(STR_UI_LAYOUT).toString().split(',');
                    while(lstPos.size() < 5)
                        lstPos << "0";
                    pChildW[0]->setGeometry(lstPos[0].toInt(), lstPos[1].toInt(), lstPos[2].toInt(), lstPos[3].toInt());
                    //pNewWidget->resize();
                }
            }

        }else if(strClass == STR_SPLIT){
            QSplitter* pSplitter = (QSplitter*)pNewWidget;
            for(auto iterChild = vecChild.begin(); iterChild != vecChild.end(); iterChild++){
                pChildW[0] = *iterChild;
                pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                pSplitter->insertWidget(pObj[0]->property(STR_UI_LAYOUT).toString().toInt(), pChildW[0]);
            }
        }else if(strClass == STR_TAB){
            QTabWidget* pTabW = (QTabWidget*)pNewWidget;
            for(auto iterChild = vecChild.begin(); iterChild != vecChild.end(); iterChild++){
                pChildW[0] = *iterChild;
                pObj[0] = (SObject*)pChildW[0]->property(STR_PTR_OBJ).value<void*>();
                pTabW->insertTab(
                            pObj[0]->property(STR_UI_LAYOUT).toString().toInt(),
                        pChildW[0],
                        pObj[0]->property(STR_UI_TITLE).toString()
                            );
            }
        }
    }else{
        pNewWidget = new QWidget(pParent);
    }

    QVariant varBkgImage;
    QStringList lstBkgInfo;
    if(!(lstBkgInfo = src->property(STR_UI_BACKIMG).toString().split(',')).isEmpty()
            && !lstBkgInfo[0].isEmpty()
            && (varBkgImage = resource(QString(STR_ICON) + STR_CUT_CHAR + lstBkgInfo[0])).isValid()){
        QPalette palette = pNewWidget->palette();
        QPixmap pix = varBkgImage.value<QPixmap>();
        palette.setBrush(QPalette::Background, QBrush(pix));
        pNewWidget->setAutoFillBackground(true);
        pNewWidget->setPalette(palette);
        if(lstBkgInfo.size() > 1
                && lstBkgInfo[1] == STR_SIZE_FIX){
            pNewWidget->setFixedSize(pix.width(), pix.height());
        }
    }
    if(src->property(STR_UI_MINW).toInt() > 0){
        pNewWidget->setMinimumWidth(src->property(STR_UI_MINW).toInt());
    }
    if(src->property(STR_UI_MINH).toInt() > 0){
        pNewWidget->setMinimumHeight(src->property(STR_UI_MINH).toInt());
    }
    if(!src->property(STR_UI_VISIBLE).toBool()){
        pNewWidget->hide();
    }
    return pNewWidget;
}

QLayout *SProject::constructLayout(const QString &strName, QWidget *pParent)
{
    if(strName.compare(STR_LAYOUT_GRID, Qt::CaseInsensitive) == 0){
        return new QGridLayout(pParent);
    }else if(strName.compare(STR_LAYOUT_VBOX, Qt::CaseInsensitive) == 0){
        return new QVBoxLayout(pParent);
    }else if(strName.compare(STR_LAYOUT_HBOX, Qt::CaseInsensitive) == 0){
        return new QHBoxLayout(pParent);
    }else if(strName.compare(STR_LAYOUT_FORM, Qt::CaseInsensitive) == 0){
        return new QFormLayout(pParent);
    }
    return NULL;
}
