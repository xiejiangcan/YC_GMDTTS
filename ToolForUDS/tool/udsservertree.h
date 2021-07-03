#ifndef UDSSERVERTREE_H
#define UDSSERVERTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QTextCodec>
#include <QVariant>
#include <QDebug>
#include <QAction>
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QMenu>

class UdsServerTree : public QTreeWidget
{
    Q_OBJECT
public:\
    enum DataRoleT{DR_TYPE = Qt::UserRole + 1, DR_OBJECT = Qt::UserRole + 2, DR_PROP_NAME = Qt::UserRole + 3,
                  DR_ATTR_NAME = Qt::UserRole + 4};
    enum ActionT{
        A_NEW_SERVER,
        A_REMOVE_SERVER,
        A_IMPORT_SERVER,
        A_EXPORT_SERVER,
        A_LENGTH
    };

public:
    explicit UdsServerTree(QWidget *parent = nullptr);

    // function
    void updateTree();
    bool openFile(const QString& strFileName);
    bool saveFile(const QString& strFileName);

    QString lastFileName();

    //static
    static void fromXML(QObject* obj, const QDomElement &eParent);
    static void toXML(QDomElement& eParent, QObject *obj);

protected:
    void contextMenuEvent(QContextMenuEvent* evt);

    QDomElement toXML(const QString& strRootName = "server_tree");

    // init
    void createAction();
    void removeItem(QTreeWidgetItem* pItem);
    void appendItem(QTreeWidgetItem* pItem);

protected slots:
    void itemDoubleClickedHandle(QTreeWidgetItem* item,int column);
    void itemChangedHandle(QTreeWidgetItem *item, int column);
    void currentItemChangedHandle(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void createServer();
    void removeServer();
    void importServers();
    void exportServers();

signals:
    void signServerChanged(const QString& str);

private:
    QAction*    mActions[A_LENGTH];
    QObject*    mManageTree;
};

#endif // UDSSERVERTREE_H
