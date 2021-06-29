#include "sprojecttreewidget.h"
#include "sobject.h"
#include "smainwindow.h"
#include "sselectplusgindlg.h"
#include "smanagecontainerwidget.h"

SProjectTreeWidget::SProjectTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setHeaderLabels(QStringList()<< tr("Property") << tr("Value"));
    setColumnWidth(0, 300);
    createAction();

    setContextMenuPolicy(Qt::DefaultContextMenu);
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotItemDoubleClickedHandle(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            SLOT(slotItemChangedHandle(QTreeWidgetItem*,int)));
    setMinimumSize(400, 600);
}

SProjectTreeWidget::~SProjectTreeWidget()
{
    if(m_projectObj)
        delete m_projectObj;
}

SObject *SProjectTreeWidget::projectObj() const
{
    return m_projectObj;
}

void SProjectTreeWidget::setProjectObj(SObject *projectObj)
{
    m_projectObj = projectObj;
    slotUpdateTree();
}

SMainWindow *SProjectTreeWidget::mainWindow() const
{
    return m_mainWindow;
}

void SProjectTreeWidget::setMainWindow(SMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
}

void SProjectTreeWidget::appendParameter(SObject *pObj, QTreeWidgetItem *parentItem, bool bHasTitle,
                                         const QString &strRootRole, const QString &strParamRole)
{
    QString strName;
    QTreeWidgetItem* pItem[2];
    QVariant varValue;
    if(bHasTitle){
        pItem[0] = new QTreeWidgetItem(parentItem, QStringList() << tr("Parameter"));
        pItem[0]->setData(0, DR_TYPE, strRootRole);
        pItem[0]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    }else{
        pItem[0] = parentItem;
    }
    auto lstNames = pObj->dynamicPropertyNames();
    for(auto iterItem = lstNames.begin(); iterItem != lstNames.end(); iterItem++){
        strName = iterItem->data();
        if(strName.startsWith("__") && strName != "__id")
            continue;
        varValue = pObj->property(iterItem->data());
        if(!varValue.isValid())
            continue;
        pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << strName);
        pItem[1]->setData(0, DR_TYPE, strParamRole);
        pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
        if(QString(varValue.typeName()) == STR_TYPE_QPIXMAP){
           pItem[1]->setIcon(1, QIcon(varValue.value<QPixmap>()));
        }else{
            pItem[1]->setData(0, DR_PROP_NAME, strName);
            pItem[1]->setData(1, Qt::EditRole, varValue);
        }
    }
}

QTreeWidgetItem *SProjectTreeWidget::appendSObjectV2(SObject *pObj, QTreeWidgetItem *parentItem,
                                                     const QString &strRole, const QString &strParamRole)
{
    QTreeWidgetItem* pItem[2];
    SObject* pItemObj;
    QVariant varSys;
    QString strSys,strName;

    strName = pObj->objectName();
    if((varSys = pObj->property(STR_PLUGIN)).isValid()
            && !(strSys = varSys.toString()).isEmpty()){
        strName += "(" + strSys + ")";
    }
    pItem[0] = new QTreeWidgetItem(parentItem,QStringList() << strName);
    pItem[0]->setData(0, DR_TYPE, strRole);
    pItem[0]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    appendParameter(pObj, pItem[0], false, STR_DEFAULT, strParamRole);
    auto lstChildren = pObj->children();
    for(auto iterChild = lstChildren.begin(); iterChild != lstChildren.end(); iterChild++){
        pItemObj = (SObject*)(*iterChild);
        appendSObjectV2(pItemObj, pItem[0], strRole, strParamRole);
    }
    return pItem[0];
}

void SProjectTreeWidget::createAction()
{
    // common
    m_actions[A_REMOVE_SOBJECT] = new QAction(tr("Remove"), this);
    connect(m_actions[A_REMOVE_SOBJECT], SIGNAL(triggered()), SLOT(slotRemoveSObject()));

    // Project
    m_actions[A_NEW_PROJ] = new QAction(tr("New project"), this);
    connect(m_actions[A_NEW_PROJ], SIGNAL(triggered()), SLOT(slotNewProject()));
    m_actions[A_REMOVE_PROJ] = new QAction(tr("Remove project"), this);
    connect(m_actions[A_REMOVE_PROJ], SIGNAL(triggered()), SLOT(slotRemoveProject()));
    m_actions[A_IMPORT_PROJ] = new QAction(tr("Import project"), this);
    connect(m_actions[A_IMPORT_PROJ], SIGNAL(triggered()), SLOT(slotImportProject()));
    m_actions[A_EXPORT_PROJ] = new QAction(tr("Export project"), this);
    connect(m_actions[A_EXPORT_PROJ], SIGNAL(triggered()), SLOT(slotExportProject()));
    m_actions[A_PREVIEW_PROJ] = new QAction(tr("Preview"), this);
    connect(m_actions[A_PREVIEW_PROJ], SIGNAL(triggered()), SLOT(slotPreviewProject()));

    // param
    m_actions[A_NEW_PROP] = new QAction(tr("New property"), this);
    connect(m_actions[A_NEW_PROP], SIGNAL(triggered()), SLOT(slotNewProperty()));
    m_actions[A_REMOVE_PROP] = new QAction(tr("Remove property"), this);
    connect(m_actions[A_REMOVE_PROP], SIGNAL(triggered()), SLOT(slotRemoveProperty()));

    // icon
    m_actions[A_IMPORT_ICON] = new QAction(tr("Import icon"), this);
    connect(m_actions[A_IMPORT_ICON], SIGNAL(triggered()), SLOT(slotImportIcon()));
    m_actions[A_REMOVE_ICON] = new QAction(tr("Remove icon"), this);
    connect(m_actions[A_REMOVE_ICON], SIGNAL(triggered()), SLOT(slotRemoveIcon()));

    // action
    m_actions[A_NEW_ACTION] = new QAction(tr("New action"), this);
    connect(m_actions[A_NEW_ACTION], SIGNAL(triggered()), SLOT(slotNewAction()));

    // menu
    m_actions[A_APPEND_MENU] = new QAction(tr("Append menu"), this);
    connect(m_actions[A_APPEND_MENU], SIGNAL(triggered()), SLOT(slotAppendMenu()));
    m_actions[A_REMOVE_MENU] = new QAction(tr("Remove menu"), this);
    connect(m_actions[A_REMOVE_MENU], SIGNAL(triggered()), SLOT(slotRemoveSObject()));
    m_actions[A_APPEND_MENU_ITEM] = new QAction(tr("Append menu item"), this);
    connect(m_actions[A_APPEND_MENU_ITEM], SIGNAL(triggered()), SLOT(slotAppendMenuItem()));
    m_actions[A_REMOVE_MENU_ITEM] = new QAction(tr("Remove menu item"), this);
    connect(m_actions[A_REMOVE_MENU_ITEM], SIGNAL(triggered()), SLOT(slotRemoveSObject()));

    // widget
    m_actions[A_NEW_INSTANCE] = new QAction(tr("New instance"), this);
    connect(m_actions[A_NEW_INSTANCE], SIGNAL(triggered()), SLOT(slotNewInstance()));
    m_actions[A_NEW_CONTAINER_WIDGET] = new QAction(tr("New container"), this);
    connect(m_actions[A_NEW_CONTAINER_WIDGET], SIGNAL(triggered()), SLOT(slotNewContainer()));
    m_actions[A_NEW_CONTAINER_GROUPBOX] = new QAction(tr("Group box"), this);
    connect(m_actions[A_NEW_CONTAINER_GROUPBOX], SIGNAL(triggered()), SLOT(slotNewContainer()));
    m_actions[A_NEW_CONTAINER_SPLIT] = new QAction(tr("Split widget"), this);
    connect(m_actions[A_NEW_CONTAINER_SPLIT], SIGNAL(triggered()), SLOT(slotNewContainer()));
    m_actions[A_NEW_CONTAINER_SCROLLAREA] = new QAction(tr("Scroll frame"), this);
    connect(m_actions[A_NEW_CONTAINER_SCROLLAREA], SIGNAL(triggered()), SLOT(slotNewContainer()));

    // communication
    m_actions[A_NEW_COMM] = new QAction(tr("New communication"), this);
    connect(m_actions[A_NEW_COMM], SIGNAL(triggered()), SLOT(slotNewCommunication()));

    // data
    m_actions[A_NEW_DATA] = new QAction(tr("New sub-data"), this);
    connect(m_actions[A_NEW_DATA], SIGNAL(triggered()), SLOT(slotNewData()));

    // pop widget
    m_actions[A_NEW_POP] = new QAction(tr("New pop-widget"), this);
    connect(m_actions[A_NEW_POP], SIGNAL(triggered()), SLOT(slotNewPopWidget()));
}

