#include "udsservertree.h"

#include <QHeaderView>

UdsServerTree::UdsServerTree(QWidget *parent)
    : QTreeWidget(parent),
      mManageTree(new QObject(this))
{
    setHeaderLabels(QStringList()<< tr("Property") << tr("Value"));
    this->header()->setSectionResizeMode(QHeaderView::Stretch);
    createAction();

    setContextMenuPolicy(Qt::DefaultContextMenu);
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(itemDoubleClickedHandle(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            SLOT(itemChangedHandle(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(currentItemChangedHandle(QTreeWidgetItem*,QTreeWidgetItem*)));

    QSettings settings;
    QString strFileName = settings.value("/config/lastServerFile").toString();
    QFileInfo info(strFileName);
    if(info.isFile()){
        openFile(strFileName);
    }
    updateTree();
}

void UdsServerTree::updateTree()
{
    clear();
    if(mManageTree == nullptr)
        return;
    QString strName;
    QTreeWidgetItem* pItem;
    QObject* pObjs;

    pObjs = mManageTree;
    pObjs->setObjectName("Server Tree");
    pItem = new QTreeWidgetItem(this, QStringList() << pObjs->objectName());
    pItem->setData(0, DR_OBJECT, QVariant::fromValue((void*)pObjs));
    pItem->setExpanded(true);
    pObjs->setProperty("item", QVariant::fromValue((void*)pItem));

    appendItem(pItem);
}

bool UdsServerTree::openFile(const QString &strFileName)
{
    QFile file(strFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return false;
    }
    QDomDocument domDocument;
    QDomElement eRoot;
    QString strErrorMsg;
    int nErrorLine, nErrorColumn;
    QTextStream readStream(&file);
    readStream.setCodec(QTextCodec::codecForName("UTF-8"));
    if(!domDocument.setContent(readStream.readAll(), &strErrorMsg, &nErrorLine, &nErrorColumn)){
        qDebug() << __FUNCTION__ << strFileName << strErrorMsg << nErrorLine << nErrorColumn;
        file.close();
        return false;
    }
    file.close();
    eRoot = domDocument.documentElement();
    QTreeWidgetItem* root = (QTreeWidgetItem*)mManageTree->property("item").value<void*>();
    removeItem(root);
    fromXML(mManageTree, eRoot);
    updateTree();
    return true;
}

bool UdsServerTree::saveFile(const QString &strFileName)
{
    QFile file(strFileName);
    if(!file.open(QFile::WriteOnly | QFile::Text))
        return false;
    QTextStream outStream(&file);
    outStream.setCodec(QTextCodec::codecForName("UTF-8"));
    QDomElement eRoot = toXML();
    eRoot.save(outStream,1);
    file.close();
    return true;
}

QString UdsServerTree::lastFileName()
{
    QSettings settings;
    QString strFileName = settings.value("/config/lastServerFile").toString();

    return strFileName;
}

void UdsServerTree::contextMenuEvent(QContextMenuEvent *evt)
{
    QMenu popMenu(this);
    if(mManageTree){
        QTreeWidgetItem* pItem = itemAt(evt->pos());
        if(pItem == NULL)
            return;
        if(pItem->parent() == nullptr){
            popMenu.addAction(mActions[A_EXPORT_SERVER]);
            popMenu.addAction(mActions[A_IMPORT_SERVER]);
        }
        popMenu.addAction(mActions[A_NEW_SERVER]);
        popMenu.addAction(mActions[A_REMOVE_SERVER]);
        this->setProperty("_current_item", QVariant::fromValue((void*)pItem));
        popMenu.exec(QCursor::pos());
    }else{
        qDebug() << "manage tree object is nullptr!";
    }
}

void UdsServerTree::createAction()
{
    mActions[A_NEW_SERVER] = new QAction(tr("Create New Server"), this);
    connect(mActions[A_NEW_SERVER], &QAction::triggered, this, &UdsServerTree::createServer);
    mActions[A_REMOVE_SERVER] = new QAction(tr("Remove Server"), this);
    connect(mActions[A_REMOVE_SERVER], &QAction::triggered, this, &UdsServerTree::removeServer);
    mActions[A_IMPORT_SERVER] = new QAction(tr("Import Servers"), this);
    connect(mActions[A_IMPORT_SERVER], &QAction::triggered, this, &UdsServerTree::importServers);
    mActions[A_EXPORT_SERVER] = new QAction(tr("Export Servers"), this);
    connect(mActions[A_EXPORT_SERVER], &QAction::triggered, this, &UdsServerTree::exportServers);
}

void UdsServerTree::itemDoubleClickedHandle(QTreeWidgetItem *item, int column)
{
    item->setFlags(item->flags()|Qt::ItemIsEditable);
}

void UdsServerTree::itemChangedHandle(QTreeWidgetItem *item, int column)
{
    QObject* pObj;
    QString strPropName;
    if(item == NULL
            || (pObj = (QObject*)item->data(0, DR_OBJECT).value<void*>()) == NULL)
        return;
    pObj->setProperty("name", item->data(0, Qt::EditRole));
    pObj->setProperty("value", item->data(1, Qt::EditRole));
}

void UdsServerTree::currentItemChangedHandle(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);
    if(current == nullptr){
        return;
    }
    QString value;
    while(current->parent() != nullptr){
        value = current->data(1, Qt::EditRole).toString() + " " + value;
        current = current->parent();
    }
    if(value.isEmpty())
        return;
    emit signServerChanged(value);
}

void UdsServerTree::createServer()
{
    QTreeWidgetItem* pItem[2];
    QObject* pObjs[2];

    pItem[0] = (QTreeWidgetItem*)property("_current_item").value<void*>();
    pObjs[0] = (QObject*)pItem[0]->data(0, DR_OBJECT).value<void*>();

    QString defaultName = "undefine";
    QString defaultValue = "00";

    pItem[1] = new QTreeWidgetItem(pItem[0], QStringList() << defaultName << defaultValue);
    pItem[1]->setExpanded(true);
    pObjs[1] = new QObject(pObjs[0]);
    pObjs[1]->setProperty("name", defaultName);
    pObjs[1]->setProperty("value", defaultValue);
    pObjs[1]->setProperty("item", QVariant::fromValue((void*)pItem[1]));

    pItem[1]->setData(0, DR_OBJECT, QVariant::fromValue((void*)pObjs[1]));

}

void UdsServerTree::removeServer()
{
    QTreeWidgetItem* pTreeItem;

    pTreeItem = (QTreeWidgetItem*)property("_current_item").value<void*>();
    this->removeItem(pTreeItem);
}

void UdsServerTree::importServers()
{
    QString strFileName;
    strFileName = QFileDialog::getOpenFileName(this,
                                               tr("Import Servers"),
                                               QString(),
                                               tr("Servers file (*.server)"));
    if(strFileName.isEmpty())
        return;
    openFile(strFileName);
    updateTree();
}

void UdsServerTree::exportServers()
{
    if(mManageTree == NULL)
        return;
    QString strFileName;
    strFileName = QFileDialog::getSaveFileName(this,
                                               tr("Export Servers"),
                                               QString(),
                                               tr("Servers file (*.server)"));
    if(strFileName.isEmpty())
        return;

    saveFile(strFileName);
    QSettings settings;
    settings.setValue("/config/lastServerFile", strFileName);
}

void UdsServerTree::fromXML(QObject *obj, const QDomElement &eParent)
{
    QDomElement eChild, eProp;
    QObject* pNew;
    int nIndex;
    QDomNamedNodeMap attrs;
    QDomAttr attr;

    obj->setObjectName(eParent.attribute("name"));
    eProp = eParent.firstChildElement("property");
    eProp = eProp.firstChildElement();
    while(!eProp.isNull()){
        nIndex = 0;
        attrs = eProp.attributes();
        for(nIndex = 0; nIndex < attrs.count(); ++nIndex){
            attr = attrs.item(nIndex).toAttr();
            obj->setProperty(attr.name().toUtf8().data(), QVariant::fromValue(attr.value()));
        }
        eProp = eProp.nextSiblingElement();
    }
    eChild = eParent.firstChildElement("children");
    eChild = eChild.firstChildElement();
    while(!eChild.isNull()){
        pNew = new QObject(obj);
        fromXML(pNew, eChild);
        eChild = eChild.nextSiblingElement();
    }
}

void UdsServerTree::toXML(QDomElement &eParent, QObject *obj)
{
    QString strName, strType;
    QVariant varValue;
    QDomElement eParamRoot, eChildRoot, eParam, eChild;
    QDomDocument doc = eParent.toDocument();
    auto lstNames = obj->dynamicPropertyNames();
    eParamRoot = doc.createElement("property");
    for(auto iter = lstNames.begin(); iter != lstNames.end(); ++iter){
        strName = iter->data();
        varValue = obj->property(strName.toUtf8().data());
        if(!varValue.isValid()){
            continue;
        }
        eParam = doc.createElement("item");
        eParam.setAttribute(strName, varValue.toString());
        eParamRoot.appendChild(eParam);
    }
    eParent.appendChild(eParamRoot);
    eChildRoot = doc.createElement("children");
    QObjectList lstChild = obj->children();
    for(auto iterChild = lstChild.begin(); iterChild != lstChild.end(); ++iterChild)
    {
        eChild = doc.createElement("item");
        eChild.setAttribute("name", (*iterChild)->objectName());
        toXML(eChild, *iterChild);
        eChildRoot.appendChild(eChild);
    }
    eParent.appendChild(eChildRoot);
}

void UdsServerTree::removeItem(QTreeWidgetItem *pItem)
{
    if(pItem == nullptr){
        return;
    }
    QObject* pObj = (QObject*)pItem->data(0, DR_OBJECT).value<void*>();
    if(pObj == nullptr){
        return;
    }

    for(int i = 0 ; i < pItem->childCount(); ++i){
        removeItem(pItem->child(i));
    }

    if(pObj->parent() == nullptr){
        return;
    }

    delete pObj;
    delete pItem;
    pObj = nullptr;
    pItem = nullptr;
}

void UdsServerTree::appendItem(QTreeWidgetItem *pItem)
{
    QObject* pObj = (QObject*)pItem->data(0, DR_OBJECT).value<void*>();

    auto lstObjs = pObj->children();
    for(auto iter = lstObjs.begin(); iter != lstObjs.end(); ++iter)
    {
        auto obj = *iter;
        QTreeWidgetItem* newItem = new QTreeWidgetItem(pItem, QStringList()
                                                       << obj->property("name").toString()
                                                       << obj->property("value").toString());
        newItem->setData(0, DR_OBJECT, QVariant::fromValue((void*)obj));
        obj->setProperty("item", QVariant::fromValue((void*)newItem));
        pItem->setExpanded(true);
        appendItem(newItem);
    }
}

QDomElement UdsServerTree::toXML(const QString &strRootName)
{
    QDomDocument    doc;
    QDomElement     eRoot;

    eRoot = doc.createElement(strRootName);
    eRoot.setAttribute("name", objectName());
    toXML(eRoot, mManageTree);
    return eRoot;
}
