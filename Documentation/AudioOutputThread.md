# Audio Output Thread
## Thread Diagram 

## How a note is played
The thread would pop a single event off the top of the queue after checking that it has at least one event within. Then the note struct is recieved giving us the event type, timing(ms), and frequency of the note. 

If the event is a note on it checks for a free coil and plays the event accordingly. 
If the event is a note off it checks coils that are busy and searches for one that has the same freuqency as the note which is desired to be turned off.

This is repeated until all events run out and the file is finished.
