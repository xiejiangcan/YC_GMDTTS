#include "decodetools.h"

DecodeTools::DecodeTools(QObject *parent)
    : QObject(parent)
    , mFlag(0)
{
    connect(&mTime, &QTimer::timeout,
            this, &DecodeTools::slotTimeOut);
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
    info.insert(QStringLiteral("CAN2"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg5_262.CAN2_STAT_OK));
    info.insert(QStringLiteral("CAN3"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg6_26C.CAN3_STAT_OK));
    info.insert(QStringLiteral("J2温度"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A.J2_TEMP_ADC));
    info.insert(QStringLiteral("CPU"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B.J2_STAT_OK));
    info.insert(QStringLiteral("BPU"), QVariant::fromValue(VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B.CAN_STAT_OK));

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
    case _CAN_ID::EMCT_MSG_5:
        mFlag |= 1 << 4;
        VsCOMM_S4_VEH_RX_Data.emcTmsg5_262.CAN2_STAT_OK = 0x1;
        break;
    case _CAN_ID::EMCT_MSG_6:
        mFlag |= 1 << 5;
        VsCOMM_S4_VEH_RX_Data.emcTmsg6_26C.CAN3_STAT_OK = 0x1;
        break;
    default: break;
    }
}

bool DecodeTools::IsDataFull()
{
    if(!mTime.isActive()){
        mTime.start(2000);
    }

    if(mFlag == 0xF){
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

    raw  = ((uint32_T)((bytes[0]))); ///< 8 bit(s) from B7
    to->J2_STAT_OK = ((raw));
    raw  = ((uint32_T)((bytes[1]))); ///< 8 bit(s) from B15
    to->CAN_STAT_OK = ((raw));
    raw  = ((uint32_T)((bytes[2]) & 0x01)); ///< 1 bit(s) from B16
    to->J2_Sys_NorFlash_Error = ((raw));
    raw  = ((uint32_T)((bytes[2] >> 1) & 0x01)); ///< 1 bit(s) from B17
    to->J2_Sys_BPU_Core0Error = ((raw));
    raw  = ((uint32_T)((bytes[2] >> 2) & 0x01)); ///< 1 bit(s) from B18
    to->J2_Sys_BPU_Core1Error = ((raw));
    raw  = ((uint32_T)((bytes[2] >> 3) & 0x01)); ///< 1 bit(s) from B19
    to->J2_Sys_Kernel_I2C_Ctrl0 = ((raw));
    raw  = ((uint32_T)((bytes[2] >> 4) & 0x01)); ///< 1 bit(s) from B20
    to->J2_Sys_Kernel_I2C_Ctrl2 = ((raw));
    raw  = ((uint32_T)((bytes[2] >> 5) & 0x01)); ///< 1 bit(s) from B21
    to->J2_Sys_Monitor_CpuLoadHigh = ((raw));
    raw  = ((uint32_T)((bytes[2] >> 6) & 0x01)); ///< 1 bit(s) from B22
    to->J2_Sys_Monitor_MemStarved = ((raw));
    raw  = ((uint32_T)((bytes[2] >> 7) & 0x01)); ///< 1 bit(s) from B23
    to->J2_Sys_Ether_Error = ((raw));
    raw  = ((uint32_T)((bytes[3]) & 0x01)); ///< 1 bit(s) from B24
    to->J2_Sys_Core_TempHigh = ((raw));
    raw  = ((uint32_T)((bytes[3] >> 1) & 0x01)); ///< 1 bit(s) from B25
    to->J2_Sys_CPUfreq = ((raw));
    raw  = ((uint32_T)((bytes[3] >> 2) & 0x01)); ///< 1 bit(s) from B26
    to->J2_Sys_Camera_ConfigError = ((raw));
    raw  = ((uint32_T)((bytes[3] >> 3) & 0x01)); ///< 1 bit(s) from B27
    to->J2_Sys_ISP_Error = ((raw));
    raw  = ((uint32_T)((bytes[3] >> 4) & 0x01)); ///< 1 bit(s) from B28
    to->J2_Sys_Mipi_HostError = ((raw));
    raw  = ((uint32_T)((bytes[3] >> 5) & 0x01)); ///< 1 bit(s) from B29
    to->J2_Sys_Mipi_DevError = ((raw));
    raw  = ((uint32_T)((bytes[3] >> 6) & 0x01)); ///< 1 bit(s) from B30
    to->J2_Sys_SIF_Error = ((raw));
    raw  = ((uint32_T)((bytes[3] >> 7) & 0x01)); ///< 1 bit(s) from B31
    to->J2_Sys_IPU_SingleError = ((raw));
    raw  = ((uint32_T)((bytes[4]) & 0x01)); ///< 1 bit(s) from B32
    to->J2_Sys_Sensor_FpsError = ((raw));
    raw  = ((uint32_T)((bytes[4] >> 1) & 0x01)); ///< 1 bit(s) from B33
    to->J2_Sys_ImageTestPatternFail = ((raw));
    raw  = ((uint32_T)((bytes[4] >> 2) & 0x01)); ///< 1 bit(s) from B34
    to->MCU_Sync_J2_Timeout = ((raw));
    raw  = ((uint32_T)((bytes[4] >> 3) & 0x01)); ///< 1 bit(s) from B35
    to->J2_Percpt_FailSafe_BlurImage = ((raw));
    raw  = ((uint32_T)((bytes[4] >> 4) & 0x01)); ///< 1 bit(s) from B36
    to->J2_Percpt_FailSafe_FullBlockage = ((raw));
    raw  = ((uint32_T)((bytes[4] >> 5) & 0x01)); ///< 1 bit(s) from B37
    to->J2_Percpt_Image_Glare = ((raw));
    raw  = ((uint32_T)((bytes[4] >> 6) & 0x01)); ///< 1 bit(s) from B38
    to->J2_Percpt_Image_RxTimeout = ((raw));
    raw  = ((uint32_T)((bytes[4] >> 7) & 0x01)); ///< 1 bit(s) from B39
    to->J2_Percpt_Image_Freezed = ((raw));
    raw  = ((uint32_T)((bytes[5]) & 0x01)); ///< 1 bit(s) from B40
    to->J2_Percpt_Image_Out_Of_Sequence = ((raw));
    raw  = ((uint32_T)((bytes[5] >> 1) & 0x01)); ///< 1 bit(s) from B41
    to->J2_Percpt_Calibration_Dynamic_Pitch = ((raw));
    raw  = ((uint32_T)((bytes[5] >> 2) & 0x01)); ///< 1 bit(s) from B42
    to->J2_Percpt_Fps_Low = ((raw));

    return true;
}

void DecodeTools::slotTimeOut()
{
    if((mFlag & (0x1 << 4))){
        // can2 ok
    }else{
        // can2 disconnect
        VsCOMM_S4_VEH_RX_Data.emcTmsg5_262.CAN2_STAT_OK = 0x0;
    }

    if((mFlag & (0x1 << 5))){
        // can3 ok
    }else{
        // can3 disconnect
        VsCOMM_S4_VEH_RX_Data.emcTmsg6_26C.CAN3_STAT_OK = 0x0;
    }

    mFlag = 0x0;
}
