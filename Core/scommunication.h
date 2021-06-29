#ifndef SCOMMUNICATION_H
#define SCOMMUNICATION_H

#include "core_global.h"
#include "sthread.h"

class CORE_EXPORT SCommunication : public QObject
{
    Q_OBJECT
public:
    struct _COMMUNICATION_STRUCT{
        const void * pBufferW=nullptr;
        uint nBufferW=0;
        //uint overTimeW=0;
        void * pBufferR=nullptr;
        uint nBufferR=0;
        //uint overTimeR=0;
        volatile bool isOK=false;
    };

public:
    SCommunication(const QString& strName = "",
                   SMainWindow *pMainWindow = NULL,
                   QObject* pParent = NULL);
    virtual ~SCommunication();

    // gettings
    SMainWindow *mainWindow();
    SObject *sobject();
    QMutex& mutex();
    int status(void);

    // settings
    void setSObject(SObject* pObj);

    // static
    static QString key(const QString& strSubKey);
    static void initializeSObject(SObject* obj);

    // function
    bool writeForBack(const void* lpInBuffer,
                      unsigned int nInBufferSize,
                      void* lpOutBuffer,
                      unsigned int nOutBufferSize,
                      unsigned int* lpBytesReturned = NULL,
                      unsigned int nTimeOutMs = 500);

protected:
    virtual bool open();
    virtual void close(void);
    virtual bool write(const void* lpBuffer,
               unsigned int nNumberOfBytesToWrite,
               unsigned int* lpNumberOfBytesWritten = NULL,
               unsigned int nTimeOutMs = ~0) = 0;
    virtual bool read(void* lpBuffer,
              unsigned int nNumberOfBytesToRead,
              unsigned int* lpNumberOfBytesRead = NULL,
              unsigned int nTimeOutMs = ~0) = 0;

    int pushBuff(_COMMUNICATION_STRUCT* p_COMMUNICATION_STRUCT);
    int popBuff(_COMMUNICATION_STRUCT* p_COMMUNICATION_STRUCT);
    int clearBuff(_COMMUNICATION_STRUCT* p_COMMUNICATION_STRUCT);
    static int controlThread(void *pParam, const bool &bRunning);

    QList<_COMMUNICATION_STRUCT*> m_bufferList;

private:
    SMainWindow*    m_mainWindow = nullptr;
    SObject*        m_object = nullptr;

    SThread         m_msThread;
    QMutex          m_mutex;
    int             m_status;
};

#endif // SCOMMUNICATION_H
