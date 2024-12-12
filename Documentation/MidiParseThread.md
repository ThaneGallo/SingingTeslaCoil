#Midi Parse Thread

## Synopsis
The song which is desired to play would be added to an sd card and using the FatFs peripheral would be converted into a file pointer. Then the file is parsed to verify it is a MIDI file and begins to step through the events within. These events being Meta events, Midi events, and Sysex events and will be described in more detail in a later section. As these events progress notes are added to a FreeRTOS queue with the frequency and duration of the notes so that a secondard thread which would be found [here](link) would propagate the signal and play the necessary note.


## System Diagram 
[System Diagram](images/../System_Diagram_Parse_Midi.jpeg)
