#ifndef _APP_H_
#define _APP_H_

void start_adc_collecting(void);

void imu_rest_cmd_task(void);

void main_adc_task(void);

void uart_send(void);

void fill_tx_data(void);

extern __IO uint8_t bat_adc_done;

extern volatile uint8_t imu_rest_tx_data[];

extern uint8_t data_upload_status;

extern uint8_t adc_dma_buffer[];

#endif
