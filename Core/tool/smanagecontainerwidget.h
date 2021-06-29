#ifndef SMANAGECONTAINERWIDGET_H
#define SMANAGECONTAINERWIDGET_H

#include "core_global.h"

class CORE_EXPORT SManageContainerWidget : public QDialog
{
    Q_OBJECT
public:
    enum ButtonT{B_OK, B_CANCEL, B_LENGTH};
    enum EditT{E_NAME, E_GROUP_TITLE, E_LENGTH};
    enum ComboBoxT{CB_TYPE, CB_LAYOUT, CB_TAB_CLOSABE, CB_TAB_ALIGN, CB_SPLIT_ORIENTATION,
                   CB_GROUP_ALIGN, CB_LENGTH};
    enum StackedLayoutT{SL_PARAM, SL_LENGTH};
    enum GroupBoxT{GB_PARAM, GB_LENGTH};
    enum LabelT{L_GROUP_TITLE, L_GROUP_ALIGN,L_LENGTH};

    explicit SManageContainerWidget(QWidget *parent = 0);

    QLineEdit* edit(int id);
    QComboBox* comboBox(int id);
signals:

public slots:
    void currentIndexChangedForTypeCombo(int index);
private:
    QPushButton* m_buttons[B_LENGTH]; /**< TODO */
    QLineEdit* m_edits[E_LENGTH]; /**< TODO */
    QComboBox* m_comboBoxs[CB_LENGTH]; /**< TODO */
    QStackedLayout* m_stackedLayouts[SL_LENGTH]; /**< TODO */
    QGroupBox* m_groupBoxs[GB_LENGTH]; /**< TODO */
    QLabel* m_labels[L_LENGTH]; /**< TODO */
};

#endif // SMANAGECONTAINERWIDGET_H
