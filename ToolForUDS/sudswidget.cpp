#include "sudswidget.h"

SUdsWidget::SUdsWidget(SMainWindow *mainWindow, QWidget *parent)
    : SWidget(mainWindow, parent)
{

}

SUdsWidget::~SUdsWidget()
{
    if ( m_timerID != -1 )
        killTimer(m_timerID);
}

void SUdsWidget::setSObject(SObject *obj)
{

}

void SUdsWidget::propertyOfSObjectChanged(SObject *obj, const QString &strPropName, const SObject::PropertyT &propChangedBy)
{

}

QString SUdsWidget::keyString()
{
    return SWidget::key(UDS_WIDGET);
}

void SUdsWidget::initSObject(SObject *obj)
{

}

void SUdsWidget::timerEvent(QTimerEvent *evt)
{

}

void SUdsWidget::analyzeData()
{

}

void SUdsWidget::initWidget()
{

}
