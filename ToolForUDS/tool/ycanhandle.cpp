#include "ycanhandle.h"
#include <QMessageBox>
#include <QMutexLocker>
#include <QMetaType>

#define TestCan

YCanHandle* YCanHandle::m_instance = nullptr;

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

    qRegisterMetaType<CAN_MESSAGE_PACKAGE>("CAN_MESSAGE_PACKAGE");
}

YCanHandle *YCanHandle::getInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new YCanHandle;
    }

    return m_instance;
}

YCanHandle::~YCanHandle()
{
    if(mThread.isRunning()){
        mThread.stop();
        mThread.wait();
        mThread.quit();
    }
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

void YCanHandle::setDeviceSize(int deviceSize)
{
    QMutexLocker locker(&mMutex);
    nDeviceSize = deviceSize;
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

bool YCanHandle::OpenAll()
{
    int failed = 0;
    for(int i = 0; i < nDeviceSize; ++i){
        if(!OpenDev(i)){
            ++failed;
        }
    }

    mThread.setUserFunction(listenAllCan);
    mThread.start();
    mIsOpen = true;
    return failed == 0;
}

bool YCanHandle::OpenDev(int devInd)
{
    // 连接设备
    auto dwRel = OpenDevice(nDeviceType, devInd, nReserved);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, 0, &mErrInfo);
        ReadErrInfo(nDeviceType, devInd, 1, &mErrInfo);
        return false;
    }

    // 初始化CAN
    dwRel = InitCAN(nDeviceType, devInd, 0, &mVic);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, 0, &mErrInfo);
        CloseDevice(nDeviceType, nDeviceInd);
        return false;
    }
    dwRel = InitCAN(nDeviceType, devInd, 1, &mVic);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, 1, &mErrInfo);
        CloseDevice(nDeviceType, nDeviceInd);
        return false;
    }

    // 启动设备
    dwRel = StartCAN(nDeviceType, devInd, 0);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, 0, &mErrInfo);
        CloseDevice(nDeviceType, nDeviceInd);
        return false;
    }

    dwRel = StartCAN(nDeviceType, devInd, 1);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, 1, &mErrInfo);
        CloseDevice(nDeviceType, nDeviceInd);
        return false;
    }

    // 获取设备信息
    dwRel = ReadBoardInfo(nDeviceType, devInd, &mBoardInfo);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, 0, &mErrInfo);
        ReadErrInfo(nDeviceType, devInd, 1, &mErrInfo);
        return false;
    }

    // 获取CAN状态
    dwRel = ReadCANStatus(nDeviceType, devInd, 0, &mCanStatus);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, 0, &mErrInfo);
        return false;
    }
    dwRel = ReadCANStatus(nDeviceType, devInd, 1, &mCanStatus);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, 1, &mErrInfo);
        return false;
    }

    return true;
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

    mThread.setUserFunction(listenCan);
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

bool YCanHandle::CloseAll()
{
    if(mThread.isRunning()){
        mThread.stop();
        mThread.wait();
        mThread.quit();
    }

    int failed = 0;
    for(int i = 0; i < nDeviceSize; ++i){
        if(CloseDev(i) == STATUS_ERR){
            ++failed;
        }
    }

    return failed == 0;
}

bool YCanHandle::CloseDev(int devInd)
{
    return CloseChan(devInd, 0) && CloseChan(devInd, 1);
}

bool YCanHandle::CloseChan(int devInd, int devChan)
{
    auto dwRel = CloseDevice(nDeviceType, devInd);
    if(dwRel == STATUS_ERR){
        ReadErrInfo(nDeviceType, devInd, devChan, &mErrInfo);
        return false;
    }
    return true;
}

bool YCanHandle::SendDataToAll(CAN_OBJ data)
{
    if(!IsOpen())
        return false;
    int failed = 0;
    for(int i = 0; i < nDeviceSize; ++i){
        if(!SendDataToDev(data, i)){
            ++failed;
        }
    }
    return failed == 0;
}

bool YCanHandle::SendDataToDev(CAN_OBJ data, int devInd)
{
    if(!IsOpen())
        return false;
    return SendDataToChan(data, devInd, 0)
            && SendDataToChan(data, devInd, 1);
}

bool YCanHandle::SendDataToChan(CAN_OBJ data, int devInd,
                                int devChan)
{
    if(!IsOpen())
        return false;
    auto dwRes = Transmit(nDeviceType, devInd, devChan,
                          &data, 1);
    if(dwRes == 1)
        return true;
    return false;
}

bool YCanHandle::IsOpen()
{
    return mIsOpen;
}