void SProjectTreeWidget::contextMenuEvent(QContextMenuEvent *evt)
{
    QMenu popMenu(this);
    if(m_projectObj == NULL){
        popMenu.addAction(m_actions[A_NEW_PROJ]);
        popMenu.addAction(m_actions[A_IMPORT_PROJ]);
    }else{
        QTreeWidgetItem* pItem = itemAt(evt->pos());
        if(pItem == NULL)
            return;
        QString strType = pItem->data(0, DR_TYPE).toString();
        if(strType.isEmpty())
            return;
        SObject* pObj = (SObject*)pItem->data(0, DR_SOBJECT).value<void*>();
        setProperty(STR_CURRENT_ITEM, QVariant::fromValue((void*)pItem));
        if(strType == STR_PROJECT){
            popMenu.addAction(m_actions[A_EXPORT_PROJ]);
            popMenu.addAction(m_actions[A_IMPORT_PROJ]);
            popMenu.addAction(m_actions[A_PREVIEW_PROJ]);
            popMenu.addAction(m_actions[A_REMOVE_PROJ]);
        }else if(strType == STR_COM_ROOT){
            popMenu.addAction(m_actions[A_NEW_COMM]);
        }else if(strType == STR_COM){
            if(pObj->children().isEmpty())
                popMenu.addAction(m_actions[A_NEW_CMD]);
            popMenu.addAction(m_actions[A_REMOVE_SOBJECT]);
        }else if(strType == STR_ICON_ROOT){
            popMenu.addAction(m_actions[A_IMPORT_ICON]);
        }else if(strType == STR_ICON){
            popMenu.addAction(m_actions[A_REMOVE_ICON]);
        }else if(strType == STR_ACTION_ROOT){
            popMenu.addAction(m_actions[A_NEW_ACTION]);
        }else if(strType == STR_MENU_ROOT){
            popMenu.addAction(m_actions[A_APPEND_MENU]);
        }else if(strType == STR_MENU){
            popMenu.addAction(m_actions[A_APPEND_MENU_ITEM]);
            popMenu.addAction(m_actions[A_REMOVE_MENU]);
        }else if(strType == STR_MENU_ITEM){
            popMenu.addAction(m_actions[A_REMOVE_MENU_ITEM]);
        }else if(strType == STR_SUB_MENU){
            popMenu.addAction(m_actions[A_APPEND_MENU_ITEM]);
        }else if(strType == STR_WIDGET_CHILDREN
                 || strType == STR_DOCK_ROOT
                 || strType == STR_STATUSBAR
                 || (strType == STR_CENTRAL_WIDGET
                     && ((SObject*)pItem->data(0, DR_SOBJECT).value<void*>())->children().isEmpty())){
            popMenu.addAction(m_actions[A_NEW_INSTANCE]);
            popMenu.addAction(m_actions[A_NEW_CONTAINER_WIDGET]);
        }else if(strType == STR_CONTAINER
                 || strType == STR_INSTANCE
                 || strType == STR_SIGNAL
                 || strType == STR_ACTION){
            popMenu.addAction(m_actions[A_REMOVE_SOBJECT]);
        }else if(strType == STR_DATA_ROOT){
            popMenu.addAction(m_actions[A_NEW_PROP]);
            popMenu.addAction(m_actions[A_NEW_DATA]);
        }else if(strType == STR_DATA){
            popMenu.addAction(m_actions[A_NEW_PROP]);
            popMenu.addAction(m_actions[A_NEW_DATA]);
            popMenu.addAction(m_actions[A_REMOVE_SOBJECT]);
        }else if(strType == STR_REMOVABLE_PARAM){
            popMenu.addAction(m_actions[A_REMOVE_PROP]);
        }else if(strType == STR_SIGNAL_ROOT){
            popMenu.addAction(m_actions[A_NEW_SIGNAL]);
        }else if(strType == STR_POPWIDGET_ROOT){
            popMenu.addAction(m_actions[A_NEW_POP]);
        }else{
            return;
        }
    }
    popMenu.exec(QCursor::pos());
}

void SProjectTreeWidget::appendMenu(SObject *pObj, QTreeWidgetItem *parentItem)
{
    QString strName;
    QTreeWidgetItem* pItem[5];
    SObject* pObjs[5];
    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    while(true){
        strName = QInputDialog::getText(this, tr("New menu"), tr(STR_NAME), QLineEdit::Normal, strName);
        if(strName.isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("The name can't be empty."));
            continue;
        }
        if(pObjs[0]->findChild<SObject*>(strName) != NULL){
            QMessageBox::critical(this, tr("Error"), tr("The name has been used. Please try another."));
            continue;
        }
        break;
    }
    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName(strName);
    pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << strName);
    pItem[1]->setData(0, DR_TYPE, STR_MENU);
    pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[1]));
}

