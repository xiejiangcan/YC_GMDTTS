#ifndef UDSSERVER_H
#define UDSSERVER_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QException>
#include <QDebug>

#include "ycanhandle.h"

class UdsServer : public QObject
{
    Q_OBJECT
public:
    enum N_STATUS
    {
        N_OK = 0,           // 数据接受成功
        N_TIMEOUT_A,        // N_Ar/N_As超时，接收方和发送方都可以触发
        N_TIMEOUT_Bs,       // N_Bs超时，只有发送方触发
        N_TIMEOUT_Cr,       // N_Cr超时，只有接收方触发
        N_WRIONG_SN,        // 数据帧SN值错误，接收方触发
        N_INVALID_FS,       // FC中有未知FlowStatus值，发送方触发
        N_UNEXP_PDU,        // 收到未知的PDU，接收方触发
        N_WFT_OVRN,         // WAIT的帧数超过规定数量
        N_BUFFER_OVFLW,     // 收到OVERFLOW的FC回报，发送方触发
        N_ERROR             // 网络层错误时触发，发送方和接收方都可
    };
    explicit UdsServer(QObject *parent = nullptr);

    void process(YCanHandle *handle);
    void process(SObject* pObj, QString prop);
    void analysisCanObj(const CAN_OBJ& obj);

    // settings
    void setCanID(UINT sendID, UINT recvID);
    void setCanID(QString sendID, QString recvID);
    void setTimeout(UINT timeout);
    void setContent(QString content);
    void setRecvParams(uint fc_bs, uint fc_st);

    // gettings
    UINT    getSendCanID() const;
    UINT    getRecvCanID() const;
    UINT    getInterval() const;
    QString getContent() const;
    N_STATUS  getStatus() const;
    QString getBackPack() const;

protected:
    void deProcess();
    void sendFC();
    void sendObj(const CAN_OBJ& obj);

public slots:
    void slotPackAna(const CAN_MESSAGE_PACKAGE &buf);

    void slotTimeOut();
    void slotSendCF();

signals:
    void signCompleted();

private:
    UINT        mSendCanID;             // 发送的CANID
    UINT        mRecvCanID;             // 接受的CANID
    QString     mContent;               // 发送数据内容
    N_STATUS    mNStatus;               // 网络状态
    QString     mBackPack;              // 回包数据
    UINT        mBegTime;               // 检测开始时间
    UINT        mEndTime;               // 检测结束时间

    // time out
    UINT        mN_Bs;                  // 发送方收到FC的最大时间间隔
    UINT        mN_Cr;                  // 接收方收到CF的最大时间间隔
    UINT        mP2;                    // 发送方收到回包数据的最大时间间隔

    YCanHandle *mHandle;                // 网络层接口

    // CF Params
    bool mIsCF;                         // 接收方参数，是否为多帧
    UINT mLastFrames;                   // 接收方参数，还剩余多少Byte数据
    BYTE mCurFrameID;                   // 接收方参数，当前帧ID，用于帧号诊断

    // FC Params
    UINT mSendBlockSize;                // 发送方接收到的流控的BS
    UINT mSendSTmin;                    // 发送方接收到的流控的ST
    UINT mRecvBlockSize;                // 接收方设置的BS
    UINT mRecvSTmin;                    // 接受方设置的ST

    // temp Params
    UINT mLastBS;                       // 接收方临时参数，用于记录剩余BS

    QQueue<CAN_OBJ> mObjs;             // 发送方发送多帧数据时切割的CF数据集

    QTimer mCFTimer;                    // 定时器，用于发送CF数据，时间间隔由接受方设置的ST定
    QTimer mOutTimer;                   // 定时器，用于超时检测

    SObject*        m_dataOutObj;       // CAN通讯对象
    QString         m_propName;         // CAN通讯对象输出数据属性名称
};

#endif // UDSSERVER_H
