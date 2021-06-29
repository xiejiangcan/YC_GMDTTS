#ifndef SACTION_H
#define SACTION_H

#include "core_global.h"


class CORE_EXPORT SAction : public QAction
{
    Q_OBJECT
public:
    SAction(SMainWindow *mainWindow, QObject *parent = 0);
    virtual ~SAction();

    // getting
    SMainWindow *mainWindow() const;
    SObject* sobject();
    QString code() const;
    QString funcName() const;
    QVariantList params() const;
    virtual QString error();

    // setting
    virtual void setSObject(SObject* obj);
    virtual bool setCode(const QString &code);
    void setFuncName(const QString &funcName);
    void setParams(const QVariantList &params);
    virtual void setError(const QString &strErr);

    // static
    static QString key(const QString &strSubKey);
    static void initailizeSObject(QObject* obj);

public slots:
    virtual void call();

private:
    SMainWindow* m_mainWindow = nullptr;
    SObject* m_sobject = nullptr;

};

#endif // SACTION_H