void SProjectTreeWidget::appendWidget(SObject *pObj, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem* pItem[5];
    SObject* pObjs[5];
    QString strType,strName;

    strType = pObj->property(STR_INS_TYPE).toString();
    if(strType == STR_INSTANCE){
        strName = pObj->property(STR_PLUGIN).toString();
    }else{
        strName = strType;
    }
    pItem[0] = new QTreeWidgetItem(
                parentItem,
            QStringList() << tr("%1(%2)").arg(pObj->objectName()).arg(strName));
    pItem[0]->setData(0, DR_TYPE, strType);
    pItem[0]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    appendParameter(pObj, pItem[0]);
    if(strType == STR_CONTAINER){

        pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << tr("Children"));
        pItem[1]->setData(0, DR_TYPE, STR_WIDGET_CHILDREN);
        pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
        auto lstChildren = pObj->children();
        for(auto iterChild = lstChildren.begin(); iterChild != lstChildren.end(); iterChild++){
            pObjs[0] = (SObject*)(*iterChild);
            appendWidget(pObjs[0], pItem[1]);
        }
    }else{
        auto lstChildren = pObj->children();
        for(auto iterChild = lstChildren.begin(); iterChild != lstChildren.end(); iterChild++){
            pObjs[0] = (SObject*)(*iterChild);
            appendSObject(pObjs[0], pItem[0]);
        }
    }
}

void SProjectTreeWidget::appendSObject(SObject *pObj, QTreeWidgetItem *parentItem,
                                       const QString &strRole, const QString &strParamRootRole,
                                       const QString &strParamRole, const QString &strChildRole)
{
    QTreeWidgetItem* pItem[2];
    SObject* pItemObj;
    QVariant varSys;
    QString strSys,strName;

    strName = pObj->objectName();
    if((varSys = pObj->property(STR_PLUGIN)).isValid()
            && !(strSys = varSys.toString()).isEmpty()){
        strName += "(" + strSys + ")";
    }else if((varSys = pObj->property(STR_INS_TYPE)).isValid()
             && !(strSys = varSys.toString()).isEmpty()){
         strName += "(" + strSys + ")";
     }
    pItem[0] = new QTreeWidgetItem(parentItem,QStringList() << strName);
    pItem[0]->setData(0, DR_TYPE, strRole);
    pItem[0]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    appendParameter(pObj, pItem[0], true, strParamRootRole, strParamRole);
    pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << tr("Children"));
    pItem[1]->setData(0, DR_TYPE, strChildRole);
    pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    auto lstChildren = pObj->children();
    for(auto iterChild = lstChildren.begin(); iterChild != lstChildren.end(); iterChild++){
        pItemObj = (SObject*)(*iterChild);
        appendSObject(pItemObj, pItem[1]);
    }
}

void SProjectTreeWidget::appendCommunication(SObject *pObj, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem* pItem[2];
    QString strName = pObj->objectName() + "(" + pObj->property(STR_PLUGIN).toString() + ")";

    pItem[0] = new QTreeWidgetItem(parentItem, QStringList() << strName);
    pItem[0]->setData(0, DR_TYPE, STR_COM);
    pItem[0]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    appendParameter(pObj, pItem[0], false);
    // command
    auto lstSignal = pObj->children();
    for(auto iterSignal = lstSignal.begin(); iterSignal != lstSignal.end(); iterSignal++){
        pItem[1] = appendSObjectV2((SObject*)(*iterSignal), pItem[0]);
        pItem[1]->setData(0, DR_TYPE, STR_COMMAND);
    }
}

void SProjectTreeWidget::appendAction(SObject *pObj, const QString &strName, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem* pItem[2];
    pItem[0] = new QTreeWidgetItem(parentItem, QStringList() << strName);
    pItem[0]->setData(0, DR_TYPE, STR_ACTION);
    pItem[0]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    pItem[0]->setData(0, DR_PROP_NAME, strName);
    pItem[1] = new QTreeWidgetItem(
                pItem[0],
            QStringList() << tr(STR_PATH) << pObj->propertyInfo()[strName].m_attribute[STR_ACTION_TYPE]);
    pItem[1]->setData(0, DR_TYPE, STR_ACTION_TYPE);
    pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    pItem[1]->setData(0, DR_PROP_NAME, strName);
    pItem[1]->setData(0, DR_ATTR_NAME, STR_ACTION_TYPE);
    pItem[1] = new QTreeWidgetItem(
                pItem[0],
            QStringList() << tr(STR_ICON) << pObj->propertyInfo()[strName].m_attribute[STR_ACTION_ICON]);
    pItem[1]->setData(0, DR_TYPE, STR_ACTION_ICON);
    pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    pItem[1]->setData(0, DR_PROP_NAME, strName);
    pItem[1]->setData(0, DR_ATTR_NAME, STR_ACTION_ICON);
    pItem[1] = new QTreeWidgetItem(
                pItem[0],
            QStringList() << tr("Code") << pObj->property(strName.toUtf8().data()).toString());
    pItem[1]->setData(0, DR_TYPE, STR_ACTION_CODE);
    pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObj));
    pItem[1]->setData(0, DR_PROP_NAME, strName);
}

