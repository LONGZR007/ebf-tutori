#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

#define INPUT_PATH     "/dev/input/by-path/platform-gpio-keys-event"

int main(int argc, char *argv[])
{
    int fd;
    struct input_event event;
    char *path;

    printf("输入设备测试\r\n");

    if (argc > 1)
    {
        path = argv[1];
    }
    else
    {
        path = INPUT_PATH;
    }
    

    fd = open(path, O_RDONLY);
    if (fd < 1)
    {
        printf("打开设备失败！\r\n");
        exit(1);
    }

    while(1)
    {
        if (read(fd, &event, sizeof(event)) == sizeof(event))
        {
            // EV_SYN 是事件分隔标志，不打印
            if (event.type != EV_SYN)
            {
                printf("event:time %ld.%ld, tpye %d, code %d, value %d\r\n", 
                        event.time.tv_sec, event.time.tv_usec, 
                        event.type, 
                        event.code, 
                        event.value);
            }
        }
        
    }

    close(fd);

    return 0;
}