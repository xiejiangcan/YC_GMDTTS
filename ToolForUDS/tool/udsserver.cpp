#include "udsserver.h"

UdsServer::UdsServer(QObject *parent)
    : QObject(parent),
      mN_Bs(1000),
      mN_Cr(1000),
      mHandle(nullptr),
      mIsCF(false),
      mLastFrames(0),
      mCurFrameID(0),
      mSendBlockSize(0),
      mRecvBlockSize(0)
{
    connect(&mOutTimer, &QTimer::timeout, this, &UdsServer::slotTimeOut);
    connect(&mCFTimer, &QTimer::timeout, this, &UdsServer::slotSendCF);
}

void UdsServer::process(YCanHandle *handle)
{
    if(!handle)
        throw QException();

    mHandle = handle;
    connect(mHandle, &YCanHandle::signCanMessage,
            this, &UdsServer::slotPackAna);

    mObjs.clear();
    QStringList content = mContent.split(' ');
    int nSize = content.size();
    if(nSize > 7){
        CAN_OBJ obj_FF;
        obj_FF.ID = mSendCanID;
        obj_FF.DataLen = 8;
        obj_FF.Data[0] = 0x10 + ((nSize & 0xF00) >> 8);
        obj_FF.Data[1] = nSize & 0xFF;
        int index = 2;
        auto iter = content.begin();
        for(;
            iter != content.end() && index < 8;
            ++iter, ++index){
            obj_FF.Data[index] = (*iter).toUInt(nullptr, 16);
        }
        content.erase(content.begin(), iter);
        index = 0;
        do{
            int lastSize = content.size();
            CAN_OBJ obj;
            obj.ID = mSendCanID;
            obj.DataLen = lastSize > 7 ? 8 : lastSize + 1;
            obj.Data[0] = 0x20 + (index++ & 0x0F);
            for(int i = 1; i < obj.DataLen; ++i){
                obj.Data[i] = content.at(i-1).toUInt(nullptr, 16);
            }
            content.erase(content.begin(), content.begin()+(obj.DataLen-1));
            mObjs.enqueue(obj);
        }while(content.size() == 0);
        sendObj(obj_FF);
        mOutTimer.start(mN_Bs);
    }else{
        CAN_OBJ obj;
        obj.DataLen = 8;
        obj.ID = mSendCanID;
        int index = 1;
        obj.Data[0] = 0x00 + nSize;
        for(auto iter = content.begin();
            iter != content.end();
            ++iter, ++index){
            obj.Data[index] = (*iter).toUInt(nullptr, 16);
        }
        sendObj(obj);
        mOutTimer.start(mP2);
    }
}

void UdsServer::process(SObject *pObj, QString prop)
{
    mHandle = YCanHandle::getInstance();
    connect(mHandle, &YCanHandle::signCanMessage,
            this, &UdsServer::slotPackAna);

    m_dataOutObj = pObj;
    m_propName = prop;

    mObjs.clear();
    mBackPack.clear();
    QStringList content = mContent.split(' ');
    int nSize = content.size();
    if(nSize > 7){
        CAN_OBJ obj_FF;
        obj_FF.ID = mSendCanID;
        obj_FF.DataLen = 8;
        obj_FF.Data[0] = 0x10 + ((nSize & 0xF00) >> 8);
        obj_FF.Data[1] = nSize & 0xFF;
        int index = 2;
        auto iter = content.begin();
        for(;
            iter != content.end() && index < 8;
            ++iter, ++index){
            obj_FF.Data[index] = (*iter).toUInt(nullptr, 16);
        }
        content.erase(content.begin(), iter);
        index = 0;
        do{
            int lastSize = content.size();
            CAN_OBJ obj;
            obj.ID = mSendCanID;
            int realSize = lastSize > 7 ? 8 : lastSize + 1;
            obj.DataLen = 8;
            obj.Data[0] = 0x21 + (index++ & 0x0F);
            for(int i = 1; i < realSize; ++i){
                obj.Data[i] = content.at(i-1).toUInt(nullptr, 16);
            }
            content.erase(content.begin(), content.begin()+(realSize-1));
            mObjs.enqueue(obj);
        }while(content.size() != 0);
        sendObj(obj_FF);
        mOutTimer.start(mN_Bs);
    }else{
        CAN_OBJ obj;
        obj.DataLen = 8;
        obj.ID = mSendCanID;
        int index = 1;
        obj.Data[0] = 0x00 + nSize;
        for(auto iter = content.begin();
            iter != content.end();
            ++iter, ++index){
            obj.Data[index] = (*iter).toUInt(nullptr, 16);
        }
        sendObj(obj);
        mOutTimer.start(mP2);
    }
}

void UdsServer::deProcess()
{
    if(mHandle){
        disconnect(mHandle, &YCanHandle::signCanMessage,
                   this, &UdsServer::slotPackAna);
        mHandle = nullptr;
    }
    mObjs.clear();
    mLastFrames = 0;
    mCurFrameID = 0;
    emit signCompleted();
}

void UdsServer::sendFC()
{
    CAN_OBJ obj;
    obj.ID = mSendCanID;
    obj.DataLen = 8;
    obj.Data[0] = 0x30;
    obj.Data[1] = mRecvBlockSize;
    obj.Data[2] = mRecvSTmin;

    sendObj(obj);
    mLastBS = mRecvBlockSize;
    mOutTimer.start(mN_Cr);
}

