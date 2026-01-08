#ifndef _SD_TASK_H_
#define _SD_TASK_H_

void reset_sd_card_task(void);

void sd_card_task(void);

void buf_data(uint8_t *sensor_data, uint8_t *mems_data);

uint32_t write_stop_sign(void);

void write_last_block_idx_task(void);

void show_data_upload_status(void);

#endif /* _SD_TASK_H_ */

