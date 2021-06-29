#include "core_global.h"
#include "sobject.h"

bool saveSObject(SObject *pObj, const QString &strType, const QString &strFileName, const QString &strComment, const QString &strAuthor)
{
    if(pObj == NULL
            || strFileName.isEmpty())
        return false;
    SObject *pObjs[2];
    bool bRet;
    pObjs[0] = new SObject();
    pObjs[0]->setObjectName(STR_SOBJ);
    pObjs[0]->setProperty(STR_AUTHOR, strAuthor);
    pObjs[0]->setProperty(STR_PATH, strFileName);
    pObjs[0]->setProperty(STR_COMMENT, strComment);
    pObjs[0]->setProperty(STR_TYPE, strType);
    pObj->lock().lockForRead();
    pObjs[1] = (SObject*)pObj->parent();
    pObj->setParent(pObjs[0]);
    bRet = pObjs[0]->saveFile(strFileName);
    pObj->setParent(pObjs[1]);
    pObj->lock().unlock();
    delete pObjs[0];
    return bRet;
}


SObject *readSObject(const QString &strFileName, QString &strType, QString &strComment, QString &strAuthor, SObject *parent)
{
    if(strFileName.isEmpty())
        return NULL;
    SObject *pObjs[2];
    pObjs[0] = new SObject();
    if(!pObjs[0]->openFile(strFileName)
            || pObjs[0]->objectName() != STR_SOBJ
            || pObjs[0]->children().isEmpty()){
        delete pObjs[0];
        return NULL;
    }
    strAuthor = pObjs[0]->property(STR_AUTHOR).toString();
    strComment = pObjs[0]->property(STR_COMMENT).toString();
    strType = pObjs[0]->property(STR_TYPE).toString();
    pObjs[1] = (SObject*)pObjs[0]->children().first();
    pObjs[1]->setParent(parent);
    delete pObjs[0];
    return pObjs[1];
}


bool isValidName(const QString &strName)
{
    if(strName.isEmpty()
            || strName.startsWith('_')
            || strName.contains(' ')
            || strName.contains(':')
            || strName.contains('(')
            || strName.contains(')')
            || strName.contains('@')
            || strName.contains('.'))
        return false;
    return true;
}


int splitNameTypeID(const QString &strSrc, QString &strName, QString &strType, uint &uID)
{
    if(strSrc.isEmpty())
        return -1;
    QStringList lst1,lst2;
    lst1 = strSrc.split('(');
    strName = lst1[0];
    if(lst1.size() < 2)
        return 1;
    lst2 = lst1[1].split(')');
    strType = lst2[0];
    if(lst2.size() < 2)
        return 2;
    uID = lst2[1].right(lst2[1].size() - lst2[1].indexOf(':')).toUInt();
    return 3;
}

/**
 * @brief assignID
 * 给对象分配一个全局唯一的ID，并设置到对象中属性中
 * @param pSrcObj 待分配ID的对象指针
 * @param szIDName
 * @param strType 对象类型，方便找到当前最大的索引
 * @param mapExist 资源映射
 * @param bInstance 是否可以被脚本引擎访问
 * @return
 */
QString assignID(QObject* pSrcObj, const char* szIDName, const QString& strType, QMap<QString, QVariantMap> &mapExist, bool bInstance)
{
    static QMap<QString, int> mapMaxID;
    QString strOrg = pSrcObj->property(szIDName).toString();
    if(!strOrg.isEmpty()){
        if(bInstance)
            return strOrg + "_i";
        else if(!mapExist.contains(strOrg))
            return strOrg;
    }
    int nMaxID = 0;
    if(mapMaxID.contains(strType)){
        nMaxID = mapMaxID[strType];
    }
    QString strID,strFmt;
    strFmt = strType + "_%1";
    do{
        strID = strFmt.arg(nMaxID++);
    }while (mapExist.contains(strID));
    mapMaxID[strType] = nMaxID;
    pSrcObj->setProperty(szIDName, strID);
    return bInstance ? (strID + "_i") : strID;
}



