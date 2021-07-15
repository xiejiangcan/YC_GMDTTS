#ifndef SUSBCANWIDGET_H
#define SUSBCANWIDGET_H

#include "ToolForUDS_global.h"

#include "tool/ycanhandle.h"

class TOOLFORUDS_EXPORT SUsbCanWidget : public SWidget
{
    Q_OBJECT
public:
    enum GroupBoxT{G_DEVTYPE, G_CHANNEL, G_BAUD, G_DEVICES, G_LENGTH};
    enum RButtonT{B_TYPE1, B_TYPE2, B_CHA1, B_CHA2, B_LENGTH};
    enum ComboBoxT{C_BAUD, C_DEVLST, C_LENGTH};
public:
    SUsbCanWidget(SMainWindow *mainWindow, QWidget *parent = 0);
    ~SUsbCanWidget();

    YCanHandle* currentHandle();
    void setSObject(SObject *obj) override;
    void propertyOfSObjectChanged(SObject *obj,
                                  const QString &strPropName,
                                  const SObject::PropertyT &propChangedBy) override;

    // static
    static QString keyString();
    static void initSObject(SObject* obj);

protected:
    void initWidget();
    void registerDevice();

    bool nativeEvent(const QByteArray &eventType, void* message, long* result) override;

    SThread m_thread;
    static int controlThread(void *pParam, const bool &bRunning);

public slots:
    void slotCanMessage(const CAN_MESSAGE_PACKAGE& buf);
    void slotBtnClicked();
    void slotTimeout();

private:
    QGroupBox*      m_groupBox[G_LENGTH];
    QRadioButton*   m_radioBtns[B_LENGTH];
    QComboBox*      m_comboBox[C_LENGTH];
    QPushButton*    m_switchBtn = nullptr;
    YCanHandle*     m_handle;
    QStringList     m_deviceList;
    QTimer          m_timer;

    QList<CAN_MESSAGE_PACKAGE>  m_dataList;

    bool                    m_isOpen = false;
};

#endif // SUSBCANWIDGET_H
