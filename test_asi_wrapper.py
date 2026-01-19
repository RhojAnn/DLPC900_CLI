"""
Python wrapper for ASI Camera DLL
This module provides a complete wrapper for ASI Camera functions with support for
video streaming and snapshot modes for GUI integration.
"""

import ctypes
import os
import numpy as np
from PIL import Image, ImageTk

# Constants from ASICamera2.h
ASI_SUCCESS = 0
ASI_IMG_Y8 = 0  # 8-bit monochrome

# Exposure status constants
ASI_EXP_IDLE = 0
ASI_EXP_WORKING = 1
ASI_EXP_SUCCESS = 2
ASI_EXP_FAILED = 3

class ASICameraWrapper:
    """Python wrapper for ASI Camera DLL functions"""
    
    def __init__(self, dll_path="bin/asi_wrapper.dll"):
        """
        Initialize the wrapper by loading the DLL
        
        Args:
            dll_path: Path to the asi_wrapper.dll file
        """
        # Load the DLL
        if not os.path.exists(dll_path):
            raise FileNotFoundError(f"DLL not found at {dll_path}")
        
        self.dll = ctypes.CDLL(dll_path)
        
        # Define function signatures
        # int asi_init_camera(int* cameraID, int* roiWidth, int* roiHeight, int* roiBin, int* imgType)
        self.dll.asi_init_camera.argtypes = [
            ctypes.POINTER(ctypes.c_int),  # cameraID
            ctypes.POINTER(ctypes.c_int),  # roiWidth
            ctypes.POINTER(ctypes.c_int),  # roiHeight
            ctypes.POINTER(ctypes.c_int),  # roiBin
            ctypes.POINTER(ctypes.c_int)   # imgType
        ]
        self.dll.asi_init_camera.restype = ctypes.c_int
        
        # int asi_get_frame(int cameraID, unsigned char* buffer, int bufferSize, int waitMs)
        self.dll.asi_get_frame.argtypes = [
            ctypes.c_int,                   # cameraID
            ctypes.POINTER(ctypes.c_ubyte), # buffer
            ctypes.c_int,                   # bufferSize
            ctypes.c_int                    # waitMs
        ]
        self.dll.asi_get_frame.restype = ctypes.c_int
        
        # int asi_stop_camera(int cameraID)
        self.dll.asi_stop_camera.argtypes = [ctypes.c_int]
        self.dll.asi_stop_camera.restype = ctypes.c_int
        
        # int asi_start_video_capture(int cameraID)
        self.dll.asi_start_video_capture.argtypes = [ctypes.c_int]
        self.dll.asi_start_video_capture.restype = ctypes.c_int
        
        # int asi_stop_video_capture(int cameraID)
        self.dll.asi_stop_video_capture.argtypes = [ctypes.c_int]
        self.dll.asi_stop_video_capture.restype = ctypes.c_int
        
        # int asi_start_exposure(int cameraID, int isDark)
        self.dll.asi_start_exposure.argtypes = [ctypes.c_int, ctypes.c_int]
        self.dll.asi_start_exposure.restype = ctypes.c_int
        
        # int asi_get_exp_status(int cameraID, int* status)
        self.dll.asi_get_exp_status.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        self.dll.asi_get_exp_status.restype = ctypes.c_int
        
        # int asi_get_data_after_exp(int cameraID, unsigned char* buffer, int bufferSize)
        self.dll.asi_get_data_after_exp.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(ctypes.c_ubyte),
            ctypes.c_int
        ]
        self.dll.asi_get_data_after_exp.restype = ctypes.c_int
        
        # int asi_set_exposure(int cameraID, long value, int isAuto)
        self.dll.asi_set_exposure.argtypes = [ctypes.c_int, ctypes.c_long, ctypes.c_int]
        self.dll.asi_set_exposure.restype = ctypes.c_int
        
        # int asi_set_gain(int cameraID, long value, int isAuto)
        self.dll.asi_set_gain.argtypes = [ctypes.c_int, ctypes.c_long, ctypes.c_int]
        self.dll.asi_set_gain.restype = ctypes.c_int
        
        self.camera_id = None
        self.roi_width = None
        self.roi_height = None
        self.video_active = False
    
    def init_camera(self, roi_width=640, roi_height=480, roi_bin=1, img_type=ASI_IMG_Y8):
        """
        Initialize the ASI camera
        
        Args:
            roi_width: Width of the region of interest
            roi_height: Height of the region of interest
            roi_bin: Binning value
            img_type: Image type (ASI_IMG_Y8 for 8-bit monochrome)
        
        Returns:
            True if successful, False otherwise
        """
        camera_id = ctypes.c_int(0)
        width = ctypes.c_int(roi_width)
        height = ctypes.c_int(roi_height)
        binning = ctypes.c_int(roi_bin)
        img_type_c = ctypes.c_int(img_type)
        
        result = self.dll.asi_init_camera(
            ctypes.byref(camera_id),
            ctypes.byref(width),
            ctypes.byref(height),
            ctypes.byref(binning),
            ctypes.byref(img_type_c)
        )
        
        if result == ASI_SUCCESS:
            self.camera_id = camera_id.value
            self.roi_width = width.value
            self.roi_height = height.value
            print(f"Camera initialized successfully!")
            print(f"Camera ID: {self.camera_id}")
            print(f"ROI: {self.roi_width}x{self.roi_height}")
            return True
        else:
            print(f"Failed to initialize camera. Error code: {result}")
            return False
    
    def get_frame(self, wait_ms=1000):
        """
        Get a frame from the camera
        
        Args:
            wait_ms: Timeout in milliseconds
        
        Returns:
            numpy array containing the frame data, or None if failed
        """
        if self.camera_id is None:
            print("Camera not initialized!")
            return None
        
        # Create buffer for the frame
        # For Y8 (8-bit monochrome), buffer size is width * height
        # For RGB24, it would be width * height * 3
        # Currently hardcoded for Y8, but can be extended
        buffer_size = self.roi_width * self.roi_height
        buffer = (ctypes.c_ubyte * buffer_size)()
        
        result = self.dll.asi_get_frame(
            self.camera_id,
            buffer,
            buffer_size,
            wait_ms
        )
        
        if result == ASI_SUCCESS:
            # Convert to numpy array
            frame = np.frombuffer(buffer, dtype=np.uint8)
            frame = frame.reshape((self.roi_height, self.roi_width))
            return frame
        else:
            print(f"Failed to get frame. Error code: {result}")
            return None
    
    def stop_camera(self):
        """
        Stop and close the camera
        
        Returns:
            True if successful, False otherwise
        """
        if self.camera_id is None:
            print("Camera not initialized!")
            return False
        
        # Stop video if active
        if self.video_active:
            self.stop_video_mode()
        
        result = self.dll.asi_stop_camera(self.camera_id)
        
        if result == ASI_SUCCESS:
            print("Camera stopped successfully!")
            self.camera_id = None
            return True
        else:
            print(f"Failed to stop camera. Error code: {result}")
            return False
    
    def start_video_mode(self):
        """
        Start video capture mode
        
        Returns:
            True if successful, False otherwise
        """
        if self.camera_id is None:
            print("Camera not initialized!")
            return False
        
        result = self.dll.asi_start_video_capture(self.camera_id)
        
        if result == ASI_SUCCESS:
            print("Video capture started")
            self.video_active = True
            return True
        else:
            print(f"Failed to start video capture. Error code: {result}")
            return False
    
    def stop_video_mode(self):
        """
        Stop video capture mode
        
        Returns:
            True if successful, False otherwise
        """
        if self.camera_id is None:
            print("Camera not initialized!")
            return False
        
        result = self.dll.asi_stop_video_capture(self.camera_id)
        
        if result == ASI_SUCCESS:
            print("Video capture stopped")
            self.video_active = False
            return True
        else:
            print(f"Failed to stop video capture. Error code: {result}")
            return False
    
    def capture_snapshot(self, timeout_ms=5000):
        """
        Capture a single snapshot (exposure mode)
        
        Args:
            timeout_ms: Maximum time to wait for exposure to complete
        
        Returns:
            numpy array containing the frame data, or None if failed
        """
        if self.camera_id is None:
            print("Camera not initialized!")
            return None
        
        # Stop video mode if active
        if self.video_active:
            self.stop_video_mode()
        
        # Start exposure
        result = self.dll.asi_start_exposure(self.camera_id, 0)  # 0 = not dark frame
        if result != ASI_SUCCESS:
            print(f"Failed to start exposure. Error code: {result}")
            return None
        
        # Wait for exposure to complete
        import time
        elapsed = 0
        poll_interval = 10  # ms
        status = ctypes.c_int()
        
        while elapsed < timeout_ms:
            result = self.dll.asi_get_exp_status(self.camera_id, ctypes.byref(status))
            if result != ASI_SUCCESS:
                print(f"Failed to get exposure status. Error code: {result}")
                return None
            
            if status.value == ASI_EXP_SUCCESS:
                # Get the image data
                buffer_size = self.roi_width * self.roi_height
                buffer = (ctypes.c_ubyte * buffer_size)()
                
                result = self.dll.asi_get_data_after_exp(
                    self.camera_id,
                    buffer,
                    buffer_size
                )
                
                if result == ASI_SUCCESS:
                    frame = np.frombuffer(buffer, dtype=np.uint8)
                    frame = frame.reshape((self.roi_height, self.roi_width))
                    return frame
                else:
                    print(f"Failed to get image data. Error code: {result}")
                    return None
            elif status.value == ASI_EXP_FAILED:
                print("Exposure failed")
                return None
            
            time.sleep(poll_interval / 1000.0)
            elapsed += poll_interval
        
        print("Exposure timeout")
        return None
    
    def set_exposure(self, value, auto=False):
        """
        Set exposure value
        
        Args:
            value: Exposure value in microseconds
            auto: Enable auto exposure
        
        Returns:
            True if successful, False otherwise
        """
        if self.camera_id is None:
            print("Camera not initialized!")
            return False
        
        result = self.dll.asi_set_exposure(self.camera_id, value, 1 if auto else 0)
        
        if result == ASI_SUCCESS:
            return True
        else:
            print(f"Failed to set exposure. Error code: {result}")
            return False
    
    def set_gain(self, value, auto=False):
        """
        Set gain value
        
        Args:
            value: Gain value
            auto: Enable auto gain
        
        Returns:
            True if successful, False otherwise
        """
        if self.camera_id is None:
            print("Camera not initialized!")
            return False
        
        result = self.dll.asi_set_gain(self.camera_id, value, 1 if auto else 0)
        
        if result == ASI_SUCCESS:
            return True
        else:
            print(f"Failed to set gain. Error code: {result}")
            return False


