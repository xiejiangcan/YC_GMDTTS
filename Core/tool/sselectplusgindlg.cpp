#include "sselectplusgindlg.h"
#include "smainwindow.h"

SSelectPlusginDlg::SSelectPlusginDlg(const QString& strType, bool bHasID, SObject* pParenObj, SMainWindow* pMainW, QWidget *parent) :
    QDialog(parent),
    m_type(strType),
    m_hasID(bHasID),
    m_parenObj(pParenObj),
    m_mainWindow(pMainW)
{
    QComboBox* pInsCombo = new QComboBox(this);
    QStringList lstNames;

    if(strType == STR_ACTION){
        lstNames << "js";
    }else if(strType == STR_SIGNAL){
        lstNames << "normal";
    }
    auto interfaces = pMainW->pluginInterfaces().find(strType);
    if(interfaces != pMainW->pluginInterfaces().end()){
        for(auto iterIf = interfaces->begin(); iterIf != interfaces->end(); iterIf++){
            lstNames << iterIf.key();
        }
    }

    pInsCombo->addItems(lstNames);
    m_widgets[E_ALL] = pInsCombo;

    m_labels[E_ALL] = new QLabel(strType, this);
    m_labels[E_NAME] = new QLabel(tr("Name"), this);
    m_labels[E_ID] = new QLabel(tr("ID"), this);
    m_labels[E_COUNT] = new QLabel("Quantity", this);
    m_widgets[E_NAME] = new QLineEdit(this);
    m_widgets[E_ID] = new QLineEdit("0", this);
    m_widgets[E_COUNT] = new QLineEdit("1", this);
    m_buttons[B_OK] = new QPushButton(tr("OK"), this);
    m_buttons[B_FINISH] = new QPushButton(tr("Cancel"), this);

    QFormLayout* pFLayout = new QFormLayout(this);
    int nIndex;
    for(nIndex = 0; nIndex < E_LENGTH; nIndex++){
        pFLayout->addRow(m_labels[nIndex], m_widgets[nIndex]);
    }
    pFLayout->addRow(m_buttons[B_OK], m_buttons[B_FINISH]);

    if(!bHasID){
        for(nIndex = E_ID; nIndex < E_LENGTH; nIndex++){
            m_labels[nIndex]->hide();
            m_widgets[nIndex]->hide();
        }
    }

    connect(m_buttons[B_OK], SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_buttons[B_FINISH], SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle(tr("Select ") + strType);
}

QString SSelectPlusginDlg::pluginName()
{
    return ((QComboBox*)(m_widgets[E_ALL]))->currentText();
}

QString SSelectPlusginDlg::name()
{
    return ((QLineEdit*)(m_widgets[E_NAME]))->text();
}

uint SSelectPlusginDlg::beginID()
{
    return ((QLineEdit*)(m_widgets[E_ID]))->text().toUInt();
}

uint SSelectPlusginDlg::quantity()
{
    return ((QLineEdit*)(m_widgets[E_COUNT]))->text().toUInt();
}

QObject *SSelectPlusginDlg::selectedPlugin(QObject *parent)
{
    return m_mainWindow->createPlugin(m_type, pluginName(), parent);
}

QList<SObject*> SSelectPlusginDlg::selectedSObject(SObject *parent)
{
    SObject* pNew;
    QList<SObject*> lstObj;
    QString strName,strPuginName,strID;

    strName = name();
    strPuginName = pluginName();
    if(m_hasID){
        uint nCount,nId,nIndex = 0;
        nCount = quantity();
        nId = beginID();
        strID = strName + ":";
        for(;nIndex < nCount; nIndex++){
            if(m_parenObj->findChild<SObject*>(strID + QString::number(nId + nIndex)) == NULL){
                if((pNew = m_mainWindow->createSObjectForPlugin(m_type, strPuginName, parent)) != NULL){
                    pNew->setObjectName(strID + QString::number(nId + nIndex));
                    pNew->setProperty(STR_PLUGIN, strPuginName);
                    lstObj.append(pNew);
                }
            }
        }
    }else{
        strID = strName;
        if(m_parenObj->findChild<SObject*>(strID) == NULL){
            if((pNew = m_mainWindow->createSObjectForPlugin(m_type, strPuginName, parent)) != NULL){
                pNew->setObjectName(strID);
                pNew->setProperty(STR_PLUGIN, strPuginName);
                lstObj.append(pNew);
            }
        }
    }
    return lstObj;
}

QLabel *SSelectPlusginDlg::label(int id)
{
    return m_labels[id];
}

QWidget *SSelectPlusginDlg::widget(int id)
{
    return m_widgets[id];
}

int SSelectPlusginDlg::exec()
{
    QString strName;
    int nRet = 0;
    do{
        if ((nRet = QDialog::exec()) != QDialog::Accepted)
            return nRet;
        strName = name();
        if(strName.isEmpty()){
            QMessageBox::warning(this, tr("Warning"), tr("The name can't be empty."));
            continue;
        }
        if(!isValidName(strName)){
            QMessageBox::warning(this, tr("Warning"), tr("The name is not valid."));
            continue;
        }
        if(m_parenObj){
            bool bFound = false;
            if(m_hasID){
                uint nCount,nId,nIndex = 0;
                nCount = quantity();
                nId = beginID();
                for(;nIndex < nCount; nIndex++){
                    if(m_parenObj->findChild<SObject*>(strName + ":"+ QString::number(nId + nIndex)) != NULL){
                        bFound = true;
                        break;
                    }
                }
            }else{
                if(m_parenObj->findChild<SObject*>(strName) != NULL){
                    bFound = true;
                }
            }
            if(bFound){
                QMessageBox::warning(this, tr("Warning"), tr("The name has existed."));
                continue;
            }
        }
        break;
    }while(true);
    return nRet;
}
