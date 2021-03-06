
#include "rtwtypes.h"

#ifndef DEFINED_TYPEDEF_FOR_CAN_MESSAGE_BUS_
#define DEFINED_TYPEDEF_FOR_CAN_MESSAGE_BUS_

typedef struct {
	uint8_T Extended;
	uint8_T Length;
	uint8_T Remote;
	uint8_T Error;
	uint32_T ID;
	real_T Timestamp;
	uint8_T Data[8];
} CAN_MESSAGE_BUS;

#endif

#ifndef DEFINED_TYPEDEF_FOR_emcTmsg1__
#define DEFINED_TYPEDEF_FOR_emcTmsg1__
/// Message: emcTmsg1 from 'emcT', DLC: 8 byte(s), MID: 600
typedef struct {
    real32_T	 VBAT_P_F_ADC;                   ///< B18:7   Destination: PC
    real32_T	 IGN_ADC;                        ///< B22:11   Destination: PC
    real32_T	 CAN_WAKE_UP_ADC;                ///< B42:31   Destination: PC
    real32_T	 V3P3_ETH_ADC;                   ///< B46:35   Destination: PC
    real32_T	 V3P3_UP_ADC;                    ///< B66:55   Destination: PC
} emcTmsg1;
#endif

#ifndef DEFINED_TYPEDEF_FOR_emcTmsg2__
#define DEFINED_TYPEDEF_FOR_emcTmsg2__
/// Message: emcTmsg2 from 'emcT', DLC: 8 byte(s), MID: 601
typedef struct {
    real32_T	 VDD_1V25_UP_ADC;                ///< B18:7   Destination: PC
    real32_T	 J2_CNN_0V9_ADC;                 ///< B22:11   Destination: PC
    real32_T	 J2_VDD_0V9_ADC;                 ///< B42:31   Destination: PC
    real32_T	 J2_VDDQ_DDR_1V1_AI_ADC;         ///< B46:35   Destination: PC
    real32_T	 J2_VDDA_1V8_AI_ADC;             ///< B66:55   Destination: PC
} emcTmsg2;
#endif

#ifndef DEFINED_TYPEDEF_FOR_emcTmsg3__
#define DEFINED_TYPEDEF_FOR_emcTmsg3__
/// Message: emcTmsg3 from 'emcT', DLC: 8 byte(s), MID: 602
typedef struct {
    real32_T	 J2A_VDD_DDR_1V0_ADC;            ///< B18:7   Destination: PC
    real32_T	 J2A_VDD_CORE_AO_AI_ADC;         ///< B22:11   Destination: PC
    real32_T	 V3P3_J2_ADC;                    ///< B42:31   Destination: PC
    real32_T	 SYS_IO_1P8V_ADC;                ///< B46:35   Destination: PC
    real32_T	 NTC_temp_ADC;                   ///< B66:55   Destination: PC
    real32_T     J2_TEMP_ADC;                    ///< B42:31   Destination: PC
} emcTmsg3;
#endif

