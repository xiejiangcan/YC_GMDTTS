#include "smanagecontainerwidget.h"

SManageContainerWidget::SManageContainerWidget(QWidget *parent) :
    QDialog(parent)
{
    QWidget* arrpParamW[5];
    QFormLayout* arrpLayout[5];
    QGridLayout* pLayout = new QGridLayout(this);

    m_edits[E_NAME] = new QLineEdit(this);
    m_edits[E_NAME]->setPlaceholderText(tr("Container name"));
    m_comboBoxs[CB_TYPE] = new QComboBox(this);
    m_comboBoxs[CB_TYPE]->addItem(STR_WIDGET, 0);
    m_comboBoxs[CB_TYPE]->addItem(STR_SCROLL, 0);
    m_comboBoxs[CB_TYPE]->addItem(STR_GROUP, 0);
    m_comboBoxs[CB_TYPE]->addItem(STR_SPLIT, 1);
    m_comboBoxs[CB_TYPE]->addItem(STR_TAB, 2);

    m_groupBoxs[GB_PARAM] = new QGroupBox(tr("Parameters"), this);
    m_stackedLayouts[SL_PARAM] = new QStackedLayout(m_groupBoxs[GB_PARAM]);

    // widget & scroll & group
    m_labels[L_GROUP_TITLE] = new QLabel(tr("Title"), this);
    m_edits[E_GROUP_TITLE] = new QLineEdit(this);
    m_labels[L_GROUP_ALIGN] = new QLabel(tr("Alignment"), this);
    m_comboBoxs[CB_GROUP_ALIGN] = new QComboBox(this);
    m_comboBoxs[CB_GROUP_ALIGN]->addItem(STR_ALIGN_LEFT, (int)Qt::AlignLeft);
    m_comboBoxs[CB_GROUP_ALIGN]->addItem(STR_ALIGN_RIGHT, (int)Qt::AlignRight);
    m_comboBoxs[CB_GROUP_ALIGN]->addItem(STR_ALIGN_CENTER, (int)Qt::AlignHCenter);
    m_comboBoxs[CB_LAYOUT] = new QComboBox(this);
    m_comboBoxs[CB_LAYOUT]->addItem(STR_LAYOUT_HBOX, 0);
    m_comboBoxs[CB_LAYOUT]->addItem(STR_LAYOUT_VBOX, 1);
    m_comboBoxs[CB_LAYOUT]->addItem(STR_LAYOUT_GRID, 2);
    m_comboBoxs[CB_LAYOUT]->addItem(STR_LAYOUT_FORM, 3);
    m_comboBoxs[CB_LAYOUT]->addItem(STR_LAYOUT_STACK, 4);
    m_comboBoxs[CB_LAYOUT]->addItem(STR_LAYOUT_GEOMETRY, 5);
    arrpParamW[0] = new QWidget(this);
    arrpLayout[0] = new QFormLayout(arrpParamW[0]);
    arrpLayout[0]->addRow(tr("Layout"), m_comboBoxs[CB_LAYOUT]);
    arrpLayout[0]->addRow(m_labels[L_GROUP_TITLE], m_edits[E_GROUP_TITLE]);
    arrpLayout[0]->addRow(m_labels[L_GROUP_ALIGN], m_comboBoxs[CB_GROUP_ALIGN]);
    arrpParamW[0]->setLayout(arrpLayout[0]);

    // split
    m_comboBoxs[CB_SPLIT_ORIENTATION] = new QComboBox(this);
    m_comboBoxs[CB_SPLIT_ORIENTATION]->addItem(STR_HORIZONTAL, 1);
    m_comboBoxs[CB_SPLIT_ORIENTATION]->addItem(STR_VERTICAL, 2);
    arrpParamW[1] = new QWidget(this);
    arrpLayout[1] = new QFormLayout(arrpParamW[1]);
    arrpLayout[1]->addRow(tr("Orientation"), m_comboBoxs[CB_SPLIT_ORIENTATION]);
    arrpParamW[1]->setLayout(arrpLayout[1]);
    // tab
    m_comboBoxs[CB_TAB_CLOSABE] = new QComboBox(this);
    m_comboBoxs[CB_TAB_CLOSABE]->addItem(STR_BOOL_TRUE, true);
    m_comboBoxs[CB_TAB_CLOSABE]->addItem(STR_BOOL_FALSE, false);
    m_comboBoxs[CB_TAB_ALIGN] = new QComboBox(this);
    m_comboBoxs[CB_TAB_ALIGN]->addItem(STR_ALIGN_NORTH, 0);
    m_comboBoxs[CB_TAB_ALIGN]->addItem(STR_ALIGN_SOUTH, 1);
    m_comboBoxs[CB_TAB_ALIGN]->addItem(STR_ALIGN_WEST, 2);
    m_comboBoxs[CB_TAB_ALIGN]->addItem(STR_ALIGN_EAST, 3);
    arrpParamW[2] = new QWidget(this);
    arrpLayout[2] = new QFormLayout(arrpParamW[2]);
    arrpLayout[2]->addRow(tr("Closabe"), m_comboBoxs[CB_TAB_CLOSABE]);
    arrpLayout[2]->addRow(tr("Tab Position"), m_comboBoxs[CB_TAB_ALIGN]);
    arrpParamW[2]->setLayout(arrpLayout[2]);


    for(int nIndex = 0; nIndex < 3; nIndex++)
        m_stackedLayouts[SL_PARAM]->addWidget(arrpParamW[nIndex]);
    m_groupBoxs[GB_PARAM]->setLayout(m_stackedLayouts[SL_PARAM]);

    m_buttons[B_OK] = new QPushButton(tr("OK"), this);
    m_buttons[B_CANCEL] = new QPushButton(tr("Cancel"), this);

    pLayout->addWidget(new QLabel("Name", this), 0, 0);
    pLayout->addWidget(m_edits[E_NAME], 0, 1, 1, 3);
    pLayout->addWidget(new QLabel("Container", this), 1, 0);
    pLayout->addWidget(m_comboBoxs[CB_TYPE], 1, 1, 1, 3);
    pLayout->addWidget(m_groupBoxs[GB_PARAM], 2, 0, 1, 4);
    pLayout->addWidget(m_buttons[B_OK], 3, 1);
    pLayout->addWidget(m_buttons[B_CANCEL], 3, 2);
    setLayout(pLayout);

    connect(m_buttons[B_OK], SIGNAL(clicked()), SLOT(accept()));
    connect(m_buttons[B_CANCEL], SIGNAL(clicked()), SLOT(reject()));
    connect(m_comboBoxs[CB_TYPE], SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChangedForTypeCombo(int)));
    currentIndexChangedForTypeCombo(0);
    setWindowTitle(tr("New container"));
}

QLineEdit *SManageContainerWidget::edit(int id)
{
    return m_edits[id];
}

QComboBox *SManageContainerWidget::comboBox(int id)
{
    return m_comboBoxs[id];
}

void SManageContainerWidget::currentIndexChangedForTypeCombo(int /*index*/)
{
    m_stackedLayouts[SL_PARAM]->setCurrentIndex(m_comboBoxs[CB_TYPE]->currentData().toInt());
    if(m_comboBoxs[CB_TYPE]->currentText() == STR_GROUP){
        m_labels[L_GROUP_TITLE]->show();
        m_edits[E_GROUP_TITLE]->show();
        m_labels[L_GROUP_ALIGN]->show();
        m_comboBoxs[CB_GROUP_ALIGN]->show();
    }else{
        m_labels[L_GROUP_TITLE]->hide();
        m_edits[E_GROUP_TITLE]->hide();
        m_labels[L_GROUP_ALIGN]->hide();
        m_comboBoxs[CB_GROUP_ALIGN]->hide();
    }
}
