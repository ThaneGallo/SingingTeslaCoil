#include "utils.h"
#include "main.h"
#include "fatfs.h"
#include <string.h>


/**
 *  @brief Processes variable length quantity (VLQ) values from a file pointer 
 * @param fp File pointer to midi file
 * @param vlq_value decoded vlq value*/
uint32_t decode_vlq(FIL *fp)
{
    /**
     * Encoding works as follows
     * 
     * each byte containing 7 bits of data and an 8th bit(MSB) as a continuation flag. 
     * if the bit is 1 the number continues into the next byte
     * if the bit is 0 the number does not continue
     *  */
    uint8_t byte;
    uint32_t vlq_value = 0;

    do
    {
        f_read(fp, &byte, 1, NULL);
        vlq_value = (vlq_value << 7) | (byte & 0x7F); // ignores msb for read byte and shifts over 7
    } while (byte & 0x80);

    return vlq_value;
}

/**
 *  @brief takes the delta time value for the given event and converts it to ms
 * @param delta_time delta time in ticks of event 
 * @param ctrl pointer to system timing info*/
uint16_t delta_time_to_ms(uint8_t delta_time, MIDI_controller *ctrl)
{
    float ms = 0;

    // printf("delta time: %d\n", delta_time);
    // printf("tick per q note: %d\n", ctrl->tick_per_q_note);
    // printf("tempo: %d\n", ctrl->tempo);

    // needs to be a float to do the division or else returns 0
    ms = ((float)delta_time / ctrl->tick_per_q_note) * (60000 / ctrl->tempo);

    return ms;
}

void myprintf(const char *fmt, ...) {
  static char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  int len = strlen(buffer);
  HAL_UART_Transmit(&huart2, (uint8_t*)buffer, len, -1);

}