#ifndef DEFINED_TYPEDEF_FOR_emcTmsg4__
#define DEFINED_TYPEDEF_FOR_emcTmsg4__
/// Message: emcTmsg4 from 'emcT', DLC: 8 byte(s), MID: 603
typedef struct {
    uint8_T		 J2_STAT_OK;                     ///< B14:7   Destination: PC
    uint8_T		 CAN_STAT_OK;                    ///< B22:15   Destination: PC
    boolean_T	 J2_Sys_NorFlash_Error;         ///< B16:16   Destination: PC
    boolean_T	 J2_Sys_BPU_Core0Error;         ///< B17:17   Destination: PC
    boolean_T	 J2_Sys_BPU_Core1Error;         ///< B18:18   Destination: PC
    boolean_T	 J2_Sys_Kernel_I2C_Ctrl0;       ///< B19:19   Destination: PC
    boolean_T	 J2_Sys_Kernel_I2C_Ctrl2;       ///< B20:20   Destination: PC
    boolean_T	 J2_Sys_Monitor_CpuLoadHigh;    ///< B21:21   Destination: PC
    boolean_T	 J2_Sys_Monitor_MemStarved;     ///< B22:22   Destination: PC
    boolean_T	 J2_Sys_Ether_Error;            ///< B23:23   Destination: PC
    boolean_T	 J2_Sys_Core_TempHigh;          ///< B24:24   Destination: PC
    boolean_T	 J2_Sys_CPUfreq;                ///< B25:25   Destination: PC
    boolean_T	 J2_Sys_Camera_ConfigError;     ///< B26:26   Destination: PC
    boolean_T	 J2_Sys_ISP_Error;              ///< B27:27   Destination: PC
    boolean_T	 J2_Sys_Mipi_HostError;         ///< B28:28   Destination: PC
    boolean_T	 J2_Sys_Mipi_DevError;          ///< B29:29   Destination: PC
    boolean_T	 J2_Sys_SIF_Error;              ///< B30:30   Destination: PC
    boolean_T	 J2_Sys_IPU_SingleError;        ///< B31:31   Destination: PC
    boolean_T	 J2_Sys_Sensor_FpsError;        ///< B32:32   Destination: PC
    boolean_T	 J2_Sys_ImageTestPatternFail;   ///< B33:33   Destination: PC
    boolean_T	 MCU_Sync_J2_Timeout;           ///< B34:34   Destination: PC
    boolean_T	 J2_Percpt_FailSafe_BlurImage;  ///< B35:35   Destination: PC
    boolean_T	 J2_Percpt_FailSafe_FullBlockage; ///< B36:36   Destination: PC
    boolean_T	 J2_Percpt_Image_Glare;         ///< B37:37   Destination: PC
    boolean_T	 J2_Percpt_Image_RxTimeout;     ///< B38:38   Destination: PC
    boolean_T	 J2_Percpt_Image_Freezed;       ///< B39:39   Destination: PC
    boolean_T	 J2_Percpt_Image_Out_Of_Sequence; ///< B40:40   Destination: PC
    boolean_T	 J2_Percpt_Calibration_Dynamic_Pitch; ///< B41:41   Destination: PC
    boolean_T	 J2_Percpt_Fps_Low;             ///< B42:42   Destination: PC
} emcTmsg4;
#endif


#ifndef DEFINED_TYPEDEF_FOR_emcTmsg5__
#define DEFINED_TYPEDEF_FOR_emcTmsg5__
/// Message: emcTmsg5 from 'emcT', DLC: 8 byte(s), MID: 610
typedef struct {
    uint8_T		 CAN2_STAT_OK;                   ///< 610   Destination: PC
} emcTmsg5;
#endif


#ifndef DEFINED_TYPEDEF_FOR_emcTmsg6__
#define DEFINED_TYPEDEF_FOR_emcTmsg6__
/// Message: emcTmsg5 from 'emcT', DLC: 8 byte(s), MID: 610
typedef struct {
    uint8_T		 CAN3_STAT_OK;                   ///< 620   Destination: PC
} emcTmsg6;
#endif

#ifndef DEFINED_TYPEDEF_FOR_S4_VEH_RX_DATA_
#define DEFINED_TYPEDEF_FOR_S4_VEH_RX_DATA_

typedef struct
{
    emcTmsg1 emcTmsg1_258;
    emcTmsg2 emcTmsg2_259;
    emcTmsg3 emcTmsg3_25A;
    emcTmsg4 emcTmsg4_25B;
    emcTmsg5 emcTmsg5_262;
    emcTmsg6 emcTmsg6_26C;
}
S4_VEH_RX_DATA;

#endif


#ifndef DEFINED_TYPEDEF_FOR_COMM_JAC_MSG_RX_TYPE_
#define DEFINED_TYPEDEF_FOR_COMM_JAC_MSG_RX_TYPE_


typedef struct {
    CAN_MESSAGE_BUS CAN10_ID_258;
    CAN_MESSAGE_BUS CAN10_ID_259;
    CAN_MESSAGE_BUS CAN10_ID_25A;
    CAN_MESSAGE_BUS CAN10_ID_25B;

} COMM_JAC_MSG_RX_TYPE;

#endif


extern COMM_JAC_MSG_RX_TYPE COMM_JAC_MSG_RX;
extern S4_VEH_RX_DATA VsCOMM_S4_VEH_RX_Data;

extern void COMM_APA_ievS4_RX_Decode(void);