void SProjectTreeWidget::addWidgetParam(SObject *pSrc)
{
    SObject* pParent = (SObject*)pSrc->parent();

    pSrc->setProperty(STR_UI_VISIBLE, true);
    pSrc->setProperty(STR_UI_LAYOUT, STR_LAYOUT_GRID);
    pSrc->setProperty(STR_UI_LAYOUT_P, STR_DEFAULT);
    pSrc->setProperty(STR_UI_MINW, 0);
    pSrc->setProperty(STR_UI_MINH, 0);
    pSrc->setProperty(STR_UI_BACKIMG, STR_DEFAULT);
    if(pParent){
        QString strLayout,strType,strClass;

        strLayout = pParent->property(STR_UI_LAYOUT).toString();
        if(strLayout.isEmpty()){
            //return;
        }else if(strLayout == STR_LAYOUT_HBOX
                 || strLayout == STR_LAYOUT_VBOX){
            pSrc->setProperty(STR_UI_LAYOUT_P, "0,0,0");
        }else if(strLayout == STR_LAYOUT_GRID){
            pSrc->setProperty(STR_UI_LAYOUT_P, "0,0,1,1,0");
        }else if(strLayout == STR_LAYOUT_FORM){
            pSrc->setProperty(STR_UI_LAYOUT_P, "0,0");
        }else if(strLayout == STR_LAYOUT_STACK){
            pSrc->setProperty(STR_UI_LAYOUT_P, "0");
        }else if(strLayout == STR_LAYOUT_GEOMETRY){
            pSrc->setProperty(STR_UI_LAYOUT_P, "1,1,100,60");
        }

        strType = pParent->property(STR_INS_TYPE).toString();
        if(strType == STR_DOCK){
            pSrc->setProperty(STR_UI_AREA, STR_LEFT);
            pSrc->setProperty(STR_UI_INDEX, 0);
            pSrc->setProperty(STR_UI_TITLE, STR_DOCK);
            pSrc->setProperty(STR_UI_FLOAT, false);
            pSrc->setProperty(STR_UI_CLOSE, true);
            pSrc->setProperty(STR_UI_ALLOW_AREAS, STR_ALL);
            return;
        }else if(strType == STR_CONTAINER){
            strClass = pParent->property(STR_UI_CLASS).toString();
            if(strClass == STR_SPLIT){
                pSrc->setProperty(STR_UI_LAYOUT_P, "0");
            }else if(strClass == STR_TAB){
                pSrc->setProperty(STR_UI_LAYOUT_P, "0");
                pSrc->setProperty(STR_UI_TITLE, STR_TAB);
                pSrc->setProperty(STR_UI_ICON, STR_DEFAULT);
            }
        }else if(strType == STR_STATUSBAR){
            pSrc->setProperty(STR_UI_INDEX, 0);
            pSrc->setProperty(STR_UI_STR, 0);
            pSrc->setProperty(STR_UI_LEFT, true);
        }
    }
}

void SProjectTreeWidget::slotNewProject()
{
    SObject *pObjs[5];
    pObjs[0] = new SObject();
    pObjs[0]->setObjectName(STR_SOBJ);
    pObjs[0]->setProperty(STR_AUTHOR, STR_DEFAULT);
    pObjs[0]->setProperty(STR_PATH, STR_DEFAULT);
    pObjs[0]->setProperty(STR_COMMENT, STR_DEFAULT);
    pObjs[0]->setProperty(STR_TYPE, STR_PROROOT);

    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName(STR_PROROOT);
    pObjs[1]->setProperty(STR_NAME, "new_project");
    pObjs[1]->setProperty(STR_COMMENT, STR_PROJECT);
    pObjs[1]->setProperty(STR_ICON, STR_DEFAULT);
    pObjs[1]->setProperty(STR_ACT_INIT, STR_DEFAULT);
    pObjs[1]->setProperty(STR_ACT_REL, STR_DEFAULT);
    pObjs[1]->setProperty(STR_TIMER_INTER, (uint)1000);
    pObjs[1]->setProperty(STR_TIMER_COUNT, uint(0));

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName(STR_COM);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName(STR_DATA);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName(STR_RES);
    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_ICON);
    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_ACTION);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName(STR_UI);
    pObjs[2]->setProperty(STR_UI_SHOW, STR_MAX);
    pObjs[2]->setProperty(STR_TOOLBAR_VISIBLE, false);
    pObjs[2]->setProperty(STR_STATUSBAR_VISIBLE, false);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_MENU);
    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_CENTRAL);
    pObjs[3]->setProperty(STR_UI_VISIBLE, true);
    pObjs[3]->setProperty(STR_UI_LAYOUT, STR_LAYOUT_VBOX);
    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_DOCK);
    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_POPWIDGET);
    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_STATUSBAR);
    setProjectObj(pObjs[0]);
}

void SProjectTreeWidget::slotRemoveProject()
{
    if(m_projectObj){
        delete m_projectObj;
        m_projectObj = NULL;
        slotUpdateTree();
    }
}

void SProjectTreeWidget::slotImportProject()
{
    QString strFileName;
    strFileName = QFileDialog::getOpenFileName(this, tr("Import project"), QString(),
                                               tr("GMDTTS project file (*.sproj)"));
    if(strFileName.isEmpty())
        return;
    bool bRes;
    SObject* pProjObj = new SObject();
    bRes = pProjObj->openFile(strFileName);
    qDebug() << tr("It was %1 to import the project file.").arg(bRes ? "successful" : "failed");
    if(!bRes){
        delete pProjObj;
        return;
    }
    setProjectObj(pProjObj);
}

void SProjectTreeWidget::slotExportProject()
{
    if(m_projectObj == NULL)
        return;
    QString strFileName;
    strFileName = QFileDialog::getSaveFileName(this, tr("Export project"),
                                               QString(), tr("GMDTTS project file (*.sproj)"));
    if(strFileName.isEmpty())
        return;
    qDebug() << tr("It was %1 to export the project file.")
                .arg(m_projectObj->saveFile(strFileName) ? "successful" : "failed");
}

void SProjectTreeWidget::slotPreviewProject()
{
    m_mainWindow->slotOpenProject(m_projectObj);
}

void SProjectTreeWidget::slotImportBoard()
{
    QString strFileName,strType,strType2,strSUffix,strComment, strAuthor;
    QTreeWidgetItem* pTreeItem;

    pTreeItem = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    strType = pTreeItem->data(0, DR_TYPE).toString();
    if(strType == STR_BORAD_ROOT){
        strSUffix = "sbf";
        strType2 = STR_BOARD;
    }else if(strType == STR_PART_ROOT){
        strSUffix = "spf";
        strType2 = STR_PART;
    }else{
        strSUffix = "sproj";
        strType2 = STR_PROROOT;
    }
    strFileName = QFileDialog::getOpenFileName(
                this,
                tr("Import %1").arg(strType2),
                QString(),
                tr("GMDTTS %1 file (*.%2)").arg(strType2).arg(strSUffix));
    if(strFileName.isEmpty())
        return;
    SObject* pObjs[2];
    pObjs[0] = (SObject*)pTreeItem->data(0, DR_SOBJECT).value<void*>();
    pObjs[1] = readSObject(strFileName, strType, strComment, strAuthor, pObjs[0]);
    if(pObjs[1] != NULL
            && strType != strType2){
        delete pObjs[1];
        pObjs[1] = NULL;
    }
    qDebug() << tr("It was %1 to import the %2 file.").arg(pObjs[1] != NULL ? "successful" : "failed").arg(strType2);
    if(pObjs[1] == NULL)
        return;
    if(strType == STR_BOARD){
        //appendBoard(pObjs[1], pTreeItem);
    }
}

