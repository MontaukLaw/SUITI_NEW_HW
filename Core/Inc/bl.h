#ifndef _BL_H_
#define _BL_H_

void bl_task(void);

void bl_link_status_check(void);

void send_all_save_data(uint16_t data_page_nmb);

void feed_back_his_nmb(uint32_t his_data_count);

extern uint8_t bl_conn_status;

extern uint8_t bl_tx_buf[];

#endif
