#include "sexportplugin.h"

#include "scanmessagewidgt.h"
#include "slistwidgetforlog.h"
#include "susbcanwidget.h"

QStringList SExportPlugin::keys() const
{
    return QStringList() << SCanMessageWidgt::keyString()
                         << SListWidgetForLog::keyString()
                         << SUsbCanWidget::keyString();
}

QObject *SExportPlugin::create(const QString &strKey, SMainWindow *pMainWindow, QObject *pParent)
{
    QWidget* pParentW = (QWidget*)pParent;
    if(SCanMessageWidgt::keyString() == strKey){
        return new SCanMessageWidgt(pMainWindow, pParentW);
    }else if(SListWidgetForLog::keyString() == strKey){
        return new SListWidgetForLog(pMainWindow, pParentW);
    }else if(SUsbCanWidget::keyString() == strKey){
        return new SUsbCanWidget(pMainWindow, pParentW);
    }
    return nullptr;
}

SObject *SExportPlugin::sobject(const QString &strKey, SObject *pParent)
{
    SObject* pObj = nullptr;
    if(SCanMessageWidgt::keyString() == strKey){
        pObj = new SObject(pParent);
        SWidget::initializeSObject(pObj);
        SCanMessageWidgt::initSObject(pObj);
    }else if(SListWidgetForLog::keyString() == strKey){
        pObj = new SObject(pParent);
        SWidget::initializeSObject(pObj);
        SListWidgetForLog::initSObject(pObj);
    }else if(SUsbCanWidget::keyString() == strKey){
        pObj = new SObject(pParent);
        SWidget::initializeSObject(pObj);
        SUsbCanWidget::initSObject(pObj);
    }
    if(pObj != NULL){
        pObj->setObjectName(strKey.split('@')[1]);
    }
    return pObj;
}
