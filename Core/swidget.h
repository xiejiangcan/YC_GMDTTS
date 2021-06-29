#ifndef SWIDGET_H
#define SWIDGET_H

#include "core_global.h"
#include "sobject.h"
#include "saction.h"

class CORE_EXPORT SWidget : public QWidget
{
    Q_OBJECT
    friend class SProject;
public:
    SWidget(SMainWindow* mainWindow, QWidget *parent = nullptr);
    virtual ~SWidget();

    // getting
    SObject* sobject();
    SMainWindow* mainWindow();
    QMap<QString, QVariantMap>& mutableMapping();
    const QMap<QString, QVariantMap>& mapping() const;
    QMap<QString, SAction*>& mutableActions();
    const QMap<QString, SAction*>& actions() const;

    // function
    bool isMapped(const QString& strPropName);
    bool triggerAction(const QString& strSignal, bool checked = false);
    void addMapping(const QString& strPropName,
                    const QString& strMappedResID);
    void addAction(const QString& strSignal,
                   const QString& strActionName);
    int writeForBack(QByteArray& buffW, const QString& mapPropW,
                     QByteArray& buffR, const QString& mapPropR);

    // static function
    static QString key(const QString& strSubKey);
    static void initializeSObject(SObject* obj);
    static void addSpecialProperty(SObject* obj,
                                   const QString& strPropName,
                                   const QString& strPropValue,
                                   const QString& strRole = "action");

protected:
    void analyseSObject(SObject* obj);

    // setting
    virtual void setSObject(SObject* obj);

    virtual void mappingChanged(const QString& strLocalPropName,
                                SObject* obj,
                                const QString& strPropName,
                                const SObject::PropertyT& propChangedBy);
public slots:
    virtual void propertyOfSObjectChanged(SObject* obj,
                                          const QString& strPropName,
                                          const SObject::PropertyT& propChangedBy);

private slots:
    void mappingChangedFilter(SObject* obj, const QString &strPropName,
                              const SObject::PropertyT& propChangedBy);

private:
    SMainWindow*                m_mainwindow = nullptr;
    SObject*                    m_object = nullptr;
    QMap<QString, QVariantMap>  m_mapping;
    QMap<QString, SAction*>     m_actions;
    QByteArray                  m_rArray;
};

#endif // SWIDGET_H
