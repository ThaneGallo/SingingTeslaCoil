#include "midi_event.h"
#include "main.h"
#include "utils.h"

#include <stdlib.h>
#include <math.h>



void note_on_event(struct note* note, FIL* fp, uint32_t delta_time, MIDI_controller *ctrl){

      uint16_t ms;
      note = malloc(sizeof(note));
      ms = delta_time_to_ms(delta_time, ctrl);
      osStatus res;

      note->on_off = true;
      note->ms = ms;




      f_read(fp, &note->number, 1, NULL);
      f_read(fp, &note->velocity, 1, NULL);
      note->frequency = 440 * pow(2.0, (note->number - 69) / 12.0);

//      myprintf("Note On @ Channel: 0x%x\n", channel);
//      myprintf("note frequency %f Hz\n", note->frequency);
//      myprintf("note timing in ms %x\n\n", ms);

      myprintf("before message put in note on\n");
      	  res = osMessagePut(NoteQueue1Handle, note, osWaitForever);
      	myprintf("before message after put");
          if(res != osOK){
          	myprintf("error in osMessagePut %d\n", res);
          }

          myprintf("note number in PARSE %x\n", note->number);



      //play note
//     free(note);

      // skip_midi_event(fp, midi_type);
}

void note_off_event(struct note* note, FIL* fp, uint32_t delta_time, MIDI_controller *ctrl){


    uint16_t ms;
    ms = delta_time_to_ms(delta_time, ctrl);
    osStatus res;

    note->on_off = false;
    note->ms = ms;

    f_read(fp, &note->number, 1, NULL);
    f_read(fp,&note->velocity, 1, NULL);
    note->frequency = 440 * pow(2.0, (note->number - 69) / 12.0);




    // myprintf("note velocity %x\n", note.velocity);
//    myprintf("Note Off @ Channel: 0x%x\n", channel);
//    myprintf("note frequency %f Hz\n", note->frequency);


    //is there space?

    //send note to queue

    myprintf("before message put in note off\n");
    // skip_midi_event(fp, midi_type);
    res = osMessagePut(NoteQueue1Handle, note, osWaitForever);
    if(res != osOK){
    	myprintf("error in osMessagePut %d\n", res);
    }

    myprintf("note number in PARSE %x\n", note->number);


}


/**
 *  @brief processes midi events
 * @param fp FIL pointer to midi FIL
 * @param delta_time time before next event is read in ticks
 * @param event handle for event type
 * @param ctrl contains system information for controls*/
void midi_event_handler(FIL *fp, uint32_t delta_time, uint8_t event, MIDI_controller *ctrl)
{
    uint8_t midi_type;
    uint8_t channel;
    struct note* note = malloc(sizeof(note));

    midi_type = 0xF0 & event; // event name
    channel = 0x0F & event;   // where the event gets sent *** for polyphonic music

    // myprintf("Midi Type: 0x%x\n", midi_type);

    switch (midi_type)
    {
    case 0x80: // Note Off
    	note_off_event(note, fp, delta_time, ctrl);
        break;

    case 0x90: // Note On
    	note_on_event(note, fp, delta_time, ctrl);
        break;

    case 0xA0: // Polyphonic Key Pressure (Aftertouch)
        myprintf("Polyphonic Key Pressure\n");
        skip_midi_event(fp, midi_type);
        break;

    case 0xB0: // Control Change
        skip_midi_event(fp, midi_type);
        break;

    case 0xC0: // Program Change
        // changes instrument type
        skip_midi_event(fp, midi_type);
        break;

    case 0xD0: // Channel Pressure (Aftertouch)
        skip_midi_event(fp, midi_type);
        break;

    case 0xE0: // Pitch Bend Change

        skip_midi_event(fp, midi_type);
        break;

    default:
        myprintf("Unknown MIDI Type Handler: 0x%x\n", midi_type);
        skip_midi_event(fp, midi_type);
        break;
    }
}

/**
 *  @brief processes meta events
 * @param fp FIL pointer to midi FIL
 * @param delta_time time before next event is read in ticks
 * @param event handle for event type
 * @param ctrl contains system information for controls
 * @return 0 on success -1 if failure*/
