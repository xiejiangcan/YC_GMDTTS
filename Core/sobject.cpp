#include "sobject.h"

#include "sevent.h"

const QEvent::Type g_evtSetProperty =
        (QEvent::Type)QEvent::registerEventType(QEvent::User+100);

SObject::SObject(SObject *parent)
    : QObject(parent)
{

}

SObject::~SObject()
{
    if(m_rootParam){
        killTimer(m_rootParam->m_timerIDForCheck);
        delete m_rootParam;
    }
}

QVariant SObject::propertyEx(const QString &strPath)
{
    QStringList lstPath = strPath.split(CUT_CHAR);
    if(lstPath.isEmpty())
        return QVariant();
    QObject* pChild = this;
    int nIndex,nPathLen;
    nPathLen = lstPath.size() - 1;
    for(nIndex = 0; nIndex < nPathLen; nIndex++) {
        pChild = pChild->findChild<QObject*>(lstPath[nIndex]);
        if(pChild == NULL)
            return QVariant();
    }
    return pChild->property(lstPath.last().toUtf8().data());
}

QMap<QString, SObject::PropertyInfo> &SObject::mutablePropertyInfo()
{
    return m_propertyInfo;
}

const QMap<QString, SObject::PropertyInfo> &SObject::propertyInfo() const
{
    return m_propertyInfo;
}

QReadWriteLock &SObject::lock()
{
    return m_lock;
}

bool SObject::setProperty(const char *name,
                          const QVariant &value,
                          const QObject *changedBy)
{
    if(value.isValid()){
        m_propertyInfo[name].m_version++;
        SObject* pRoot = root();
        if(pRoot != NULL){
            PropertyT prop(changedBy == NULL ? this : (QObject*)changedBy);
            QMutexLocker lock(&pRoot->m_rootParam->m_mutex);
            pRoot->m_rootParam->m_changed[this][name] = prop;
        }
    }else{
        m_propertyInfo.remove(name);
    }
    return QObject::setProperty(name, value);
}

bool SObject::setProperty(const QString &name, const QVariant &value, const QObject *changedBy)
{
    return setProperty(name.toUtf8().data(), value, changedBy);
}

void SObject::setPropertyS(const QString &name,
                           const QVariant &value,
                           const QObject *changedBy)
{
    SEvent* evt = new SEvent(g_evtSetProperty);
    evt->setParam(STR_NAME, name);
    evt->setParam(STR_VALUE, value);
    evt->setParam(STR_CHANGEDBY, QVariant::fromValue((void*)changedBy));
    QApplication::postEvent(this, evt);
}

bool SObject::setPropertyEx(const QString &strPath,
                            const QVariant &varValue,
                            const QObject *changedBy)
{
    QStringList lstPath = strPath.split(CUT_CHAR);
    if(lstPath.isEmpty())
        return false;
    SObject* pChild = this;
    int nIndex,nPathLen;
    nPathLen = lstPath.size() - 1;
    for(nIndex = 0; nIndex < nPathLen; nIndex++) {
        pChild = pChild->findChild<SObject*>(lstPath[nIndex]);
        if(pChild == NULL){
            pChild = new SObject(this);
            pChild->setObjectName(lstPath[nIndex]);
        }
    }
    return pChild->setProperty(lstPath.last().toUtf8().data(),
                               varValue, changedBy);
}

void SObject::setPropertyByText(const QString &name,
                                const QString &value,
                                const QObject *changedBy)
{
    QVariant tmpValue = stringToVariant(property(name.toUtf8().data()).typeName(), value);
    setProperty(name.toUtf8().data(),
                tmpValue,
                changedBy);
}

void SObject::setPropertyInfo(
        const QMap<QString,SObject::PropertyInfo> &propertyInfo)
{
    m_propertyInfo = propertyInfo;
}

void SObject::setSObjectName(const QString &name)
{
    if(name.isEmpty()
            || name.indexOf(CUT_CHAR) != -1)
        return;
    QObject::setObjectName(name);
}

void SObject::setAsRoot(bool bRoot)
{
    if(( bRoot && m_rootParam != NULL)
            || ( !bRoot && m_rootParam == NULL)
            )
        return;
    if(bRoot){
        m_rootParam = new RootParamT();
        m_rootParam->m_timerIDForCheck = startTimer(500);
    }else{
        killTimer(m_rootParam->m_timerIDForCheck);
    }
}

