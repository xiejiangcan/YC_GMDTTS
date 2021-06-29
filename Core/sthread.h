#ifndef STHREAD_H
#define STHREAD_H

#include "core_global.h"

typedef int (*SThreadFunc)(void* pParam, const bool& bRunning);

class CORE_EXPORT SThread : public QThread
{
    Q_OBJECT
public:
    explicit SThread(QObject *parent = nullptr);
    SThreadFunc userFunction() const;
    void setUserFunction(const SThreadFunc &fnUserFunction);

    void *userParam() const;

    void setUserParam(void *pUserParam);

    void stop();

protected:
    virtual void run();

    SThreadFunc     m_userFunction; /**< TODO */
    void*           m_userParam; /**< TODO */
    bool            m_running = false; /**< TODO */
};

#endif // STHREAD_H