void SProjectTreeWidget::slotNewData()
{
    SObject *pObjs[2];
    QTreeWidgetItem* pItem[2];
    QString strName;
    bool bOK;
    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();

    do{
        strName = QInputDialog::getText(this, tr("New sub-data"), tr(STR_NAME), QLineEdit::Normal, strName, &bOK);
        if(!bOK)
            return;
        if(strName.isEmpty()){
            QMessageBox::warning(this, tr("Warning"), tr("The name can't be empty."));
            continue;
        }
        if(pObjs[0]->findChild<SObject*>(strName.toUtf8()) != NULL){
            QMessageBox::warning(this, tr("Warning"), tr("The name has existed."));
            continue;
        }
        pObjs[1] = new SObject(pObjs[0]);
        pObjs[1]->setObjectName(strName);
        appendSObjectV2(pObjs[1], pItem[0], STR_DATA, STR_REMOVABLE_PARAM);
    }while(true);
}

void SProjectTreeWidget::slotNewProperty()
{
    QDialog dlgContainer(this);
    QLineEdit* pEdits[2];
    QPushButton* pButtons[2];
    QGridLayout* pLayout = new QGridLayout(&dlgContainer);
    QComboBox* pTypeCombo = new QComboBox(&dlgContainer);

    dlgContainer.setWindowTitle("Create property");
    pTypeCombo->addItems(SObject::supportedTypes());
    pEdits[0] = new QLineEdit(&dlgContainer);
    pEdits[1] = new QLineEdit("1", &dlgContainer);
    pButtons[0] = new QPushButton(tr("Add"), &dlgContainer);
    pButtons[1] = new QPushButton(tr("Finish"), &dlgContainer);

    pLayout->addWidget(new QLabel(tr(STR_PATH), &dlgContainer), 0, 0);
    pLayout->addWidget(pTypeCombo, 0, 1, 1, 3);
    pLayout->addWidget(new QLabel(tr(STR_NAME), &dlgContainer), 1, 0);
    pLayout->addWidget(pEdits[0], 1, 1, 1, 3);
    pLayout->addWidget(new QLabel(tr("Value"), &dlgContainer), 2, 0);
    pLayout->addWidget(pEdits[1], 2, 1, 1, 3);
    pLayout->addWidget(pButtons[0], 3, 1);
    pLayout->addWidget(pButtons[1], 3, 2);

    connect(pButtons[0], SIGNAL(clicked()), &dlgContainer, SLOT(accept()));
    connect(pButtons[1], SIGNAL(clicked()), &dlgContainer, SLOT(reject()));

    SObject *pObjs[2];
    QTreeWidgetItem* pTreeItem[2];
    QVariant varValue;
    QString strPropName;
    pTreeItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pTreeItem[0]->data(0, DR_SOBJECT).value<void*>();

    do{
        if(dlgContainer.exec() != QDialog::Accepted)
            return;
        strPropName = pEdits[0]->text();
        if(strPropName.isEmpty()){
            QMessageBox::warning(this, tr("Warning"), tr("The name can't be empty."));
            continue;
        }
        if(pObjs[0]->dynamicPropertyNames().contains(strPropName.toUtf8())){
            QMessageBox::warning(this, tr("Warning"), tr("The property has existed."));
            continue;
        }
        varValue = SObject::stringToVariant(pTypeCombo->currentText(), pEdits[1]->text());
        if(!varValue.isValid())
            continue;
        pObjs[0]->setProperty(strPropName.toUtf8().data(), varValue);
        pTreeItem[1] = new QTreeWidgetItem(pTreeItem[0], QStringList() << strPropName);
        pTreeItem[1]->setData(1, Qt::EditRole, varValue);
        pTreeItem[1]->setData(0, DR_TYPE, STR_REMOVABLE_PARAM);
        pTreeItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[0]));
        pTreeItem[1]->setData(0, DR_PROP_NAME, strPropName);
    }while(true);
}

void SProjectTreeWidget::slotRemoveProperty()
{
    SObject *pObjs[1];
    QTreeWidgetItem* pTreeItem[1];

    pTreeItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pTreeItem[0]->data(0, DR_SOBJECT).value<void*>();
    pObjs[0]->setProperty(pTreeItem[0]->data(0, DR_PROP_NAME).toString().toUtf8().data(), QVariant());
    delete pTreeItem[0];
}

void SProjectTreeWidget::slotImportIcon()
{
    QString strFileName;

    strFileName = QFileDialog::getOpenFileName(
                this,
                tr("Image"),
                QString(),
                tr("Image file (*.png *.jpg *.jpeg *.bmp *.ico *.PNG *.JPG *JPEG *.BMP *.ICO)"));
    if(strFileName.isEmpty())
        return;
    QPixmap img;
    if(!img.load(strFileName))
        return;
    QTreeWidgetItem* pItem[2];
    QFileInfo fileInfo(strFileName);
    SObject* pObjs[2];
    bool bFound;
    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    strFileName = fileInfo.baseName();
    auto lstNames = pObjs[0]->dynamicPropertyNames();
    while(true){
        strFileName = QInputDialog::getText(this, tr("New Icon"), tr(STR_NAME), QLineEdit::Normal, strFileName);
        if(strFileName.isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("The name can't be empty."));
            continue;
        }
        bFound = false;
        for(auto iterName = lstNames.begin(); iterName != lstNames.end(); iterName++){
            if(strFileName == iterName->data()){
                bFound = true;
                break;
            }
        }
        if(bFound){
            QMessageBox::critical(this, tr("Error"), tr("The name has been used. Please try another."));
            continue;
        }
        break;
    }
    pObjs[0]->setProperty(strFileName.toUtf8().data(), img);
    pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << strFileName);
    pItem[1]->setIcon(1, QIcon(img));
    pItem[1]->setData(0, DR_TYPE, STR_ICON);
}

void SProjectTreeWidget::slotRemoveIcon()
{
    SObject *pObjs[5];
    QTreeWidgetItem* pTreeItem[5];

    pTreeItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pTreeItem[0]->parent()->data(0, DR_SOBJECT).value<void*>();
    pObjs[0]->setProperty(pTreeItem[0]->text(0).toUtf8().data(), QVariant());
    delete pTreeItem[0];
}

void SProjectTreeWidget::slotNewAction()
{
    QTreeWidgetItem* pItem[5];
    SObject* pObjs[5];

    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    SSelectPlusginDlg dlgPlugin(STR_ACTION, false, pObjs[0], mainWindow(), this);
    //((QLineEdit*)dlgPlugin.widget(SSelectPlusginDlg::E_NAME))->setText("command");
    if(dlgPlugin.exec() != QDialog::Accepted)
        return;
    QList<SObject*> lstObj = dlgPlugin.selectedSObject(pObjs[0]);
    for(auto iterComm = lstObj.begin(); iterComm != lstObj.end(); iterComm++){
        appendSObjectV2(*iterComm, pItem[0], STR_ACTION);
    }
}

