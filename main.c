#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct MIDI_header_chunk
{
    uint32_t MThd;    // string to start MIDI file
    uint32_t hdr_len; // header length
    uint16_t format;
    uint16_t num_tracks;
    uint16_t division; // uinit of time for delta timing
} MIDI_header_chunk;

uint32_t decode_vlq(FILE *fp)
{
    uint32_t vlq_value;
    uint8_t byte;

    do
    {
        byte = fgetc(fp);
        vlq_value = (vlq_value << 7) | (byte & 0x7F); // ignores msb for read byte and shifts over 7
    } while (byte & 0x80);

    return vlq_value;
}

uint8_t meta_event_handler(FILE *fp, uint32_t delta_time)
{

    uint8_t *buf;
    uint8_t meta_type;

    buf = malloc(sizeof(uint8_t));
    if (!buf)
    {
        printf("Buffer in meta_event_handler is NULL");
        return -1;
    }

    /*
    basic meta event structure is
    Delta time
    FF (signifies meta event)
    Event Code (1 byte)
    */

    switch (meta_type)
    {
    case 0x00:
        // Handle Sequence Number event
        break;
    case 0x01:
        // Handle Text Event
        break;
    case 0x02:
        // Handle Copyright Notice
        break;
    case 0x03:
        // Handle Sequence or Track Name
        break;
    case 0x04:
        // Handle Instrument Name
        break;
    case 0x05:
        // Handle Lyric Text
        break;
    case 0x06:
        // Handle Marker Text
        break;
    case 0x07:
        // Handle Cue Point
        break;
    case 0x20:
        // Handle MIDI Channel Prefix Assignment
        break;
    case 0x2F:
        // Handle End of Track
        
        return 1; // sucess!!
    case 0x51:
        // Handle Tempo Setting
        break;
    case 0x54:
        // Handle SMPTE Offset
        break;
    case 0x58:
        // Handle Time Signature
        break;
    case 0x59:
        // Handle Key Signature
        break;
    case 0x7F:
        // Handle Sequencer Specific Event
        break;
    default:
        printf("Unknown event type: 0x%02X\n", meta_type);
        return -1;
    }

    return 0;
}

// music event
void midi_event_handler(FILE *fp, uint32_t delta_time)
{
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

    uint8_t event_type;
    uint32_t delta_time;

    buf = (uint8_t *)malloc(sizeof(uint8_t) * 8);
    if (!buf)
    {
        printf("Buffer in parse_midi is null");
        return -1;
    }

    fread(buf, 1, 8, fp);

    trk_hdr = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if (trk_hdr != 0x4D54726B)
    { // verifies the track start is valid
        printf("Invalid track header recieved: %x", trk_hdr);
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

    // dummy read to skip track info
    fread(buf, 1, trk_len, fp);

    delta_time = decode_vlq(fp);

    event_type = fgetc(fp);

    if (event_type == 0xFF)
    {
        meta_event_handler(fp, delta_time);
    }
    else if ((event_type >= 0xF0) && (event_type != 0xFF))
    {
        sysex_event_handler(fp, delta_time);
    }
    else
    {
        midi_event_handler(fp, delta_time);
    }

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

    while (hdr.num_tracks > 0)
    {

        process_one_track(fp);

        hdr.num_tracks--;
    }

    return 0;
}