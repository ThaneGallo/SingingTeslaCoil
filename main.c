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

void send_data();

uint8_t process_one_track(FILE *fp)
{
    uint8_t *buf;
    uint32_t trk_hdr;
    uint32_t trk_len;

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
    // TODO become send midi

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