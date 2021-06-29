#ifndef SUDSWIDGET_H
#define SUDSWIDGET_H

#include "ToolForUDS_global.h"

class SUdsWidget : public SWidget
{
    Q_OBJECT
public:
    enum LEditT{E_CANID, E_SDATA, E_LENGTH};
    enum LabelT{L_SERPARAM, L_SERPARM_V, L_CANID,
                L_SDATA, L_ADATA, L_ADATA_V,
                L_BDATA, L_BDATA_V, L_LENGTH};
    enum PButtonT{B_SEND, B_ADD, B_LENGTH};
    enum GroupBoxT{G_MAPTREE, G_SERLST, G_SENDCONF, G_BACK, G_LENGTH};
    enum TableViewT{T_DATA, T_MAP, T_LENGTH};
public:
    SUdsWidget(SMainWindow *mainWindow, QWidget *parent = 0);
    ~SUdsWidget();

    void setSObject(SObject *obj) override;
    void propertyOfSObjectChanged(SObject *obj,
                                  const QString &strPropName,
                                  const SObject::PropertyT &propChangedBy) override;

    // static
    static QString keyString();
    static void initSObject(SObject* obj);

protected:
    void timerEvent(QTimerEvent* evt) override;
    void analyzeData();
    void initWidget();

private:
    QTableView*     m_tables[T_LENGTH];
    QLabel*         m_labels[L_LENGTH];
    QLineEdit*      m_lEdits[E_LENGTH];
    QPushButton*    m_pButtons[B_LENGTH];
    QGroupBox*      m_GroupBoxs[G_LENGTH];
    QTreeView*      m_serverTree = nullptr;

    int             m_timerID = -1;
};

#endif // SUDSWIDGET_H
