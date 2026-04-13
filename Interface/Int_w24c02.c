#include "Int_w24c02.h"

void Int_w24c02_Init(void)
{
}

// 读取单个字节数据
uint8_t Int_w24c02_ReadByte(uint32_t addr)
{
    uint8_t data;
    HAL_I2C_Mem_Read(&hi2c2, W24C02_ADDR_READ, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    return data;
}

// 写入单个字节数据
uint8_t Int_w24c02_WriteByte(uint32_t addr, uint8_t data)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c2, W24C02_ADDR, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    return status;
}

// 写入多个字节数据
uint8_t Int_w24c02_WriteBytes(uint32_t addr, uint8_t *data, uint32_t len)
{
    // 检查参数是否有效
    if (data == NULL || len == 0)
    {
        return HAL_ERROR;
    }

    // 当超过最大写入长度时，需要分多次写入
    if (len > 256)
    {
        printf("Int_w24c02_WriteBytes: len = %d, max = 256\n", len);
        return HAL_ERROR;
    }

    // 当超过一页地址时 分多次写入
    uint8_t page_addr = addr % 16;
    uint8_t remain_page_len = 16 - page_addr;

    if (len <= remain_page_len)
    {
        // 写入字节
        HAL_I2C_Mem_Write(&hi2c2, W24C02_ADDR, addr, I2C_MEMADD_SIZE_8BIT, data, len, 100);
        return HAL_OK;
    }
    else
    {
        uint8_t start_page_addr = addr;
        // 已经写入的页数
        uint8_t page_count = 0;
        while (remain_page_len < len)
        {

            // 将当前页剩余的空间 写满
            HAL_I2C_Mem_Write(&hi2c2, W24C02_ADDR, start_page_addr, I2C_MEMADD_SIZE_8BIT, data + page_count * 16, remain_page_len, 1000);

            // 下一页的起始地址 =》一页的开头
            page_count++;
            start_page_addr += remain_page_len;
            // 数据还剩下的长度
            len -= remain_page_len;
            // 当前页的剩余空间
            remain_page_len = 16;

            // 延时
            HAL_Delay(10);
        }

        // 最后一页写入
        if (len != 0)
        {
            HAL_I2C_Mem_Write(&hi2c2, W24C02_ADDR, start_page_addr, I2C_MEMADD_SIZE_8BIT, data + page_count * 16, len, 1000);

            // 延时
            HAL_Delay(10);
        }
    }

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c2, W24C02_ADDR, addr, I2C_MEMADD_SIZE_8BIT, data, len, 100);
    return status;
}

// 读取多个字节数据
uint8_t Int_w24c02_ReadBytes(uint32_t addr, uint8_t *data, uint32_t len)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c2, W24C02_ADDR_READ, addr, I2C_MEMADD_SIZE_8BIT, data, len, 100);
    return status;
}
