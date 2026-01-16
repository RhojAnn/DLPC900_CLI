# ASI Camera Python Wrapper

This directory contains files to create a Python wrapper for the ASI Camera C++ functions.

## Files Created

### C++ Wrapper Files
- **asi_wrapper.h** - Header file declaring the DLL-exported functions
- **asi_wrapper.cpp** - Implementation of the wrapper functions that expose C++ functions via C ABI
- **asi_lib.cpp** - Library version of asi_view.cpp (without main function) that can be compiled into a DLL

### Build Scripts
- **build_asi_wrapper.bat** - Windows batch script to compile the wrapper as a DLL

### Python Test Scripts
- **test_asi_wrapper.py** - Python script demonstrating how to use the DLL with ctypes

## How to Build (Windows)

1. Make sure you have MinGW-w64 with g++ installed
2. Run the build script:
   ```
   build_asi_wrapper.bat
   ```
3. This will create `bin/asi_wrapper.dll` along with required dependencies

## How to Use from Python

```python
from test_asi_wrapper import ASICameraWrapper

# Create wrapper instance
camera = ASICameraWrapper()

# Initialize camera with desired ROI
if camera.init_camera(roi_width=640, roi_height=480):
    # Get a frame
    frame = camera.get_frame(wait_ms=1000)
    
    if frame is not None:
        print(f"Frame shape: {frame.shape}")
        # Process the frame...
    
    # Stop camera when done
    camera.stop_camera()
```

## Architecture

The wrapper follows this architecture:

1. **ASI SDK** (ASICamera2.dll) - ZWO's native camera SDK
2. **C++ Library** (asi_lib.cpp) - Camera control functions
3. **C Wrapper** (asi_wrapper.cpp/h) - Exports functions with C ABI for DLL compatibility
4. **Python Wrapper** (test_asi_wrapper.py) - Uses ctypes to call DLL functions

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

### asi_get_frame
Gets a frame from the camera.

**Parameters:**
- `cameraID`: Camera ID
- `buffer`: Pointer to buffer for frame data
- `bufferSize`: Size of buffer in bytes
- `waitMs`: Timeout in milliseconds

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

### asi_stop_camera
Stops video capture and closes the camera.

**Parameters:**
- `cameraID`: Camera ID

**Returns:** 0 (ASI_SUCCESS) on success, error code otherwise

## Error Codes

- `0` (ASI_SUCCESS) - Operation successful
- `-1` - Invalid parameters
- Other codes - See ASICamera2.h for ASI_ERROR_CODE definitions

## Testing

Before full integration:
1. Build the DLL with `build_asi_wrapper.bat`
2. Run the test script: `python test_asi_wrapper.py`
3. Verify camera initialization and frame capture work correctly

## Integration with GUI

The `gui/camera_controls.py` can be updated to use this wrapper:

```python
import ctypes
from test_asi_wrapper import ASICameraWrapper

# In the CameraControls class
self.camera = ASICameraWrapper()

# In the initialization callback
def on_init_camera():
    if self.camera.init_camera(roi_width=640, roi_height=480):
        # Update status panel
        pass
```

## Notes

- The wrapper is designed for Windows (uses `__declspec(dllexport)`)
- Requires ASICamera2.dll and OpenCV DLLs in the same directory or on PATH
- Currently supports basic functions; additional functions can be added following the same pattern
- For DMD wrapper, follow the same approach in a separate dmd_wrapper.cpp/h

## Future Enhancements

To add more functionality to the wrapper:

1. Add function declaration in `asi_wrapper.h` with `extern "C"` and `ASI_WRAPPER_API`
2. Implement wrapper function in `asi_wrapper.cpp` that calls the corresponding function from `asi_lib.cpp`
3. Update `test_asi_wrapper.py` to define the function signature and add a Python method
4. Rebuild the DLL with `build_asi_wrapper.bat`

## Troubleshooting

- **DLL not found**: Make sure `bin/asi_wrapper.dll` exists and all dependencies are in `bin/`
- **Camera not detected**: Check USB connection and ensure camera drivers are installed
- **Access denied**: Make sure no other application is using the camera
