#ifndef __INT_W25Q32_H__
#define __INT_W25Q32_H__
#include "stdint.h"

#define W25Q32_READ_DATA 0x03
#define W25Q32_WRITE_DATA 0x02

#define W25Q32_STATUS_REG 0x05
#define W25Q32_WRITE_ENABLE 0x06
#define W25Q32_WRITE_DISABLE 0x04

#define W25Q32_SECTOR_ERASE 0x20

void Int_w25q32_WriteEnable(void);

void Int_w25q32_WriteDisable(void);

void Int_w25q32_WriteByte(uint8_t byte);

uint8_t Int_w25q32_ReadByte(void);

void Int_w25q32_ReadID(uint8_t *u_mf, uint16_t *u_id);

void Int_w25q32_WaitBusy(void);

void Int_w25q32_ReadData(uint8_t block, uint8_t sector, uint8_t page, uint8_t offset, uint8_t *data, uint16_t len);

void Int_w25q32_WriteData(uint8_t block, uint8_t sector, uint8_t page, uint8_t offset, uint8_t *data, uint16_t len);

void Int_w25q32_EraseSector(uint8_t block, uint8_t sector);

#endif /* __INT_W25Q32_H__ */
