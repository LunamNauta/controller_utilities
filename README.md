# Controller Utilities

### Why?
This project was made in order to provide an API to access controller devices on Linux. 

### What Exists Right Now:
Currently, there is only support for wired Xbox controllers. I don't have access to any other controllers 
at this time, and therefore can't test anything else.

### Things to do
=======
- [x] Detection of wired controllers
    - [x] Xbox
    - [ ] Playstation
- [ ] Detection of bluetooth controllers
    - [ ] Xbox
    - [ ] Playstation
- [ ] Detect if a controller is disconnected
    - [ ] Xbox
    - [ ] Playstation
- [x] Detect all joystick, trigger, button events, etc
    - [x] Xbox
    - [ ] Playstation
- [x] Enable/disable automatic polling of controller event file
    - [x] Xbox
    - [ ] Playstation
- [ ] Lock file descriptor mutex before disable polling on said descriptor
    - [ ] Xbox
    - [ ] Playstation
- [x] Controller rumble (and modifying the strength of that rumble)
    - [x] Xbox
    - [ ] Playstation
- [ ] Support for controllers that aren't Xbox one
- [ ] Gernalization that will allow the creation of maps between any controller and the internal ABI
=======