void SProjectTreeWidget::slotAppendMenu()
{
    QString strName;
    QTreeWidgetItem* pItem[5];
    SObject* pObjs[5];
    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    while(true){
        strName = QInputDialog::getText(this, tr("New menu"), tr(STR_NAME), QLineEdit::Normal, strName);
        if(strName.isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("The name can't be empty."));
            continue;
        }
        if(pObjs[0]->findChild<SObject*>(strName) != NULL){
            QMessageBox::critical(this, tr("Error"), tr("The name has been used. Please try another."));
            continue;
        }
        break;
    }
    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName(strName);
    pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << strName);
    pItem[1]->setData(0, DR_TYPE, STR_MENU);
    pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[1]));
}

void SProjectTreeWidget::slotAppendMenuItem()
{
    QString strName;
    QTreeWidgetItem* pItem[5];
    SObject* pObjs[5];
    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    while(true){
        strName = QInputDialog::getText(this, tr("New menu item"), tr(STR_NAME), QLineEdit::Normal, strName);
        if(strName.isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("The name can't be empty."));
            continue;
        }
        if(pObjs[0]->findChild<SObject*>(strName) != NULL){
            QMessageBox::critical(this, tr("Error"), tr("The name has been used. Please try another."));
            continue;
        }
        break;
    }
    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName(strName);
    pObjs[1]->setProperty(STR_ACTION, STR_DEFAULT);
    pObjs[1]->setProperty(STR_ENABLE, true);
    pObjs[1]->setProperty(STR_TOOLBAR, false);
    pObjs[1]->setProperty(STR_CHECKABLE, false);
    pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << strName);
    pItem[1]->setData(0, DR_TYPE, STR_MENU_ITEM);
    pItem[1]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[1]));
    appendParameter(pObjs[1], pItem[1], true, "parameter_menu_item");
    pItem[2] = new QTreeWidgetItem(pItem[1], QStringList() << tr("Sub Menu"));
    pItem[2]->setData(0, DR_TYPE, STR_SUB_MENU);
    pItem[2]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[1]));
}

void SProjectTreeWidget::slotNewInstance()
{
    QTreeWidgetItem* pItem[2];
    SObject* pObjs[2];

    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    SSelectPlusginDlg dlgPlugin(STR_WIDGET, false, pObjs[0], mainWindow(), this);
    ((QLineEdit*)dlgPlugin.widget(SSelectPlusginDlg::E_NAME))->setText(STR_WIDGET);
    if(dlgPlugin.exec() != QDialog::Accepted)
        return;
    QList<SObject*> lstObj = dlgPlugin.selectedSObject(pObjs[0]);
    for(auto iterComm = lstObj.begin(); iterComm != lstObj.end(); iterComm++){
        pObjs[1] = *iterComm;
        pObjs[1]->setProperty(STR_INS_TYPE, STR_INSTANCE);
        addWidgetParam(pObjs[1]);
        appendSObject(pObjs[1], pItem[0], STR_INSTANCE);
    }
}

void SProjectTreeWidget::slotNewContainer()
{
    QString strName,strType;
    QTreeWidgetItem* pItem[5];
    SObject* pObjs[5];
    SManageContainerWidget containerW(this);
    do{
        if (containerW.exec() != QDialog::Accepted)
            return;
        strName = containerW.edit(SManageContainerWidget::E_NAME)->text();
        if(strName.isEmpty()){
            QMessageBox::warning(&containerW, tr("Warning"), tr("The name can't be empty."));
            continue;
        }
        break;
    }while(true);
    strType = containerW.comboBox(SManageContainerWidget::CB_TYPE)->currentText();
    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName(strName);
    addWidgetParam(pObjs[1]);
    pObjs[1]->setProperty(STR_INS_TYPE, STR_CONTAINER);
    pObjs[1]->setProperty(STR_UI_CLASS, strType);
    if(strType == STR_WIDGET
            || strType == STR_SCROLL){
        pObjs[1]->setProperty(STR_UI_LAYOUT, containerW.comboBox(SManageContainerWidget::CB_LAYOUT)->currentText());
    }else if(strType == STR_SPLIT){
        pObjs[1]->setProperty(STR_UI_ORIENTATION, containerW.comboBox(SManageContainerWidget::CB_SPLIT_ORIENTATION)->currentText());
    }else if(strType == STR_TAB){
        pObjs[1]->setProperty(STR_UI_CLOSE, containerW.comboBox(SManageContainerWidget::CB_TAB_CLOSABE)->currentData());
        pObjs[1]->setProperty(STR_UI_ALIGN, containerW.comboBox(SManageContainerWidget::CB_TAB_ALIGN)->currentText());
    }else if(strType == STR_GROUP){
        pObjs[1]->setProperty(STR_UI_LAYOUT, containerW.comboBox(SManageContainerWidget::CB_LAYOUT)->currentText());
        pObjs[1]->setProperty(STR_UI_TITLE, containerW.edit(SManageContainerWidget::E_GROUP_TITLE)->text());
        pObjs[1]->setProperty(STR_UI_ALIGN, containerW.comboBox(SManageContainerWidget::CB_GROUP_ALIGN)->currentText());
    }
    appendWidget(pObjs[1], pItem[0]);
}

void SProjectTreeWidget::slotNewPopWidget()
{
    QDialog dlg(this);
    QComboBox* comboBoxs[5];
    QFormLayout* pLayout;
    QLineEdit* pEdit[3];
    QPushButton* pButton[2];

    pEdit[0] = new QLineEdit(&dlg);
    pEdit[0]->setPlaceholderText(tr(STR_NAME));

    comboBoxs[0] = new QComboBox(&dlg);
    comboBoxs[0]->addItem(STR_DIALOG, 0);
    // comboBoxs[0]->addItem("Wizard", 1);

    comboBoxs[1] = new QComboBox(&dlg);
    comboBoxs[1]->addItem(STR_LAYOUT_HBOX, 0);
    comboBoxs[1]->addItem(STR_LAYOUT_VBOX, 1);
    comboBoxs[1]->addItem(STR_LAYOUT_GRID, 2);
    comboBoxs[1]->addItem(STR_LAYOUT_FORM, 3);
    comboBoxs[1]->addItem(STR_LAYOUT_STACK, 4);
    comboBoxs[1]->addItem(STR_LAYOUT_GEOMETRY, 5);

    pButton[0] = new QPushButton(tr("OK"), &dlg);
    pButton[1] = new QPushButton(tr("Cancel"), &dlg);

    pLayout = new QFormLayout(&dlg);
    pLayout->addRow(tr(STR_NAME), pEdit[0]);
    pLayout->addRow(tr(STR_PATH), comboBoxs[0]);
    pLayout->addRow(tr(STR_LAYOUT), comboBoxs[1]);
    pLayout->addRow(pButton[0], pButton[1]);

    connect(pButton[0], SIGNAL(clicked()), &dlg, SLOT(accept()));
    connect(pButton[1], SIGNAL(clicked()), &dlg, SLOT(reject()));
    dlg.setWindowTitle(tr("New pop-widget"));

    QTreeWidgetItem* pItem[2];
    SObject* pObjs[2];

    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    while(dlg.exec() == QDialog::Accepted){
        if(pEdit[0]->text().isEmpty()){
            QMessageBox::warning(this, tr("Warning"), tr("The name can't be empty."));
            continue;
        }
        if(pObjs[0]->findChild<SObject*>(pEdit[0]->text()) != NULL){
            QMessageBox::warning(this, tr("Warning"), tr("The name has been used."));
            continue;
        }
        pObjs[1] = new SObject(pObjs[0]);
        pObjs[1]->setObjectName(pEdit[0]->text());
        pObjs[1]->setProperty(STR_INS_TYPE, STR_CONTAINER);
        pObjs[1]->setProperty(STR_UI_CLASS, comboBoxs[0]->currentText().toLower());
        pObjs[1]->setProperty(STR_UI_TITLE, "title");
        pObjs[1]->setProperty(STR_UI_LAYOUT, comboBoxs[1]->currentText());
        pObjs[1]->setProperty(STR_UI_LAYOUT_P, STR_DEFAULT);
        pObjs[1]->setProperty(STR_UI_MINW, 0);
        pObjs[1]->setProperty(STR_UI_MINH, 0);
        pObjs[1]->setProperty(STR_UI_BACKIMG, STR_DEFAULT);
        pObjs[1]->setProperty(STR_ACT_INIT, STR_DEFAULT);
        appendWidget(pObjs[1], pItem[0]);
    }
}

