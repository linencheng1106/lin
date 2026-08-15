#ifndef CAN_PROTOCOL_H
#define CAN_PROTOCOL_H

#include <stdint.h>
#include "stm32f4xx_hal_can.h"

/* 协议版本 */
#define CAN_PROTOCOL_VERSION                         0x0100U

/* CAN波特率：1 Mbps */
#define CAN_PROTOCOL_BAUDRATE                        1000000UL

/* 发送周期 */
#define CAN_FLOAT_FEEDBACK_PERIOD_MS                 10U    /* 100 Hz */
#define CAN_NOISE_PERIOD_MS                          2U     /* 500 Hz */
#define CAN_FILTER_STATUS_PERIOD_MS                  1000U  /* 1 Hz */

/* 500 Hz接收计数参考值 */
#define CAN_EXPECTED_NOISE_COUNT_PER_SECOND          500U
#define CAN_NOISE_COUNT_NORMAL_MIN                   490U
#define CAN_NOISE_COUNT_NORMAL_MAX                   510U

/* ========================================================= */
/* 0x001：主板发送呼吸灯控制，标准数据帧，DLC=3              */
/* Data[0]：开关；Data[1]：速度高8位；Data[2]：速度低8位     */
/* ========================================================= */

#define CAN_ID_BREATH_CONTROL                        0x001U
#define CAN_IDE_BREATH_CONTROL                       CAN_ID_STD
#define CAN_RTR_BREATH_CONTROL                       CAN_RTR_DATA
#define CAN_DLC_BREATH_CONTROL                       3U

#define CAN_BREATH_ENABLE_INDEX                      0U
#define CAN_BREATH_SPEED_HIGH_INDEX                  1U
#define CAN_BREATH_SPEED_LOW_INDEX                   2U

#define CAN_BREATH_DISABLE                           0x00U
#define CAN_BREATH_ENABLE                            0x01U

#define CAN_BREATH_SPEED_MIN                         0x0001U /* 最小速度，对应4000 ms */
#define CAN_BREATH_SPEED_MAX                         0x1000U /* 最大速度，对应400 ms */

/* ========================================================= */
/* 0x002：从板发送float反馈，标准数据帧，DLC=4，100 Hz       */
/* Data[0..3]：IEEE-754 float，小端序，使用memcpy打包         */
/* ========================================================= */

#define CAN_ID_FLOAT_FEEDBACK                        0x002U
#define CAN_IDE_FLOAT_FEEDBACK                       CAN_ID_STD
#define CAN_RTR_FLOAT_FEEDBACK                       CAN_RTR_DATA
#define CAN_DLC_FLOAT_FEEDBACK                       4U

#define CAN_FLOAT_DATA_INDEX                         0U
#define CAN_FLOAT_DATA_SIZE                          4U


/* ========================================================= */
/* 0x012：主板发送500 Hz特定帧，标准数据帧，DLC=8           */
/* Data[0..3]：递增序号；Data[4..7]：固定图案               */
/* ========================================================= */

#define CAN_ID_MASTER_NOISE                          0x012U
#define CAN_IDE_MASTER_NOISE                         CAN_ID_STD
#define CAN_RTR_MASTER_NOISE                         CAN_RTR_DATA
#define CAN_DLC_MASTER_NOISE                         8U

/* ========================================================= */
/* 0x02010101：从板发送500 Hz特定帧，扩展数据帧，DLC=8      */
/* Data[0..3]：递增序号；Data[4..7]：固定图案               */
/* ========================================================= */

#define CAN_ID_SLAVE_NOISE                           0x02010101UL
#define CAN_IDE_SLAVE_NOISE                          CAN_ID_EXT
#define CAN_RTR_SLAVE_NOISE                          CAN_RTR_DATA
#define CAN_DLC_SLAVE_NOISE                          8U

/* 特定帧递增序号，uint32_t小端序 */
#define CAN_NOISE_SEQUENCE_INDEX                     0U
#define CAN_NOISE_SEQUENCE_SIZE                      4U

#define CAN_NOISE_SEQUENCE_BYTE0_INDEX               0U
#define CAN_NOISE_SEQUENCE_BYTE1_INDEX               1U
#define CAN_NOISE_SEQUENCE_BYTE2_INDEX               2U
#define CAN_NOISE_SEQUENCE_BYTE3_INDEX               3U

//CAN_NOISE_PATTERN_0_INDEX：写到哪个字节 例如 Data[4]
//CAN_NOISE_PATTERN_0：写入什么值 例如 0xA5
/* 特定帧固定图案 */
#define CAN_NOISE_PATTERN_0_INDEX                    4U
#define CAN_NOISE_PATTERN_1_INDEX                    5U
#define CAN_NOISE_PATTERN_2_INDEX                    6U
#define CAN_NOISE_PATTERN_3_INDEX                    7U

#define CAN_NOISE_PATTERN_0                          0xA5U
#define CAN_NOISE_PATTERN_1                          0x5AU
#define CAN_NOISE_PATTERN_2                          0xA5U
#define CAN_NOISE_PATTERN_3                          0x5AU

/* ========================================================= */
/* 0x123：CANABLE发送蜂鸣器指令，标准数据帧，DLC=1          */
/* Data[0]：蜂鸣次数，0表示不蜂鸣，最大20次                  */
/* ========================================================= */

#define CAN_ID_BUZZER_COMMAND                        0x123U
#define CAN_IDE_BUZZER_COMMAND                       CAN_ID_STD
#define CAN_RTR_BUZZER_COMMAND                       CAN_RTR_DATA
#define CAN_DLC_BUZZER_COMMAND                       1U

#define CAN_BUZZER_COUNT_INDEX                       0U
#define CAN_BUZZER_COUNT_MIN                         0U
#define CAN_BUZZER_COUNT_MAX                         20U

/*
 * 从板过滤器放行：
 * 0x001标准帧、0x123标准帧
 *
 * 主板过滤器放行：
 * 0x002标准帧、0x003标准帧、
 * 0x123标准帧
 */

#endif /* CAN_PROTOCOL_H */
