#ifndef __INT_BOOT_LOADER_H__
#define __INT_BOOT_LOADER_H__
#include "usart.h"

// 定义一个接收数组长度
#define BOOTLOADER_RX_BUF_SIZE 512

//定义一个flash地址 start地址 bootloader地址
#define BOOTLOADER_FLASH_ADDR (0x08000000)

//定义一个 application程序地址
#define APPLICATION_FLASH_START_ADDR (0x08004000)

//定义一个 application程序地址 end地址 等于512KB-16KB 4000等于16KB
#define APPLICATION_FLASH_END_ADDR (0x08080000)

// 定义一个 栈地址
#define STACK_ADDR (0x20000000)


void Int_boot_loader_Init(void);

uint8_t Int_boot_jump_to_application(void);

extern uint8_t rx_buf[BOOTLOADER_RX_BUF_SIZE];
extern uint16_t rx_len;

#endif /* __INT_BOOT_LOADER_H__ */
