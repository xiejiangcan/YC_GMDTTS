#ifndef SEMCTABLEWIDGET_H
#define SEMCTABLEWIDGET_H

#include "ToolForUDS_global.h"

#include "model/emcresultmodel.h"

class TOOLFORUDS_EXPORT SEmcTableWidget : public SWidget
{
    Q_OBJECT
public:
    SEmcTableWidget(SMainWindow *mainWindow, QWidget *parent = 0);
    ~SEmcTableWidget();

    // swidget
    void setSObject(SObject *obj) override;
    void propertyOfSObjectChanged(SObject *obj, const QString &strPropName,
                                  const SObject::PropertyT &propChangedBy) override;

    // static
    static QString keyString();
    static void initSObject(SObject* obj);

protected:
    SThread m_thread;
    static int controlThread(void *pParam, const bool &bRunning);

signals:
    void signalUpdateTable(QString str);

protected slots:
    void slotUpdateTable(QString str);

private:
    QTableView*         m_table;
    EmcResultModel*     m_model;
    QLabel*             m_titleLabel;
};

#endif // SEMCTABLEWIDGET_H
