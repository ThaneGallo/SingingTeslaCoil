#include <stdint.h>
#include <stdio.h>


typedef struct MIDI_header_chunk
{
    uint32_t MThd;    // string to start MIDI file
    uint32_t hdr_len; // header length
    uint16_t format;
    uint16_t num_tracks;
    uint16_t division; // uinit of time for delta timing
} MIDI_header_chunk;

// struct MIDI_track_chunk{
//     uint32_t MTrk; //string to indicate track chunk start
//     uint32_t trk_len; //header length

// }MIDI_track_chunk;

uint8_t parse_midi(FILE *fp)
{

    MIDI_header_chunk hdr;
    uint8_t buf[14];

    fread(buf, 1, 14, fp);

    // all big endian
    hdr.MThd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if (hdr.MThd != 0x4D546864)
    {                                                         // string of MThd
        printf("Unexpected Header - Recieved: %c", hdr.MThd); // return characters for easy read
        return -1;
    }
    

    hdr.hdr_len = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
    if (hdr.hdr_len != 0x0006)
    { // always 6 bytes
        printf("Unexpected Header Length - Recieved: %u", hdr.hdr_len);
        return -1;
    }

    hdr.format = (buf[8] << 8) | buf[9];
    if (hdr.format != 0x00 && hdr.format != 0x01 && hdr.format != 0x02)
    {
        printf("Unexpected Header Format - Recieved: %u", hdr.format);
        return -1;
    }

    hdr.num_tracks = (buf[10] << 8) | buf[11];
    hdr.division = (buf[12] << 8) | buf[13];

    // to check all header info
    printf("Header string is: %x\n", hdr.MThd);
    printf("Length is: %x\n", hdr.hdr_len);
    printf("Format is:%x \n", hdr.format);
    printf("Number of tracks is: %x\n", hdr.num_tracks);
    printf("Division is: %x\n", hdr.division);

    return 0;
}

int main(int argc, char *argv[])
{
    FILE *fp; // for midi file operations

    

    fp = fopen(argv[1], "r");

    if (!fp)
    {
        printf("fp is Null");
        return -1;
    }

    parse_midi(fp); // grabs header info



    return 0;
}