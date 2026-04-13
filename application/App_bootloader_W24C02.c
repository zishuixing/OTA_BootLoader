#include "app_bootloader_w24c02.h"
#include "App_bootloader.h"
#include <string.h>
// 标志位是否需要更新
uint8_t is_need_update = BOOTLOADER_FLAG_UNUPDATE;
uint8_t App_bootloader_check_update(void)
{

    // 读取3个字节数据 主要是更新标志位和校验位
    uint8_t data_check[3];
    Int_w24c02_ReadBytes(BOOTLOADER_FLAG_ADDR, data_check, 3);

    // 检查校验位是否正确
    uint16_t check_security = data_check[1] | (data_check[2] << 8);

    if (check_security != CHECK_SECURITY)
    {

        // // 打印校验位
        // printf("data_check[1]:%02x\r\n", data_check[1]);
        // printf("check_security:%02x\r\n", check_security & 0xff);

        // printf("data_check[2]:%02x\r\n", data_check[2]);
        // printf("check_security:%02x\r\n", check_security >> 8);

        // 说明校验位错误 被恶意修改
        printf("check_security error\r\n");
        // 自行更新校验位
        uint8_t check_security_bytes[2] = {CHECK_SECURITY & 0xff, CHECK_SECURITY >> 8};
        Int_w24c02_WriteBytes(0x01, check_security_bytes, 2);
        HAL_Delay(100);
        // 自行更新更新标志位
        Int_w24c02_WriteByte(0x00, BOOTLOADER_FLAG_UNUPDATE);
        HAL_Delay(100);

        return 0;
    }
    else
    {
        printf("check_security success\r\n");
        // 说明校验位正确
        // 说明需要更新
        is_need_update = data_check[0];
        printf("is_need_update:%02x\r\n", is_need_update);

        return 1;
    }
}

// 设置一个函数判断是否需要更新
uint8_t App_bootloader_is_need_update(void)
{
    if (is_need_update == BOOTLOADER_FLAG_UPDATE)
    {

        // 需要更新

        // 执行flash从spi读取数据
        printf("is_need_update:%d\r\n", is_need_update);

        return 1;
    }
    if (is_need_update == BOOTLOADER_FLAG_UNUPDATE)
    {
        printf("is_no_need_update:%d\r\n", is_need_update);
        // 说明不需要更新
        return 0;
    }
}
// 执行跳转程序
void App_bootloader_jump_program(void)
{
    // 跳转程序
    App_bootloader_jump_app();
}