void SObject::changeProperty(const QString &strPropName,
                             const QObject *pFromObj,
                             const QString &strFromProp)
{
    if(pFromObj == NULL
            || strPropName.isEmpty()
            || pFromObj == (QObject*)this)
        return;
    SObject* pRoot = root();
    if(pRoot != NULL){
        PropertyT prop((QObject*)pFromObj, strFromProp);
        QMutexLocker lock(&pRoot->m_rootParam->m_mutex);
        pRoot->m_rootParam->m_changed[this][strPropName] = prop;
    }else if(!strFromProp.isEmpty()){
        QVariant varValue = pFromObj->property(strFromProp.toUtf8().data());
        if(varValue.isNull())
            return;
        setProperty(strPropName.toUtf8().data(), varValue);
    }
}

SObject *SObject::find(const QString &strPath)
{
    QStringList lstPath = strPath.split(CUT_CHAR);
    QObject* pChild = this;
    int nIndex,nPathLen;
    nPathLen = lstPath.size();
    for(nIndex = 0; nIndex < nPathLen; nIndex++) {
        pChild = pChild->findChild<QObject*>(lstPath[nIndex]);
        if(pChild == NULL)
            break;
    }
    return (SObject*)pChild;
}

SObject *SObject::clone() const
{

    SObject* pNew;
    pNew = new SObject();
    SObject::clone(*pNew, *this);
    return pNew;
}

QDomElement SObject::toXML(const QString &strRootName)
{
    QDomDocument doc;
    QDomElement eRoot;

    eRoot = doc.createElement(strRootName);
    eRoot.setAttribute(STR_NAME, objectName());
    toXML(eRoot, *this);
    return eRoot;
}

void SObject::fromXML(const QDomElement &eRoot)
{
    fromXML(*this, eRoot);
}

bool SObject::saveFile(const QString &strFileName)
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

