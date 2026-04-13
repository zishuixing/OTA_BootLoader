#ifndef __INT_W24C02_H__
#define __INT_W24C02_H__
#include "i2c.h"

// 从机地址
#define W24C02_ADDR 0xA0
// 从机地址 读
#define W24C02_ADDR_READ 0xA1
// 定义寻址size
#define W24C02_ADDR_SIZE 8
// 页size 字节
#define W24C02_PAGE_SIZE 16

// 读取单个字节数据
uint8_t Int_w24c02_ReadByte(uint32_t addr);

// 写入单个字节数据
uint8_t Int_w24c02_WriteByte(uint32_t addr, uint8_t data);

// 写入多个字节数据
uint8_t Int_w24c02_WriteBytes(uint32_t addr, uint8_t *data, uint32_t len);

// 读取多个字节数据
uint8_t Int_w24c02_ReadBytes(uint32_t addr, uint8_t *data, uint32_t len);

void Int_w24c02_Init(void);

#endif /* __INT_W24C02_H__ */
