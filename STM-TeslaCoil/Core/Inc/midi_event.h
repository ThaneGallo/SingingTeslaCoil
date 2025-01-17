#ifndef MIDI_EVENT_H
#define MIDI_EVENT_H

#include "common.h"
#include "utils.h"
#include "fatfs.h"


void note_on_event(struct note* note, FIL* fp, uint32_t delta_time, MIDI_controller *ctrl);

void note_off_event(struct note* note, FIL* fp, uint32_t delta_time, MIDI_controller *ctrl);

/**
 *  @brief processes midi events
 * @param fp FIL pointer to midi FIL
 * @param delta_time time before next event is read in ticks
 * @param event handle for event type
 * @param ctrl contains system information for controls
 */
void midi_event_handler(FIL *fp, uint32_t delta_time, uint8_t event, MIDI_controller *ctrl);

/**
 *  @brief processes meta events
 * @param fp FIL pointer to midi FIL
 * @param delta_time time before next event is read in ticks
 * @param ctrl contains system information for controls
 * @return 0 on success -1 if failure
 */
uint8_t meta_event_handler(FIL *fp, uint32_t delta_time, MIDI_controller *ctrl);

/**
 *  @brief processes sysex events
 * @param fp FIL pointer to midi FIL
 * @param delta_time time before next event is read in ticks
 */
void sysex_event_handler(FIL *fp, uint32_t delta_time);

/**
 *  @brief skips meta events
 * @param fp FIL pointer to midi FIL
 * @return 0 on success -1 on failure
 */
uint8_t skip_meta_event(FIL *fp);

/**
 *  @brief skips midi events
 * @param fp FIL pointer to midi FIL
 * @param type 8-bit handle of midi type
 * @return 0 on success -1 on failure
 */
uint8_t skip_midi_event(FIL *fp, uint8_t type);

/**
 *  @brief finds key based on tone parsed
 * @param key key signature value
 * @param tone major/minor scale indicator
 * @param controller pointer to control structure
 * @return 0 on success -1 on failure
 */
uint8_t find_key(int8_t key, uint8_t tone, MIDI_controller *controller);

/**
 *  @brief makes a copy of old FIL pointer and moves the original to the next track
 * @param fp FIL pointer to music
 * @return returns copied FIL pointer
 */
FIL *get_track(FIL *fp);

/**
 *  @brief plays one track dealing with midi, meta, and sysex events successfully
 * @param fp FIL pointer to music
 * @param controller pointer to control structure
 * @return returns 0 on success other on failure
 */
uint8_t play_one_track(FIL *fp, MIDI_controller *ctrl);

/**
 * @brief removes important track data from header chunk and moves fp forward to first track
 * @param fp FIL pointer to music
 * @param hdr pointer to header structure
 * @return returns 0 on success other on failure
 */
MIDI_header_chunk parse_midi_header(FIL *fp, MIDI_header_chunk hdr);

#endif
