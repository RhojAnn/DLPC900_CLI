# Microscopy Control — DMD + Camera

This repository implements a microscopy control application that uses a TI DLPC900-driven DMD together with an ASI camera. The DMD patterns steer illumination between two optical paths: white pixels map to the "science" side, and black pixels map to the "acquisition" side. The GUI generates BMP patterns, displays them on the DMD, and controls camera capture and device health.

### Highlights (recent updates):
- Threaded camera capture with a single-frame queue to avoid UI lag
- Debounced resize, exposure and gain controls (non-blocking hardware calls)
- Background BMP generation and safe UI scheduling for DMD display

### Known issues & planned improvements
- Improve DMD status reporting: device connection/state is sometimes out of sync with the UI (investigate status propagation and health-check behavior)
- Fix Standby wake behavior: the DMD can fail to display patterns after long Standby periods (investigate wake/park timing and recovery The issue may also be caused by pressing other DMD buttons while Standby is active.)
- Reduce UI latency: further offload blocking operations and optimize the video/frame pipeline to make the GUI more responsive.
- Simplify pattern generation: refactor the pattern API to avoid multiple small function calls (consider batching or template-based generation)
- Interactive selection: add a mode to select a DMD cell directly from the live video feed (mouse click -> generate & display corresponding pattern)

### Specfication
**Supported platform**
- Windows x64 (development and testing performed on only Windows). Python 3.10+ recommended. MingW64 compiler.

**Requirement:**
- Pillow
- numpy

**Running the GUI**
1. Start from the repository root.
2. Run the GUI:
```bash
python GUI/main.py
```

## Architecture
- Backend: native DMD and ASI camera sources are stored in `lib/` and `src/`. These backend scripts are written in C and C++ and are compiled into native DLLs that the Python wrappers consume via `ctypes`.
- Frontend: the Python side (under `GUI/`) contains wrappers (`GUI/asi_wrapper.py` and `GUI/dmd_wrapper.py`) and the Tk GUI/BMP generator which use those wrappers to control hardware and display patterns.
- Notes: ASI (ZWO) camera support requires the ZWO ASI driver/SDK on Windows; ensure the ASI driver is installed and the camera is accessible before running the GUI.

Directory layout (high level)
```
DLPC900_CLI/
├── GUI/                # Python/Tk GUI and wrappers
├── hidapi/             # HID USB support for DLPC900
├── lib/                # DLPC900 and ASICAmera native C library sources
├── src/                # Example command-line utilities and native app
├── row_pattern/        # Generated BMP patterns (runtime)
├── README.md
```

## Resources
- DLPC900 Programmer's Guide: https://www.ti.com/lit/ug/dlpu018j/dlpu018j.pdf
- ASICamera 2 SDK Guide: [ASICamera2 Software Development Kit](ASICamera2%20Software%20Development%20Kit.pdf)

## How to use

<img src="demo\demo1.png" width="800">

The GUI layout
- Left: Control panel (status, DMD controls, camera controls)
- Right: Live video feed from the ASI camera (or single-frame snapshot preview)

## Control Panel
The control panel is divided into three sections: **Status Panel**, **DMD Controls**, and **Camera Controls**.

### Status Panel
- Shows device connection state for the DMD and ASI camera.
- Green = Connected, Red/Grey = Not connected or error.
- The panel surface displays quick status text and small indicators so you can tell at-a-glance which devices are available.
- **Reset DMD**: attempts a software reset/cleanup of the DMD (use when the device is connected but patterns fail to appear).
- Note: the DMD Status is currently bugged (23/01/2026)

### DMD Controls Panel
- Power mode: radio buttons let you choose `Normal` or `Standby` power modes for the DMD. 
    - `Normal` mode: lets you to display patterns and use the DMD. 
    - `Standby` mode: used to keep the DMD's longevity. It is used when the DMD won't be used for a long period of time. **Please let the DMD be on Standby mode before powering off.**
        - Note: Standby has a grace period of 120 seconds before being fully into Standby mode (The mirrors would be fully parked once 120 seconds pass)
- Pattern entry: enter `Grid`, `Row`, and `Col` values and press **Display Pattern**. The BMP is generated in a background thread and the button is disabled until the operation completes.
- **Stop Pattern**: sends a clear-pattern command to the DMD.

### Camera Controls Panel
- Mode: switch between `Video` (live capture) and `Snapshot` (single-frame capture).
    - `Video`: captures multiple frames in 60 FPS.
    - `Snapshot`: capture a single frame and save it as an image file.**Save Snapshot** button appears at the bottom once snapshot mode is selected. Once clicked, it will prompt you to save the image with the image format and file location. 
- ROI & Position: set region-of-interest and X/Y offsets used for capture. Below the text boxes shows the minimum and maximum valid values. 
    - Clicking on **Apply ROI** and **Apply Position** confirms selection.
- Exposure & Gain: sliders and text inputs control camera exposure and gain. Pressing the enter key for text inputs confirms selection for both the slider and text inputs. Below the sliders shows the minimum and maximum valid values. 

## Camera Feed
The camera feed displays the most recent frame from the ASI camera.

- Video mode: continuous live preview updated from a background capture thread using a single-frame queue (shows the newest frame, avoids backlog). Frame rate depends on camera hardware and exposure settings.
- Snapshot mode: shows a single captured frame; use the **Save Snapshot** button to save the image (file dialog will prompt for location and format).
- The feed respects the ROI and position settings and preserves aspect ratio when resized.
- Connection and error state are shown in the Status Panel; when the camera disconnects the feed pauses and the UI will attempt reconnects per the health-check logic.

Troubleshooting: verify the ASI driver/SDK is installed, confirm the camera appears in the Status Panel, and check exposure/gain values if frames are dark or noisy.
