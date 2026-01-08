#include "user_comm.h"

void his_data_sd_task(void)
{
    if (data_upload_status != USB_TX_STATUS_SENDING_HIST_DATA)
        return;

    // 获取历史数据数量
    uint32_t his_data_count = get_his_data_count();
    DBG_PRINTF("His data count: %d\r\n", his_data_count);

    data_upload_status = USB_TX_STATUS_IDLE;
}

