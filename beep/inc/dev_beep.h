#ifndef __DEV_BEEP__
#define __DEV_BEEP__

#define BEEP_GPIO_INDEX     "19"

int beep_init(void);
int beep_deinit(void);
int beep_on(void);
int beep_off(void);

#endif