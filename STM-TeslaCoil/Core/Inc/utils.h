#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include "main.h"
#include <stdarg.h>
#include "fatfs.h"
#include "cmsis_os.h"


uint32_t decode_vlq(FIL *fp);
uint16_t delta_time_to_ms(uint8_t delta_time, MIDI_controller *ctrl);
void myprintf(const char *fmt, ...);
void generate_pwm(double freq, double duty, TIM_HandleTypeDef *timer);



#endif
