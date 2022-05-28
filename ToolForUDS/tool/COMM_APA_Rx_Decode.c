

#ifndef __GENEARTED_DBC_PARSER
#define __GENERATED_DBC_PARSER
#endif
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "COMM_APA_Rx_Decode.h"
#ifndef success
#define success 0
#endif
/* Decode module input */
COMM_JAC_MSG_RX_TYPE COMM_JAC_MSG_RX;

/* Decode module output */
S4_VEH_RX_DATA VsCOMM_S4_VEH_RX_Data;

/// Decode emcT's 'emcTmsg1' message
static inline boolean_T dbc_decode_emcTmsg1(emcTmsg1 *to, const uint8_T bytes[8])
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

    return success;
}

/// Decode emcT's 'emcTmsg2' message
static inline boolean_T dbc_decode_emcTmsg2(emcTmsg2 *to, const uint8_T bytes[8])
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

    return success;
}

/// Decode emcT's 'emcTmsg3' message
static inline boolean_T dbc_decode_emcTmsg3(emcTmsg3 *to, const uint8_T bytes[8])
{
    uint32_T raw;
    raw  = ((uint32_T)((bytes[0]))) << 4; ///< 8 bit(s) from B7
    raw |= ((uint32_T)((bytes[1] >> 4) & 0x0f)); ///< 4 bit(s) from B15
    to->J2A_VDD_DDR_1V0_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[1]) & 0x0f)) << 8; ///< 4 bit(s) from B11
    raw |= ((uint32_T)((bytes[2]))); ///< 8 bit(s) from B23
    to->J2A_VDD_CORE_AO_AI_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[3]))) << 4; ///< 8 bit(s) from B31
    raw |= ((uint32_T)((bytes[4] >> 4) & 0x0f)); ///< 4 bit(s) from B39
    to->V3P3_J2_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[4]) & 0x0f)) << 8; ///< 4 bit(s) from B35
    raw |= ((uint32_T)((bytes[5]))); ///< 8 bit(s) from B47
    to->SYS_IO_1P8V_ADC = ((raw * 0.01));
    raw  = ((uint32_T)((bytes[6]))) << 4; ///< 8 bit(s) from B55
    raw |= ((uint32_T)((bytes[7] >> 4) & 0x0f)); ///< 4 bit(s) from B63
    to->NTC_temp_ADC = ((raw * 0.01));

    return success;
}

/// Decode emcT's 'emcTmsg4' message
static inline boolean_T dbc_decode_emcTmsg4(emcTmsg4 *to, const uint8_T bytes[8])
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

    return success;
}

void COMM_APA_ievS4_RX_Decode(void)
{
    
    (void)dbc_decode_emcTmsg1(&(VsCOMM_S4_VEH_RX_Data.emcTmsg1_258),COMM_JAC_MSG_RX.CAN10_ID_258.Data);
    (void)dbc_decode_emcTmsg2(&(VsCOMM_S4_VEH_RX_Data.emcTmsg2_259),COMM_JAC_MSG_RX.CAN10_ID_259.Data);
    (void)dbc_decode_emcTmsg3(&(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A),COMM_JAC_MSG_RX.CAN10_ID_25A.Data);
    (void)dbc_decode_emcTmsg4(&(VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B),COMM_JAC_MSG_RX.CAN10_ID_25B.Data);

}
