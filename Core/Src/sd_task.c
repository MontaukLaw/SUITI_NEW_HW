#include "user_comm.h"

uint32_t writing_block_idx = 0;

volatile uint8_t data_insert_idx = 0;
volatile uint8_t time_to_save_data_to_sd = 0;
static sd_log_page_t saving_page;

void reset_sd_card_task(void)
{
    writing_block_idx = 0;
}

// 写入数据到 sd_log_page，然后写入 SD 卡
void buf_data(uint8_t *sensor_data, uint8_t *mems_data)
{

    if (USB_TX_STATUS_SAVING_DATA != data_upload_status)
    {
        return;
    }

    memset(&saving_page, 0, sizeof(sd_log_page_t));
    saving_page.magic = SD_LOG_MAGIC;
    saving_page.seq = writing_block_idx;
    saving_page.timestamp = HAL_GetTick();

    memcpy(saving_page.data, (const void *)sensor_data, TOTAL_POINTS);
    memcpy(&saving_page.data[TOTAL_POINTS], (const void *)mems_data, sizeof(float) * 4);

    // 计算 crc
    saving_page.crc = crc32_soft((uint8_t *)&saving_page.data, SAVED_DATA_LEN);
    DBG_PRINTF("Writing stop sign to block %lu \r\n", writing_block_idx);

    // save to SD card
    uint8_t rtn = Write_Single_Block(writing_block_idx, (uint8_t *)&saving_page);
    if (rtn != SD_OK)
    {
        DBG_PRINTF("Write block %lu failed with code %u\r\n", writing_block_idx, rtn);
        error_handler();
    }

    writing_block_idx++;

    if (writing_block_idx >= SD_BlockNR)
    {
        DBG_PRINTF("SD card full!\r\n");
        writing_block_idx = 0;
    }

    // 闪红灯
    HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
}

uint32_t write_stop_sign(void)
{
    sd_log_page_t stop_page;
    stop_page.magic = 0xDEADBEEF;
    stop_page.seq = writing_block_idx;
    stop_page.timestamp = HAL_GetTick();
    stop_page.crc = 0;

    DBG_PRINTF("Writing stop sign to block %lu\r\n", writing_block_idx);

    uint8_t rtn = Write_Single_Block(writing_block_idx, (uint8_t *)&stop_page);
    if (rtn != SD_OK)
    {
        DBG_PRINTF("Write stop sign block %lu failed with code %u\r\n", writing_block_idx, rtn);
        error_handler();
    }

    // 灭红灯
    HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);

    return writing_block_idx;
}

void write_last_block_idx_task(void)
{

    if (USB_TX_STATUS_WRITE_END_BLOCK != data_upload_status)
        return;

    DBG_PRINTF("Writing last block index task\r\n");
    // uint32_t stop_sign_block_idx = write_stop_sign();

    feed_back_his_nmb(writing_block_idx);

    data_upload_status = USB_TX_STATUS_IDLE;
}

void show_data_upload_status(void)
{
    uint32_t now = HAL_GetTick();
    static uint32_t last_toggle_ts = 0;
    if (now - last_toggle_ts < 1000)
        return;

    last_toggle_ts = now;
    switch (data_upload_status)
    {
    case USB_TX_STATUS_NORMAL:
        DBG_PRINTF("USB TX STATUS: NORMAL\r\n");
        break;

    case USB_TX_STATUS_SAVING_DATA:
        DBG_PRINTF("USB TX STATUS: SAVING DATA TO SD\r\n");
        break;
    case USB_TX_STATUS_IDLE:
        DBG_PRINTF("USB TX STATUS: IDLE\r\n");
        break;
    case USB_TX_STATUS_SENDING_HIST_DATA:
        DBG_PRINTF("USB TX STATUS: SENDING HISTORICAL DATA\r\n");
        break;
    case USB_TX_STATUS_WRITE_END_BLOCK:
        DBG_PRINTF("USB TX STATUS: WRITE END BLOCK\r\n");
        break;
    default:
        DBG_PRINTF("USB TX STATUS: UNKNOWN %u\r\n", data_upload_status);
        break;
    }
}
