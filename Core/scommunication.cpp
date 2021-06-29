#include "scommunication.h"

#define SDBG(msg)// qDebug()<<msg
#define SCOMM_STATUS_OPENED 1

SCommunication::SCommunication(const QString &strName,
                               SMainWindow *pMainWindow,
                               QObject *pParent)
    : QObject(parent()),
      m_mainWindow(pMainWindow)
{
    setProperty("name", strName);

    m_msThread.setUserFunction(controlThread);
    m_msThread.setUserParam(this);
    m_msThread.start();
}

SCommunication::~SCommunication()
{
    if(m_msThread.isRunning()){
        m_msThread.stop();
        m_msThread.wait();
    }
}

SMainWindow *SCommunication::mainWindow()
{
    return m_mainWindow;
}

SObject *SCommunication::sobject()
{
    return m_object;
}

QMutex &SCommunication::mutex()
{
    return m_mutex;
}

int SCommunication::status()
{
    return m_status;
}

void SCommunication::setSObject(SObject *pObj)
{
    m_object = pObj;
}

QString SCommunication::key(const QString &strSubKey)
{
    return QStringLiteral(STR_COM) + STR_CUT_CHAR + strSubKey;
}

void SCommunication::initializeSObject(SObject *obj)
{
    Q_UNUSED(obj)
}

bool SCommunication::writeForBack(const void *lpInBuffer,
                                  unsigned int nInBufferSize,
                                  void *lpOutBuffer,
                                  unsigned int nOutBufferSize,
                                  unsigned int *lpBytesReturned,
                                  unsigned int nTimeOutMs)
{
    _COMMUNICATION_STRUCT tmp_COMMUNICATION_STRUCT;
    tmp_COMMUNICATION_STRUCT.pBufferW= lpInBuffer;
    tmp_COMMUNICATION_STRUCT.nBufferW= nInBufferSize;
    tmp_COMMUNICATION_STRUCT.pBufferR= lpOutBuffer;
    tmp_COMMUNICATION_STRUCT.nBufferR= nOutBufferSize;

    if(pushBuff(&tmp_COMMUNICATION_STRUCT)==0){
        int nCount=0;
        QTime tmEmptyPkt;
        tmEmptyPkt.start();
        do{
            QCoreApplication::processEvents();
            int req=popBuff(&tmp_COMMUNICATION_STRUCT);
            if(req==0){
                *lpBytesReturned= nOutBufferSize;
                SDBG("com_writeForBack_ok");
                return true;
            }else if(req== -2){
                tmEmptyPkt.restart();
            }else if(req== -3){
                break;
            }
            SDBG("com_writeForBack_wrong");
            nCount++;
        }while(tmEmptyPkt.elapsed()<nTimeOutMs || nCount<=3);
    }else
        SDBG("com_writeForBack_error_push");
    qDebug()<<"com_writeForBack_error"<<((uint*) lpInBuffer)[0]<<" "<<((uint*) lpInBuffer)[3];
    return false;
}

bool SCommunication::open()
{
    m_status |= SCOMM_STATUS_OPENED;
    return true;
}

void SCommunication::close()
{
    m_status &= ~SCOMM_STATUS_OPENED;
}

int SCommunication::pushBuff(_COMMUNICATION_STRUCT *p_COMMUNICATION_STRUCT)
{
    if(p_COMMUNICATION_STRUCT){
        while(1){
            if(!m_mutex.tryLock())
                QCoreApplication::processEvents();
            else
                break;
        }
        const uint* pTmp= (uint*)(p_COMMUNICATION_STRUCT->pBufferW);
        if((pTmp[0]== 0xFFFFF201) &&
                (pTmp[3]== 0x00000004)){
            if(pTmp[7]<= 10 ){
                m_bufferList.insert(0,p_COMMUNICATION_STRUCT);
            }else{
                m_bufferList.append(p_COMMUNICATION_STRUCT);
            }
        }else{
            m_bufferList.append(p_COMMUNICATION_STRUCT);
        }
        m_mutex.unlock();
        SDBG("com_buff_push_ok");
        return 0;
    }else
        qDebug("com_buff_push_errorParam");
    return -1;
}

int SCommunication::popBuff(_COMMUNICATION_STRUCT *p_COMMUNICATION_STRUCT)
{
    if(p_COMMUNICATION_STRUCT){
        while(1){
            try{
                if(!m_mutex.tryLock())
                    QCoreApplication::processEvents();
                else
                    break;
            }catch(...){}
        }
        for(QList<_COMMUNICATION_STRUCT*>::iterator i=m_bufferList.begin(); i!=m_bufferList.end(); i++){
            if(*i == p_COMMUNICATION_STRUCT){
                SDBG("com_buff_pop_waiting");
                m_mutex.unlock();
                return -2;//ing
            }
        }
        m_mutex.unlock();
    }else{
        qDebug("com_buff_pop_errorParam");
        return -1;//errorParam
    }
    if(p_COMMUNICATION_STRUCT->isOK){
        SDBG("com_buff_pop_ok");
        return 0;//isOK
    }
    SDBG("com_buff_pop_closed");
    return -3;//usb closed
}