def main():
    """Test the ASI Camera wrapper"""
    print("=" * 50)
    print("ASI Camera Wrapper Test")
    print("=" * 50)
    
    # Create wrapper instance
    try:
        camera = ASICameraWrapper()
    except FileNotFoundError as e:
        print(f"Error: {e}")
        print("Please run build_asi_wrapper.bat first to build the DLL")
        return
    
    # Initialize camera
    print("\n1. Initializing camera...")
    if not camera.init_camera(roi_width=640, roi_height=480):
        print("Failed to initialize camera. Make sure camera is connected.")
        return
    
    # Get a test frame
    print("\n2. Getting a frame...")
    frame = camera.get_frame(wait_ms=1000)
    
    if frame is not None:
        print(f"Frame received! Shape: {frame.shape}")
        print(f"Frame data type: {frame.dtype}")
        print(f"Frame min/max values: {frame.min()}/{frame.max()}")
        
        # Optionally save the frame (requires OpenCV)
        try:
            import cv2
            cv2.imwrite("test_frame.png", frame)
            print("Frame saved as test_frame.png")
        except ImportError:
            print("OpenCV not available, skipping frame save")
    else:
        print("Failed to get frame")
    
    # Stop camera
    print("\n3. Stopping camera...")
    camera.stop_camera()
    
    print("\n" + "=" * 50)
    print("Test completed!")
    print("=" * 50)


if __name__ == "__main__":
    main()
