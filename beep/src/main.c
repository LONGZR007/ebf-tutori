#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "inc/dev_beep.h"

int main(int argc, char *argv[])
{
    int res;
    char buf[5];

    printf("beep init\r\n");

    res = beep_init();
    if (res)
    {
        printf("beep init error code %d\r\n", res);
    }

    while (1)
    {
        printf("Please input the value : 0--off 1--on q--exit\n");
        
        scanf("%5s", buf);
    
        switch(buf[0])
        {
            case '1':
                beep_on();
                break;

            case '0':
                beep_off();
                break;

            case 'q':
                printf("exit\r\n");
                exit(0);
                break;

            default:
                printf("input error！\r\n");
                break;
        }
    }
    
}