#include "saction.h"
#include "sobject.h"
#include "smainwindow.h"
#include "sproject.h"

SAction::SAction(SMainWindow *mainWindow, QObject *parent)
    : QAction(parent),
      m_mainWindow(mainWindow)
{
    connect(this, SIGNAL(triggered()), SLOT(call()));
    setError("Failed");
}

SAction::~SAction()
{

}

SMainWindow *SAction::mainWindow() const
{
    return m_mainWindow;
}

SObject *SAction::sobject()
{
    return m_sobject;
}

void SAction::setSObject(SObject *obj)
{
    m_sobject = obj;
    QString strName = obj->property(STR_ICON).toString();
    if(!strName.isEmpty()){
        QVariant pix = m_mainWindow->project()->resource(QString(STR_ICON) + STR_CUT_CHAR + strName);
        if(!pix.isNull()){
            setIcon(QIcon(pix.value<QPixmap>()));
        }
    }
}

QString SAction::key(const QString &strSubKey)
{
    return QString(STR_ACTION) + STR_CUT_CHAR + strSubKey;
}

void SAction::initailizeSObject(QObject *obj)
{
    obj->setProperty(STR_ICON, STR_DEFAULT);
    obj->setProperty(STR_CODE, STR_DEFAULT);
}

QString SAction::code() const
{
    return m_sobject->property(STR_CODE).toString();
}

bool SAction::setCode(const QString &code)
{
    m_sobject->setProperty(STR_CODE, code);
    return true;
}

void SAction::call()
{

}

QString SAction::error()
{
    return property(STR_ERROR).toString();
}

void SAction::setError(const QString &strErr)
{
    setProperty(STR_ERROR, strErr);
}

QString SAction::funcName() const
{
    return property(STR_FUNCTION).toString();
}

void SAction::setFuncName(const QString &funcName)
{
    setProperty(STR_FUNCTION, funcName);
}

QVariantList SAction::params() const
{
    return property(STR_PARAMS).toList();
}

void SAction::setParams(const QVariantList &params)
{
    setProperty(STR_PARAMS, params);
}
