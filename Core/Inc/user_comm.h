#ifndef _USER_COMM_H_
#define _USER_COMM_H_

#include "main.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "sd_card.h"
#include "usb_app.h"
#include "sys.h"
#include "iwdg.h"
#include "usbd_cdc_if.h"
#include "spi.h"
#include "soft_crc.h"
#include "i2c.h"
#include "icm42688.h"
#include "icm42688_hard_i2c.h"
#include "g_sensor.h"
#include "qmi8658_app.h"
#include "app_internal_flash.h"
#include "mems.h"
#include "qst_ahrs.h"

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

#endif /* _USER_COMM_H_ */
