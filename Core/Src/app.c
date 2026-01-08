#include "user_comm.h"

uint8_t adc_dma_buffer[ADC_BUFFER_SIZE] = {0};
__IO uint8_t bat_adc_done = 0;
volatile uint8_t check_reset_imu = 0;
volatile uint8_t bl_uart_tx_done = 0;
volatile uint8_t need_send_reset_signal = 0;
volatile uint8_t imu_rest_tx_data[5] = {0x03, 0xAA, 0x55, 0x03, 0x99};

volatile uint8_t points_data[TOTAL_POINTS] = {0};

volatile uint8_t tx_data[OLD_FRAME_LEN] = {0};

uint8_t data_upload_status = USB_TX_STATUS_NORMAL;

static void move_data(void);
static void usb_send_imu_reset_ack(void);
static void usb_send_main_data(void);

void start_adc_collecting(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer, ADC_BUFFER_SIZE); // != HAL_OK;
}

void imu_rest_cmd_task(void)
{
    static uint8_t imu_reseted_sent = 0;
    if (check_reset_imu)
    {
        if (imu_reseted && imu_reseted_sent == 0)
        {

            need_send_reset_signal = 1;

            imu_reseted_sent = 1;
        }

        check_reset_imu = 0;
    }

    if (bl_uart_tx_done)
    {

        if (imu_reseted && imu_reseted_sent)
        {

            HAL_UART_Transmit(&huart2, (uint8_t *)imu_rest_tx_data, 5, 0xffff);

            imu_reseted_sent = 0;
            imu_reseted = 0;
        }

        bl_uart_tx_done = 0;
    }
}

void adc_data_handler_with_idx(uint16_t point_nmb, uint8_t *points_data)
{
    // 简单计算平均值
    float adc_sum = 0;
    uint32_t i = 0;

    for (i = 0; i < ADC_BUFFER_SIZE; i++)
    {
        adc_sum += (float)adc_dma_buffer[i];
    }

    float result = adc_sum / ADC_BUFFER_SIZE;

    points_data[point_nmb] = (uint8_t)result; // 将结果存储到points_data中
}

void main_adc_task(void)
{

    // for test, 降为1秒一帧
    uint32_t current_ms = HAL_GetTick();
    static uint32_t last_ms = 0;
    if (current_ms - last_ms < 1000)
    {
        return;
    }

    last_ms = current_ms;

    uint16_t input_idx = 0;
    uint16_t adc_idx = 0;
    uint16_t point_nmb = 0;

    uint32_t start_ms = HAL_GetTick();

    for (input_idx = 0; input_idx < INPUT_CH_NUMBER; input_idx++)
    {

        // 先打开GPIO输出
        // set_channel_pin(input_idx_v2[input_idx], GPIO_PIN_SET);
        set_channel_pin(input_idx, GPIO_PIN_SET);

        for (adc_idx = 0; adc_idx < ADC_CHANNEL_NUMBER; adc_idx++)
        {

            set_adc_ch(adc_idx);

            delay_us(10);

            point_nmb = input_idx * ADC_CHANNEL_NUMBER + adc_idx;

            adc_data_handler_with_idx(point_nmb, (uint8_t *)points_data);
        }

        // 关闭GPIO输出
        // set_channel_pin(input_idx_v2[input_idx], GPIO_PIN_RESET);
        set_channel_pin(input_idx, GPIO_PIN_RESET);
    }

#if USE_PRESS

    // 数据压缩处理
    press256((const uint8_t *)points_data, (uint8_t *)points_data_after_proc, ADC_CHANNEL_NUMBER, INPUT_CH_NUMBER, 5, BY_COL);

#endif

    // move_data();

    // usb_send_imu_reset_ack();
    // usb_send_main_data();

    buf_data((uint8_t *)points_data, (uint8_t *)q_out);
    
}

