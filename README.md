# Controller Utilities

### Why?
This project was made in order to provide an API to access controller devices on Linux. 

### What Exists Right Now:
Currently, there is only support for wired Xbox controllers. I don't have access to any other controllers 
at this time, and therefore can't test anything else.

### Things to do
Support for more controllers needs to be added. In order to do this, I'd need access to such controllers in order
to see what it's event codes are. The library has been partially reworked to allow insertion of new controllers by
simply setting a map between an enum and the code values.

Something else that should be added is the ability to cause the controller to rumble.
This is a simple operation, and can be done by writing data to a handler file.  
  
A generalization of the entire system would probably also be a good idea. Currently, the library
is set up so that if you wanted to add something like a PlayStation controller, you'd need
an entirely separate namespace. This is pointless since much of the logic is identical. Mostly,
the difference is in magic numbers. This is an easy fix, and mostly just requires moving some structures
around to different files.

One major thing that I haven't addressed is detecting if the controller has been disconnected / the file descriptor is no longer valid. 
Right now, if you disconnect the controller, the library stills tries to read the stream, and never closes the event file.

Another is that when disabling polling for a controller, I need to mutex guard the file descriptor so that it doesn't try to read
from the file after the descriptor is closed but before the function returns

I also need to find a way to detect if a controller is bluetooth or wired. Currently, I can easily detect what console
a controller is from. I need to find both the console and if the controller is wired or not, as both of these things change
the event codes sent by the Linux kernel

The 'detect_controllers' function should also be reworked. When it is called, all controllers should be forgotten,
and then the list should be updated. In order to do this, the polling thread needs to be locked. If a controller
that was previously found is not found, it should remain out of the list, and its descriptor should be closed 
(note, this is not the same thing as detecting a disconnect)

