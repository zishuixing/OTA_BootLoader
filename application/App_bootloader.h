#ifndef __APP_BOOTLOADER_H__
#define __APP_BOOTLOADER_H__
#include "usart.h"

/*
 * @brief 初始化bootloader =>打印日志启动
 *
 */
void App_bootloader_Init(void);

/*
 * @brief 初始化bootloader =>打印日志启动
 *
 */
void App_bootloader_run(void);

/*
 * @brief 接收数据
 *
 */
void App_bootloader_rec_data(void);

/*
 * @brief 处理数据
 *
 */
void App_bootloader_handle_data(void);

/*
 * @brief 检查数据
 *
 */
uint8_t App_bootloader_check_data(void);

/*
 * @brief 跳转应用
 *
 */
uint8_t App_bootloader_jump_app(void);

/*
 * @brief 擦除页 =>需要解锁flash
 * @param page_addr: 页地址
 * @param page_size: 页大小
 *
 */
void App_bootloader_erase_page(uint32_t page_addr, uint32_t page_size);

#endif /* __APP_BOOTLOADER_H__ */
