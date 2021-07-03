#ifndef SUDSWIDGET_H
#define SUDSWIDGET_H

#include "ToolForUDS_global.h"

#include "tool/udsservertree.h"
#include "tool/udsserver.h"
#include "model/udscanidmapmodel.h"

class TOOLFORUDS_EXPORT SUdsWidget : public SWidget
{
    Q_OBJECT
public:
    enum LEditT{E_CANID, E_SDATA, E_PCBS, E_PCST, E_P2TIME, E_LENGTH};
    enum LabelT{L_SERPARAM, L_SERPARAM_V, L_CANID,
                L_SDATA, L_ADATA, L_ADATA_V,
                L_BDATA, L_BDATA_V, L_PCBS, L_PCST, L_P2TIME, L_LENGTH};
    enum PButtonT{B_SEND, B_ADD, B_LENGTH};
    enum GroupBoxT{G_MAPTABLE, G_SERTREE, G_SENDCONF,
                   G_BACK, G_CONFIG, G_HANDLST, G_LENGTH};
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
    SThread m_thread;
    static int controlThread(void *pParam, const bool &bRunning);

    void initWidget();

protected slots:
    void slotSendClicked();
    void slotAddClicked();

private:
    QTableView*         m_tables[T_LENGTH];
    QLabel*             m_labels[L_LENGTH];
    QLineEdit*          m_lEdits[E_LENGTH];
    QPushButton*        m_pButtons[B_LENGTH];
    QGroupBox*          m_groupBoxs[G_LENGTH];
    UdsServerTree*      m_serverTree = nullptr;
    UdsCanIdMapModel*   m_canIDMapModel = nullptr;
    QCheckBox*          m_heartBeat = nullptr;

    UdsServer*          m_server = nullptr;

};

#endif // SUDSWIDGET_H
