#include "Int_w25q32.h"
#include "spi.h"

// 片选引脚start
void Int_w25q32_CS_Start(void)
{
    HAL_GPIO_WritePin(W25Q32_CS_GPIO_Port, W25Q32_CS_Pin, GPIO_PIN_RESET);
}

// 片选引脚stop
void Int_w25q32_CS_Stop(void)
{
    HAL_GPIO_WritePin(W25Q32_CS_GPIO_Port, W25Q32_CS_Pin, GPIO_PIN_SET);
}

// w25q32 写使能
void Int_w25q32_WriteEnable(void)
{
    // 等待忙状态
    Int_w25q32_WaitBusy();

    Int_w25q32_CS_Start();
    Int_w25q32_WriteByte(W25Q32_WRITE_ENABLE);
    Int_w25q32_CS_Stop();
    Int_w25q32_WaitBusy();
}

// w25q32 写禁用
void Int_w25q32_WriteDisable(void)
{
    // 等待忙状态
    Int_w25q32_WaitBusy();

    Int_w25q32_CS_Start();
    Int_w25q32_WriteByte(W25Q32_WRITE_DISABLE);
    Int_w25q32_CS_Stop();
}

// 写入spi一个字节
void Int_w25q32_WriteByte(uint8_t byte)
{
    HAL_SPI_Transmit(&hspi1, &byte, 1, 0xFFFF);
}

// 读取spi一个字节
uint8_t Int_w25q32_ReadByte(void)
{
    uint8_t byte = 0;
    HAL_SPI_Receive(&hspi1, &byte, 1, 0xFFFF);
    return byte;
}

// 读取芯片型号 测试功能
void Int_w25q32_ReadID(uint8_t *u_mf, uint16_t *u_id)
{
    Int_w25q32_CS_Start();

    Int_w25q32_WriteByte(0x9F);
    *u_mf = Int_w25q32_ReadByte();
    *u_id = Int_w25q32_ReadByte() << 8 | Int_w25q32_ReadByte();

    Int_w25q32_CS_Stop();
}

// 等待忙状态
void Int_w25q32_WaitBusy(void)
{
    // start
    Int_w25q32_CS_Start();

    while (1)
    {
        Int_w25q32_WriteByte(W25Q32_STATUS_REG);
        if ((Int_w25q32_ReadByte() & 0x01) == 0)
        {
            break;
        }
    }
    // stop
    Int_w25q32_CS_Stop();
}

/**
 * @brief 读取数据 多字节
 * @param addr 地址 0x 3F F F FF 一共22位 分为 3FF64个block(每个block64KB) 16个sector(每个sector4KB) F.16个页每个页256字节  addr是8位共256字节
 * @param data 数据指针
 * @param len 数据长度
 */

void Int_w25q32_ReadData(uint8_t block, uint8_t sector, uint8_t page, uint8_t offset, uint8_t *data, uint16_t len)
{
    // 等待忙状态
    Int_w25q32_WaitBusy();

    Int_w25q32_CS_Start();

    Int_w25q32_WriteByte(W25Q32_READ_DATA);
    // 拼接成32位地址拼接成24位地址
    uint32_t addr_24bit = (block << 16) | (sector << 12) | (page << 8) | offset;

    // 拼接成32位地址写入
    Int_w25q32_WriteByte(addr_24bit >> 16 & 0xFF);
    Int_w25q32_WriteByte(addr_24bit >> 8 & 0xFF);
    Int_w25q32_WriteByte(addr_24bit & 0xFF);

    for (uint16_t i = 0; i < len; i++)
    {
        data[i] = Int_w25q32_ReadByte();
    }

    Int_w25q32_CS_Stop();
}

// 写入多字节
void Int_w25q32_WriteData(uint8_t block, uint8_t sector, uint8_t page, uint8_t offset, uint8_t *data, uint16_t len)
{

    // 写使能
    Int_w25q32_WriteEnable();
    Int_w25q32_CS_Start();

    // 写入地址
    Int_w25q32_WriteByte(W25Q32_WRITE_DATA);

    // 拼接成32位地址拼接成24位地址
    uint32_t addr_24bit = (block << 16) | (sector << 12) | (page << 8) | offset;

    // 拼接成32位地址写入
    Int_w25q32_WriteByte(addr_24bit >> 16 & 0xFF);
    Int_w25q32_WriteByte(addr_24bit >> 8 & 0xFF);
    Int_w25q32_WriteByte(addr_24bit & 0xFF);

    // 写入数据
    for (uint16_t i = 0; i < len; i++)
    {
        Int_w25q32_WriteByte(data[i]);
    }
    Int_w25q32_CS_Stop();
}

// sector段擦除
void Int_w25q32_EraseSector(uint8_t block, uint8_t sector)
{
    // 等待忙状态
    Int_w25q32_WriteEnable();
    Int_w25q32_CS_Start();

    Int_w25q32_WriteByte(W25Q32_SECTOR_ERASE);

    uint32_t addr_24bit = (block << 16) | (sector << 12) | (0x00) | 0x00;

    Int_w25q32_WriteByte(addr_24bit >> 16 & 0xFF);
    Int_w25q32_WriteByte(addr_24bit >> 8 & 0xFF);
    Int_w25q32_WriteByte(addr_24bit & 0xFF);
    Int_w25q32_CS_Stop();
}
