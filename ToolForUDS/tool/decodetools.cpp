#include "decodetools.h"

DecodeTools::DecodeTools(QObject *parent)
    : QObject(parent)
    , mFlag(0)
{

}

DecodeTools::~DecodeTools()
{

}

S4_VEH_RX_DATA DecodeTools::GetData() const
{
    return VsCOMM_S4_VEH_RX_Data;
}

QVariantMap DecodeTools::GetMap() const
{
    QVariantMap info;
    info.insert(QStringLiteral("电源"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg1_258.VBAT_P_F_ADC));
    info.insert(QStringLiteral("KEYON唤醒源"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg1_258.IGN_ADC));
    info.insert(QStringLiteral("CAN唤醒源"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg1_258.CAN_WAKE_UP_ADC));
    info.insert(QStringLiteral("ETH_V3P3"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg1_258.V3P3_ETH_ADC));
    info.insert(QStringLiteral("V3P3_UP"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg1_258.V3P3_UP_ADC));
    info.insert(QStringLiteral("VDD_1V25"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg2_259.VDD_1V25_UP_ADC));
    info.insert(QStringLiteral("VDD_CNN_0V9"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg2_259.J2_CNN_0V9_ADC));
    info.insert(QStringLiteral("VDD_0V9"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg2_259.J2_VDD_0V9_ADC));
    info.insert(QStringLiteral("VDDQ_DDR_1V1"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg2_259.J2_VDDQ_DDR_1V1_AI_ADC));
    info.insert(QStringLiteral("VDDA_1V8"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg2_259.J2_VDDA_1V8_AI_ADC));
    info.insert(QStringLiteral("VDD_DDR_1V0"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A.J2A_VDD_DDR_1V0_ADC));
    info.insert(QStringLiteral("VDD_CORE_A0"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A.J2A_VDD_CORE_AO_AI_ADC));
    info.insert(QStringLiteral("V3P3_J2"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A.V3P3_J2_ADC));
    info.insert(QStringLiteral("SYS_IO_1P8V"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A.SYS_IO_1P8V_ADC));
    info.insert(QStringLiteral("板上温度"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A.NTC_temp_ADC));
    info.insert(QStringLiteral("J2工作状态"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B.J2_STAT_OK));
    info.insert(QStringLiteral("CAN2"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B.CAN_STAT_OK));
    info.insert(QStringLiteral("J2温度"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A.J2_TEMP_ADC));
    info.insert(QStringLiteral("CPU"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B.CPU_USED_RATE));
    info.insert(QStringLiteral("BPU"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B.BPU_USED_RATE));
    return info;
}

void DecodeTools::AddNewMessage(UINT ID, const uint8_T bytes[])
{
    switch (ID) {
    case _CAN_ID::EMCT_MSG_1:
        mFlag |= 1;
        dbc_decode_emcTmsg1(&VsCOMM_S4_VEH_RX_Data.emcTmsg1_258, bytes);
        break;
    case _CAN_ID::EMCT_MSG_2:
        mFlag |= 1 << 1;
        dbc_decode_emcTmsg2(&VsCOMM_S4_VEH_RX_Data.emcTmsg2_259, bytes);
        break;
    case _CAN_ID::EMCT_MSG_3:
        mFlag |= 1 << 2;
        dbc_decode_emcTmsg3(&VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A, bytes);
        break;
    case _CAN_ID::EMCT_MSG_4:
        mFlag |= 1 << 3;
        dbc_decode_emcTmsg4(&VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B, bytes);
        break;
    default: break;
    }
}

bool DecodeTools::IsDataFull()
{
    if(mFlag == 15){
        mFlag = 0;
        return true;
    }
    return false;
}

