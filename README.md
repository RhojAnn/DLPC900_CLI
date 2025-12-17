# DMD DLPC900

## ToDos
- [ ] Test 
- [ ] Make python wrapper for GUI
- [ ] Remove uncessary files to run DMD (Splash, compress but still undetermined)
    - Need to read up the use for these files
- [ ] Check the following function for mirror control w/o LEDs


## Directory Structure
```
DLPC900_CLI/
├── hidapi/             
│   ├── hid.c *
│   └── hidapi.h *
├── lib/
│   ├── API.c *
│   ├── API.h *
│   ├── BMPParser.c *
│   ├── BMPParser.h *
│   ├── common.h *
│   ├── compress.c *
│   ├── compress.h *
│   ├── diagnosticFile.c
│   ├── diagnosticwindow.h *
│   ├── Error.c *
│   ├── Error.h *
│   ├── pattern.c *
│   ├── pattern.h *
│   ├── splash.c
│   ├── splash.h
│   ├── usb.c
│   └── usb.h
├── src/
│   └── main.c
├── build.bat
├── dlpc900_cli.exe
└── README.md
```
**Where `*` means it originated from the DLPC900REF-GUI codebase**

- `hidapi` folder contains USB HID protocal communication
- `lib` folder contains core DLPC900 API Library
- `src` folder contains the main application code

## Important Resources

[DLPC900 Programmer's Guide](https://www.ti.com/lit/ug/dlpu018j/dlpu018j.pdf?ts=1765949573573&ref_url=https%253A%252F%252Fwww.bing.com%252F)
- This document specifies the command and control interface to the DLPC900 controller and defines all applicable 
commands, default settings, and control register bit definitions.