"""
Python wrapper test for ASI Camera DLL
This script demonstrates how to use the asi_wrapper.dll with ctypes
"""

import ctypes
import os
import numpy as np

# Constants from ASICamera2.h
ASI_SUCCESS = 0
ASI_IMG_Y8 = 0  # 8-bit monochrome

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
        
        self.camera_id = None
        self.roi_width = None
        self.roi_height = None
    
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
        
        result = self.dll.asi_stop_camera(self.camera_id)
        
        if result == ASI_SUCCESS:
            print("Camera stopped successfully!")
            self.camera_id = None
            return True
        else:
            print(f"Failed to stop camera. Error code: {result}")
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
