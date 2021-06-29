#include "ycanhandle.h"
#include <QMessageBox>
#include <QMutexLocker>

#define TestCan

YCanHandle::YCanHandle(QObject *parent)
    : AbstractHandle(parent)
{
    // 初始化参数列表
    nDeviceType = USBCAN2;
    nDeviceInd = 0;
    nReserved = 0;      //无意义参数
    nCANInd = 0;

    mVic.AccCode = 0;
    mVic.AccMask = 0xffffff;
    mVic.Filter = 0;
    mVic.Timing0 = 0x31;
    mVic.Timing1 = 0x1C;
    mVic.Mode = 0;

    mIsOpen = false;
    // 初始化监听线程
    mThread.setUserParam(this);
    mThread.setUserFunction(listenCan);

    qRegisterMetaType<CAN_OBJ>("CAN_OBJ");
}

YCanHandle::~YCanHandle()
{
    Close();
}

void YCanHandle::setDeviceType(int deviceType)
{
    QMutexLocker locker(&mMutex);
    nDeviceType = deviceType;
}

void YCanHandle::setDeviceInd(int deviceInd)
{
    QMutexLocker locker(&mMutex);
    nDeviceInd = deviceInd;
}

void YCanHandle::setCanInd(int canInd)
{
    QMutexLocker locker(&mMutex);
    nCANInd = canInd;
}

void YCanHandle::setBaudType(BaudType baudType)
{
    switch (baudType) {
    case BaudType::Baud5:
        mVic.Timing0 = 0xBF;
        mVic.Timing1 = 0xFF;
        break;
    case BaudType::Baud10:
        mVic.Timing0 = 0x31;
        mVic.Timing1 = 0x1C;
        break;
    case BaudType::Baud20:
        mVic.Timing0 = 0x18;
        mVic.Timing1 = 0x1C;
        break;
    case BaudType::Baud40:
        mVic.Timing0 = 0x87;
        mVic.Timing1 = 0xFF;
        break;
    case BaudType::Baud50:
        mVic.Timing0 = 0x09;
        mVic.Timing1 = 0x1C;
        break;
    case BaudType::Baud80:
        mVic.Timing0 = 0x83;
        mVic.Timing1 = 0xFF;
        break;
    case BaudType::Baud100:
        mVic.Timing0 = 0x04;
        mVic.Timing1 = 0x1C;
        break;
    case BaudType::Baud125:
        mVic.Timing0 = 0x03;
        mVic.Timing1 = 0x1C;
        break;
    case BaudType::Baud200:
        mVic.Timing0 = 0x81;
        mVic.Timing1 = 0xFA;
        break;
    case BaudType::Baud250:
        mVic.Timing0 = 0x01;
        mVic.Timing1 = 0x1C;
        break;
    case BaudType::Baud400:
        mVic.Timing0 = 0x80;
        mVic.Timing1 = 0xFA;
        break;
    case BaudType::Baud500:
        mVic.Timing0 = 0x00;
        mVic.Timing1 = 0x1C;
        break;
    case BaudType::Baud666:
        mVic.Timing0 = 0x80;
        mVic.Timing1 = 0xB6;
        break;
    case BaudType::Baud800:
        mVic.Timing0 = 0x00;
        mVic.Timing1 = 0x16;
        break;
    case BaudType::Baud1000:
        mVic.Timing0 = 0x00;
        mVic.Timing1 = 0x14;
        break;
    }
}

int YCanHandle::DeviceType() const
{
    return nDeviceType;
}

int YCanHandle::DeviceInd() const
{
    return nDeviceInd;
}

int YCanHandle::CanInd() const
{
    return nCANInd;
}

BOARD_INFO YCanHandle::BoardInfo() const
{
    return mBoardInfo;
}

bool YCanHandle::Open()
{
    // 连接设备
    auto dwRel = OpenDevice(nDeviceType, nDeviceInd, nReserved);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, nDeviceInd, nCANInd, &mErrInfo);
        return false;
    }
    // 初始化CAN
    dwRel = InitCAN(nDeviceType, nDeviceInd, nCANInd, &mVic);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, nDeviceInd, nCANInd, &mErrInfo);
        CloseDevice(nDeviceType, nDeviceInd);
        return false;
    }
    // 启动设备
    dwRel = StartCAN(nDeviceType, nDeviceInd, nCANInd);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, nDeviceInd, nCANInd, &mErrInfo);
        CloseDevice(nDeviceType, nDeviceInd);
        return false;
    }
    // 获取设备信息
    dwRel = ReadBoardInfo(nDeviceType, nDeviceInd, &mBoardInfo);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, nDeviceInd, nCANInd, &mErrInfo);
        return false;
    }
    // 获取CAN状态
    dwRel = ReadCANStatus(nDeviceType, nDeviceInd, nCANInd, &mCanStatus);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, nDeviceInd, nCANInd, &mErrInfo);
        return false;
    }

    mThread.start();
    mIsOpen = true;
    return true;
}

