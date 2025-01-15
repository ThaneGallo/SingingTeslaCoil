#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "common.h"
#include "midi_event.h"
#include "utils.h"


int main(int argc, char *argv[])
{
    FILE *fp;              // for midi file operations
    MIDI_header_chunk hdr; // to container header info
    MIDI_controller *ctrl = malloc(sizeof(MIDI_controller));
    // uint8_t i = 0;

    fp = fopen(argv[1], "r");

    if (!fp)
    {
        printf("fp is Null");
        return -1;
    }

    hdr = parse_midi_header(fp, hdr); // grabs header info

    ctrl->tick_per_q_note = hdr.division;


    //BaseType_t xReturned;
    //xReturned = xTaskCreate(parseMidi, "Parse Midi", 2048, fp, Handle)



    play_one_track(fp, ctrl);
    


    // if (hdr.format == 0)
    // {
    //     play_one_track(fp);
    // }
    // else if (hdr.format == 1) // intended to play all tracks at once
    // {
    //     controller = malloc(sizeof(MIDI_controller) * hdr.num_tracks);
    //     if (!controller)
    //     {
    //         printf("Malloc Failed for controller");
    //         return -1;
    //     }

    //     while (hdr.num_tracks > 0)
    //     {
    //         controller[i].ctrl_handle = i;
    //         controller[i].trk_buf = get_track(fp);
    //     }
    // }
    // else // can play independedntly but doesnt have to be together ie multiple instruments
    // {

    //     // while (hdr.num_tracks > 0)
    //     // {

    //     //     play_one_track(fp);

    //     //     hdr.num_tracks--;
    //     // }
    // }

    // play_one_track(fp);
    // play_one_track(fp);

    return 0;
}