Q_DECLARE_METATYPE(CAN_OBJ)
void UdsServer::sendObj(const CAN_OBJ &obj)
{
    //    if(mHandle){
    //        if(!mHandle->SendData(obj)){
    //            mNStatus = N_ERROR;
    //            deProcess();
    //        }
    //    }else{
    //        m_dataOutObj->setPropertyS(m_propName, QVariant::fromValue(obj));
    //    }
    mNStatus = N_ERROR;
    m_dataOutObj->setPropertyS(m_propName, QVariant::fromValue(obj));
}

void UdsServer::analysisCanObj(const CAN_OBJ &buf, const CAN_MESSAGE_PACKAGE &pkg)
{
    if(buf.ID != mRecvCanID)
        return;
    const BYTE* pData = buf.Data;
    UINT n_pcitype = (pData[0] & 0xF0) >> 4;
    emit signalShowMsg(pkg);
    switch (n_pcitype) {
    case 0:{
        // 单帧数据
        UINT pci = pData[0] & 0x0F;
        if(pci == 0x0 || pci > 0x07){    // ignore this frame
            qDebug() << "ignore this frame";
            return;
        }
        mBackPack.clear();
        for(UINT i = 1; i <= pci; ++i){
            mBackPack.append(QString("%1 ").arg(pData[i], 2, 16, QChar('0')));
        }
        if(pData[3] == 0x78)
            break;
        mNStatus = N_OK;
        deProcess();
        break;
    }
    case 1:{
        // 多帧数据流开始
        UINT pci = pData[0] & 0x0F;
        pci = (pci << 8) + pData[1];
        if(pci >= 0x0 && pci < 0x7){ // ignore this frame
            qDebug() << "ignore this frame";
            return;
        }else if(pci > 0xFFF){ // overflow
            qDebug() << "data length overflow";
            return;
        }else{
            mIsCF = true;
            mLastFrames = pci - 6;
            mBackPack.clear();
            for(UINT i = 2; i < 8; ++i){
                mBackPack.append(QString("%1 ").arg(pData[i], 2, 16, QChar('0')));
            }
            sendFC();
        }
        break;
    }
    case 2:{
        // 多帧数据流
        BYTE SN = pData[0] & 0x0F;
        if(SN != 0x0 && SN != mCurFrameID+1){
            qDebug() << "N_WRONG_SN";
            mNStatus = N_WRIONG_SN;
            mBackPack.clear();
            deProcess();
            return;
        }
        mCurFrameID = SN;
        --mLastBS;
        if(mLastFrames / 7){
            for(UINT i = 1; i < 8; ++i){
                mBackPack.append(QString("%1 ").arg(pData[i], 2, 16, QChar('0')));
            }
            mLastFrames -= 7;
        }else{
            for(UINT i = 1; i <= mLastFrames % 7; ++i){
                mBackPack.append(QString("%1 ").arg(pData[i], 2, 16, QChar('0')));
            }
            mLastFrames = 0;
        }
        if(mLastFrames == 0){
            mNStatus = N_OK;
            deProcess();
            return;
        }
        if(mLastBS == 0)
            sendFC();
        break;
    }
    case 3:{
        BYTE FS = pData[0] & 0x0F;
        mSendBlockSize = pData[1] == 0 ? 0xFF : pData[1];
        mSendSTmin = pData[2];
        switch (FS) {
        case 0:{    // continue to send
            qDebug() << "begin muti frame";
            mCFTimer.start(mSendSTmin);
            break;
        }
        case 1:{    // wait
            break;
        }
        case 2:{    // overflow
            qDebug() << "N_BUFFER_OVFLW";
            mNStatus = N_BUFFER_OVFLW;
            deProcess();
            return;
        }
        default:    // reserved
            break;
        }
        break;
    }
    default:{
        break;
    }
    }
    mOutTimer.stop();
}

void UdsServer::setCanID(UINT sendID, UINT recvID)
{
    mSendCanID = sendID;
    mRecvCanID = recvID;
}

void UdsServer::setCanID(QString sendID, QString recvID)
{
    mSendCanID = sendID.toUInt(nullptr, 16);
    mRecvCanID = recvID.toUInt(nullptr, 16);
}

void UdsServer::setTimeout(UINT timeout)
{
    mP2 = timeout;
}

void UdsServer::setContent(QString content)
{
    mContent = content.trimmed();
}

void UdsServer::setRecvParams(uint fc_bs, uint fc_st)
{
    mRecvBlockSize = fc_bs;
    mRecvSTmin = fc_st;
}

UINT UdsServer::getSendCanID() const
{
    return mSendCanID;
}

UINT UdsServer::getRecvCanID() const
{
    return mRecvCanID;
}

UINT UdsServer::getInterval() const
{
    return mEndTime - mBegTime;
}

QString UdsServer::getContent() const
{
    return mContent;
}

UdsServer::N_STATUS UdsServer::getStatus() const
{
    return mNStatus;
}

QString UdsServer::getBackPack() const
{
    return mBackPack;
}

void UdsServer::slotPackAna(const CAN_MESSAGE_PACKAGE &buf)
{
    CAN_OBJ obj = buf.canObj.value<CAN_OBJ>();
    analysisCanObj(obj, buf);
}

void UdsServer::slotTimeOut()
{
    mOutTimer.stop();
    qDebug() << "time out";
    deProcess();
}

void UdsServer::slotSendCF()
{
    if(mSendBlockSize > 0 && mObjs.size() > 0){
        CAN_OBJ obj = mObjs.dequeue();
        sendObj(obj);
        --mSendBlockSize;
    }else{
        mCFTimer.stop();
        if(mObjs.size() == 0){
            mOutTimer.start(mP2);
        }else{
            mOutTimer.start(mN_Bs);
        }
    }
}
