#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

// ARM 开发板 LED 设备的路径
#define RLED_DEV_PATH "/sys/class/leds/red/brightness"
#define GLED_DEV_PATH "/sys/class/leds/green/brightness"
#define BLED_DEV_PATH "/sys/class/leds/blue/brightness"

int main(int argc, char *argv[])
{
    int r_fd, g_fd, b_fd;    // 文件描述符

    printf("led 测试实验，使用系统调用控制\r\n");

    r_fd = open(RLED_DEV_PATH, O_WRONLY);    // 以写的方式打开这个文件
    if (r_fd < 0)
    {
        printf("打开%s失败\r\n", RLED_DEV_PATH);
        exit(1);    // 退出返回错误
    }
    
    g_fd = open(GLED_DEV_PATH, O_WRONLY);    // 以写的方式打开这个文件
    if (r_fd < 0)
    {
        printf("打开%s失败\r\n", GLED_DEV_PATH);
        exit(1);    // 退出返回错误
    }

    b_fd = open(BLED_DEV_PATH, O_WRONLY);    // 以写的方式打开这个文件
    if (r_fd < 0)
    {
        printf("打开%s失败\r\n", BLED_DEV_PATH);
        exit(1);    // 退出返回错误
    }

    while(1)
    {
        write(r_fd, "255", 3);
        sleep(1);
        write(r_fd, "0", 1);

        write(g_fd, "255", 3);
        sleep(1);
        write(g_fd, "0", 1);

        write(b_fd, "255", 3);
        sleep(1);
        write(b_fd, "0", 1);
    }
}