void SProjectTreeWidget::slotNewCommunication()
{
    QTreeWidgetItem* pItem[2];
    SObject* pObjs[2];

    pItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pItem[0]->data(0, DR_SOBJECT).value<void*>();
    SSelectPlusginDlg dlgPlugin(STR_COM, false, pObjs[0], mainWindow(), this);
    if(dlgPlugin.exec() != QDialog::Accepted)
        return;
    QList<SObject*> lstObj = dlgPlugin.selectedSObject(pObjs[0]);
    for(auto iterComm = lstObj.begin(); iterComm != lstObj.end(); iterComm++){
        appendCommunication(*iterComm, pItem[0]);
    }
}

void SProjectTreeWidget::slotItemDoubleClickedHandle(QTreeWidgetItem *item, int column)
{
    if (column==1)
    {
        item->setFlags(item->flags()|Qt::ItemIsEditable);
    }
    else
    {
        item->setFlags(item->flags()&~(Qt::ItemIsEditable));
    }
}

void SProjectTreeWidget::slotItemChangedHandle(QTreeWidgetItem *item, int column)
{
    SObject* pObj;
    QString strPropName;
    if(item == NULL
            || column != 1
            || (pObj = (SObject*)item->data(0, DR_SOBJECT).value<void*>()) == NULL
            || (strPropName = item->data(0, DR_PROP_NAME).toString()).isEmpty())
        return;
    QString strAttrName;
    if((strAttrName = item->data(0, DR_ATTR_NAME).toString()).isEmpty()){
        setProperty(strPropName.toUtf8().data(), item->data(1, Qt::EditRole));
        pObj->changeProperty(strPropName, this, strPropName);
    }else{
        pObj->mutablePropertyInfo()[strPropName].m_attribute[strAttrName] = item->data(1, Qt::EditRole).toString();
    }
}

