#include "swidget.h"
#include "smainwindow.h"
#include "sproject.h"

static QStringList WidgetProps = {"acceptDrops", "accessibleDescription",
                                  "accessibleName", "autoFillBackground",
                                 "baseSize", "childrenRect", "contextMenuPolicy",
                                 "cursor", "enabled", "focus", "focusPolicy",
                                 "font", "frameGeometry", "frameSize",
                                  "fullScreen", "geometry", "height",
                                 "inputMethodHints", "isActiveWindow",
                                  "layoutDirection", "locale", "maximized",
                                 "maximumHeight", "maximumSize", "maximumWidth",
                                  "minimized", "minimumHeight", "minimumSize",
                                  "minimumSizeHint", "minimumWidth", "modal",
                                 "mouseTracking", "normalGeometry", "palette",
                                  "pos", "rect", "size", "sizeHint", "sizeIncrement",
                                  "sizePolicy", "statusTip", "styleSheet",
                                  "toolTip", "toolTipDuration", "updatesEnabled",
                                  "visible", "whatsThis", "width", "windowFilePath",
                                  "windowFlags", "windowIcon", "windowIconText",
                                  "windowModality", "windowModified",
                                  "windowOpacity", "windowTitle", "x", "y"};

SWidget::SWidget(SMainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
      m_mainwindow(mainWindow)
{
    setProperty(STR_SW_FLAG, true);
}

SWidget::~SWidget()
{

}

SObject *SWidget::sobject()
{
    return m_object;
}

SMainWindow *SWidget::mainWindow()
{
    return m_mainwindow;
}

QMap<QString, QVariantMap> &SWidget::mutableMapping()
{
    return m_mapping;
}

const QMap<QString, QVariantMap> &SWidget::mapping() const
{
    return m_mapping;
}

QMap<QString, SAction *> &SWidget::mutableActions()
{
    return m_actions;
}

const QMap<QString, SAction *> &SWidget::actions() const
{
    return m_actions;
}

bool SWidget::isMapped(const QString &strPropName)
{
    auto iterObj = m_mapping.find(strPropName);
    return iterObj == m_mapping.end() ? false : true;
}

bool SWidget::triggerAction(const QString &strSignal, bool checked)
{
    SAction* pAction;
    auto iterAction = m_actions.find(strSignal);
    if(iterAction != m_actions.end()
            || (pAction = *iterAction) == nullptr){
        return false;
    }
    emit pAction->triggered(checked);
    return true;
}

void SWidget::addMapping(const QString &strPropName,
                         const QString &strMappedResID)
{
    SProject* pProj;
    if(strPropName.isEmpty()
            || strMappedResID.isEmpty()
            || m_mainwindow == nullptr
            || (pProj = m_mainwindow->project()) == nullptr){
        return;
    }
    QVariantMap mapRes = pProj->resourceByID(strMappedResID);
    if(mapRes.isEmpty()){
        SProject::DataT dat;
        if(!pProj->_data(strMappedResID, dat)){
            qDebug() << QStringLiteral("It was failed to adds the mapping of %1 from %2.")
                        .arg(strPropName).arg(strMappedResID);
            return;
        }
        mapRes[STR_TYPE] = dat.type;
        mapRes[STR_VALUE] = QVariant::fromValue((void*)dat.object);
        if(mapRes[STR_TYPE] == STR_PROP){
            mapRes[STR_PROP] = dat.property;
            connect(dat.object, &SObject::propertyChanged,
                    this, &SWidget::mappingChangedFilter);
        }
    }
    m_mapping[strPropName] = mapRes;
}

void SWidget::addAction(const QString &strSignal,
                        const QString &strActionName)
{
    SProject* pProj;
    if(strSignal.isEmpty()
            || strActionName.isEmpty()
            || m_mainwindow == nullptr
            || (pProj = m_mainwindow->project()) == nullptr){
        return;
    }
    SAction* pAction = nullptr;
    pAction = (SAction*)pProj->resource(STR_ACTION + STR_CUT_CHAR + strActionName).value<void*>();
    if(pAction == nullptr){
        auto mapAct = pProj->resourceByID(strActionName);
        if(mapAct.isEmpty()
                || mapAct[STR_TYPE].toString() != STR_ACTION){
            qDebug() << QStringLiteral("It was failed to adds the action of %1 for the signal of %2.")
                        .arg(strActionName).arg(strSignal);
            return;
        }
        pAction = (SAction*)mapAct[STR_VALUE].value<void*>();
    }
    m_actions[strSignal] = pAction;
}

int SWidget::writeForBack(QByteArray &buffW,
                          const QString &mapPropW,
                          QByteArray &buffR,
                          const QString &mapPropR)
{
    if(!isMapped(mapPropW)|| !isMapped(mapPropR)){
        qDebug()<<"E_SWidget_writeForBack1_emptyMap";
        return -1;
    }
    if(buffW.size()>0){
        auto mapDataOut = mapping()[mapPropW];
        if(mapDataOut.isEmpty()){
            qDebug()<<"E_SWidget_writeForBack2_emptyMap";
            return -1;
        }
        SObject* ObjDataOut =(SObject*)mapDataOut["value"].value<void*>();
        if(!ObjDataOut){
            qDebug()<<"E_SWidget_writeForBack3_emptyMap";
            return -1;
        }
        QByteArray PropDataOut =mapDataOut["property"].toString().toUtf8();
        if(PropDataOut.isEmpty()){
            qDebug()<<"E_SWidget_writeForBack4_emptyMap";
            return -1;
        }

        auto mapDataIn = mapping()[mapPropR];
        if(mapDataIn.isEmpty()){
            qDebug()<<"E_SWidget_writeForBack5_emptyMap";
            return -2;
        }
        SObject* ObjDataIn =(SObject*)mapDataIn["value"].value<void*>();
        if(!ObjDataIn){
            qDebug()<<"E_SWidget_writeForBack6_emptyMap";
            return -2;
        }
        QByteArray PropDataIn =mapDataIn["property"].toString().toUtf8();
        if(PropDataIn.isEmpty()){
            qDebug()<<"E_SWidget_writeForBack7_emptyMap";
            return -2;
        }

        bool wOver= false, rOver= false;
        QTime tmEmptyPkt;
        uint tmpV=0;
        tmEmptyPkt.start();
        do{
            if(!wOver){
                try{
                    if(ObjDataOut->lock().tryLockForWrite()){
                        wOver= true;
                        tmpV= ObjDataIn->propertyInfo()[PropDataIn.data()].m_version;
                        ObjDataOut->setProperty(PropDataOut.data(),buffW);
                        ObjDataOut->lock().unlock();
                    }
                }catch(...){
                    return -5;
                }
            }
            if(wOver){
                do{
                    try{
                        if(ObjDataIn->lock().tryLockForWrite()){
                            if(tmpV!= ObjDataIn->propertyInfo()[PropDataIn.data()].m_version){
                                m_rArray= ObjDataIn->property(PropDataIn.data()).toByteArray();
                                ObjDataIn->lock().unlock();
                                if(!m_rArray.isEmpty()){
                                    buffR.resize(m_rArray.size());
                                    memcpy(buffR.data(),m_rArray.data(), m_rArray.size());
                                    rOver= true;
                                }
                            }else
                                ObjDataIn->lock().unlock();
                            if(rOver)
                                return 0;
                        }
                    }catch(...){
                        return -6;
                    }
                    if(tmEmptyPkt.elapsed()> 3000){
                        qDebug()<<"E_SWidget_writeForBack_disconnect1";return -2;
                    }
                    QCoreApplication::processEvents();
                }while(1);
            }
            if(tmEmptyPkt.elapsed()> 6000){
                qDebug()<<"E_SWidget_writeForBack_disconnect2";return -2;
            }
            QCoreApplication::processEvents();
        }while(1);
        return -3;
    }else{
        return -4;
    }
}

QString SWidget::key(const QString &strSubKey)
{
    return STR_WIDGET + "@" + strSubKey;
}

void SWidget::initializeSObject(SObject *obj)
{
    obj->setProperty(STR_UI_BACKIMG, "");
    obj->setProperty(STR_UI_SHOW, true);
}

void SWidget::addSpecialProperty(SObject *obj,
                                 const QString &strPropName,
                                 const QString &strPropValue,
                                 const QString &strRole)
{
    if(obj == NULL
            || strPropName.isEmpty())
        return;
    obj->setProperty(strPropName.toUtf8().data(), strPropValue);
    obj->mutablePropertyInfo()[strPropName].m_attribute[STR_ROLE] = strRole;
}

void SWidget::analyseSObject(SObject *obj)
{
    connect(obj, SIGNAL(propertyChanged(SObject*,QString, SObject::PropertyT)),
            SLOT(propertyOfSObjectChanged(SObject*,QString, SObject::PropertyT)));
    auto lstPropName = obj->dynamicPropertyNames();
    QString strPropRole;
    QVariant varPropVal;
    SObject::PropertyT prop(this);
    foreach (QString strPropName, lstPropName) {
        varPropVal = obj->property(strPropName.toUtf8().data());
        auto attributes = obj->propertyInfo()[strPropName].m_attribute;
        auto iterRole = attributes.find(STR_ROLE);
        if(iterRole != attributes.end()){
            strPropRole = iterRole.value();
            if(strPropRole == STR_ROLE_ACTION){
                addAction(strPropName, varPropVal.toString());
            }else if(strPropRole == STR_ROLE_MAPPING){
                addMapping(strPropName, varPropVal.toString());
            }
        }
        propertyOfSObjectChanged(obj, strPropName, prop);
    }

    auto iterChildren = obj->children();
    foreach (auto iterChild, iterChildren) {
        analyseSObject((SObject*)iterChild);
    }
}

void SWidget::setSObject(SObject *obj)
{
    m_object = obj;
    analyseSObject(obj);
}

void SWidget::mappingChanged(const QString &strLocalPropName,
                             SObject *obj, const QString &strPropName,
                             const SObject::PropertyT &propChangedBy)
{
    Q_UNUSED(strLocalPropName)
    Q_UNUSED(obj)
    Q_UNUSED(strPropName)
    Q_UNUSED(propChangedBy)
}

void SWidget::propertyOfSObjectChanged(SObject *obj,
                                       const QString &strPropName,
                                       const SObject::PropertyT &propChangedBy)
{
    Q_UNUSED(obj)
    Q_UNUSED(propChangedBy)

    QWidget *pWidget = this;
    if(pWidget == NULL
            || strPropName.isEmpty())
        return;

    QVariantMap mapBkgImage;
    QVariant varValue = obj->property(strPropName.toUtf8().data());
    if(strPropName == STR_UI_BACKIMG){
        if((mapBkgImage = m_mainwindow->project()->resourceByID(varValue.toString())).isEmpty())
            return;
        QPalette palette = pWidget->palette();
        QPixmap pix = mapBkgImage[STR_VALUE].value<QPixmap>();
        palette.setBrush(QPalette::Background, QBrush(pix));
        pWidget->setAutoFillBackground(true);
        pWidget->setPalette(palette);
        pWidget->setFixedSize(pix.width(), pix.height());
    }else if(strPropName == STR_UI_SHOW){
        QString strShow = varValue.toString();
        if(strShow.compare(STR_BOOL_TRUE, Qt::CaseInsensitive) == 0)
            pWidget->show();
        else
            pWidget->hide();
    }

    if(!WidgetProps.contains(strPropName))
        return;
    pWidget->setProperty(strPropName.toUtf8().data(), varValue);
}

void SWidget::mappingChangedFilter(SObject *obj,
                                   const QString &strPropName,
                                   const SObject::PropertyT &propChangedBy)
{
    for(auto iterMap = m_mapping.begin(); iterMap != m_mapping.end(); iterMap++) {
        if(((SObject*)iterMap.value()[STR_VALUE].value<void*>()) == obj
                && iterMap.value()[STR_PROP].toString() == strPropName){
            mappingChanged(iterMap.key(), obj, strPropName, propChangedBy);
            break;
        }
    }
}
