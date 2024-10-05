#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
    uint16_t division; // uinit of time for delta timing
} MIDI_header_chunk;

typedef struct MIDI_controller
{
    uint8_t tim_sig; // time signiture
    uint8_t key_sig; // key signiture

} MIDI_controller;

uint32_t decode_vlq(FILE *fp)
{
    uint32_t vlq_value = 0;
    uint8_t byte;

    do
    {
        byte = fgetc(fp);
        vlq_value = (vlq_value << 7) | (byte & 0x7F); // ignores msb for read byte and shifts over 7
    } while (byte & 0x80);

    return vlq_value;
}

uint8_t skip_meta_event(FILE *fp)
{

    uint32_t event_len;
    uint8_t err;

    event_len = decode_vlq(fp);

    err = fseek(fp, event_len, SEEK_CUR);
    if (err == -1)
    {
        printf("fseek in skip event failed");
        return -1;
    }

    return 0;
}

uint8_t skip_midi_event(FILE *fp, uint8_t type)
{

    switch (type)
    {

    case 0x80: // Note Off
        printf("Skip Note Off\n");
        fseek(fp, 2, SEEK_CUR);
        break;

    case 0x90: // Note On
        printf("Skip Note On\n");
        fseek(fp, 2, SEEK_CUR);
        break;

    case 0xA0: // Polyphonic Key Pressure (Aftertouch)
        printf("Skip Polyphonic Key Pressure\n");
        fseek(fp, 2, SEEK_CUR);
        break;

    case 0xB0: // Control Change
        printf("Skip Control Change\n");
        fseek(fp, 2, SEEK_CUR);
        break;

    case 0xC0: // Program Change // 1 byte
        printf("Skip Program Change\n");
        fseek(fp, 1, SEEK_CUR);
        break;

    case 0xD0: // Channel Pressure (Aftertouch) // 1 byte
        printf("Skip Channel Pressure\n");
        fseek(fp, 1, SEEK_CUR);
        break;

    case 0xE0: // Pitch Bend Change
        printf("Skip Pitch Bend Change\n");
        fseek(fp, 2, SEEK_CUR);
        break;

    default:
        abort();
        printf("Skip Unknown MIDI Type: 0x%x\n", type);
        fseek(fp, 2, SEEK_CUR);
        break;
    }

    return 0;
}

uint8_t find_key(int8_t key, uint8_t tone, MIDI_controller controller)
{

    uint8_t major_keys[15] = {
        C_FLAT_MAJOR,  // 0x00
        G_FLAT_MAJOR,  // 0x01
        D_FLAT_MAJOR,  // 0x02
        A_FLAT_MAJOR,  // 0x03
        E_FLAT_MAJOR,  // 0x04
        B_FLAT_MAJOR,  // 0x05
        F_MAJOR,       // 0x06
        C_MAJOR,       // 0x07
        G_MAJOR,       // 0x08
        D_MAJOR,       // 0x09
        A_MAJOR,       // 0x0A
        E_MAJOR,       // 0x0B
        B_MAJOR,       // 0x0C
        F_SHARP_MAJOR, // 0x0D
        C_SHARP_MAJOR  // 0x0E
    };

    uint8_t minor_keys[15] = {
        A_FLAT_MINOR,  // A♭ Minor: 7 flats
        E_FLAT_MINOR,  // E♭ Minor: 6 flats
        B_FLAT_MINOR,  // B♭ Minor: 5 flats
        F_MINOR,       // F Minor: 4 flats
        C_MINOR,       // C Minor: 3 flats
        G_MINOR,       // G Minor: 2 flats
        D_MINOR,       // D Minor: 1 flat
        A_MINOR,       // A Minor: 0 flats/sharps
        E_MINOR,       // E Minor: 1 sharp
        B_MINOR,       // B Minor: 2 sharps
        F_SHARP_MINOR, // F♯ Minor: 3 sharps
        C_SHARP_MINOR, // C♯ Minor: 4 sharps
        G_SHARP_MINOR, // G♯ Minor: 5 sharps
        D_SHARP_MINOR, // D♯ Minor: 6 sharps
        A_SHARP_MINOR  // A♯ Minor: 7 sharps
    };

    // minor
    if (tone == 0x00)
    {
        controller.key_sig = minor_keys[(sizeof(minor_keys) / 2) + key];
        printf("Key Signiture: 0x%x\n", controller.key_sig);
    }
    // major
    else if (tone == 0x01)
    {
        controller.key_sig = major_keys[(sizeof(minor_keys) / 2) + key];
        printf("Key Signiture: 0x%x\n", controller.key_sig);
    }

    else
    {
        printf("invalid tone: 0x%x\n", tone);
        return -1;
    }

    return 0;
}

