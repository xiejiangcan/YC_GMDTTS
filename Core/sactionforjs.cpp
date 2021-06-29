#include "sactionforjs.h"
#include "sproject.h"
#include "smainwindow.h"

SActionForJS::SActionForJS(SMainWindow *mainWindow, QObject *parent) :
    SAction(mainWindow, parent)
{
    SProject* pProj;
    if(mainWindow == NULL
            || (pProj = mainWindow->project()) == NULL)
        return;
    m_scriptEngine = &pProj->scriptEngine();
}

QString SActionForJS::key()
{
    return SAction::key("js");
}

void SActionForJS::initailizeSObject(QObject *obj)
{
    SAction::initailizeSObject(obj);
}

void SActionForJS::call()
{
    call(m_scriptEngine);
}

void SActionForJS::call(QScriptEngine *scriptEngine)
{
    if(scriptEngine == NULL){
        setError("No engine");
        return;
    }
    QString strCode;
    if((strCode = code()).isEmpty()){
        return;
    }
    QString strFunc,strParams,strType;
    QScriptValue value;
    value = scriptEngine->evaluate(strCode);
    if (scriptEngine->hasUncaughtException()) {
        int line = scriptEngine->uncaughtExceptionLineNumber();
        qDebug() << "uncaught exception at line" << line << ":" << value.toString();
        setError(value.toString());
    }
    if(!(strFunc = funcName()).isEmpty()){
        QVariantList lstParam = params();
        if(!lstParam.isEmpty()){
            for(auto iterParam = lstParam.begin(); iterParam != lstParam.end(); iterParam++){
                strType = iterParam->typeName();
                if(strType == "QString"
                        || strType == "string"){
                    strParams +="\"" + iterParam->toString() + "\"";
                }else{
                    strParams += iterParam->toString();
                }
                strParams += ",";
            }
            strParams = strParams.remove(strParams.size() - 1, 1);
        }
        value = scriptEngine->evaluate(strFunc + "(" + strParams + ")");
        if (scriptEngine->hasUncaughtException()) {
            int line = scriptEngine->uncaughtExceptionLineNumber();
            qDebug() << "uncaught exception at line" << line << ":" << value.toString();
            setError(value.toString());
        }
    }
}
