#include "user_comm.h"

uint8_t uart1_rx_buf[STANDARD_PROTOCAL_LEN];
uint8_t uart2_rx_buf[STANDARD_PROTOCAL_LEN];
static uint8_t got_rx = 0;
uint8_t got_rx_u2 = 0;

#define U2_RX_BUF_LEN 256
#define BL_FRAME_MAX 64

static uint8_t bl_frame_buf[BL_FRAME_MAX];
static uint16_t bl_frame_len = 0;

static uint8_t u2_rx_dma_buf[U2_RX_BUF_LEN];
static volatile uint16_t u2_rx_pos = 0; // 当前DMA写到的位置(由IDLE更新)
static volatile uint8_t u2_rx_idle_flag = 0;

void uart2_rx_dma_start(void)
{
    // DMA循环模式要在 CubeMX 里把 DMA mode 设为 CIRCULAR
    HAL_UART_Receive_DMA(&huart2, u2_rx_dma_buf, U2_RX_BUF_LEN);

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
}

// uint8_t bl_tx_buf[STANDARD_PROTOCAL_LEN];

// protocal
// 帧头4字节 0xaa 0x55 0x03 0x99
// 报文序号 1字节
// 报文类型 1字节
// 指令    1字节
// 校验和 1字节
void start_uart_rx(void)
{
    // __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    // __HAL_UART_CLEAR_IDLEFLAG(&huart1);

    // HAL_UART_Receive_DMA(&huart1, uart1_rx_buf, UART_RX_BUF_LEN);

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&huart2);

    HAL_UART_Receive_DMA(&huart2, uart2_rx_buf, UART_RX_BUF_LEN);
}

// 0xaa 0x55 0x03 0x99
void recv_handler(uint16_t len)
{
    // step 1, check frame head
    if (uart1_rx_buf[0] != 0xaa || uart1_rx_buf[1] != 0x55 || uart1_rx_buf[2] != 0x03 || uart1_rx_buf[3] != 0x99)
    {
        // 不是帧头, 清除数据
        memset(uart1_rx_buf, 0, UART_RX_BUF_LEN);
        return;
    }

    // step 2, check frame total sum
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len - 1; i++)
    {
        sum += uart1_rx_buf[i];
    }

    if (sum != uart1_rx_buf[len - 1])
    {
        // 校验和不对, 清除数据
        memset(uart1_rx_buf, 0, UART_RX_BUF_LEN);
        return;
    }

    // uint8_t frame_id = uart1_rx_buf[4];   // 帧序号
    uint8_t frame_type = uart1_rx_buf[5]; // 帧类型
    uint8_t cmd = uart1_rx_buf[6];        // 指令

    // step 3, check command
    if (cmd == CMD_GET_DATA && frame_type == CMD_TYPE_REQUEST)
    {
        // send_data(frame_id);
    }
}

void comm_handler(void)
{
    if (got_rx == CMD_TOTAL_LEN)
    {
        recv_handler(got_rx);
        got_rx = 0;
    }
}

void bl_cmd_handler(uint8_t *data, uint8_t len)
{

    DBG_PRINTF("BL RX data len: %d\r\n", len);

    DBG_PRINTF("Received 0x%02X from BL\r\n", data[0]);

    // 暂时就这样.
    switch (data[0])
    {
        // stop usb sending, start saving data to flash
    case 0x0A:
        data_upload_status = USB_TX_STATUS_SAVING_DATA;
        reset_sd_card_task();
        DBG_PRINTF("Received save command from BL\r\n");
        break;

    case 0x0B:
        data_upload_status = USB_TX_STATUS_WRITE_END_BLOCK;
        DBG_PRINTF("Received stop save command from BL, back to send data to usb\r\n");
        // 回复历史记录数量
        break;

    case 0x0C:
        // xt25f_chip_erase();
        break;

    case 0x0D:
        // 加一层保护
        if (USB_TX_STATUS_IDLE == data_upload_status)
        {
            data_upload_status = USB_TX_STATUS_SENDING_HIST_DATA;
            DBG_PRINTF("Received resume usb send command from BL\r\n");
        }

        break;

    case 0x0E:
        data_upload_status = USB_TX_STATUS_NORMAL;
        DBG_PRINTF("Received resume usb send command from BL\r\n");
        break;

    default:
        break;
    }
}