uint8_t meta_event_handler(FILE *fp, uint32_t delta_time)
{

    uint8_t *buf;
    uint8_t meta_type;
    uint32_t event_len;

    /*
    basic meta event structure is
    Delta time
    FF (signifies meta event)
    Event Code (1 byte)
    */

    meta_type = fgetc(fp);

    switch (meta_type)
    {
    case 0x00:
        printf("Handle Sequence Number event\n");
        skip_meta_event(fp);

        break;
    case 0x01:
        printf("Handle Text Event\n");
        skip_meta_event(fp);
        break;
    case 0x02:
        printf("Handle Copyright Notice\n");
        skip_meta_event(fp);
        break;
    case 0x03:
        printf("Track Name\n");
        event_len = decode_vlq(fp);
        int i = 0;

        buf = malloc(sizeof(uint8_t) * event_len);
        if (!buf)
        {
            printf("Malloc Failed in track name\n");
            return -1;
        }

        fread(buf, 1, event_len, fp);

        while (i < event_len)
        {
            printf("%c", buf[i]);
            i++;
        }

        printf("\n\n");

        // skip_meta_event(fp);
        break;
    case 0x04:
        printf("Handle Instrument Name\n");
        skip_meta_event(fp);
        break;
    case 0x05:
        printf("Handle Lyric Text\n");
        skip_meta_event(fp);
        break;
    case 0x06:
        printf("Handle Marker Text\n");
        skip_meta_event(fp);
        break;
    case 0x07:
        printf("Handle Cue Point\n");
        skip_meta_event(fp);
        break;
    case 0x20:
        printf("Handle MIDI Channel Prefix Assignment\n");
        skip_meta_event(fp);
        break;
    case 0x2F:
        printf("Handle End of Track\n\n\n");
        fgetc(fp); // skips final value

        return 1; // success!!
    case 0x51:
        // printf("Handle Tempo Setting\n");

        // uint32_t num_micro = 0;

        // buf = malloc(sizeof(uint8_t) * 3);
        // if (!buf)
        // {
        //     printf("Malloc Failed in Time Signiture");
        //     return -1;
        // }
        // fgetc(fp);
        // fread(buf, 1, 3, fp); // reads 4 bytes
        // num_micro = (buf[0] << 16) | (buf[1] << 8) | (buf[2]);
        // printf("Microseconds per quarter note: %d\n", num_micro);

        skip_meta_event(fp);
        break;
    case 0x54:
        printf("Handle SMPTE Offset\n");
        skip_meta_event(fp);
        break;
    case 0x58:     // time signiture
        fgetc(fp); // always 0x04

        buf = malloc(sizeof(uint8_t) * 4);
        if (!buf)
        {
            printf("Malloc Failed in Time Signiture");
            return -1;
        }

        fread(buf, 1, 4, fp); // reads 4 bytes

        printf("Time signiture: \n num:%x\n denom:%x\n clocks per beat:%x\n 32nds per quarter:%x\n\n", buf[0], buf[1], buf[2], buf[3]);

        break;
    case 0x59:
        printf("Handle Key Signature\n");
        uint8_t tone;
        uint8_t key_sig;
        MIDI_controller controller;

        fseek(fp, 1, SEEK_CUR); // shifts past length byte 0x02

        key_sig = fgetc(fp); // number of sharps or flats
        tone = fgetc(fp);    // major(1) / minor(0)

        find_key(key_sig, tone, controller);

        break;
    case 0x7F: // sequencer specific event
        // printf("Handle Sequencer Specific Event\n");
        // event_len = fgetc(fp);

        // buf = malloc(sizeof(uint8_t) * event_len);
        // if (!buf)
        // {
        //     printf("Malloc Failed in sequencer specific event\n");
        //     return -1;
        // }

        // fread(buf, 1, event_len, fp);

        skip_meta_event(fp);
        break;
    default:
        printf("Unknown event type: 0x%02X\n", meta_type);
        skip_meta_event(fp);
        return -1;
    }

    return 0;
}

