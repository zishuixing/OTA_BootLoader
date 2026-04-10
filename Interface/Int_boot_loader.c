#include "Int_boot_loader.h"

// 全局数组存储 接收数据
uint8_t rx_buf[BOOTLOADER_RX_BUF_SIZE];
uint16_t rx_len = 0;

void Int_boot_loader_Init(void)
{

    // 可能存储usart的bug 清除接收中断标志位
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_IDLE);

    // 初始化bootloader
    // usart 接收函数
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buf, BOOTLOADER_RX_BUF_SIZE);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 接收完成回调函数
    if (huart->Instance == USART1)
    {
        // 接收到数据
        // 处理接收数据
        // 例如：解析数据、发送响应等
        // ...
        printf("rx_buf:%s\r\n", rx_buf);
        printf("rx_len:%d\r\n", rx_len);

        // 清空接收缓冲区
        memset(rx_buf, 0, BOOTLOADER_RX_BUF_SIZE);
        rx_len += Size;

        // 重新开启接收中断
        HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buf, BOOTLOADER_RX_BUF_SIZE);
    }
}
