#ifndef UTILS_H
#define UTILS_H

#include "common.h"


uint32_t decode_vlq(FILE *fp);
uint16_t delta_time_to_ms(uint8_t delta_time, MIDI_controller *ctrl);

#endif