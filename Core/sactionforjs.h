#ifndef SACTIONFORJS_H
#define SACTIONFORJS_H

#include "core_global.h"
#include "saction.h"

class SActionForJS : public SAction
{
    Q_OBJECT
public:
    explicit SActionForJS(SMainWindow *mainWindow, QObject *parent = 0);

    static QString key();
    static void initailizeSObject(QObject* obj);
public slots:
    virtual void call();
    void call(QScriptEngine* scriptEngine);
private:
    QScriptEngine* m_scriptEngine = NULL; /**< TODO */

};

#endif // SACTIONFORJS_H
