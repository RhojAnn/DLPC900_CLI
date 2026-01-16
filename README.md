# DMD DLPC900

## ToDos (according to priority)
- [ ] Control exposure and gain in camera
- [x] Turn code into a dll file for python wrapper GUI
- [x] Make python wrapper for GUI (ASI Camera - initial implementation)
- [ ] Make python wrapper for DMD (similar to ASI Camera approach)
- [ ] Ensure that DMD Communication Status isn't an hardware issue
- [ ] Remove uncessary files to run DMD (Splash, compress but still undetermined)
    - Need to read up the use for these files

## Done
- [x] Continue cleaning out code
- [x] Test Communication Status
- [x] Clean up main.c so that cmd functions are seperated according to its category
- [x] Add comments for functions
- [x] Check the following function for mirror control w/o LEDs: `LCR_GetDMDBlocks()`, `LCR_SetDMDBlocks()`
- [x] Make BMP image display into the screen

## How to run
This software is only **Windows x64** system compatible 
<br>*(Could be compatible with Windows x32 systems but have not been tested or developed in this enviroment)*

### DMD CLI Tool
1. Open this directory in a CLI *(Note: the CLI may print out errors while compiling. However, most are formatting warnings and the program will run as intended)*
2. Execute build.bat `./build.bat`

### ASI Camera Viewer
1. Execute asi_build.bat `./asi_build.bat`
2. The camera viewer window will open

### Python Wrapper for ASI Camera
See [ASI_WRAPPER_README.md](ASI_WRAPPER_README.md) for details on using the ASI Camera from Python.

1. Build the DLL: `./build_asi_wrapper.bat`
2. Test the wrapper: `python test_asi_wrapper.py`
3. Use in your Python code:
   ```python
   from test_asi_wrapper import ASICameraWrapper
   camera = ASICameraWrapper()
   camera.init_camera(roi_width=640, roi_height=480)
   frame = camera.get_frame()
   camera.stop_camera()
   ```

## Directory Structure
```
DLPC900_CLI/
├── asi/                     # ASI Camera integration
│   ├── ASICamera2.h *
│   ├── ASICamera2.lib *
│   ├── asi_view.cpp         # Camera viewer standalone app
│   ├── asi_lib.cpp          # Camera library (for DLL)
│   ├── asi_wrapper.cpp      # Python wrapper implementation
│   └── asi_wrapper.h        # Python wrapper header
├── GUI/                     # Python GUI (work in progress)
│   ├── main.py
│   ├── camera_controls.py
│   ├── dmd_controls.py
│   ├── status_panel.py
│   └── video_panel.py
├── hidapi/             
│   ├── hid.c *
│   └── hidapi.h *
├── LCR5000YX_Images/ *
│   ├── bmp images *    
├── lib/
│   ├── asi/                 # ASI Camera DLLs
│   │   ├── ASICamera2.dll
│   │   └── OpenCV DLLs
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
│   └── cmd_connection.c
│   └── cmd_image.c
│   └── cmd_pattern.c
│   └── cmd_status.c
│   └── main.c
├── build.bat                # Build DMD CLI
├── asi_build.bat            # Build ASI Camera viewer
├── build_asi_wrapper.bat    # Build Python wrapper DLL
├── test_asi_wrapper.py      # Python wrapper test script
├── ASI_WRAPPER_README.md    # Python wrapper documentation
├── dlpc900_cli.exe
├── diagnostic.log
└── README.md
```
**Where `*` means it originated from the DLPC900REF-GUI or ZWO ASI SDK codebase**

- `asi` folder contains ASI Camera integration code
- `GUI` folder contains Python GUI application
- `hidapi` folder contains USB HID protocal communication
- `LCR500YX_Images` folder contains sample images from the DLPC900 Library
- `lib` folder contains core DLPC900 API Library
- `src` folder contains the main application code

## Important Resources

[DLPC900 Programmer's Guide](https://www.ti.com/lit/ug/dlpu018j/dlpu018j.pdf?ts=1765949573573&ref_url=https%253A%252F%252Fwww.bing.com%252F)
- This document specifies the command and control interface to the DLPC900 controller and defines all **applicable 
commands**, default settings, and control **register bit definitions**.

## Notes
- I'm pretty sure you can turn off the LEDs in all times using the `LCR_SetEnables(0, 0, 0, 0);` but have to check
    - Through this way, the mirrors could be manipulated via BMP images w/o LEDs turning on
- Will be using BMP to control mirrors, now figure out how to turn the BMP into mirrors ON/OF controls
- DMD Mirror Sketch
<img src="mirror_sketch.jpeg" width="500">