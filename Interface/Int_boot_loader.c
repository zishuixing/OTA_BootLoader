#include "Int_boot_loader.h"

// 全局数组存储 接收数据
uint8_t rx_buf[BOOTLOADER_RX_BUF_SIZE];
uint16_t rx_len = 0;

uint16_t rx_len_Size = 0;

// 记录当前写入程序的偏移量
uint32_t flash_write_offset = 0;

// 最后一个字节是否单独处理
uint8_t is_last_byte = 0;
// flag 标志位
uint8_t is_flag_byte = 0;

void Int_boot_loader_Init(void)
{

    // 可能存储usart的bug 清除接收中断标志位
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_IDLE);

    // 初始化bootloader
    // usart 接收函数
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buf, BOOTLOADER_RX_BUF_SIZE);
}

// 上一次没有剩余 则需要单独处理函数
void write_without_last_byte(void)
{
    for (uint32_t i = 0; i < rx_len_Size; i += 2)
    {
        uint32_t flash_addr = APPLICATION_FLASH_START_ADDR + flash_write_offset + i;
        uint16_t data16;

        if (i + 1 < rx_len_Size)
        {

            // 写入16位数据
            data16 = (rx_buf[i]) | (rx_buf[i + 1] << 8);
            // 写入flash
            HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flash_addr, data16);
            if (status != HAL_OK)
            {
                // 抓到了！在这里打断点，查看是配置错误还是Flash被锁了
                uint32_t error_code = HAL_FLASH_GetError();
                // 打印错误码
                printf("Flash Error Code: 0x%08X\n", error_code);
                break;
            }
        }
    }
}

// 上一次有剩余 则需要单独处理函数
void write_with_last_byte(void)
{
    // 1  3 或者 1  2
    for (uint32_t i = 0; i < rx_len_Size; i += 2)
    {
        uint32_t flash_addr = APPLICATION_FLASH_START_ADDR + flash_write_offset + i;
        uint16_t data16;

        if (i == 0)
        {
            // 第一个字节 直接写入
            data16 = is_last_byte | (rx_buf[0] << 8);
        }
        else
        {
            // 其他字节 直接写入
            data16 = (rx_buf[i - 1]) | (rx_buf[i] << 8);
        }
        // 写入flash
        HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flash_addr, data16);
        if (status != HAL_OK)
        {
            // 抓到了！在这里打断点，查看是配置错误还是Flash被锁了
            uint32_t error_code = HAL_FLASH_GetError();
            // 打印错误码
            printf("Flash Error Code: 0x%08X\n", error_code);
            break;
        }
    }
}

/**
 * @brief 接收完成回调函数
 * @param huart: UART handle pointer
 * @param Size: Number of bytes received
 * @retval None
 * @note  串口开启中断接收之后 触发空闲帧时 使用的回调函数
 * 总长度 需要接收4584字节数据 串口协议稳定性差 发送长文件的时候 容易丢失字节
 * 修改波特率能够提高稳定性 => 高波特率性能比较高
 * hal串口代码比较繁琐 如果在中断回调函数中调用串口输出 会非常占用资源
 * */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 接收完成回调函数
    if (huart->Instance == USART1)
    {
        rx_len_Size = Size;
        rx_len += Size;

        // 接收到数据
        // 解锁flash
        HAL_FLASH_Unlock();

        // 2. 判断当前写入的地址是否新的一页 =>需要擦除
        //  2.1 遍历需要写入的地址 长度为当前接收的数据长度 如果全部内容都是0xff 则说明已经擦除过了
        uint8_t is_erase = 0;
        uint32_t page_addr = 0;

        for (uint32_t i = 0; i < Size; i++)
        {
            // 读取每一个位置 的值

            uint8_t data = *(volatile uint8_t *)(APPLICATION_FLASH_START_ADDR + flash_write_offset + i);

            // 判断是否是0xff 如果不是 则说明需要擦除
            if (data != 0xff)
            {
                is_erase = 1;
                // 记录当前页的起始地址
                // TODO 把地址“拉回到页的起点”
                page_addr = (APPLICATION_FLASH_START_ADDR + flash_write_offset + i) - ((APPLICATION_FLASH_START_ADDR + flash_write_offset + i) % FLASH_PAGE_SIZE);
                break;
            }
        }

        // 2.2 如果需要擦除 则擦除当前页
        if (is_erase)
        {
            FLASH_EraseInitTypeDef erase_init;
            // 擦除单独页
            erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
            erase_init.Banks = FLASH_BANK_1;
            // 擦除第一个bank

            erase_init.Banks = FLASH_BANK_1;

            // 擦除当前页
            erase_init.PageAddress = page_addr;
            erase_init.NbPages = 1;
            uint32_t page_error = 0;
            // flash擦除比较耗费性能
            HAL_FLASHEx_Erase(&erase_init, &page_error);
        }

        // 2.3 写入数据 处理最后一个字节问题
        if ((Size + is_flag_byte) % 2 == 0)
        {
            if (is_flag_byte == 1)
            {
                // 上一次有剩余 则需要单独处理
                write_with_last_byte();
                // 记录偏移量
                flash_write_offset += Size + 1;
                is_flag_byte = 0;
            }
            else
            {
                // 上一次没有剩余 则直接写入
                write_without_last_byte();
                // 记录偏移量
                flash_write_offset += Size;

                // 代表写入无单独处理的字节
                is_flag_byte = 0;
            }
        }

        else
        {

            if (is_flag_byte == 1)
            {
                // 上一次有剩余 则需要单独处理 且这次也剩余一个字节
                write_with_last_byte();
                // 记录偏移量
                flash_write_offset += Size;
            }
            else
            {
                // 上一次没有剩余 则直接写入 且这次也剩余一个字节
                write_without_last_byte();
                // 记录偏移量
                flash_write_offset += Size - 1;
            }

            // 代表下次有单独处理的字节
            is_last_byte = rx_buf[Size - 1];
            is_flag_byte = 1;
        }

        // 2.3 使用16位写入 => 比较贴合实际情况

        // 加锁
        HAL_FLASH_Lock();
        // 清空接收缓冲区
        memset(rx_buf, 0, BOOTLOADER_RX_BUF_SIZE);

        // 可能存储usart的bug 清除接收中断标志位
        __HAL_UART_CLEAR_OREFLAG(&huart1);
        __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_IDLE);

        // 重新开启接收中断
        HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buf, BOOTLOADER_RX_BUF_SIZE);
    }
}
