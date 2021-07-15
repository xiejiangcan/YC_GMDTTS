#ifndef SCANMESSAGEWIDGT_H
#define SCANMESSAGEWIDGT_H

#include "ToolForUDS_global.h"
#include "model/canmessagemodel.h"
#include "model/canmessageproxymodel.h"

class TOOLFORUDS_EXPORT SCanMessageWidgt : public SWidget
{
    Q_OBJECT
public:
    enum CheckBoxT{C_HEX, C_AUTO, C_LENGTH};
    enum LineEditT{L_DEV, L_TIMEL, L_TIMEH, L_CANIDL, L_CANIDH, L_LENGTH};
    enum GroupBoxT{G_DEV, G_TIME, G_CANID, G_LENGTH};
public:
    SCanMessageWidgt(SMainWindow *mainWindow, QWidget *parent = 0);
    ~SCanMessageWidgt();

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

    void initWidget();

signals:
    void signUpdateTable();

protected slots:
    void slotFilterTable();
    void slotIsHexStateChanged(int state);

private:
    QTableView*             m_table = nullptr;
    CanMessageModel*        m_model = nullptr;
    QCheckBox*              m_checkBox[C_LENGTH];
    QLineEdit*              m_lineEdit[L_LENGTH];
    QGroupBox*              m_groupBox[G_LENGTH];
    QPushButton*            m_filterBtn = nullptr;
    QWidget*                m_topWidget = nullptr;
    CanMessageProxyModel*   m_proxyModel = nullptr;

    QVector<uint>       m_versions;

};

#endif // SCANMESSAGEWIDGT_H
