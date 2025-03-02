# SingingTeslaCoil

## Abstract 

A [bluetooth controlled tesla coil](https://www.amazon.com/Joytech-Bluetooth-Loudspeaker-Transmission-Experiment/dp/B08KDG6CFK/ref=sr_1_1?dib=eyJ2IjoiMSJ9.jcJKfVIxfNRr2NYpISjmYrbMUUJ3lBEZphMIlKq8sg6zjZEekDJb9W24SPRy5rwD8pWmO0Qw2kqsUxoXW3BbQ604sA6yZjjdyAPkIhQYs-folgYbeQcCFXR0aduB3LFU8Lyu0xs-FqMQB9cuBs8yC1oDPR6SXmdb0cqRl93jRVKuO8ZMPxTWok0gZpAM6pvj0Gbqf_QJNVKl38TiKO_3N8x9LfDNSLLnbwStXQUxuzE.3JayxM7fZaikcn5mz6IQdINlx1W8vY8ROc_Jz39axT8&dib_tag=se&keywords=singing%2Btesla%2Bcoil&qid=1729105038&sr=8-1) was purchased off of amazon and reverse engineered with a tedious plan id-ing chips and understanding how the system functions overall a more detailed explaination can be found [here]() but the takeaway is that based on how the device is set up we can trigger a comparator to generate the control signal to the necessary MOSFETS to charge and discharge the coil to play notes and songs.


This system would require to be broken down the parts as it requires concurrent playing of different notes to create chords and play more complex music. This would mean that there are hardware requirements so that the quality and complexity of sucessful music playing would heavily depend on the number of coils availible as they would only be able to play a single note at a time. Up to 13 coils are supported however this is a bit excessive and most songs can play with just a few.

## High Level System Diagram

![System Diagram](images/System_Diagram_Tesla_Coil_Array.jpeg)

Note: Current implimentation supports up to 16 timers and coils but could vary based on STM SOC used. Specifics on thread implimentation can be found in the links below:

[MidiParseThread](Documentation/MidiParseThread.md)

[AudioOutputThread](Documentation/AudioOutputThread.md)


## [MIDI Header](https://ccrma.stanford.edu/~craig/14q/midifile/MidiFileFormat.html)
The first step of this process would be to begin by removing all necessary info from the MIDI file header. More detailed information about the header can be found in the hyperlink on the title. For this system specifically however, the most important pieces of info would be the format and number of tracks as well as the division (units per beat).

The formats are as follows:

Format 0 -> A single track

Format 1 -> Multiple tracks format (to be played concurrently)

Format 2 -> Multiple song format

## System Diagram 