void SProjectTreeWidget::slotUpdateTree()
{
    clear();
    if(m_projectObj == NULL)
        return;
    QString strName;
    QTreeWidgetItem* pItem[20];
    SObject* pObjs[20];
    //int nIndex;
    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), NULL, NULL);
    // project
    pObjs[0] = m_projectObj;
    pItem[0] = new QTreeWidgetItem(this, QStringList() << pObjs[0]->objectName());
    pItem[0]->setData(0, DR_TYPE, STR_PROJECT);
    pItem[0]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[0]));
    pItem[0]->setExpanded(true);
    appendParameter(pObjs[0], pItem[0]);
    // instrument
    pObjs[1] = pObjs[0]->findChild<SObject*>(STR_PROROOT);
    if(pObjs[1] == nullptr){
        qDebug() << "please create project root sobject";
        return;
    }
    pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << tr(STR_PROROOT));
    pItem[1]->setExpanded(true);
    appendParameter(pObjs[1], pItem[1]);
    // communication
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_COM)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_COM);
    }
    pItem[2] = new QTreeWidgetItem(pItem[1], QStringList() << tr(STR_COM));
    pItem[2]->setData(0, DR_TYPE, STR_COM_ROOT);
    pItem[2]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[2]));
    auto lstComm = pObjs[2]->children();
    for(auto iterComm = lstComm.begin(); iterComm != lstComm.end(); iterComm++){
        pObjs[3] = (SObject*)(*iterComm);
        appendCommunication(pObjs[3], pItem[2]);
    }
    pItem[2]->setExpanded(true);
    // data
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_DATA)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_DATA);
    }
    pItem[2] = appendSObjectV2(pObjs[2], pItem[1], STR_DATA, STR_REMOVABLE_PARAM);
    pItem[2]->setData(0, DR_TYPE, STR_DATA_ROOT);

    // ui
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_UI)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_UI);
    }
    pItem[2] = new QTreeWidgetItem(pItem[1], QStringList() << tr(STR_UI));
    appendParameter(pObjs[2], pItem[2]);
    pItem[2]->setExpanded(true);
    // menu
    if((pObjs[3] = pObjs[2]->findChild<SObject*>(STR_MENU)) == NULL){
        pObjs[3] = new SObject(pObjs[2]);
        pObjs[3]->setObjectName(STR_MENU);
    }
    pItem[3] = new QTreeWidgetItem(pItem[2], QStringList() << tr(STR_MENU));
    pItem[3]->setData(0, DR_TYPE, STR_MENU_ROOT);
    pItem[3]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[3]));
    auto lstMenu = pObjs[3]->children();
    for(auto iterMenu = lstMenu.begin(); iterMenu != lstMenu.end(); iterMenu++){
        pObjs[4] = (SObject*)(*iterMenu);
        pItem[4] = new QTreeWidgetItem(pItem[3], QStringList() << pObjs[4]->objectName());
        pItem[4]->setData(0, DR_TYPE, STR_MENU);
        pItem[4]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[4]));
        appendMenu(pObjs[4], pItem[4]);
    }
    // central widget
    if((pObjs[3] = pObjs[2]->findChild<SObject*>(STR_CENTRAL)) == NULL){
        pObjs[3] = new SObject(pObjs[2]);
        pObjs[3]->setObjectName(STR_CENTRAL);
    }
    pItem[3] = new QTreeWidgetItem(pItem[2], QStringList() << tr("Central Widget"));
    pItem[3]->setData(0, DR_TYPE, STR_CENTRAL_WIDGET);
    pItem[3]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[3]));
    appendParameter(pObjs[3], pItem[3]);
    if(!pObjs[3]->children().isEmpty()){
        appendWidget((SObject*)pObjs[3]->children().first(), pItem[3]);
    }
    // dock widget
    if((pObjs[3] = pObjs[2]->findChild<SObject*>(STR_DOCK)) == NULL){
        pObjs[3] = new SObject(pObjs[2]);
        pObjs[3]->setObjectName(STR_DOCK);
    }
    pObjs[3]->setProperty(STR_INS_TYPE, STR_DOCK);
    pItem[3] = new QTreeWidgetItem(pItem[2], QStringList() << tr("Dock Widget"));
    pItem[3]->setData(0, DR_TYPE, STR_DOCK_ROOT);
    pItem[3]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[3]));
    auto lstDock = pObjs[3]->children();
    for(auto iterDock = lstDock.begin(); iterDock != lstDock.end(); iterDock++){
        pObjs[4] = (SObject*)(*iterDock);
        appendWidget(pObjs[4], pItem[3]);
    }
    // pop widget
    if((pObjs[3] = pObjs[2]->findChild<SObject*>(STR_POPWIDGET)) == NULL){
        pObjs[3] = new SObject(pObjs[2]);
        pObjs[3]->setObjectName(STR_POPWIDGET);
    }
    pItem[3] = new QTreeWidgetItem(pItem[2], QStringList() << tr("Pop Widget"));
    pItem[3]->setData(0, DR_TYPE, STR_POPWIDGET_ROOT);
    pItem[3]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[3]));
    auto lstPops = pObjs[3]->children();
    for(auto iterPop = lstPops.begin(); iterPop != lstPops.end(); iterPop++){
        pObjs[4] = (SObject*)(*iterPop);
        appendWidget(pObjs[4], pItem[3]);
    }

    // statusbar
    if((pObjs[3] = pObjs[2]->findChild<SObject*>(STR_STATUSBAR)) == NULL){
        pObjs[3] = new SObject(pObjs[2]);
        pObjs[3]->setObjectName(STR_STATUSBAR);
    }
    pItem[3] = new QTreeWidgetItem(pItem[2], QStringList() << tr(STR_STATUSBAR));
    pObjs[3]->setProperty(STR_INS_TYPE, STR_STATUSBAR);
    pItem[3]->setData(0, DR_TYPE, STR_STATUSBAR);
    pItem[3]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[3]));
    auto lstStatusBar = pObjs[3]->children();
    for(auto iterStatusItem = lstStatusBar.begin(); iterStatusItem != lstStatusBar.end(); iterStatusItem++){
        pObjs[4] = (SObject*)(*iterStatusItem);
        appendWidget(pObjs[4], pItem[3]);
    }

    // resource
    if((pObjs[2] = pObjs[1]->findChild<SObject*>(STR_RES)) == NULL){
        pObjs[2] = new SObject(pObjs[1]);
        pObjs[2]->setObjectName(STR_RES);
    }
    pItem[2] = new QTreeWidgetItem(pItem[1], QStringList() << tr(STR_RES));
    pItem[2]->setExpanded(true);
    // icon
    QVariant varValue;
    if((pObjs[3] = pObjs[2]->findChild<SObject*>(STR_ICON)) == NULL){
        pObjs[3] = new SObject(pObjs[2]);
        pObjs[3]->setObjectName(STR_ICON);
    }
    pItem[3] = new QTreeWidgetItem(pItem[2], QStringList() << tr(STR_ICON));
    pItem[3]->setData(0, DR_TYPE, STR_ICON_ROOT);
    pItem[3]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[3]));
    auto lstNames = pObjs[3]->dynamicPropertyNames();
    for(auto iterItem = lstNames.begin(); iterItem != lstNames.end(); iterItem++){
        strName = iterItem->data();
        varValue = pObjs[3]->property(strName.toUtf8().data());
        if(!varValue.isValid())
            continue;
        pItem[4] = new QTreeWidgetItem(pItem[3], QStringList() << strName);
        pItem[4]->setData(0, DR_TYPE, STR_ICON);
        pItem[4]->setIcon(1, QIcon(varValue.value<QPixmap>()));
    }
    // action
    if((pObjs[3] = pObjs[2]->findChild<SObject*>(STR_ACTION)) == NULL){
        pObjs[3] = new SObject(pObjs[2]);
        pObjs[3]->setObjectName(STR_ACTION);
    }
    pItem[3] = new QTreeWidgetItem(pItem[2], QStringList() << tr(STR_ACTION));
    pItem[3]->setData(0, DR_TYPE, STR_ACTION_ROOT);
    pItem[3]->setData(0, DR_SOBJECT, QVariant::fromValue((void*)pObjs[3]));
    auto lstAction = pObjs[3]->children();
    for(auto iterItem = lstAction.begin(); iterItem != lstAction.end(); iterItem++){
        pObjs[4] = (SObject*)(*iterItem);
        appendSObjectV2(pObjs[4], pItem[3], STR_ACTION);
    }
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(slotItemChangedHandle(QTreeWidgetItem*,int)));
}

void SProjectTreeWidget::slotExportSObject()
{
    QString strFileName,strType,strSUffix;
    QTreeWidgetItem* pTreeItem;
    pTreeItem = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    strType = pTreeItem->data(0, DR_TYPE).toString();
    if(strType == STR_BOARD){
        strSUffix = "sbf";
    }else if(strType == STR_PART){
        strSUffix = "spf";
    }else if(strType == STR_PROROOT){
        strSUffix = "sproj";
    }else{
        strSUffix = "sobj";
    }
    strFileName = QFileDialog::getSaveFileName(
                this,
                tr("Export %1").arg(strType),
                QString(),
                tr("GMDTTS %1 file (*.%2)").arg(strType).arg(strSUffix));
    if(strFileName.isEmpty())
        return;
    SObject *pObj;

    pObj = (SObject*)pTreeItem->data(0, DR_SOBJECT).value<void*>();
    saveSObject(pObj, strType, strFileName);
}

void SProjectTreeWidget::slotRemoveSObject()
{
    SObject *pObjs[5];
    QTreeWidgetItem* pTreeItem[5];

    pTreeItem[0] = (QTreeWidgetItem*)property(STR_CURRENT_ITEM).value<void*>();
    pObjs[0] = (SObject*)pTreeItem[0]->data(0, DR_SOBJECT).value<void*>();
    delete pTreeItem[0];
    delete pObjs[0];
}