bool SObject::openFile(const QString &strFileName, QMap<QString, QVariantMap> *pIDs)
{
    QFile file(strFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    QDomDocument domDocument;
    QDomElement eRoot;
    QString strErrorMsg;
    int nErrorLine,nErrorColumn;
    QTextStream readStream(&file);
    readStream.setCodec(QTextCodec::codecForName("UTF-8"));
    if(!domDocument.setContent(readStream.readAll(), &strErrorMsg, &nErrorLine, &nErrorColumn))
    {
        //qDebug()<< __FUNCTION__<<strFileName << strErrorMsg<< nErrorLine<< nErrorColumn;
        file.close();
        return false;
    }
    file.close();
    eRoot = domDocument.documentElement();
    fromXML(*this, eRoot, pIDs);
    return true;
}

void SObject::updateProperty(const QString &strPropName)
{
    SObject* pRoot = root();
    if(pRoot != NULL){
        PropertyT prop;
        QMutexLocker locker(&pRoot->m_rootParam->m_mutex);
        pRoot->m_rootParam->m_changed[this][strPropName] = prop;
    }
}

void SObject::toXML(QDomElement &eParent, const SObject &obj)
{
    QString strName,strType;
    QVariant varValue;
    QDomElement eParamRoot, eChildRoot,eParam,eChild;
    QDomDocument doc = eParent.toDocument();
    auto lstNames = obj.dynamicPropertyNames();
    eParamRoot = doc.createElement(STR_PROP);
    for(auto iterParam = lstNames.begin(); iterParam != lstNames.end(); iterParam++){
        strName = iterParam->data();
        varValue = obj.property(strName.toUtf8().data());
        if(!varValue.isValid())
            continue;
        eParam = doc.createElement(STR_ITEM);
        auto iterEnd = obj.m_propertyInfo[strName].m_attribute.end();
        for(auto iterName = obj.m_propertyInfo[strName].m_attribute.begin(); iterName != iterEnd; iterName++){
            eParam.setAttribute(iterName.key(), iterName.value());
        }
        eParam.setAttribute(STR_VALUE, variantToString(varValue, strType));
        eParam.setAttribute(STR_NAME, strName);
        eParam.setAttribute(STR_TYPE, strType);
        eParamRoot.appendChild(eParam);
    }
    eParent.appendChild(eParamRoot);
    eChildRoot = doc.createElement(STR_CHILDREN);
    QObjectList lstChild = obj.children();
    for(auto iterChild = lstChild.begin(); iterChild != lstChild.end(); iterChild++){
        eChild = doc.createElement(STR_ITEM);
        eChild.setAttribute(STR_NAME, (*iterChild)->objectName());
        toXML(eChild, *((SObject*)(*iterChild)));
        eChildRoot.appendChild(eChild);
    }
    eParent.appendChild(eChildRoot);
}

void SObject::fromXML(SObject &obj,
                      const QDomElement &eParent,
                      QMap<QString, QVariantMap> *pIDs)
{
    QDomElement eChild,eProp;
    QString strType,strName,strValue;
    SObject* pNew;
    int nIndex;
    QDomNamedNodeMap attrs;
    QDomAttr attr;
    static int nIDCount = 0;

    obj.setObjectName(eParent.attribute(STR_NAME));
    eProp = eParent.firstChildElement(STR_PROP);
    eProp = eProp.firstChildElement();
    while(!eProp.isNull()){
        strName = eProp.attribute(STR_NAME);
        nIndex = 0;
        attrs = eProp.attributes();
        for(nIndex = 0; nIndex < attrs.count(); nIndex++){
            attr = attrs.item(nIndex).toAttr();
            if(attr.name() == STR_NAME){
                continue;
            }else if(attr.name() == STR_TYPE){
                strType = attr.value();
                continue;
            }else if(attr.name() == STR_VALUE){
                strValue = attr.value();
                continue;
            }
            obj.m_propertyInfo[strName].m_attribute[attr.name()] = attr.value();
        }
        obj.setProperty(strName.toUtf8().data(), stringToVariant(strType, strValue));
        eProp = eProp.nextSiblingElement();
    }
    if(pIDs){
        QString strID = obj.property(STR_ID).toString();
        if(strID.isEmpty()){
            do{
                strID = tr("obj_%1").arg(nIDCount++);
            }while (pIDs->contains(strID));
            obj.QObject::setProperty(STR_ID, strID);
        }
        QVariantMap mapNew;
        mapNew[STR_TYPE] = STR_SOBJ;
        mapNew[STR_VALUE] = QVariant::fromValue((void*)&obj);
        pIDs->insert(strID, mapNew);
    }
    eChild = eParent.firstChildElement(STR_CHILDREN);
    eChild = eChild.firstChildElement();
    while(!eChild.isNull()){
        pNew = new SObject(&obj);
        fromXML(*pNew, eChild, pIDs);
        eChild = eChild.nextSiblingElement();
    }
}

QVariant SObject::stringToVariant(const QString &strTypeName,
                                  const QString &strValue)
{
    if(strTypeName == STR_TYPE_BOOL){
        return (strValue == STR_BOOL_TRUE) ? true : false;
    }else if(strTypeName == STR_TYPE_INT){
        return strValue.toInt();
    }else if(strTypeName == STR_TYPE_UINT){
        return strValue.toUInt();
    }else if(strTypeName == STR_TYPE_DOUB){
        return strValue.toDouble();
    }else if(strTypeName == STR_TYPE_CHAR){
        return (char)strValue.toInt();
    }else if(strTypeName == STR_TYPE_QSTR){
        return strValue;
    }else if(strTypeName == STR_TYPE_FLOAT){
        return strValue.toFloat();
    }else if(strTypeName == STR_TYPE_QLL){
        return strValue.toLongLong();
    }else if(strTypeName == STR_TYPE_SHORT){
        return strValue.toShort();
    }else if(strTypeName == STR_TYPE_QULL){
        return strValue.toULongLong();
    }else if(strTypeName == STR_TYPE_USHORT){
        return strValue.toUShort();
    }else if(strTypeName == STR_TYPE_UCHAR){
        return (uchar)strValue.toUInt();
    }else if(strTypeName == STR_TYPE_QIMG){
        QImage img;
        if(img.loadFromData(QByteArray::fromBase64(strValue.toLatin1())))
            return img;
        return img;
    }else if(strTypeName == STR_TYPE_QPIXMAP){
        QPixmap img;
        if(img.loadFromData(QByteArray::fromBase64(strValue.toLatin1())))
            return img;
        return img;
    }else if(strTypeName == STR_TYPE_QBYTEARRAY){
        return QByteArray::fromBase64(strValue.toLatin1());
    }
    return QVariant();
}

QString SObject::variantToString(const QVariant &varValue,
                                 QString &strTypeName)
{
    strTypeName = varValue.typeName();
    if(strTypeName == STR_TYPE_QPIXMAP){
        QByteArray ba;
        QBuffer buffer(&ba);
        QPixmap image = varValue.value<QPixmap>();
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG"); // writes image into ba in PNG format

        return QString::fromLatin1(ba.toBase64());
    }else if(strTypeName == STR_TYPE_QIMG){
        QByteArray ba;
        QBuffer buffer(&ba);
        QImage image = varValue.value<QImage>();
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG"); // writes image into ba in PNG format

        return QString::fromLatin1(ba.toBase64());
    }else if(strTypeName == STR_TYPE_QBYTEARRAY){
        return QString::fromLatin1(varValue.toByteArray().toBase64());
    }
    return varValue.toString();
}

void SObject::clone(SObject &des, const SObject &src)
{
    SObject *pChild;
    des.setObjectName(src.objectName());
    des.m_propertyInfo = src.m_propertyInfo;
    auto lstNames = src.dynamicPropertyNames();
    for(auto iterParam = lstNames.begin(); iterParam != lstNames.end(); iterParam++){
        des.setProperty(iterParam->data(), src.property(iterParam->data()));
    }
    QObjectList lstChild = src.children();
    for(auto iterChild = lstChild.begin(); iterChild != lstChild.end(); iterChild++){
        pChild = new SObject(&des);
        clone(*pChild, *((SObject*)(*iterChild)));
    }
}

QStringList SObject::supportedTypes()
{
    return QStringList() << STR_TYPE_BOOL << STR_TYPE_INT << STR_TYPE_UINT
                         << STR_TYPE_DOUB << STR_TYPE_CHAR << STR_TYPE_QSTR
                         << STR_TYPE_FLOAT << STR_TYPE_QLL << STR_TYPE_SHORT
                         << STR_TYPE_QULL << STR_TYPE_USHORT << STR_TYPE_UCHAR
                         << STR_TYPE_QIMG << STR_TYPE_QPIXMAP << STR_TYPE_QBYTEARRAY;
}

void SObject::timerEvent(QTimerEvent *event)
{
    if(m_rootParam != NULL
            && event->timerId() == m_rootParam->m_timerIDForCheck) {
        SObject* pRoot = root();

        if(pRoot == NULL
                || pRoot->m_rootParam->m_changed.isEmpty())
            return;
        SObject* pChangedObj;
        QVariant varValue;
        QString strName;
        QByteArray baNameUtf;
        PropertyT propFrom;

        QMutexLocker locker(&pRoot->m_rootParam->m_mutex);
        for(auto iterObj = pRoot->m_rootParam->m_changed.begin();
            iterObj != pRoot->m_rootParam->m_changed.end();
            iterObj++){
            pChangedObj = iterObj.key();
            for(auto iterProp = iterObj->begin(); iterProp != iterObj->end(); iterProp++){
                propFrom = iterProp.value();
                strName = iterProp.key();
                baNameUtf = strName.toUtf8();
                if(!propFrom.m_property.isEmpty()
                        && propFrom.m_object != (QObject*)pChangedObj){
                    varValue = propFrom.m_object->property(propFrom.m_property.toUtf8().data());
                    if(varValue.isNull())
                        continue;
                    pChangedObj->QObject::setProperty(baNameUtf.data(), varValue);
                    pChangedObj->mutablePropertyInfo()[strName].m_version++;
                }
                emit pChangedObj->propertyChanged(pChangedObj, strName, propFrom);
            }
        }
        pRoot->m_rootParam->m_changed.clear();
    }
}

void SObject::customEvent(QEvent *event)
{
    if(event->type() == g_evtSetProperty){
        SEvent* pEvt = static_cast<SEvent*>(event);
        if(pEvt != NULL
                && !pEvt->params().empty()){
            //qDebug() <<"obj:" << this<< "\tevt:" << pEvt->params().firstKey();
            setProperty(pEvt->params()[STR_NAME].toString().toUtf8().data(), pEvt->params()[STR_VALUE],
                    (QObject*)pEvt->params()[STR_CHANGEDBY].value<void*>());
        }
    }
}

SObject *SObject::root()
{
    SObject* pRoot = this;
    while(pRoot != NULL){
        if(pRoot->m_rootParam != NULL)
            return pRoot;
        pRoot = (SObject*)pRoot->parent();
    }
    return NULL;
}