// music event
void midi_event_handler(FILE *fp, uint32_t delta_time, uint8_t event)
{
    uint8_t midi_type;
    uint8_t channel;

    // printf("Event: 0x%x\n", event);

    midi_type = 0xF0 & event; // event name
    channel = 0x0F & event;   // where the event gets sent *** for polyphonic music

    // printf("Midi Type: 0x%x\n", midi_type);

    switch (midi_type)
    {
    case 0x80: // Note Off
        printf("Note Off @ Channel: 0x%x\n", channel);
        skip_midi_event(fp, midi_type);
        break;

    case 0x90: // Note On
        printf("Note On @ Channel: 0x%x\n", channel);
        skip_midi_event(fp, midi_type);
        break;

    case 0xA0: // Polyphonic Key Pressure (Aftertouch)
        printf("Polyphonic Key Pressure\n");
        skip_midi_event(fp, midi_type);
        break;

    case 0xB0: // Control Change
        printf("Control Change\n");
        skip_midi_event(fp, midi_type);
        break;

    case 0xC0: // Program Change
        printf("Program Change\n");
        skip_midi_event(fp, midi_type);
        break;

    case 0xD0: // Channel Pressure (Aftertouch)
        printf("Channel Pressure\n");
        skip_midi_event(fp, midi_type);
        break;

    case 0xE0: // Pitch Bend Change
        printf("Pitch Bend Change\n");
        skip_midi_event(fp, midi_type);
        break;

    default:
        printf("Unknown MIDI Type Handler: 0x%x\n", midi_type);
        skip_midi_event(fp, midi_type);
        break;
    }
}

// midi system exclusive message
void sysex_event_handler(FILE *fp, uint32_t delta_time)
{
}

uint8_t process_one_track(FILE *fp)
{
    uint8_t *buf;
    uint32_t trk_hdr; // string header at front of all tracks
    uint32_t trk_len; // length of track data
    uint8_t end;

    uint8_t event_type;
    uint32_t delta_time;

    buf = (uint8_t *)malloc(sizeof(uint8_t) * 8);
    if (!buf)
    {
        printf("Buffer in parse_midi is null\n");
        return -1;
    }

    fread(buf, 1, 8, fp);

    trk_hdr = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if (trk_hdr != 0x4D54726B)
    { // verifies the track start is valid
        printf("Invalid track header recieved: %x\n", trk_hdr);
        return -1;
    }

    trk_len = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]; //  track length

    free(buf);

    buf = (uint8_t *)malloc(sizeof(uint8_t) * trk_len);
    if (!buf)
    {
        printf("Buffer in parse_midi is null");
        return -1;
    }

    // for debug
    printf("Track header: %x\n", trk_hdr);
    printf("Track length: %x\n", trk_len);

    // // dummy read to skip track info
    // fread(buf, 1, trk_len, fp);

    do
    {

        delta_time = decode_vlq(fp);

        event_type = fgetc(fp);

        // printf("Event Type: %x\n", event_type);

        if (event_type == 0xFF)
        {
            end = meta_event_handler(fp, delta_time);
        }
        else if ((event_type >= 0xF0) && (event_type != 0xFF))
        {
            printf("sysex event called: 0x%x\n", event_type);
            // fgetc(fp);
            // skip_meta_event(fp);
            // sysex_event_handler(fp, delta_time);
        }
        else
        {
            midi_event_handler(fp, delta_time, event_type);
        }

    } while (end != 1);

    free(buf);

    return 0;
}

MIDI_header_chunk parse_midi(FILE *fp, MIDI_header_chunk hdr)
{

    uint8_t *buf;

    buf = (uint8_t *)malloc(14);
    if (!buf)
    {
        printf("Buffer in parse_midi is null");
    }

    fread(buf, 1, 14, fp);

    // all big endian
    hdr.MThd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if (hdr.MThd != 0x4D546864)
    {                                                           // string of MThd
        printf("Unexpected Header - Recieved: %c\n", hdr.MThd); // return characters for easy read
    }

    hdr.hdr_len = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
    if (hdr.hdr_len != 0x0006)
    { // always 6 bytes
        printf("Unexpected Header Length - Recieved: %u\n", hdr.hdr_len);
    }

    hdr.format = (buf[8] << 8) | buf[9];
    if (hdr.format != 0x00 && hdr.format != 0x01 && hdr.format != 0x02)
    {
        printf("Unexpected Header Format - Recieved: %u\n", hdr.format);
    }

    hdr.num_tracks = (buf[10] << 8) | buf[11];
    hdr.division = (buf[12] << 8) | buf[13];

    // to check all header info
    printf("Header string is: %x\n", hdr.MThd);
    printf("Length is: %x\n", hdr.hdr_len);
    printf("Format is:%x \n", hdr.format);
    printf("Number of tracks is: %x\n", hdr.num_tracks);
    printf("Division is: %x\n", hdr.division);

    free(buf);

    return hdr;
}

int main(int argc, char *argv[])
{
    FILE *fp;              // for midi file operations
    MIDI_header_chunk hdr; // to container header info

    fp = fopen(argv[1], "r");

    if (!fp)
    {
        printf("fp is Null");
        return -1;
    }

    hdr = parse_midi(fp, hdr); // grabs header info

    process_one_track(fp);
    process_one_track(fp);

    // while (hdr.num_tracks > 0)
    // {

    //     process_one_track(fp);

    //     hdr.num_tracks--;
    // }

    return 0;
}