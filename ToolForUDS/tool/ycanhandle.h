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

    // get
    int DeviceType() const;
    int DeviceInd() const;
    int CanInd() const;
    BOARD_INFO BoardInfo() const;

    // func
    bool Open() Q_DECL_OVERRIDE;
    bool Close() Q_DECL_OVERRIDE;

    bool IsOpen();
    bool SendData(CAN_OBJ obj);
    bool SendData(UINT CanID, const BYTE *data, uint32_t dataLength);

    QString GetLastError();

protected:
    SThread mThread;
    static int listenCan(void* pParam, const bool& bRunning);
    void analysisBuf(const QVector<CAN_OBJ>& bufs);

signals:
    void signError();
    void signCanMessage(const CAN_OBJ& buf);

private:
    int nDeviceType;
    int nDeviceInd;
    int nCANInd;
    int nReserved;
    INIT_CONFIG mVic;
    _ERR_INFO mErrInfo;
    BOARD_INFO mBoardInfo;
    CAN_STATUS mCanStatus;

    bool mIsOpen;
    QMutex mMutex;
};

#endif // YCANHANDLE_H