static void move_data(void)
{

#if USE_PRESS
    //  memcpy(&tx_data[2], (const void *)points_data_after_proc, TOTAL_POINTS / 2);
    // memcpy(&tx_data[TOTAL_POINTS / 2 + 8], (const void *)&points_data_after_proc[TOTAL_POINTS / 2], TOTAL_POINTS / 2);
#else

    // 将一半数据复制到tx_data
    memcpy((uint8_t *)&tx_data[FIRST_PACK_START_IDX], (const void *)points_data, TOTAL_POINTS / 2);

    // 另一半数据复制到tx_data
    memcpy((uint8_t *)&tx_data[SECOND_PACK_START_IDX], (const void *)&points_data[TOTAL_POINTS / 2], TOTAL_POINTS / 2);

#endif

    // 4元数放在最后16个字节
    memcpy((uint8_t *)&tx_data[TOTAL_POINTS + PACK_TYPE_LEN * 2 + PACK_IDX_LEN * 2 + PACKAGE_HEAD_LEN * 2], (const void *)q_out, sizeof(float) * 4);

    // 将数据复制到bl的传输数组中
    memcpy(bl_tx_buf, (const void *)tx_data, OLD_FRAME_LEN);
}

void fill_tx_data(void)
{
    // 第一个包
    tx_data[0] = 0xaa;
    tx_data[1] = 0x55;
    tx_data[2] = 0x03;
    tx_data[3] = 0x99;
    tx_data[4] = FIRST_PACK_IDX;
    tx_data[5] = LEFT_HAND;

    // 第二个包
    tx_data[FIRST_PACK_START_IDX + TOTAL_POINTS / 2] = 0xaa;
    tx_data[FIRST_PACK_START_IDX + TOTAL_POINTS / 2 + 1] = 0x55;
    tx_data[FIRST_PACK_START_IDX + TOTAL_POINTS / 2 + 2] = 0x03;
    tx_data[FIRST_PACK_START_IDX + TOTAL_POINTS / 2 + 3] = 0x99;
    tx_data[FIRST_PACK_START_IDX + TOTAL_POINTS / 2 + 4] = SECOND_PACK_IDX;
    tx_data[FIRST_PACK_START_IDX + TOTAL_POINTS / 2 + 5] = LEFT_HAND;
}

#if 0
void uart_send(void)
{
    // HAL_StatusTypeDef status = HAL_OK;
    // 先发送复位信号
    if (need_send_reset_signal)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)imu_rest_tx_data, 5, 0xffff);
        need_send_reset_signal = 0;
    }

#if USE_PRESS
    //  memcpy(&tx_data[2], (const void *)points_data_after_proc, TOTAL_POINTS / 2);
    // memcpy(&tx_data[TOTAL_POINTS / 2 + 8], (const void *)&points_data_after_proc[TOTAL_POINTS / 2], TOTAL_POINTS / 2);
#else

    // 将一半数据复制到tx_data
    memcpy((uint8_t *)&tx_data[FIRST_PACK_START_IDX], (const void *)points_data, TOTAL_POINTS / 2);

    // 另一半数据复制到tx_data
    memcpy((uint8_t *)&tx_data[SECOND_PACK_START_IDX], (const void *)&points_data[TOTAL_POINTS / 2], TOTAL_POINTS / 2);

#endif

    // 4元数放在最后16个字节
    memcpy((uint8_t *)&tx_data[TOTAL_POINTS + PACK_TYPE_LEN * 2 + PACK_IDX_LEN * 2 + PACKAGE_HEAD_LEN * 2], (const void *)q_out, sizeof(float) * 4);

    // 将数据复制到bl的传输数组中
    memcpy(bl_tx_buf, (const void *)tx_data, OLD_FRAME_LEN);

    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_data, OLD_FRAME_LEN);
}

#endif

static void usb_send_main_data(void)
{
    if (USB_TX_STATUS_NORMAL != data_upload_status)
        return;

    CDC_Transmit_FS((uint8_t *)tx_data, OLD_FRAME_LEN);
}

static void usb_send_imu_reset_ack(void)
{
    if (USB_TX_STATUS_NORMAL != data_upload_status)
        return;

    if (need_send_reset_signal == 0)
        return;

    uint8_t reset_ack[5] = {0x04, 0xAA, 0x55, 0x04, 0x99};
    CDC_Transmit_FS((uint8_t *)reset_ack, 5);
    need_send_reset_signal = 0;
}