bool YCanHandle::Close()
{
    if(mThread.isRunning()){
        mThread.stop();
        mThread.wait();
        mThread.quit();
    }
    auto dwRel = CloseDevice(nDeviceType, nDeviceInd);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, nDeviceInd, nCANInd, &mErrInfo);
        return false;
    }
    mIsOpen = false;
    return true;
}

bool YCanHandle::IsOpen()
{
    return mIsOpen;
}

bool YCanHandle::SendData(CAN_OBJ obj)
{
    auto dwRes = Transmit(nDeviceType, nDeviceInd, nCANInd,
                          &obj, 1);
    if(dwRes == 1)
        return true;
    return false;
}

bool YCanHandle::SendData(UINT CanID, const BYTE *data, uint32_t dataLength)
{
    QVector<CAN_OBJ> dataPool;
    for(uint32_t i = 0; i < dataLength;){
        if(dataLength - i >= 8){
            CAN_OBJ obj;
            obj.ID = CanID;
#ifdef TestCan
            obj.SendType = 2;
#elif
            obj.sendType = 0;
#endif
            obj.DataLen = 8;
            memcpy(obj.Data, data+i, sizeof(BYTE)*8);

            i += 8;
            dataPool.append(obj);
        }else{
            int nRes = dataLength - i;
            CAN_OBJ obj;
            obj.ID = CanID;
#ifdef TestCan
            obj.SendType = 2;
#elif
            obj.sendType = 0;
#endif
            obj.DataLen = nRes;
            memcpy(obj.Data, data+i, sizeof(BYTE)*nRes);

            i += nRes;
            dataPool.append(obj);
        }
    }

    auto dwRes = Transmit(nDeviceType, nDeviceInd, nCANInd,
                          dataPool.data(), dataPool.size());
    if(dwRes == dataPool.size())
        return true;
    return false;
}

QString YCanHandle::GetLastError()
{
    switch (mErrInfo.ErrCode) {
    case ERR_CAN_OVERFLOW:
        return QString(tr("CAN控制器内部FIFO溢出"));
    case ERR_CAN_ERRALARM:
        return QString(tr("CAN控制器错误报警"));
    case ERR_CAN_PASSIVE:
        return QString(tr("CAN控制器消极错误"));
    case ERR_CAN_LOSE:
        return QString(tr("CAN控制器仲裁丢失"));
    case ERR_CAN_BUSERR:
        return QString(tr("CAN控制器总线错误"));
    case ERR_CAN_REG_FULL:
        return QString(tr("CAN接收寄存器满"));
    case ERR_CAN_REG_OVER:
        return QString(tr("CAN接收寄存器溢出"));
    case ERR_CAN_ZHUDONG:
        return QString(tr("CAN控制器主动错误"));
    case ERR_DEVICEOPENED:
        return QString(tr("设备已经打开"));
    case ERR_DEVICEOPEN:
        return QString(tr("打开设备错误"));
    case ERR_DEVICENOTOPEN:
        return QString(tr("设备没有打开"));
    case ERR_BUFFEROVERFLOW:
        return QString(tr("缓冲区溢出"));
    case ERR_DEVICENOTEXIST:
        return QString(tr("此设备不存在"));
    case ERR_LOADKERNELDLL:
        return QString(tr("装载动态库失败"));
    case ERR_CMDFAILED:
        return QString(tr("执行命令失败错误码"));
    case ERR_BUFFERCREATE:
        return QString(tr("内存不足"));
    default:
        return QString(tr("未知错误"));
    }
}

int YCanHandle::listenCan(void *pParam, const bool &bRunning)
{
    YCanHandle* handle = static_cast<YCanHandle*>(pParam);
    if(!handle){
        return -1;
    }

    while(bRunning){
        // 获取CAN状态
        auto dwRel = ReadCANStatus(handle->nDeviceType,
                                   handle->nDeviceInd,
                                   handle->nCANInd,
                                   &handle->mCanStatus);
        if(dwRel == STATUS_ERR){
            ReadErrInfo(handle->nDeviceType,
                        handle->nDeviceInd,
                        handle->nCANInd,
                        &handle->mErrInfo);
            emit handle->signError();
        }

        QMutexLocker locker(&handle->mMutex);
        DWORD nRes = GetReceiveNum(handle->nDeviceType, handle->nDeviceInd, handle->nCANInd);
        if(nRes != 0){
            DWORD nBeg = 0;
            QVector<CAN_OBJ> dataPool(nRes);
            while(nRes){
                DWORD readed = Receive(handle->nDeviceType, handle->nDeviceInd, handle->nCANInd,
                                       dataPool.data()+nBeg, nRes, 100);
                nBeg += readed;
                nRes -= readed;
            }
            handle->analysisBuf(dataPool);
        }
        QThread::msleep(1);
    }

    return 0;
}



void YCanHandle::analysisBuf(const QVector<CAN_OBJ> &bufs)
{
    foreach(auto buf, bufs){
        emit signCanMessage(buf);
    }
}


