# Controller Utilities

### Why?
This project was made in order to provide an API to access controller devices on Linux. 

### What Exists Right Now:
Currently, there is only support for wired Xbox controllers. I don't have access to any other controllers 
at this time, and therefore can't test anything else.

### Things to do
Some functions need to be reworked in order to add support for more controller types. 
In particular, "update_from_input_event" has to be rewritten. Each console and style of controller has its
own codes. Having a massive switch statement like that is simply not efficient. A hash map of some kind would be much better.
  
Something else that should be added is the ability to cause the controller to rumble.
This is a simple operation, and can be done by writing data to a handler file.  
  
A generalization of the entire system would probably also be a good idea. Currently, the library
is set up so that if you wanted to add something like a PlayStation controller, you'd need
an entirely separate namespace. This is pointless since much of the logic is identical. Mostly,
the difference is in magic numbers. This is an easy fix, and mostly just requires moving some structures
around to different files.

One major thing that I haven't addressed is detecting if the controller has been disconnected / the file descriptor is no longer valid. 
Right now, if you disconnect the controller, the library stills tries to read the stream, and never closes the event file.
