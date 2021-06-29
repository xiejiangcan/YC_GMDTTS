#include "slistwidgetforlog.h"

SListWidgetForLog::SListWidgetForLog(SMainWindow *mainWindow, QWidget *parent) :
    SWidget(mainWindow, parent)
{
    QHBoxLayout* pLayout = new QHBoxLayout(this);
    m_logList = new QListWidget(this);
    m_logList->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_logList->setProperty(STR_NAME, LOG_LIST);
    pLayout->addWidget(m_logList);
    m_timerID = startTimer(200);
}

SListWidgetForLog::~SListWidgetForLog()
{
    killTimer(m_timerID);
    if(m_file.isOpen())
        m_file.close();
}

void SListWidgetForLog::propertyOfSObjectChanged(SObject* pObj, const QString& strPropName, const SObject::PropertyT& propChangedBy)
{
    Q_UNUSED(propChangedBy)

    if(strPropName == LOG_SAVE){
        QVariant varValue = pObj->property(strPropName.toUtf8().data());
        if(varValue.toBool()){
            m_file.setFileName(pObj->property(LOG_SAVE_FILE).toString());
            if(m_file.open(QIODevice::Append))
                m_fileTextStream.setDevice(&m_file);
        }else if(m_file.isOpen()){
            m_file.close();
        }
    }
}

QString SListWidgetForLog::keyString()
{
    return SWidget::key(LOG_LIST);
}

void SListWidgetForLog::initSObject(SObject *pObj)
{
    pObj->setObjectName(LOG_LIST);
    pObj->setProperty(LOG_SAVE, false);
    pObj->setProperty(LOG_SAVE_FILE, "");
}

void SListWidgetForLog::timerEvent(QTimerEvent *evt)
{
    if(evt->timerId() != m_timerID)
        return;
    QStringList lstLogs = SMainWindow::logs(m_logID);
    if(lstLogs.isEmpty())
        return;
    if(m_logList->count() > 200){
        m_logList->clear();
    }
    m_logList->addItems(lstLogs);
    m_logList->scrollToBottom();
    if(m_file.isOpen()){
        m_fileTextStream << lstLogs.join('\n');
        m_file.flush();
    }
}