bool YCanHandle::SendData(CAN_OBJ obj)
{
    if(!IsOpen())
        return false;
    auto dwRes = Transmit(nDeviceType, nDeviceInd, nCANInd,
                          &obj, 1);
    if(dwRes == 1)
        return true;
    return false;
}

bool YCanHandle::SendData(UINT CanID, const BYTE *data, uint32_t dataLength)
{
    if(!IsOpen())
        return false;
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
        return QString(QStringLiteral("CAN控制器内部FIFO溢出"));
    case ERR_CAN_ERRALARM:
        return QString(QStringLiteral("CAN控制器错误报警"));
    case ERR_CAN_PASSIVE:
        return QString(QStringLiteral("CAN控制器消极错误"));
    case ERR_CAN_LOSE:
        return QString(QStringLiteral("CAN控制器仲裁丢失"));
    case ERR_CAN_BUSERR:
        return QString(QStringLiteral("CAN控制器总线错误"));
    case ERR_CAN_REG_FULL:
        return QString(QStringLiteral("CAN接收寄存器满"));
    case ERR_CAN_REG_OVER:
        return QString(QStringLiteral("CAN接收寄存器溢出"));
    case ERR_CAN_ZHUDONG:
        return QString(QStringLiteral("CAN控制器主动错误"));
    case ERR_DEVICEOPENED:
        return QString(QStringLiteral("设备已经打开"));
    case ERR_DEVICEOPEN:
        return QString(QStringLiteral("打开设备错误"));
    case ERR_DEVICENOTOPEN:
        return QString(QStringLiteral("设备没有打开"));
    case ERR_BUFFEROVERFLOW:
        return QString(QStringLiteral("缓冲区溢出"));
    case ERR_DEVICENOTEXIST:
        return QString(QStringLiteral("此设备不存在"));
    case ERR_LOADKERNELDLL:
        return QString(QStringLiteral("装载动态库失败"));
    case ERR_CMDFAILED:
        return QString(QStringLiteral("执行命令失败错误码"));
    case ERR_BUFFERCREATE:
        return QString(QStringLiteral("内存不足"));
    default:
        return QString(QStringLiteral("未知错误"));
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
            handle->analysisBuf(handle->nDeviceInd, handle->nCANInd, dataPool);
        }
        QThread::msleep(1);
    }

    return 0;
}

int YCanHandle::listenAllCan(void *pParam, const bool &bRunning)
{
    YCanHandle* handle = static_cast<YCanHandle*>(pParam);
    if(!handle){
        return -1;
    }

    while(bRunning){
        for(int i = 0; i < handle->nDeviceSize; ++i){
            for(int j = 0; j < 2; ++j){
                // 获取CAN状态
                auto dwRel = ReadCANStatus(handle->nDeviceType,
                                           i,
                                           j,
                                           &handle->mCanStatus);
                if(dwRel == STATUS_ERR){
                    ReadErrInfo(handle->nDeviceType,
                                i,
                                j,
                                &handle->mErrInfo);
                    emit handle->signError();
                }
                QMutexLocker locker(&handle->mMutex);
                DWORD nRes = GetReceiveNum(handle->nDeviceType, i, j);
                if(nRes != 0){
                    DWORD nBeg = 0;
                    QVector<CAN_OBJ> dataPool(nRes);
                    while(nRes){
                        DWORD readed = Receive(handle->nDeviceType, i, j,
                                               dataPool.data()+nBeg, nRes, 100);
                        nBeg += readed;
                        nRes -= readed;
                    }
                    handle->analysisBuf(i, j, dataPool);
                }
            }
        }

        QThread::msleep(1);
    }

    return 0;
}

Q_DECLARE_METATYPE(CAN_OBJ)
void YCanHandle::analysisBuf(int devInd, int devChan, const QVector<CAN_OBJ> &bufs)
{
    //QList<CAN_MESSAGE_PACKAGE> msgs;
    foreach(auto buf, bufs){
        CAN_MESSAGE_PACKAGE msg;
        msg.devInd = devInd;
        msg.devChan = devChan;
        msg.canObj = QVariant::fromValue(buf);
        emit signCanMessage(msg);
        //msgs.append(msg);
    }
}


QByteArray can2ByteArray(const CAN_OBJ &obj)
{
    const uint nSize = sizeof(CAN_OBJ);
    QByteArray res(nSize, '0');
    memcpy(res.data(), (const void*)(&obj), nSize);
    return res;
}

CAN_OBJ byteArray2Can(const QByteArray &array)
{
    const uint nSize = sizeof(CAN_OBJ);
    CAN_OBJ res;
    if(array.size() != nSize){
        qDebug() << "byteArray to CAN_OBJ error";
        return res;
    }
    memcpy(&res, array.data(), nSize);
    return res;
}