uint8_t meta_event_handler(FIL *fp, uint32_t delta_time, MIDI_controller *ctrl)
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

    f_read(fp, &meta_type, 1, NULL);

    switch (meta_type)
    {
    case 0x00:
        myprintf("Handle Sequence Number event\n");
        skip_meta_event(fp);

        break;
    case 0x01:
        myprintf("Handle Text Event\n");
        skip_meta_event(fp);
        break;
    case 0x02:
        myprintf("Handle Copyright Notice\n");
        skip_meta_event(fp);
        break;
    case 0x03:
        myprintf("Track Name\n");
        event_len = decode_vlq(fp);
        int i = 0;

        buf = malloc(sizeof(uint8_t) * event_len);
        if (!buf)
        {
            myprintf("Malloc Failed in track name\n");
            return -1;
        }

        f_read(fp, buf, event_len, NULL);

        // prints track name
        while (i < event_len)
        {
            myprintf("%c", buf[i]);
            i++;
        }

        myprintf("\n\n");

        // skip_meta_event(fp);
        break;
    case 0x04:
        myprintf("Handle Instrument Name\n");
        skip_meta_event(fp);
        break;
    case 0x05:
        myprintf("Handle Lyric Text\n");
        skip_meta_event(fp);
        break;
    case 0x06:
        myprintf("Handle Marker Text\n");
        skip_meta_event(fp);
        break;
    case 0x07:
        myprintf("Handle Cue Point\n");
        skip_meta_event(fp);
        break;
    case 0x20:
        myprintf("Handle MIDI Channel Prefix Assignment\n");
        skip_meta_event(fp);
        break;
    case 0x2F:
        myprintf("Handle End of Track\n\n\n");
        f_read(fp, NULL, 1, NULL);; // skips final value

        return 1; // success!!
    case 0x51:
        myprintf("Sets Tempo\n");

        uint32_t num_micro = 0;

        buf = malloc(sizeof(uint8_t) * 3);
        if (!buf)
        {
            myprintf("Malloc Failed in Time Signiture");
            return -1;
        }
        f_read(fp, &event_len, 1, NULL);
        f_read(fp, buf, 3, NULL);
        num_micro = (buf[0] << 16) | (buf[1] << 8) | (buf[2]);
        myprintf("Microseconds per quarter note: %d\n", num_micro);

        ctrl->tempo = 60000000 / num_micro; // tempo in bpm

        myprintf("tempo in BPM: %d\n", ctrl->tempo);

        // skip_meta_event(fp);
        break;
    case 0x54:
        myprintf("Handle SMPTE Offset\n");
        skip_meta_event(fp);
        break;
    case 0x58: // time signiture

        f_read(fp, &event_len, 1, NULL);; // gets length byte (always 0x04)

        buf = malloc(sizeof(uint8_t) * 4);
        if (!buf)
        {
            myprintf("Malloc Failed in Time Signiture");
            return -1;
        }

        /* Byte order
        numerator
        denominator
        metrinome clicks
        32nd notes per quarter
        */

        f_read(fp,buf,4,NULL);

        myprintf("Time signiture: \n num:%x\n denom:%x\n clocks per beat:%x\n 32nds per quarter:%x\n\n", buf[0], buf[1], buf[2], buf[3]);

        break;
    case 0x59:
        myprintf("Handle Key Signature\n");
        uint8_t tone;
        uint8_t key_sig;

        f_lseek(fp, f_tell(fp) + 1); // shifts past length byte 0x02

        f_read(fp, &key_sig, 1, NULL);; // number of sharps or flats
        f_read(fp, &tone, 1, NULL);;    // major(1) / minor(0)

        find_key(key_sig, tone, ctrl);

        break;
    case 0x7F: // sequencer specific event
        // myprintf("Handle Sequencer Specific Event\n");
        // event_len = f_read(fp, &byte, 1, NULL);;

        // buf = malloc(sizeof(uint8_t) * event_len);
        // if (!buf)
        // {
        //     myprintf("Malloc Failed in sequencer specific event\n");
        //     return -1;
        // }

        // fread(buf, 1, event_len, fp);

        skip_meta_event(fp);
        break;
    default:
        myprintf("Unknown meta event type: 0x%02X\n", meta_type);
        skip_meta_event(fp);
        return -1;
    }

    return 0;
}

