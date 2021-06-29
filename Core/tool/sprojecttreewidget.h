#ifndef SPROJECTTREEWIDGET_H
#define SPROJECTTREEWIDGET_H

#include "core_global.h"

class CORE_EXPORT SProjectTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    enum DataRoleT{DR_TYPE = Qt::UserRole + 1, DR_SOBJECT = Qt::UserRole + 2, DR_PROP_NAME = Qt::UserRole + 3,
                  DR_ATTR_NAME = Qt::UserRole + 4};
    enum ActionT{A_NEW_PROJ, A_REMOVE_PROJ, A_IMPORT_PROJ, A_EXPORT_PROJ, A_PREVIEW_PROJ,
                 A_IMPORT_BOARD, A_EXPORT_BOARD,
                 A_NEW_PROP, A_REMOVE_PROP,
                 A_IMPORT_ICON, A_REMOVE_ICON, A_NEW_ACTION,
                 A_APPEND_MENU, A_REMOVE_MENU, A_APPEND_MENU_ITEM, A_REMOVE_MENU_ITEM, A_NEW_INSTANCE,
                 A_NEW_CONTAINER_WIDGET, A_NEW_CONTAINER_GROUPBOX, A_NEW_CONTAINER_SPLIT, A_NEW_CONTAINER_SCROLLAREA,
                 A_REMOVE_SOBJECT, A_NEW_DOCK, A_NEW_SIGNAL, A_NEW_COMM, A_NEW_CMD, A_NEW_DATA, A_NEW_POP,
                 A_LENGTH};

    explicit SProjectTreeWidget(QWidget *parent = 0);
    ~SProjectTreeWidget();

    SObject *projectObj() const;

    void setProjectObj(SObject *projectObj);

    SMainWindow *mainWindow() const;

    void setMainWindow(SMainWindow *mainWindow);

    static void appendParameter(SObject* pObj, QTreeWidgetItem* parentItem,
                         bool bHasTitle = true,
                         const QString& strRootRole = "",
                         const QString& strParamRole = "parameter");

    static QTreeWidgetItem* appendSObjectV2(SObject *pObj,
                         QTreeWidgetItem *parentItem,
                       const QString& strRole = "sobject",
                       const QString& strParamRole = "parameter");
protected:
    void createAction();
    void contextMenuEvent(QContextMenuEvent *evt);
    void appendMenu(SObject* pObj, QTreeWidgetItem* parentItem);
    void appendWidget(SObject* pObj, QTreeWidgetItem* parentItem);
    void appendSObject(SObject *pObj,
                       QTreeWidgetItem *parentItem,
                       const QString& strRole = "sobject",
                       const QString& strParamRootRole = "",
                       const QString& strParamRole = "parameter",
                       const QString&  strChildRole = "sobject_children");
    void appendCommunication(SObject* pObj, QTreeWidgetItem* parentItem);
    void appendAction(SObject* pObj, const QString& strName, QTreeWidgetItem* parentItem);
    static void addWidgetParam(SObject* pSrc);
public slots:
    void slotNewProject();
    void slotRemoveProject();
    void slotImportProject();
    void slotExportProject();
    void slotPreviewProject();
    void slotImportBoard();
    void slotNewData();
    void slotNewProperty();
    void slotRemoveProperty();
    void slotImportIcon();
    void slotRemoveIcon();
    void slotNewAction();
    void slotAppendMenu();
    void slotAppendMenuItem();
    void slotNewInstance();
    void slotNewContainer();
    void slotNewPopWidget();
    void slotNewCommunication();
    void slotItemDoubleClickedHandle(QTreeWidgetItem* item,int column);
    void slotItemChangedHandle(QTreeWidgetItem *item, int column);
    void slotUpdateTree();
    void slotExportSObject();
    void slotRemoveSObject();

private:
    SMainWindow* m_mainWindow = NULL; /**< TODO */
    SObject* m_projectObj = NULL; /**< TODO */
    QAction* m_actions[A_LENGTH]; /**< TODO */
};


#endif // SPROJECTTREEWIDGET_H
