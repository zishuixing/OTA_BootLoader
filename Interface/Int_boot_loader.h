#ifndef __INT_BOOT_LOADER_H__
#define __INT_BOOT_LOADER_H__
#include "usart.h"

// 定义一个接收数组长度
#define BOOTLOADER_RX_BUF_SIZE 512

void Int_boot_loader_Init(void);

extern uint8_t rx_buf[BOOTLOADER_RX_BUF_SIZE];
extern uint16_t rx_len;

#endif /* __INT_BOOT_LOADER_H__ */
