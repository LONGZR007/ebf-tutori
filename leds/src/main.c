#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ARM 开发板 LED 设备的路径
#define RLED_DEV_PATH "/sys/class/leds/red/brightness"
#define GLED_DEV_PATH "/sys/class/leds/green/brightness"
#define BLED_DEV_PATH "/sys/class/leds/blue/brightness"

int main(int argc, char *argv[])
{
    FILE *r_fd, *g_fd, *b_fd;    // 文件描述符

    printf("led 测试实验\r\n");

    r_fd = fopen(RLED_DEV_PATH, "w");    // 以写的方式打开这个文件
    if (r_fd < 0)
    {
        printf("打开%s失败\r\n", RLED_DEV_PATH);
        exit(1);    // 退出返回错误
    }
    
    g_fd = fopen(GLED_DEV_PATH, "w");    // 以写的方式打开这个文件
    if (r_fd < 0)
    {
        printf("打开%s失败\r\n", GLED_DEV_PATH);
        exit(1);    // 退出返回错误
    }

    b_fd = fopen(BLED_DEV_PATH, "w");    // 以写的方式打开这个文件
    if (r_fd < 0)
    {
        printf("打开%s失败\r\n", BLED_DEV_PATH);
        exit(1);    // 退出返回错误
    }

    while(1)
    {
        fwrite("255", 3, 1, r_fd);
        fflush(r_fd);
        sleep(1);
        fwrite("0", 3, 1, r_fd);
        fflush(r_fd);

        fwrite("255", 3, 1, g_fd);
        fflush(g_fd);
        sleep(1);
        fwrite("0", 3, 1, g_fd);
        fflush(g_fd);

        fwrite("255", 3, 1, b_fd);
        fflush(b_fd);
        sleep(1);
        fwrite("0", 3, 1, b_fd);
        fflush(b_fd);
    }
}