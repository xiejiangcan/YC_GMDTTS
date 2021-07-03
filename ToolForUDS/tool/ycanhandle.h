#ifndef YCANHANDLE_H
#define YCANHANDLE_H

#include "ToolForUDS_global.h"
#include "abstracthandle.h"

enum BaudType {
    Baud5 = 0,
    Baud10,
    Baud20,
    Baud40,
    Baud50,
    Baud80,
    Baud100,
    Baud125,
    Baud200,
    Baud250,
    Baud400,
    Baud500,
    Baud666,
    Baud800,
    Baud1000
};

struct CAN_MESSAGE_PACKAGE
{
    int         devInd;
    int         devChan;
    QVariant    canObj;
};

class YCanHandle : public AbstractHandle
{
    Q_OBJECT
    Q_PROPERTY(int DeviceType READ DeviceType WRITE setDeviceType)
    Q_PROPERTY(int DeviceInd READ DeviceInd WRITE setDeviceInd)
    Q_PROPERTY(int CanInd READ CanInd WRITE setCanInd)
public:
    explicit YCanHandle(QObject *parent = nullptr);
    ~YCanHandle();

    // set
    void setDeviceType(int deviceType);
    void setDeviceInd(int deviceInd);
    void setCanInd(int canInd);
    void setBaudType(BaudType baudType);
    void setDeviceSize(int deviceSize);

    // get
    int DeviceType() const;
    int DeviceInd() const;
    int CanInd() const;
    BOARD_INFO BoardInfo() const;

    // func
    bool OpenAll();
    bool CloseAll();
    bool SendDataToAll(CAN_OBJ data);

    bool Open() Q_DECL_OVERRIDE;
    bool Close() Q_DECL_OVERRIDE;

    bool IsOpen();
    bool SendData(CAN_OBJ obj);
    bool SendData(UINT CanID, const BYTE *data, uint32_t dataLength);

    QString GetLastError();

protected:
    bool OpenDev(int devInd);
    bool OpenChan(int devInd, int devChan);
    bool CloseDev(int devInd);
    bool CloseChan(int devInd, int devChan);
    bool SendDataToDev(CAN_OBJ data, int devInd);
    bool SendDataToChan(CAN_OBJ data, int devInd, int devChan);


protected:
    SThread mThread;
    static int listenCan(void* pParam, const bool& bRunning);
    static int listenAllCan(void* pParam, const bool& bRunning);
    void analysisBuf(int devInd, int devChan, const QVector<CAN_OBJ>& bufs);

signals:
    void signError();
    void signCanMessage(const CAN_MESSAGE_PACKAGE& buf);

private:
    int nDeviceType;
    int nDeviceInd;
    int nCANInd;
    int nReserved;
    int nDeviceSize;
    INIT_CONFIG mVic;
    _ERR_INFO mErrInfo;
    BOARD_INFO mBoardInfo;
    CAN_STATUS mCanStatus;

    bool mIsOpen;
    QMutex mMutex;
};

QByteArray can2ByteArray(const CAN_OBJ& obj);
CAN_OBJ byteArray2Can(const QByteArray& array);

#endif // YCANHANDLE_H
