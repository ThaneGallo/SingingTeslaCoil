#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <fatfs.h>
#include <freertos.h>

// major key signitures
#define C_FLAT_MAJOR (0x00)
#define G_FLAT_MAJOR (0x01)
#define D_FLAT_MAJOR (0x02)
#define A_FLAT_MAJOR (0x03)
#define E_FLAT_MAJOR (0x04)
#define B_FLAT_MAJOR (0x05)
#define F_MAJOR (0x06)
#define C_MAJOR (0x07)
#define G_MAJOR (0x08)
#define D_MAJOR (0x09)
#define A_MAJOR (0x0A)
#define E_MAJOR (0x0B)
#define B_MAJOR (0x0C)
#define F_SHARP_MAJOR (0x0D)
#define C_SHARP_MAJOR (0x0E)

// minor key signitures
#define A_FLAT_MINOR (0x0F)  // A♭ Minor: 7 flats
#define E_FLAT_MINOR (0x10)  // E♭ Minor: 6 flats
#define B_FLAT_MINOR (0x11)  // B♭ Minor: 5 flats
#define F_MINOR (0x12)       // F Minor: 4 flats
#define C_MINOR (0x13)       // C Minor: 3 flats
#define G_MINOR (0x14)       // G Minor: 2 flats
#define D_MINOR (0x15)       // D Minor: 1 flat
#define A_MINOR (0x16)       // A Minor: 0 flats/sharps
#define E_MINOR (0x17)       // E Minor: 1 sharp
#define B_MINOR (0x18)       // B Minor: 2 sharps
#define F_SHARP_MINOR (0x19) // F♯ Minor: 3 sharps
#define C_SHARP_MINOR (0x1A) // C♯ Minor: 4 sharps
#define G_SHARP_MINOR (0x1B) // G♯ Minor: 5 sharps
#define D_SHARP_MINOR (0x1C) // D♯ Minor: 6 sharps
#define A_SHARP_MINOR (0x1D) // A♯ Minor: 7 sharps

// minor key signitures

typedef struct MIDI_header_chunk
{
    uint32_t MThd;    // string to start MIDI file
    uint32_t hdr_len; // header length
    uint16_t format;
    uint16_t num_tracks;
    uint16_t division; // ticks per quarter note
} MIDI_header_chunk;

typedef struct MIDI_controller
{
    uint8_t ctrl_handle;      // id of coil
    uint8_t tim_sig; // time signiture
    uint8_t key_sig; // key signiture

    uint16_t tick_per_q_note;
    uint8_t tempo;

    FIL* trk_buf; // to contain tracks for playing synchronously
    osMessageQId queue;
    uint8_t format;

} MIDI_controller;

typedef struct Coil{
    bool is_busy;
    float cur_note;
} Coil;

typedef struct note {
    uint8_t number; //60 is middle C
    uint8_t velocity; //loudness of key?
    float frequency;
    bool on_off;
    uint8_t ms;
}note;


#endif
