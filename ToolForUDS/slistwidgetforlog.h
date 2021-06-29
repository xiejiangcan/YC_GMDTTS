#ifndef SLISTWIDGETFORLOG_H
#define SLISTWIDGETFORLOG_H

#include "ToolForUDS_global.h"

class TOOLFORUDS_EXPORT SListWidgetForLog : public SWidget
{
    Q_OBJECT
public:
    explicit SListWidgetForLog(SMainWindow *mainWindow, QWidget *parent = 0);

    ~SListWidgetForLog();

    void propertyOfSObjectChanged(SObject* pObj, const QString& strPropName,
                                  const SObject::PropertyT& propChangedBy) override;

    static QString keyString();
    static void initSObject(SObject *pObj);
protected:
    void timerEvent(QTimerEvent* evt) override;

public slots:
private:
    QListWidget*    m_logList = nullptr;
    int             m_timerID = 0;
    int             m_logID = 0;
    QFile           m_file;
    QTextStream     m_fileTextStream;
};

#endif // SLISTWIDGETFORLOG_H
