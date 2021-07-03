#ifndef DECODETOOLS_H
#define DECODETOOLS_H

#include <QObject>
#include <QVariantMap>

#include "COMM_APA_Rx_Decode.h"
#include "ECanVci.h"

enum _CAN_ID
{
    EMCT_MSG_1 = 600,
    EMCT_MSG_2 = 601,
    EMCT_MSG_3 = 602,
    EMCT_MSG_4 = 603,
};

class DecodeTools : public QObject
{
    Q_OBJECT
public:
    explicit DecodeTools(QObject *parent = nullptr);
    ~DecodeTools();

    // get
    S4_VEH_RX_DATA GetData() const;
    QVariantMap GetMap() const;

    // operation
    void AddNewMessage(UINT ID, const uint8_T bytes[8]);
    bool IsDataFull();

    // static inline function
    static inline bool dbc_decode_emcTmsg1(emcTmsg1 *to, const uint8_T bytes[8]);
    static inline bool dbc_decode_emcTmsg2(emcTmsg2 *to, const uint8_T bytes[8]);
    static inline bool dbc_decode_emcTmsg3(emcTmsg3 *to, const uint8_T bytes[8]);
    static inline bool dbc_decode_emcTmsg4(emcTmsg4 *to, const uint8_T bytes[8]);

private:
    S4_VEH_RX_DATA      VsCOMM_S4_VEH_RX_Data;
    uint                mFlag;
};

#endif // DECODETOOLS_H