bool DecodeTools::dbc_decode_emcTmsg1(emcTmsg1 *to, const uint8_T bytes[])
{
    uint32_T raw;
    raw  = ((uint32_T)((bytes[0]))) << 4; ///< 8 bit(s) from B7
    raw |= ((uint32_T)((bytes[1] >> 4) & 0x0f)); ///< 4 bit(s) from B15
    to->VBAT_P_F_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[1]) & 0x0f)) << 8; ///< 4 bit(s) from B11
    raw |= ((uint32_T)((bytes[2]))); ///< 8 bit(s) from B23
    to->IGN_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[3]))) << 4; ///< 8 bit(s) from B31
    raw |= ((uint32_T)((bytes[4] >> 4) & 0x0f)); ///< 4 bit(s) from B39
    to->CAN_WAKE_UP_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[4]) & 0x0f)) << 8; ///< 4 bit(s) from B35
    raw |= ((uint32_T)((bytes[5]))); ///< 8 bit(s) from B47
    to->V3P3_ETH_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[6]))) << 4; ///< 8 bit(s) from B55
    raw |= ((uint32_T)((bytes[7] >> 4) & 0x0f)); ///< 4 bit(s) from B63
    to->V3P3_UP_ADC = ((raw * 0.01));

    return true;
}

bool DecodeTools::dbc_decode_emcTmsg2(emcTmsg2 *to, const uint8_T bytes[])
{
    uint32_T raw;
    raw  = ((uint32_T)((bytes[0]))) << 4; ///< 8 bit(s) from B7
    raw |= ((uint32_T)((bytes[1] >> 4) & 0x0f)); ///< 4 bit(s) from B15
    to->VDD_1V25_UP_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[1]) & 0x0f)) << 8; ///< 4 bit(s) from B11
    raw |= ((uint32_T)((bytes[2]))); ///< 8 bit(s) from B23
    to->J2_CNN_0V9_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[3]))) << 4; ///< 8 bit(s) from B31
    raw |= ((uint32_T)((bytes[4] >> 4) & 0x0f)); ///< 4 bit(s) from B39
    to->J2_VDD_0V9_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[4]) & 0x0f)) << 8; ///< 4 bit(s) from B35
    raw |= ((uint32_T)((bytes[5]))); ///< 8 bit(s) from B47
    to->J2_VDDQ_DDR_1V1_AI_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[6]))) << 4; ///< 8 bit(s) from B55
    raw |= ((uint32_T)((bytes[7] >> 4) & 0x0f)); ///< 4 bit(s) from B63
    to->J2_VDDA_1V8_AI_ADC = ((raw * 0.01));

    return true;
}

bool DecodeTools::dbc_decode_emcTmsg3(emcTmsg3 *to, const uint8_T bytes[])
{
    uint32_T raw;
    raw  = ((uint32_T)((bytes[0]))) << 4; ///< 8 bit(s) from B7
    raw |= ((uint32_T)((bytes[1] >> 4) & 0x0f)); ///< 4 bit(s) from B15
    to->J2A_VDD_DDR_1V0_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[1]) & 0x0f)) << 8; ///< 4 bit(s) from B11
    raw |= ((uint32_T)((bytes[2]))); ///< 8 bit(s) from B23
    to->J2A_VDD_CORE_AO_AI_ADC = ((raw * 0.01));
    to->V3P3_J2_ADC = 0;
    raw  = ((uint32_T)((bytes[3]))) << 4; ///< 8 bit(s) from B31
    raw |= ((uint32_T)((bytes[4] >> 4) & 0x0f)); ///< 4 bit(s) from B39
    to->J2_TEMP_ADC = ((raw * 0.01))*3;
    raw  = ((uint32_T)((bytes[4]) & 0x0f)) << 8; ///< 4 bit(s) from B35
    raw |= ((uint32_T)((bytes[5]))); ///< 8 bit(s) from B47
    to->SYS_IO_1P8V_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[6]))) << 4; ///< 8 bit(s) from B55
    raw |= ((uint32_T)((bytes[7] >> 4) & 0x0f)); ///< 4 bit(s) from B63
    to->NTC_temp_ADC = ((raw * 0.1) - 50);

    return true;
}

bool DecodeTools::dbc_decode_emcTmsg4(emcTmsg4 *to, const uint8_T bytes[])
{
    uint32_T raw;
    to->J2_STAT_OK = 0;
    to->CAN_STAT_OK = 0;

    raw  = ((uint32_T)((bytes[0]))); ///< 8 bit(s) from B7
    to->CPU_USED_RATE = ((raw));
    raw  = ((uint32_T)((bytes[1]))); ///< 8 bit(s) from B15
    to->BPU_USED_RATE = ((raw));

    return true;
}
