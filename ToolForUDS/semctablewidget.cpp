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
    //    QObject::blockSignals(true);
    //    if(strPropName == STR_SIGNALOUT){
    //        auto& mapMapping = mapping();
    //        if(mapMapping.contains(STR_SIGNALOUT)
    //                && mapMapping[STR_SIGNALOUT][STR_TYPE].toString() == STR_PROP){
    //            m_thread.setUserFunction(controlThread);
    //            m_thread.setUserParam(this);
    //            m_thread.start();
    //        }
    //    }
    //    QObject::blockSignals(false);
}

QString SEmcTableWidget::keyString()
{
    return SWidget::key(EMC_TABLE);
}

void SEmcTableWidget::initSObject(SObject *obj)
{
    obj->setObjectName(EMC_TABLE);
    obj->setProperty(EMC_RESULT, STR_DEFAULT);
    obj->setProperty(STR_ID, "emcResult");
}

int SEmcTableWidget::controlThread(void *pParam, const bool &bRunning)
{
    SEmcTableWidget* pWidget = (SEmcTableWidget*)pParam;
    if(pWidget){
        uint uVersion = 0;
        QString resultMap;
        while(bRunning){
            SObject* pObjBuff = pWidget->sobject();
            if(pObjBuff){
                if(pObjBuff->propertyInfo().contains(EMC_RESULT)){
                    uint tempVersion = pObjBuff->propertyInfo()[EMC_RESULT].m_version;
                    if(uVersion != tempVersion){
                        pObjBuff->lock().lockForRead();
                        resultMap =
                                pObjBuff->property(EMC_RESULT).toString();
                        pObjBuff->lock().unlock();
                        if(!resultMap.isEmpty()){
                            uVersion = tempVersion;
                            emit pWidget->signalUpdateTable(resultMap);
                        }
                    }

                }
            }
            QThread::msleep(1);
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