void uart1_it_handler(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
    {
        // 清除 IDLE 标志
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);

        // 此时说明可能一帧数据接收完了
        // 获取当前 DMA 计数器剩余量, 计算本次接收字节数
        uint16_t remain = __HAL_DMA_GET_COUNTER(huart1.hdmarx);
        if (remain)
        {
            uint16_t data_len = UART_RX_BUF_LEN - remain;
            got_rx = data_len;
        }

        // 处理完毕后，可以选择清零或记录这段数据
        // 然后继续保持 DMA 接收，让它随时准备接收新的数据
        HAL_UART_AbortReceive(&huart1);
        // HAL_UART_DMAStop(&huart1);
        // 重新开启接收
        HAL_UART_Receive_DMA(&huart1, uart1_rx_buf, UART_RX_BUF_LEN);
    }
}

void uart2_it_handler_(void)
{
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
    {
        // 清除 IDLE 标志
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);

        // 此时说明可能一帧数据接收完了
        // 获取当前 DMA 计数器剩余量, 计算本次接收字节数
        uint16_t remain = __HAL_DMA_GET_COUNTER(huart2.hdmarx);
        if (remain)
        {
            uint16_t data_len = UART_RX_BUF_LEN - remain;
            got_rx_u2 = data_len;
        }

        // 处理完毕后，可以选择清零或记录这段数据
        // 然后继续保持 DMA 接收，让它随时准备接收新的数据
        HAL_UART_AbortReceive(&huart2);
        // HAL_UART_DMAStop(&huart1);
        // 重新开启接收
        HAL_UART_Receive_DMA(&huart2, uart2_rx_buf, UART_RX_BUF_LEN);
    }
}

void uart2_it_handler(void)
{
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);

        // DMA剩余计数 = NDTR
        uint16_t remain = __HAL_DMA_GET_COUNTER(huart2.hdmarx);
        uint16_t pos = (uint16_t)(U2_RX_BUF_LEN - remain);

        u2_rx_pos = pos;
        u2_rx_idle_flag = 1;
    }
}

void com_task(void)
{
    comm_handler();
}

static void bl_on_frame(const uint8_t *frame, uint16_t len)
{
    // len至少>=3: cmd + \r\n
    bl_cmd_handler((uint8_t *)frame, (uint8_t)len);
}

void bl_rx_stream_consume(const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        uint8_t b = data[i];

        if (bl_frame_len < BL_FRAME_MAX)
            bl_frame_buf[bl_frame_len++] = b;
        else
            bl_frame_len = 0; // 溢出：丢弃，重新找帧

        // 检查帧尾 \r\n
        if (bl_frame_len >= 2 &&
            bl_frame_buf[bl_frame_len - 2] == 0x0D &&
            bl_frame_buf[bl_frame_len - 1] == 0x0A)
        {
            if (bl_frame_len >= 3)

                bl_on_frame(bl_frame_buf, bl_frame_len);

            bl_frame_len = 0; // 下一帧
        }
    }
}

// 10ms跑一次
void u2_task(void)
{
    static uint16_t old_pos = 0;

    if (bl_conn_status == 0)
        return;
        
    uint32_t now = HAL_GetTick();

    static uint32_t last_run_ts = 0;
    if (now - last_run_ts < 10)
        return;

    last_run_ts = now;

    // 也可以不依赖 idle_flag，直接周期性跑（比如每1ms/5ms）
    // if (!u2_rx_idle_flag)
    //     return;

    __disable_irq();
    uint16_t pos = u2_rx_pos;
    u2_rx_idle_flag = 0;
    __enable_irq();

    if (pos == old_pos)
        return;

    if (pos > old_pos)
    {
        // 新数据是 [old_pos, pos)
        bl_rx_stream_consume(&u2_rx_dma_buf[old_pos], (uint16_t)(pos - old_pos));
    }
    else
    {
        // 回绕：新数据分两段
        bl_rx_stream_consume(&u2_rx_dma_buf[old_pos], (uint16_t)(U2_RX_BUF_LEN - old_pos));
        bl_rx_stream_consume(&u2_rx_dma_buf[0], pos);
    }

    old_pos = pos;
}