void analyseObjectID(QObject *pSrcObj, QMap<QString, QVariantMap> &mapDes, const QString strType)
{
    if(pSrcObj == NULL)
        return;
    QString strID;
    strID = assignID(pSrcObj, STR_ID, strType, mapDes, false);
    mapDes[strID][STR_TYPE] = strType;
    mapDes[strID][STR_VALUE] = QVariant::fromValue((void*)pSrcObj);

    auto lstChildren = pSrcObj->children();
    foreach (auto iterChild, lstChildren) {
        analyseObjectID(iterChild, mapDes, strType);
    }
}


void vl2ba(const QVariantList &vl, QByteArray &ba)
{
    ba.clear();
    for(auto iterItem = vl.begin(); iterItem != vl.end(); iterItem++){
        ba.append((char)vl.first().toInt());
    }
}

void ba2vl(const QByteArray &ba, QVariantList &vl)
{
    vl.clear();
    for(auto iterItem = ba.begin(); iterItem != ba.end(); iterItem++){
        vl.append((int)*iterItem);
    }
}


void addWidgetParam(SObject *pSrc)
{
    SObject* pParent = (SObject*)pSrc->parent();

    pSrc->setProperty(STR_UI_VISIBLE, true);
    pSrc->setProperty(STR_UI_LAYOUT, STR_LAYOUT_GRID);
    pSrc->mutablePropertyInfo()[STR_UI_LAYOUT].m_attribute[STR_ROLE] = STR_ROLE_SELECTSIG;
    pSrc->mutablePropertyInfo()[STR_UI_LAYOUT].m_attribute[STR_ROLE_PARAM] = STR_LAYOUTS;
    pSrc->setProperty(STR_UI_LAYOUT_P, STR_DEFAULT);
    pSrc->mutablePropertyInfo()[STR_UI_LAYOUT_P].m_attribute[STR_ROLE] = STR_ROLE_LAYOUT_P;
    pSrc->setProperty(STR_UI_MINW, 0);
    pSrc->setProperty(STR_UI_MINH, 0);
    pSrc->setProperty(STR_UI_BACKIMG, STR_DEFAULT);
    pSrc->mutablePropertyInfo()[STR_UI_BACKIMG].m_attribute[STR_ROLE] = STR_MAP_IMAGE;
    if(pParent){
        QString strLayout,strType,strClass;

        strLayout = pParent->property(STR_UI_LAYOUT).toString();
        if(!strLayout.isEmpty()){
            if(strLayout == STR_LAYOUT_HBOX
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
            pSrc->mutablePropertyInfo()[STR_UI_LAYOUT_P].m_attribute[STR_ROLE] = STR_ROLE_LAYOUT_V;
        }

        strType = pParent->property(STR_INS_TYPE).toString();
        if(strType == STR_DOCK){
            pSrc->setProperty(STR_UI_AREA, STR_ALIGN_LEFT);
            pSrc->mutablePropertyInfo()[STR_UI_AREA].m_attribute[STR_ROLE] = STR_ROLE_SELECTSIG;
            pSrc->mutablePropertyInfo()[STR_UI_AREA].m_attribute[STR_ROLE_PARAM] = STR_AREAS;
            pSrc->setProperty(STR_UI_INDEX, 0);
            pSrc->setProperty(STR_UI_TITLE, STR_DOCK);
            pSrc->setProperty(STR_UI_FLOAT, false);
            pSrc->setProperty(STR_UI_CLOSE, true);
            pSrc->setProperty(STR_UI_ALLOW_AREAS, STR_ALL);
            pSrc->mutablePropertyInfo()[STR_UI_ALLOW_AREAS].m_attribute[STR_ROLE] = STR_ROLE_SELECTMUL;
            pSrc->mutablePropertyInfo()[STR_UI_ALLOW_AREAS].m_attribute[STR_ROLE_PARAM] = STR_AREAS;
            return;
        }else if(strType == STR_CONTAINER){
            strClass = pParent->property(STR_UI_CLASS).toString();
            if(strClass == STR_SPLIT){
                pSrc->setProperty(STR_UI_LAYOUT_P, "0");
            }else if(strClass == STR_TAB){
                pSrc->setProperty(STR_UI_LAYOUT_P, "0");
                pSrc->setProperty(STR_UI_TITLE, STR_TAB);
                pSrc->setProperty(STR_UI_ICON, STR_DEFAULT);
                pSrc->mutablePropertyInfo()[STR_UI_ICON].m_attribute[STR_ROLE] = STR_MAP_IMAGE;
            }
            pSrc->mutablePropertyInfo()[STR_UI_LAYOUT_P].m_attribute[STR_ROLE] = STR_ROLE_LAYOUT_V;
        }else if(strType == STR_STATUSBAR){
            pSrc->setProperty(STR_UI_INDEX, 0);
            pSrc->setProperty(STR_UI_STR, 0);
            pSrc->setProperty(STR_UI_LEFT, true);
        }
    }
}


SObject* createContainerSObject(const QString& strType, const QString& strName, QMap<QString, QVariantMap> &mapIDs, SObject* pParent)
{
    SObject* pObjs[5];
    QString strID;
    pObjs[0] = pParent;
    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName(strName);
    pObjs[1]->setProperty(STR_INS_TYPE, STR_CONTAINER);
    pObjs[1]->setProperty(STR_UI_CLASS, strType);
    strID = assignID(pObjs[1], STR_ID, STR_CONTAINER, mapIDs);
    mapIDs[strID][STR_TYPE] = STR_SOBJ;
    mapIDs[strID][STR_VALUE] = QVariant::fromValue((void*)pObjs[1]);
    addWidgetParam(pObjs[1]);
    if(strType == STR_WIDGET
            || strType == STR_SCROLL){
    }else if(strType == STR_SPLIT){
        pObjs[1]->setProperty(STR_UI_ORIENTATION, STR_HORIZONTAL);
        pObjs[1]->mutablePropertyInfo()[STR_UI_ORIENTATION].m_attribute[STR_ROLE] = STR_ROLE_SELECTSIG;
        pObjs[1]->mutablePropertyInfo()[STR_UI_ORIENTATION].m_attribute[STR_ROLE_PARAM] = STR_ORIENTATIONS;
    }else if(strType == STR_TAB){
        pObjs[1]->setProperty(STR_UI_CLOSE, true);
        pObjs[1]->setProperty(STR_UI_ALIGN, STR_TOP);
        pObjs[1]->mutablePropertyInfo()[STR_UI_ALIGN].m_attribute[STR_ROLE] = STR_ROLE_SELECTSIG;
        pObjs[1]->mutablePropertyInfo()[STR_UI_ALIGN].m_attribute[STR_ROLE_PARAM] = STR_AREAS;
    }else if(strType == STR_GROUP){
        pObjs[1]->setProperty(STR_UI_TITLE, STR_GROUP);
        pObjs[1]->setProperty(STR_UI_ALIGN, STR_TOP);
        pObjs[1]->mutablePropertyInfo()[STR_UI_ALIGN].m_attribute[STR_ROLE] = STR_ROLE_SELECTSIG;
        pObjs[1]->mutablePropertyInfo()[STR_UI_ALIGN].m_attribute[STR_ROLE_PARAM] = STR_AREAS;
    }
    return pObjs[1];
}

SObject* createProjectSObject(QMap<QString, QVariantMap> &mapIDs, const QString& strProjectName)
{
    SObject *pObjs[5];

    pObjs[0] = new SObject();
    pObjs[0]->setObjectName(STR_SOBJ);
    pObjs[0]->setProperty(STR_AUTHOR, STR_DEFAULT);
    pObjs[0]->setProperty(STR_PATH, STR_DEFAULT);
    pObjs[0]->setProperty(STR_COMMENT, STR_DEFAULT);
    pObjs[0]->setProperty(STR_TYPE, STR_PROROOT);
    pObjs[0]->setProperty(STR_ID, STR_SOBJ);
    mapIDs[STR_SOBJ][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_SOBJ][STR_VALUE] = QVariant::fromValue((void*)pObjs[0]);

    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName(STR_PROROOT);
    pObjs[1]->setProperty(STR_NAME, strProjectName);
    pObjs[1]->setProperty(STR_COMMENT, STR_PROJECT);
    pObjs[1]->setProperty(STR_ICON, STR_DEFAULT);
    pObjs[1]->mutablePropertyInfo()[STR_ICON].m_attribute[STR_ROLE] = STR_MAP_IMAGE;
    pObjs[1]->setProperty(STR_ACT_INIT, STR_DEFAULT);
    pObjs[1]->mutablePropertyInfo()[STR_ACT_INIT].m_attribute[STR_ROLE] = STR_MAP_ACTION;
    pObjs[1]->setProperty(STR_ACT_REL, STR_DEFAULT);
    pObjs[1]->mutablePropertyInfo()[STR_ACT_REL].m_attribute[STR_ROLE] = STR_MAP_ACTION;
    pObjs[1]->setProperty(STR_TIMER_INTER, (uint)1000);
    pObjs[1]->setProperty(STR_TIMER_COUNT, uint(0));
    pObjs[1]->setProperty(STR_ID, STR_PROROOT);
    mapIDs[STR_PROROOT][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_PROROOT][STR_VALUE] = QVariant::fromValue((void*)pObjs[1]);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName(STR_COM);
    pObjs[2]->setProperty(STR_ID, STR_COM);
    mapIDs[STR_COM][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_COM][STR_VALUE] = QVariant::fromValue((void*)pObjs[2]);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName(STR_DATA);
    pObjs[2]->setProperty(STR_ID, STR_DATA);
    mapIDs[STR_DATA][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_DATA][STR_VALUE] = QVariant::fromValue((void*)pObjs[2]);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName(STR_RES);
    pObjs[2]->setProperty(STR_ID, STR_RES);
    mapIDs[STR_RES][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_RES][STR_VALUE] = QVariant::fromValue((void*)pObjs[2]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_ICON);
    pObjs[3]->setProperty(STR_ID, STR_ICON);
    mapIDs[STR_ICON][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_ICON][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);
    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_ACTION);
    pObjs[3]->setProperty(STR_ID, STR_ACTION);
    mapIDs[STR_ACTION][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_ACTION][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName(STR_UI);
    pObjs[2]->setProperty(STR_UI_SHOW, STR_MAX);
    pObjs[2]->mutablePropertyInfo()[STR_UI_SHOW].m_attribute[STR_TIP] = SObject::tr("[max] | [normal] | [width,height]");
    pObjs[2]->setProperty(STR_TOOLBAR_VISIBLE, true);
    pObjs[2]->setProperty(STR_STATUSBAR_VISIBLE, true);
    pObjs[2]->setProperty(STR_ID, STR_UI);
    mapIDs[STR_UI][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_UI][STR_VALUE] = QVariant::fromValue((void*)pObjs[2]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_MENU);
    pObjs[3]->setProperty(STR_ID, STR_ACTION);
    mapIDs[STR_ACTION][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_ACTION][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_CENTRAL);
    pObjs[3]->setProperty(STR_UI_VISIBLE, true);
    pObjs[3]->setProperty(STR_UI_LAYOUT, STR_LAYOUT_VBOX);
    pObjs[3]->setProperty(STR_ID, STR_CENTRAL);
    mapIDs[STR_CENTRAL][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_CENTRAL][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_DOCK);
    pObjs[3]->setProperty(STR_ID, STR_DOCK);
    mapIDs[STR_DOCK][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_DOCK][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_POPWIDGET);
    pObjs[3]->setProperty(STR_ID, STR_POPWIDGET);
    mapIDs[STR_POPWIDGET][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_POPWIDGET][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName(STR_STATUSBAR);
    pObjs[3]->setProperty(STR_ID, STR_STATUSBAR);
    mapIDs[STR_STATUSBAR][STR_TYPE] = STR_SOBJ;
    mapIDs[STR_STATUSBAR][STR_VALUE] = QVariant::fromValue((void*)pObjs[3]);

    return pObjs[0];
}