/**
 *  @brief processes sysex events
 * @param fp FIL pointe to midi FIL
 * @param delta_time time before next event is read in ticks
 * */
void sysex_event_handler(FIL *fp, uint32_t delta_time)
{
}

/**
 *  @brief skips meta events
 * @param fp FIL pointe to midi FIL
 * @return 0 on sucess -1 on failure
 * */
uint8_t skip_meta_event(FIL *fp)
{

    uint32_t event_len;
    uint8_t err;

    event_len = decode_vlq(fp);

    err = f_lseek(fp, f_tell(fp) + event_len);
    if (err == -1)
    {
        myprintf("f_lseek in skip event failed");
        return -1;
    }

    return 0;
}

/**
 *  @brief skips midi events
 * @param fp FIL pointe to midi FIL
 * @param type 8-bit handle of midi type
 * @return 0 on sucess -1 on failure
 * */
uint8_t skip_midi_event(FIL *fp, uint8_t type)
{

    switch (type)
    {

    case 0x80: // Note Off
        myprintf("Skip Note Off\n");
        f_lseek(fp, f_tell(fp) + 2);
        break;

    case 0x90: // Note On
        myprintf("Skip Note On\n");
        f_lseek(fp, f_tell(fp) + 2);
        break;

    case 0xA0: // Polyphonic Key Pressure (Aftertouch)
        myprintf("Skip Polyphonic Key Pressure\n");
        f_lseek(fp, f_tell(fp) + 2);
        break;

    case 0xB0: // Control Change
        myprintf("Skip Control Change\n");
        f_lseek(fp, f_tell(fp) + 2);
        break;

    case 0xC0: // Program Change // 1 byte
        myprintf("Skip Program Change\n");
        f_lseek(fp, f_tell(fp) + 1);
        break;

    case 0xD0: // Channel Pressure (Aftertouch) // 1 byte
        myprintf("Skip Channel Pressure\n");
        f_lseek(fp, f_tell(fp) + 1);
        break;

    case 0xE0: // Pitch Bend Change
        myprintf("Skip Pitch Bend Change\n");
        f_lseek(fp, f_tell(fp) + 2);
        break;

    default:
        abort();
        myprintf("Skip Unknown MIDI Type: 0x%x\n", type);
        f_lseek(fp, f_tell(fp) + 2);
        break;
    }

    return 0;
}

/**
 *  @brief finds key based on tone parsed
 * @param key meow
 * @param tone meow
 * @param controller pointer to control structure
 * @return 0 on sucess -1 on failure
 * */
uint8_t find_key(int8_t key, uint8_t tone, MIDI_controller *controller)
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
        controller->key_sig = minor_keys[(sizeof(minor_keys) / 2) + key];
        myprintf("Key Signiture: 0x%x\n", controller->key_sig);
    }
    // major
    else if (tone == 0x01)
    {
        controller->key_sig = major_keys[(sizeof(minor_keys) / 2) + key];
        myprintf("Key Signiture: 0x%x\n", controller->key_sig);
    }

    else
    {
        myprintf("invalid tone: 0x%x\n", tone);
        return -1;
    }

    return 0;
}

/**
 *  @brief makes a copy of old FIL pointer and moves the original to the next track
 * @param fp FIL pointer to music
 * @return returns copied FIL pointer
 * */