int SCommunication::clearBuff(_COMMUNICATION_STRUCT *p_COMMUNICATION_STRUCT)
{
    if(p_COMMUNICATION_STRUCT){
        while(1){
            try{
                if(!m_mutex.tryLock())
                    QCoreApplication::processEvents();
                else
                    break;
            }catch(...){}
        }
        for(QList<_COMMUNICATION_STRUCT*>::iterator i=m_bufferList.begin(); i!=m_bufferList.end(); i++){
            if(*i == p_COMMUNICATION_STRUCT){
                m_bufferList.removeOne(p_COMMUNICATION_STRUCT);
                m_mutex.unlock();
                SDBG("com_buff_remove");
                return 0;
            }
        }
        m_mutex.unlock();
    }
    return 0;
}

int SCommunication::controlThread(void *pParam, const bool &bRunning)
{
    SCommunication* pCommunication = (SCommunication*)pParam;
    QTime tmEmptyPkt;
    tmEmptyPkt.start();
    if(pCommunication){
        _COMMUNICATION_STRUCT* tmp_COMMUNICATION_STRUCT= nullptr;
        uint ulLength=0,wLength=0,rLength=0;
        bool bError=false;
        while(bRunning){
            pCommunication->m_mutex.lock();
            if(!pCommunication->m_bufferList.isEmpty()){
                tmp_COMMUNICATION_STRUCT= pCommunication->m_bufferList.first();
                pCommunication->m_bufferList.removeFirst();
                if(tmp_COMMUNICATION_STRUCT){
                    ulLength= wLength= rLength=0;
                    bError=false;
                    tmEmptyPkt.restart();
                    do{
                        try{
                            if(!pCommunication->write((int*)tmp_COMMUNICATION_STRUCT->pBufferW+wLength, tmp_COMMUNICATION_STRUCT->nBufferW-wLength,
                                                      &wLength,(tmp_COMMUNICATION_STRUCT->nBufferW-wLength)/5000+500)){//(tmp_COMMUNICATION_STRUCT->nBufferW-wLength)/5000+200)){
                                qDebug("usb_write_disconnect");
                                bError=true;
                                break;
                            }
                        }catch(...){
                            qDebug("usb_write_disconnect");
                            bError=true;
                            break;
                        }
                        if((uint)tmEmptyPkt.elapsed() > ((tmp_COMMUNICATION_STRUCT->nBufferW/5000)+500)*5){//((tmp_COMMUNICATION_STRUCT->nBufferW/5000)+200)*5){
                            qDebug("usb_write_overtime");
                            bError=true;
                            break;
                        }
                    }while(ulLength+ wLength <tmp_COMMUNICATION_STRUCT->nBufferW);
                    if(!bError){
                        ulLength = 0;
                        bError=false;
                        tmEmptyPkt.restart();
                        do{
                            try{
                                if(!pCommunication->read(tmp_COMMUNICATION_STRUCT->pBufferR,tmp_COMMUNICATION_STRUCT->nBufferR-rLength,
                                                         &rLength,(tmp_COMMUNICATION_STRUCT->nBufferR-rLength)/5000+500)){//tmp_COMMUNICATION_STRUCT->overTimeR)){//(tmp_COMMUNICATION_STRUCT->nBufferR-rLength)/5000+200)){
                                    qDebug("usb_read_disconnect");
                                    bError=true;
                                    break;
                                }
                            }catch(...){
                                qDebug("usb_read_disconnect");
                                bError=true;
                                break;
                            }
                            if((uint)tmEmptyPkt.elapsed() > ((tmp_COMMUNICATION_STRUCT->nBufferR/5000)+500)*5){//((tmp_COMMUNICATION_STRUCT->overTimeR){//tmp_COMMUNICATION_STRUCT->nBufferR/5000)+200)*5){
                                qDebug("usb_read_overtime");
                                bError=true;
                                break;
                            }
                        }while(ulLength+ rLength<tmp_COMMUNICATION_STRUCT->nBufferR);
                        if(!bError)
                            tmp_COMMUNICATION_STRUCT->isOK=true;
                    }
                }
                pCommunication->m_mutex.unlock();
            }else{
                pCommunication->m_mutex.unlock();
                QThread::usleep(100);
            }
        }
    }
    return true;
}
