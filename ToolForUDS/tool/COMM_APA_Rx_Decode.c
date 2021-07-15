

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
    to->CPU_USED_RATE = ((raw));
    raw  = ((uint32_T)((bytes[1]))); ///< 8 bit(s) from B15
    to->BPU_USED_RATE = ((raw));

    return success;
}

void COMM_APA_ievS4_RX_Decode(void)
{
    
    (void)dbc_decode_emcTmsg1(&(VsCOMM_S4_VEH_RX_Data.emcTmsg1_258),COMM_JAC_MSG_RX.CAN10_ID_258.Data);
    (void)dbc_decode_emcTmsg2(&(VsCOMM_S4_VEH_RX_Data.emcTmsg2_259),COMM_JAC_MSG_RX.CAN10_ID_259.Data);
    (void)dbc_decode_emcTmsg3(&(VsCOMM_S4_VEH_RX_Data.emcTmsg3_25A),COMM_JAC_MSG_RX.CAN10_ID_25A.Data);
    (void)dbc_decode_emcTmsg4(&(VsCOMM_S4_VEH_RX_Data.emcTmsg4_25B),COMM_JAC_MSG_RX.CAN10_ID_25B.Data);

}
