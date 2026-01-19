# ASI Camera Python Wrapper

This directory contains files to create a Python wrapper for the ASI Camera C++ functions with full GUI integration support.

## Files Created

### C++ Wrapper Files
- **asi_wrapper.h** - Header file declaring the DLL-exported functions
- **asi_wrapper.cpp** - Implementation of the wrapper functions that expose C++ functions via C ABI
- **asi_lib.cpp** - Library version of asi_view.cpp (without main function) that can be compiled into a DLL

### Build Scripts
- **build_asi_wrapper.bat** - Windows batch script to compile the wrapper as a DLL

### Python Scripts
- **test_asi_wrapper.py** - Python wrapper module with ASICameraWrapper class using ctypes

### GUI Integration
- **GUI/camera_controls.py** - Camera control panel with initialization, mode selection, and parameter adjustment
- **GUI/video_panel.py** - Video display panel for live streaming and snapshot preview
- **GUI/main.py** - Main application window with integrated camera and DMD controls

## How to Build (Windows)

1. Make sure you have MinGW-w64 with g++ installed
2. Run the build script:
   ```
   build_asi_wrapper.bat
   ```
3. This will create `bin/asi_wrapper.dll` along with required dependencies

## How to Use from Python

### Standalone Usage

```python
from test_asi_wrapper import ASICameraWrapper

# Create wrapper instance
camera = ASICameraWrapper()

# Initialize camera with desired ROI
if camera.init_camera(roi_width=640, roi_height=480):
    # Video mode
    camera.start_video_mode()
    frame = camera.get_frame(wait_ms=1000)
    camera.stop_video_mode()
    
    # Or snapshot mode
    frame = camera.capture_snapshot(timeout_ms=5000)
    
    # Adjust camera settings
    camera.set_exposure(50000, auto=False)  # 50ms exposure
    camera.set_gain(100, auto=False)
    
    # Stop camera when done
    camera.stop_camera()
```

### GUI Usage

Run the integrated GUI:
```
cd GUI
python main.py
```

The GUI provides:
- Camera initialization button
- Video/Snapshot mode selection
- Live video streaming (30 FPS)
- Snapshot capture with save dialog
- Exposure and gain adjustment sliders
- Real-time video display panel

## Architecture

The wrapper follows this architecture:

1. **ASI SDK** (ASICamera2.dll) - ZWO's native camera SDK
2. **C++ Library** (asi_lib.cpp) - Camera control functions
3. **C Wrapper** (asi_wrapper.cpp/h) - Exports functions with C ABI for DLL compatibility
4. **Python Wrapper** (test_asi_wrapper.py) - Uses ctypes to call DLL functions
5. **GUI** (GUI/*.py) - Tkinter-based user interface with live video display

## Exported Functions

### asi_init_camera
Initializes the camera and sets ROI (Region of Interest).

**Parameters:**
- `cameraID`: Pointer to int (output)
- `roiWidth`: Pointer to int (input/output)
- `roiHeight`: Pointer to int (input/output)
- `roiBin`: Pointer to int (input)
- `imgType`: Pointer to int (input)

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

### asi_start_video_capture / asi_stop_video_capture
Start or stop video capture mode.

**Parameters:**
- `cameraID`: Camera ID

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

### asi_get_frame
Gets a frame from the camera in video mode.

**Parameters:**
- `cameraID`: Camera ID
- `buffer`: Pointer to buffer for frame data
- `bufferSize`: Size of buffer in bytes
- `waitMs`: Timeout in milliseconds

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

### asi_start_exposure
Start exposure for snapshot mode.

**Parameters:**
- `cameraID`: Camera ID
- `isDark`: 1 for dark frame, 0 for normal frame

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

### asi_get_exp_status
Get exposure status (for snapshot mode).

**Parameters:**
- `cameraID`: Camera ID
- `status`: Pointer to store status (ASI_EXP_IDLE, ASI_EXP_WORKING, ASI_EXP_SUCCESS, ASI_EXP_FAILED)

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

### asi_get_data_after_exp
Get image data after exposure completes (snapshot mode).

**Parameters:**
- `cameraID`: Camera ID
- `buffer`: Pointer to buffer for frame data
- `bufferSize`: Size of buffer in bytes

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

### asi_set_exposure / asi_set_gain
Set camera exposure or gain values.

**Parameters:**
- `cameraID`: Camera ID
- `value`: Exposure value (microseconds) or gain value
- `isAuto`: 1 for auto mode, 0 for manual

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

### asi_stop_camera
Stops video capture and closes the camera.

**Parameters:**
- `cameraID`: Camera ID

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

## GUI Features

### Camera Controls Panel
- **Initialize Camera**: Button to initialize camera with specified ROI
- **Mode Selection**: Radio buttons to switch between Video and Snapshot modes
- **ROI Settings**: Width and height input fields
- **Position Settings**: X and Y position controls
- **Exposure Control**: Slider for real-time exposure adjustment
- **Gain Control**: Slider for real-time gain adjustment
- **Save Snapshot**: Button to capture and save snapshot (Snapshot mode only)

### Video Panel
- **Live Display**: Shows live video feed or captured snapshot
- **Auto-Scaling**: Automatically scales image to fit panel while maintaining aspect ratio
- **Real-Time Update**: Updates at ~30 FPS in video mode

## Error Codes

- `0` (ASI_SUCCESS) - Operation successful
- `-1` - Invalid parameters
- Other codes - See ASICamera2.h for ASI_ERROR_CODE definitions

## Testing

Before full integration:
1. Build the DLL with `build_asi_wrapper.bat`
2. Run the GUI: `python GUI/main.py`
3. Click "Initialize Camera" button
4. Select Video mode to see live stream
5. Select Snapshot mode to capture single frames
6. Adjust exposure and gain with sliders

## Integration Status

✅ Video mode fully integrated into GUI
✅ Snapshot mode fully integrated into GUI  
✅ Live video streaming at 30 FPS
✅ Real-time parameter adjustment
✅ Snapshot capture with save dialog
✅ Proper cleanup on application close

## Notes

- The wrapper is designed for Windows (uses `__declspec(dllexport)`)
- Requires ASICamera2.dll and OpenCV DLLs in the same directory or on PATH
- Video mode requires `ASIStartVideoCapture` to be called before getting frames
- Snapshot mode automatically stops video mode when activated
- Exposure and gain values are mapped from slider ranges to camera-appropriate values
- For DMD wrapper, follow the same approach in separate dmd_wrapper.cpp/h

## Future Enhancements

To add more functionality to the wrapper:

1. Add function declaration in `asi_wrapper.h` with `extern "C"` and `ASI_WRAPPER_API`
2. Implement wrapper function in `asi_wrapper.cpp` that calls the corresponding ASI SDK function
3. Update `test_asi_wrapper.py` to define the function signature and add a Python method
4. Update GUI controls as needed in `camera_controls.py`
5. Rebuild the DLL with `build_asi_wrapper.bat`

## Troubleshooting

- **DLL not found**: Make sure `bin/asi_wrapper.dll` exists and all dependencies are in `bin/`
- **Camera not detected**: Check USB connection and ensure camera drivers are installed
- **Access denied**: Make sure no other application is using the camera
- **Video not displaying**: Ensure camera is initialized and video mode is selected
- **Slow frame rate**: Check exposure time - higher exposure reduces frame rate
