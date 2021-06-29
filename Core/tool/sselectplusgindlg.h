#ifndef SSELECTPLUSGINDLG_H
#define SSELECTPLUSGINDLG_H

#include "core_global.h"

class CORE_EXPORT SSelectPlusginDlg : public QDialog
{
    Q_OBJECT
public:
    enum ElemT{E_ALL, E_NAME, E_ID, E_COUNT, E_LENGTH};
    enum ButtonT{B_OK, B_FINISH, B_LENGTH};

    explicit SSelectPlusginDlg(const QString& strType,
                               bool bHasID, SObject* pParenObj,
                               SMainWindow* pMainW,
                               QWidget *parent = 0);
    QString pluginName();

    QString name();

    uint beginID();

    uint quantity();

    QObject* selectedPlugin(QObject* parent);

    QList<SObject*> selectedSObject(SObject* parent);

    QLabel* label(int id);

    QWidget* widget(int id);
signals:

public slots:

    int exec();
private:
    bool m_hasID; /**< TODO */
    SObject* m_parenObj; /**< TODO */
    SMainWindow* m_mainWindow; /**< TODO */
    QString m_type; /**< TODO */
    QLabel* m_labels[E_LENGTH]; /**< TODO */
    QWidget* m_widgets[E_LENGTH]; /**< TODO */
    QPushButton* m_buttons[B_LENGTH]; /**< TODO */
};

#endif // SSELECTPLUSGINDLG_H
