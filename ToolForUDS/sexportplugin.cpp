#include "sexportplugin.h"

#include "scanmessagewidgt.h"
#include "slistwidgetforlog.h"
#include "susbcanwidget.h"
#include "sudswidget.h"
#include "semcwidget.h"
#include "semctablewidget.h"

QStringList SExportPlugin::keys() const
{
    return QStringList() << SCanMessageWidgt::keyString()
                         << SListWidgetForLog::keyString()
                         << SUsbCanWidget::keyString()
                         << SUdsWidget::keyString()
                         << SEmcWidget::keyString()
                         << SEmcTableWidget::keyString();
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
    }else if(SUdsWidget::keyString() == strKey){
        return new SUdsWidget(pMainWindow, pParentW);
    }else if(SEmcWidget::keyString() == strKey){
        return new SEmcWidget(pMainWindow, pParentW);
    }else if(SEmcTableWidget::keyString() == strKey){
        return new SEmcTableWidget(pMainWindow, pParentW);
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
    }else if(SUdsWidget::keyString() == strKey){
        pObj = new SObject(pParent);
        SWidget::initializeSObject(pObj);
        SUdsWidget::initSObject(pObj);
    }else if(SEmcWidget::keyString() == strKey){
        pObj = new SObject(pParent);
        SWidget::initializeSObject(pObj);
        SEmcWidget::initSObject(pObj);
    }else if(SEmcTableWidget::keyString() == strKey){
        pObj = new SObject(pParent);
        SWidget::initializeSObject(pObj);
        SEmcTableWidget::initSObject(pObj);
    }
    if(pObj != NULL){
        pObj->setObjectName(strKey.split('@')[1]);
    }
    return pObj;
}
