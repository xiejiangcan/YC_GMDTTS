#include "semctablewidget.h"


SEmcTableWidget::SEmcTableWidget(SMainWindow *mainWindow, QWidget *parent)
    : SWidget(mainWindow, parent),
      m_table(new QTableView(this)),
      m_model(new EmcResultModel(this)),
      m_titleLabel(new QLabel("***********************************Result Table***********************************", this))
{
    m_table->setModel(m_model);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    this->setLayout(new QVBoxLayout(this));

    this->layout()->addWidget(m_titleLabel);
    this->layout()->addWidget(m_table);

    connect(this, &SEmcTableWidget::signalUpdateTable,
            this, &SEmcTableWidget::slotUpdateTable);
}

SEmcTableWidget::~SEmcTableWidget()
{
    if(m_thread.isRunning()){
        m_thread.stop();
        m_thread.wait();
        m_thread.quit();
    }
}

void SEmcTableWidget::setSObject(SObject *obj)
{
    SWidget::setSObject(obj);
    obj->setProperty(EMC_RESULT, STR_DEFAULT);

    m_thread.setUserFunction(controlThread);
    m_thread.setUserParam(this);
    m_thread.start();

}

void SEmcTableWidget::propertyOfSObjectChanged(SObject *obj, const QString &strPropName, const SObject::PropertyT &propChangedBy)
{
    Q_UNUSED(obj)
    Q_UNUSED(strPropName)
    Q_UNUSED(propChangedBy)
    //        QObject::blockSignals(true);
    //        if(strPropName == STR_SIGNALOUT){
    //            auto& mapMapping = mapping();
    //            if(mapMapping.contains(STR_SIGNALOUT)
    //                    && mapMapping[STR_SIGNALOUT][STR_TYPE].toString() == STR_PROP){
    //                m_thread.setUserFunction(controlThread);
    //                m_thread.setUserParam(this);
    //                m_thread.start();
    //            }
    //        }
    //        QObject::blockSignals(false);
}

QString SEmcTableWidget::keyString()
{
    return SWidget::key(EMC_TABLE);
}

void SEmcTableWidget::initSObject(SObject *obj)
{
    obj->setObjectName(EMC_TABLE);
    obj->setProperty(EMC_RESULTNUM, 6);
    obj->setProperty(STR_ID, "emcResult");
}

int SEmcTableWidget::controlThread(void *pParam, const bool &bRunning)
{
    SEmcTableWidget* pWidget = (SEmcTableWidget*)pParam;
    if(pWidget){
        QVector<uint> uVersion;
        QString propName;
        QString resultMap;
        while(bRunning){
            SObject* pObjBuff = pWidget->sobject();
            if(pObjBuff){
                int resultNum = pObjBuff->property(EMC_RESULTNUM).toInt();
                while(uVersion.size() != resultNum){
                    if(uVersion.size()> resultNum)
                        uVersion.removeLast();
                    else
                        uVersion.append(0);
                }

                for(int i = 0; i < uVersion.size(); ++i){
                    propName = QString(EMC_RESULT) + QString::number(i);
                    if(pObjBuff->propertyInfo().contains(propName)){
                        uint tempVersion = pObjBuff->propertyInfo()[propName].m_version;
                        if(uVersion[i] != tempVersion){
                            uVersion[i] = tempVersion;
                            resultMap = pObjBuff->property(propName.toLatin1()).toString();
                            if(!resultMap.isEmpty())
                                emit pWidget->signalUpdateTable(resultMap);
                        }
                    }
                }
            }
            QThread::msleep(100);
        }
    }
    return 0;
}

void SEmcTableWidget::slotUpdateTable(QString str)
{
    QStringList strList = str.split("@");
    if(strList.size() != 2){
        return;
    }
    QString signalName = strList.first();
    if(strList.last().isEmpty()){
        // Time out
        m_model->setDevTimeout(signalName);
        return;
    }
    QStringList propMap = strList.last().split(";");
    QMap<QString, QVariantMap> result;
    for(auto iter = propMap.begin(); iter != propMap.end(); ++iter){
        if(iter->isEmpty()){
            continue;
        }
        QStringList propInfo = iter->split(":");
        if(propInfo.size() != 2 && propInfo.begin()->isEmpty()){
            continue;
        }
        QVariantMap mapList;
        QStringList tempList = propInfo.last().split("&");
        for(auto item = tempList.begin(); item != tempList.end(); ++item){
            if(item->isEmpty())
                continue;
            auto mapStr = item->split("-");
            if(mapStr.size() != 2 && mapStr.first().isEmpty()){
                continue;
            }
            mapList.insert(mapStr.first(), QVariant::fromValue(mapStr.last()));
        }
        result[propInfo.first()] = mapList;
    }

    m_model->updateModel(signalName, result);

}
