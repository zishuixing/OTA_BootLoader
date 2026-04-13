#ifndef __APP_BOOTLOADER_W24C02_H__
#define __APP_BOOTLOADER_W24C02_H__
#include "Int_W24C02.h"
// 宏定义 更新标志位地址
#define BOOTLOADER_FLAG_ADDR 0x00
// 宏定义 更新标志位值 未更新
#define BOOTLOADER_FLAG_UNUPDATE 0x33
// 宏定义 更新标志位值 已更新
#define BOOTLOADER_FLAG_UPDATE 0x55

// 宏定义 校验位地址
#define BOOTLOADER_CHECK_SECURITY 0x01
// 宏定义 校验位值 未校验
#define CHECK_SECURITY 0xabcd

// 定义一个函数检查是否需要更新
uint8_t App_bootloader_check_update(void);

uint8_t App_bootloader_is_need_update(void);

void App_bootloader_jump_program(void);


#endif /* __APP_BOOTLOADER_W24C02_H__ */
