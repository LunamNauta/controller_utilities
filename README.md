# Controller Utilities

### Why?
This project was made in order to provide an API to access controller devices on Linux. 

### What Exists Right Now:
Currently, there is only support for wired Xbox controllers. I don't have access to any other controllers 
at this time, and therefore can't test anything else.

### Things to do
- [x] Detection of wired controllers
    - [x] Xbox (Some)
- [ ] Detection of bluetooth controllers
    - [x] Xbox (Some)
- [ ] Detect if a controller is disconnected
    - [ ] Xbox
- [x] Detect all joystick, trigger, button events, etc
    - [x] Xbox (Some)
- [x] Enable/disable automatic polling of controller event file
    - [x] Xbox (All)
- [ ] Lock file descriptor mutex before disable polling on said descriptor
    - [ ] Xbox
- [x] Controller rumble (and modifying the strength of that rumble)
    - [x] Xbox (All)
- [ ] Support for controllers that aren't Xbox one
- [ ] Gernalization that will allow the creation of maps between any controller and the internal ABI

### Build
```
mkdir build
cd build
cmake ../
make
```