FIL *get_track(FIL *fp)
{
    FIL *track;
    uint8_t *buf;
    uint32_t trk_hdr; // string header at front of all tracks
    uint32_t trk_len; // length of track data

    buf = (uint8_t *)malloc(sizeof(uint8_t) * 8);
    if (!buf)
    {
        myprintf("Buffer in get_track is null\n");
    }

    f_read(fp, buf, 8, NULL);

    trk_hdr = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if (trk_hdr != 0x4D54726B)
    { // verifies the track start is valid
        myprintf("Invalid track header recieved: %x\n", trk_hdr);
    }

    trk_len = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]; //  track length

    free(buf);

    f_open(track, "tempFIL.txt", FA_WRITE);

    buf = malloc(sizeof(uint8_t) * trk_len);
    if (!buf)
    {
        myprintf("Buffer in get_track is null");
    }


    f_read(fp, buf, trk_len, NULL);

    f_write(fp, track, trk_len, NULL);

    return track;
}

/**
 *  @brief plays one track dealing with midi, meta, and sysex events sucessfullyt
 * @param fp FIL pointer to music
 * @param controller pointer to control structure
 * @return returns 0 on sucess other on failure
 * */
uint8_t play_one_track(FIL *fp, MIDI_controller *ctrl)
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
        myprintf("Buffer in parse_midi is null\n");
        return -1;
    }

    f_read(fp,buf,8,NULL);

    trk_hdr = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if (trk_hdr != 0x4D54726B)
    { // verifies the track start is valid
        myprintf("Invalid track header recieved: %x\n", trk_hdr);
        return -1;
    }

    trk_len = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]; //  track length

    free(buf);

    buf = (uint8_t *)malloc(sizeof(uint8_t) * trk_len);
    if (!buf)
    {
        myprintf("Buffer in parse_midi is null");
        return -1;
    }

    myprintf("before track header\n");

    // for debug
    myprintf("Track header: %x\n", trk_hdr);
    myprintf("Track length: %x\n", trk_len);

    do
    {

        delta_time = decode_vlq(fp);

        f_read(fp, &event_type, 1, NULL);

        myprintf("event type %d\n", event_type);

        if (event_type == 0xFF)
        {
            end = meta_event_handler(fp, delta_time, ctrl);
        }
        else if ((event_type >= 0xF0) && (event_type != 0xFF))
        {
            myprintf("sysex event called: 0x%x\n", event_type);
            // f_read(fp, &byte, 1, NULL);;
            // skip_meta_event(fp);
            // sysex_event_handler(fp, delta_time);
        }
        else
        {
            midi_event_handler(fp, delta_time, event_type, ctrl);
        }

    } while (end != 1);

    free(buf);

    return 0;
}

/**
 * @brief removes important track data from header chunk and moves fp forward to first track
 * @param fp FIL pointer to music
 * @param hdr pointer to header structure
 * @return returns 0 on sucess other on failure
 * */
MIDI_header_chunk parse_midi_header(FIL *fp, MIDI_header_chunk hdr)
{

    uint8_t *buf;

    buf = (uint8_t *)malloc(14);
    if (!buf)
    {
        myprintf("Buffer in parse_midi is null");
    }

    f_read(fp, buf, 14, NULL);

    // all big endian
    hdr.MThd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if (hdr.MThd != 0x4D546864)
    {                                                           // string of MThd
        myprintf("Unexpected Header - Recieved: %c\n", hdr.MThd); // return characters for easy read
    }

    hdr.hdr_len = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
    if (hdr.hdr_len != 0x0006)
    { // always 6 bytes
        myprintf("Unexpected Header Length - Recieved: %u\n", hdr.hdr_len);
    }

    hdr.format = (buf[8] << 8) | buf[9];
    if (hdr.format != 0x00 && hdr.format != 0x01 && hdr.format != 0x02)
    {
        myprintf("Unexpected Header Format - Recieved: %u\n", hdr.format);
    }

    hdr.num_tracks = (buf[10] << 8) | buf[11];
    hdr.division = (buf[12] << 8) | buf[13];

    // to check all header info
    myprintf("Header string is: %x\n", hdr.MThd);
    myprintf("Length is: %x\n", hdr.hdr_len);
    myprintf("Format is:%x \n", hdr.format);
    myprintf("Number of tracks is: %x\n", hdr.num_tracks);
    myprintf("Division is: %x\n", hdr.division);

    free(buf);

    return hdr;
}
