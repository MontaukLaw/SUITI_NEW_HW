#ifndef _USER_COMM_H_
#define _USER_COMM_H_

#include "main.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "adc.h"
#include "iwdg.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"

#include "sd_card.h"
#include "usb_app.h"
#include "sys.h"
#include "usbd_cdc_if.h"
#include "soft_crc.h"
#include "icm42688.h"
#include "icm42688_hard_i2c.h"
#include "g_sensor.h"
#include "qmi8658_app.h"
#include "app_internal_flash.h"
#include "mems.h"
#include "qst_ahrs.h"

#include "bat_val.h"
#include "check_mems_type.h"
#include "led.h"
#include "charge_state.h"
#include "comm.h"
#include "app.h"
#include "bl.h"
#include "key.h"
#include "switch_hal.h"
#include "sd_task.h"
#include "his_data_sd_task.h"
#include "start_control.h"  

#define CH_DEF(n) {OUT##n##_GPIO_Port, OUT##n##_Pin}

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} GPIO_Channel;

#define ADC2_DMA_BUF_LEN 100

#define MAX_STATIC_CALI_COUNTER 100

#define MEMS_TYPE_ICM42688 1
#define MEMS_TYPE_QMI8658 2
#define MEMS_TYPE_UNKNOWN 0

#define DEBUG_PRINT 1

#if DEBUG_PRINT
#define DBG_PRINTF(fmt, ...) \
    printf("[%s:%d] " fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define DBG_PRINTF(...) ((void)0)
#endif

#define ENABLE_IWDG 1

#define DEG2RAD 0.017453293f /* 度转弧度 π/180 */

#define ADC_BUFFER_SIZE 10 // 80 // 10 // Define the size of the ADC buffer

#define CHARGING_FINISHED 1
#define CHARGING_ONGOING 2
#define CHARGING_NO_POWER 0

#define INPUT_CH_NUMBER 16

#define ADC_CHANNEL_NUMBER 16

#define TOTAL_POINTS (ADC_CHANNEL_NUMBER * INPUT_CH_NUMBER)

#define STANDARD_PROTOCAL_LEN 64

#define UART_RX_BUF_LEN STANDARD_PROTOCAL_LEN

// 采集数据指令
#define CMD_GET_DATA 0x01

#define CMD_TYPE_REQUEST 0x00
#define CMD_TYPE_RESPONSE 0x01
#define CMD_TYPE_NOTIFICATION 0x02

#define CMD_TOTAL_LEN 0x08

#define CMD_RESULT_SUCCESS 0x00
#define CMD_RESULT_FAIL 0x01

#define RESET_GQ_KEY_SHAKE_DELAY 5

// 6秒关灯
#define POWER_DOWN_COUNTER 350 // 600

/////////////////////////////////// STATUS /////////////////////////////////
#define USB_TX_STATUS_NORMAL 1
#define USB_TX_STATUS_SAVING_DATA 2
#define USB_TX_STATUS_IDLE 3
#define USB_TX_STATUS_SENDING_HIST_DATA 4
#define USB_TX_STATUS_WRITE_END_BLOCK 5

/////////////////////////////////// MAIN ADC ////////////////////////////////
#define LEFT_HAND 0x01
#define RIGHT_HAND 0x02

#define HAND_DIRECT LEFT_HAND
#define FIRST_PACK_IDX 1
#define SECOND_PACK_IDX 2

#define PACKAGE_1_LEN TOTAL_POINTS / 2
#define PACKAGE_2_LEN TOTAL_POINTS / 2 + 16

#define FIRST_PACK_START_IDX 6
#define SECOND_PACK_START_IDX (FIRST_PACK_START_IDX + PACKAGE_1_LEN + 4 + 2)

#define PACKAGE_HEAD_LEN 4
#define PACK_IDX_LEN 1
#define PACK_TYPE_LEN 1

// 旧版本的帧长度
// 1. 包头 4字节*2
// 2. 包索引 1字节*2
// 3. 包类型 1字节*2
// 4. 数据区 TOTAL_POINTS 字节
// 5. 四元数 16字节
#define OLD_FRAME_LEN (TOTAL_POINTS + PACKAGE_HEAD_LEN + PACKAGE_HEAD_LEN + PACK_IDX_LEN + PACK_IDX_LEN + PACK_TYPE_LEN + PACK_TYPE_LEN + sizeof(float) * 4)

#define SAVED_DATA_LEN (TOTAL_POINTS + sizeof(float) * 4)

typedef struct
{
    uint8_t data[SAVED_DATA_LEN];
    // uint8_t sensor_data[TOTAL_POINT_NUM];
    // uint8_t mems_data[MEMS_FLOAT_BYTES]; // 6轴
} sd_raw_data_t;

typedef struct
{
    uint32_t magic;     // 固定标志
    uint32_t seq;       // 递增序号
    uint32_t timestamp; // 可选
    uint32_t crc;       // 3个raw_data的crc32
    // sd_raw_data_t data_1;
    // sd_raw_data_t data_2;
    // sd_raw_data_t data_3;

    uint8_t data[OLD_FRAME_LEN];

} sd_log_page_t;

#endif /* _USER_COMM_H_ */
