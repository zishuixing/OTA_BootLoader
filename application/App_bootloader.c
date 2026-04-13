#include "app_bootloader.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "Int_boot_loader.h"

#define BOOTLOADER_RX_BUF_SIZE 64

uint8_t rx_bufs[BOOTLOADER_RX_BUF_SIZE];
uint16_t rx_lens = 0;

uint32_t bin_bytes = 0;
extern uint16_t rx_len;

extern uint32_t last_rec_time;

// 设置标志位 接收完毕
uint8_t rx_complete_flag = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY1_Pin)
    {
        // 重启bootloader
        rx_complete_flag = 1;
    }
}

typedef enum
{
    BOOTLOADER_STATE_INIT = 0,
    BOOTLOADER_STATE_RUN = 1,
    BOOTLOADER_STATE_REC = 2,
    BOOTLOADER_STATE_HANDLE = 3,
    BOOTLOADER_STATE_CHECK = 4,
    BOOTLOADER_STATE_JUMP = 5,
} Bootloader_State_t;

Bootloader_State_t bootloader_state = BOOTLOADER_STATE_INIT;

/*
 * @brief 擦除页 =>需要解锁flash
 * @param page_addr: 页地址
 * @param page_size: 页大小
 *
 */
void App_bootloader_erase_page(uint32_t page_addr, uint32_t page_size)
{
    // 解锁flash
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase_init;
    // 擦除单独页
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Banks = FLASH_BANK_1;
    // 擦除第一个bank

    erase_init.Banks = FLASH_BANK_1;

    // 擦除当前页
    erase_init.PageAddress = page_addr;
    erase_init.NbPages = page_size;
    uint32_t page_error = 0;
    // flash擦除比较耗费性能
    HAL_FLASHEx_Erase(&erase_init, &page_error);
    // 锁定flash
    HAL_FLASH_Lock();
}

/*
 * @brief 初始化bootloader =>打印日志启动
 *
 */
void App_bootloader_Init(void)
{
    printf("Bootloader Init\n");
    // 用户发送start:字节数启动bootloader模式
    printf("start:bytes start\n");
    // 状态
    bootloader_state = BOOTLOADER_STATE_INIT;
}

/*
 * @brief 初始化bootloader =>打印日志启动
 *
 */
void App_bootloader_run(void)
{
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
    // 接收数据
    HAL_UARTEx_ReceiveToIdle(&huart1, rx_bufs, BOOTLOADER_RX_BUF_SIZE, &rx_lens, 0xffffffff);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    // 处理数据
    if (rx_lens > 0)
    {
        // 判断字符串中是否包含start:
        char *start = strstr(rx_bufs, "start:");
        if (start != NULL)
        {
            // 解析start: 后面的字节数
            bin_bytes = atoi(start + 6);
            if (bin_bytes > 0)
            {
                printf("bytes: %d\n", bin_bytes);
                // 修改boot状态
                bootloader_state = BOOTLOADER_STATE_RUN;
            }
            else
            {
                printf("bytes error\n");
            }
        }
        else
        {
            printf("start error\n");
        }
    }
}

/*
 * @brief 接收数据
 *
 */
void App_bootloader_rec_data(void)
{
    // 接收完成之后 检查数据
    if (last_rec_time != 0 && HAL_GetTick() - last_rec_time > 2000)
    {
        // 2秒没接收到数据了 认为传输完成 校验数据长度

        // 状态改变
        bootloader_state = BOOTLOADER_STATE_CHECK;
    }
}

/*
 * @brief 检查数据
 * @return 0: 校验通过
 * @return 1: 校验不通过
 */
uint8_t App_bootloader_check_data(void)
{
    // 校验数据长度
    if (rx_len != bin_bytes)
    {
        printf("data len error\n");
        // 校验不通过 打印两个字节错误信息
        printf("bin data receive len error: %d,start check len: %d\n", rx_len, bin_bytes);

        return 1;
    }
    else
    {
        printf("data len ok\n");
        // 状态改变
        bootloader_state = BOOTLOADER_STATE_JUMP;
        return 0;
    }
}

/*
 * @brief 跳转应用
 *
 */
uint8_t App_bootloader_jump_app(void)
{
    printf("jump app fuck\n");

    // 跳转应用
    return Int_boot_jump_to_application();
}

/*
 * @brief 状态机工作处理
 *
 */
void App_bootloader_work(void)
{

    switch (bootloader_state)
    {
    case BOOTLOADER_STATE_INIT:

        App_bootloader_Init();
        App_bootloader_run();
        break;
    case BOOTLOADER_STATE_RUN:
        /* 运行bootloader */

        // 接收数据的前置准备工作
        App_bootloader_erase_page(APPLICATION_FLASH_START_ADDR, 10);
        // 擦除数据完成
        printf("erase page done\n");

        // 接收数据
        printf("ready rec data\n");

        // 状态改变
        bootloader_state = BOOTLOADER_STATE_REC;

        Int_boot_loader_Init();

        break;
    case BOOTLOADER_STATE_REC:
        /* 接收数据 */

        // App_bootloader_rec_data();

        if (rx_complete_flag == 1)
        {
            // 接收完成 校验数据
            bootloader_state = BOOTLOADER_STATE_CHECK;
        }

        break;
    // case BOOTLOADER_STATE_HANDLE:
    //     /* 处理数据 */
    //     App_bootloader_handle_data();
    //     bootloader_state = BOOTLOADER_STATE_CHECK;
    //     break;
    case BOOTLOADER_STATE_CHECK:
        /* 检查数据 */

        if (App_bootloader_check_data() == 1)
        {
            /* code */
            // 校验不通过 重启单片机
            NVIC_SystemReset();
        }

        break;
    case BOOTLOADER_STATE_JUMP:
        /* 跳转应用 */
        // 跳转应用
        if (App_bootloader_jump_app() == 0)
        {
            // 跳转成功
            printf("jump app success\n");
        }
        else
        {
            // 跳转失败
            printf("jump app fail\n");
            // 重启单片机
            NVIC_SystemReset();
        }
        break;

    default:
        break;
    }
}
