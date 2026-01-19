import ctypes
from ctypes import c_int, c_long, c_ubyte, POINTER, byref
import numpy as np
from PIL import Image
from tkinter import filedialog
from pathlib import Path
import os


class ImgType:
    """ASI Image Types"""
    RAW8 = 0
    RGB24 = 1
    RAW16 = 2
    Y8 = 3


class ExpStatus:
    """Exposure Status"""
    IDLE = 0
    WORKING = 1
    SUCCESS = 2
    FAILED = 3


class ASICamera:
    """Python wrapper for ASI Camera DLL"""
    
    def __init__(self, dll_path: str = None):
        """
        Initialize the ASI camera wrapper.
        
        Args:
            dll_path: Path to the compiled asi_api.dll (defaults to bin/asi_api.dll)
        """
        if dll_path is None:
            dll_path = Path(__file__).parent.parent / "bin" / "asi_api.dll"
        else:
            dll_path = Path(dll_path)
        
        # Add the DLL directory to the search path so dependencies are found
        dll_dir = dll_path.parent.resolve()
        os.add_dll_directory(str(dll_dir))
        
        self.dll = ctypes.CDLL(str(dll_path.resolve()))
        self._define_functions()
        
        self.camera_id: int | None = None
        self.roi_width: int = 0
        self.roi_height: int = 0
        self.roi_bin: int = 1
        self.img_type: int = ImgType.RAW8
    
    def _define_functions(self):
        
        # Connection
        self.dll.num_of_camera_connected.argtypes = []
        self.dll.num_of_camera_connected.restype = c_int
        
        self.dll.cam_init_camera.argtypes = [POINTER(c_int), c_int, c_int, c_int, c_int]
        self.dll.cam_init_camera.restype = c_int
        
        self.dll.cam_stop_camera.argtypes = [c_int]
        self.dll.cam_stop_camera.restype = c_int
        
        # Controls
        self.dll.cam_set_pos.argtypes = [c_int, c_int, c_int]
        self.dll.cam_set_pos.restype = c_int
        
        self.dll.cam_get_pos.argtypes = [c_int, POINTER(c_int), POINTER(c_int)]
        self.dll.cam_get_pos.restype = c_int
        
        self.dll.cam_set_ROI.argtypes = [c_int, c_int, c_int, c_int, c_int]
        self.dll.cam_set_ROI.restype = c_int
        
        self.dll.cam_get_ROI.argtypes = [c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
        self.dll.cam_get_ROI.restype = c_int
        
        self.dll.cam_set_exposure.argtypes = [c_int, c_long, c_int]
        self.dll.cam_set_exposure.restype = c_int
        
        self.dll.cam_get_exposure.argtypes = [c_int, POINTER(c_long), POINTER(c_int)]
        self.dll.cam_get_exposure.restype = c_int
        
        self.dll.cam_get_exposure_range.argtypes = [c_int, POINTER(c_long), POINTER(c_long)]
        self.dll.cam_get_exposure_range.restype = c_int
        
        self.dll.cam_set_gain.argtypes = [c_int, c_long, c_int]
        self.dll.cam_set_gain.restype = c_int
        
        self.dll.cam_get_gain.argtypes = [c_int, POINTER(c_long), POINTER(c_int)]
        self.dll.cam_get_gain.restype = c_int
        
        self.dll.cam_get_gain_range.argtypes = [c_int, POINTER(c_long), POINTER(c_long)]
        self.dll.cam_get_gain_range.restype = c_int
        
        self.dll.cam_set_offset.argtypes = [c_int, c_long, c_int]
        self.dll.cam_set_offset.restype = c_int
        
        self.dll.cam_get_offset.argtypes = [c_int, POINTER(c_long), POINTER(c_int)]
        self.dll.cam_get_offset.restype = c_int
        
        # Video Mode
        self.dll.cam_start_video.argtypes = [c_int]
        self.dll.cam_start_video.restype = c_int
        
        self.dll.cam_stop_video.argtypes = [c_int]
        self.dll.cam_stop_video.restype = c_int
        
        self.dll.cam_get_frame.argtypes = [c_int, POINTER(c_ubyte), c_int, c_int]
        self.dll.cam_get_frame.restype = c_int
        
        # Snap Mode
        self.dll.cam_start_exposure.argtypes = [c_int, c_int]
        self.dll.cam_start_exposure.restype = c_int
        
        self.dll.cam_get_exposure_status.argtypes = [c_int, POINTER(c_int)]
        self.dll.cam_get_exposure_status.restype = c_int
        
        self.dll.cam_stop_exposure.argtypes = [c_int]
        self.dll.cam_stop_exposure.restype = c_int
        
        self.dll.cam_get_data_after_exp.argtypes = [c_int, POINTER(c_ubyte), c_int]
        self.dll.cam_get_data_after_exp.restype = c_int
        
        self.dll.cam_snap.argtypes = [c_int, POINTER(c_ubyte), c_int, c_int, c_int]
        self.dll.cam_snap.restype = c_int
    
    # Connection Methods
    
    def get_num_cameras(self) -> int:
        """Get number of connected cameras."""
        return self.dll.num_of_camera_connected()
    
    def init_camera(self, roi_width: int, roi_height: int, roi_bin: int = 1, img_type: int = ImgType.RAW8) -> int:
        """
        Initialize camera with ROI settings.
        
        Args:
            roi_width: Width of ROI in pixels
            roi_height: Height of ROI in pixels
            roi_bin: Binning factor (1, 2, 4, etc.)
            img_type: Image type (ImgType.RAW8, RGB24, RAW16, Y8)
            
        Returns:
            0 on success, negative error code on failure
            -1: Invalid cameraID pointer
            -2: No camera connected
            -3: Failed to get camera property
            -4: Failed to open camera
            -5: Failed to init camera
            -6: Failed to set ROI
            -7: Failed to set position
        """
        camera_id = c_int()
        result = self.dll.cam_init_camera(byref(camera_id), roi_width, roi_height, roi_bin, img_type)
        
        if result == 0:
            self.camera_id = camera_id.value
            self.roi_width = roi_width
            self.roi_height = roi_height
            self.roi_bin = roi_bin
            self.img_type = img_type
            print(f"Camera initialized: ID={self.camera_id}, ROI={roi_width}x{roi_height}")
        else:
            errors = {
                -1: "Invalid cameraID pointer",
                -2: "No camera connected",
                -3: "Failed to get camera property",
                -4: "Failed to open camera",
                -5: "Failed to init camera",
                -6: "Failed to set ROI",
                -7: "Failed to set position"
            }
            print(f"Camera init failed: {errors.get(result, f'Unknown error {result}')}")
        
        return result
    
    def stop_camera(self) -> int:
        """Stop and close camera."""
        if self.camera_id is None:
            return -1
        result = self.dll.cam_stop_camera(self.camera_id)
        if result == 0:
            print("Camera stopped")
            self.camera_id = None
        return result
    
    @property
    def is_connected(self) -> bool:
        """Check if camera is connected."""
        return self.camera_id is not None
    
    # Controls Methods
    
    def set_pos(self, start_x: int, start_y: int) -> int:
        """Set ROI start position."""
        if self.camera_id is None:
            return -1
        return self.dll.cam_set_pos(self.camera_id, start_x, start_y)
    
    def get_pos(self) -> tuple[int, int, int]:
        """
        Get ROI start position.
        
        Returns:
            (result, start_x, start_y)
        """
        if self.camera_id is None:
            return -1, 0, 0
        x, y = c_int(), c_int()
        result = self.dll.cam_get_pos(self.camera_id, byref(x), byref(y))
        return result, x.value, y.value
    
    def set_ROI(self, width: int, height: int, bin_val: int = 1, img_type: int = ImgType.RAW8) -> int:
        """Set ROI format."""
        if self.camera_id is None:
            return -1
        result = self.dll.cam_set_ROI(self.camera_id, width, height, bin_val, img_type)
        if result == 0:
            self.roi_width = width
            self.roi_height = height
            self.roi_bin = bin_val
            self.img_type = img_type
        return result
    
    def get_ROI(self) -> tuple[int, int, int, int, int]:
        """
        Get ROI format.
        
        Returns:
            (result, width, height, bin, img_type)
        """
        if self.camera_id is None:
            return -1, 0, 0, 0, 0
        w, h, b, t = c_int(), c_int(), c_int(), c_int()
        result = self.dll.cam_get_ROI(self.camera_id, byref(w), byref(h), byref(b), byref(t))
        return result, w.value, h.value, b.value, t.value
    
    def set_exposure(self, value_us: int, auto: bool = False) -> int:
        """
        Set exposure in microseconds.
        
        Args:
            value_us: Exposure time in microseconds
            auto: Enable auto exposure (only works in video mode)
        """
        if self.camera_id is None:
            return -1
        return self.dll.cam_set_exposure(self.camera_id, value_us, 1 if auto else 0)
    
    def get_exposure(self) -> tuple[int, int, bool]:
        """
        Get current exposure.
        
        Returns:
            (result, value_us, is_auto)
        """
        if self.camera_id is None:
            return -1, 0, False
        val, auto = c_long(), c_int()
        result = self.dll.cam_get_exposure(self.camera_id, byref(val), byref(auto))
        return result, val.value, bool(auto.value)
    
    def get_exposure_range(self) -> tuple[int, int, int]:
        """
        Get exposure range.
        
        Returns:
            (result, min_value_us, max_value_us)
        """
        if self.camera_id is None:
            return -1, 0, 0
        min_val, max_val = c_long(), c_long()
        result = self.dll.cam_get_exposure_range(self.camera_id, byref(min_val), byref(max_val))
        return result, min_val.value, max_val.value
    
    def set_gain(self, value: int, auto: bool = False) -> int:
        """Set gain value."""
        if self.camera_id is None:
            return -1
        return self.dll.cam_set_gain(self.camera_id, value, 1 if auto else 0)
    
    def get_gain(self) -> tuple[int, int, bool]:
        """
        Get current gain.
        
        Returns:
            (result, value, is_auto)
        """
        if self.camera_id is None:
            return -1, 0, False
        val, auto = c_long(), c_int()
        result = self.dll.cam_get_gain(self.camera_id, byref(val), byref(auto))
        return result, val.value, bool(auto.value)
    
    def get_gain_range(self) -> tuple[int, int, int]:
        """
        Get gain range.
        
        Returns:
            (result, min_value, max_value)
        """
        if self.camera_id is None:
            return -1, 0, 0
        min_val, max_val = c_long(), c_long()
        result = self.dll.cam_get_gain_range(self.camera_id, byref(min_val), byref(max_val))
        return result, min_val.value, max_val.value
    
    def set_offset(self, value: int, auto: bool = False) -> int:
        """Set offset value."""
        if self.camera_id is None:
            return -1
        return self.dll.cam_set_offset(self.camera_id, value, 1 if auto else 0)
    
    def get_offset(self) -> tuple[int, int, bool]:
        """
        Get current offset.
        
        Returns:
            (result, value, is_auto)
        """
        if self.camera_id is None:
            return -1, 0, False
        val, auto = c_long(), c_int()
        result = self.dll.cam_get_offset(self.camera_id, byref(val), byref(auto))
        return result, val.value, bool(auto.value)
    
    # ============== Buffer Helpers ==============
    
    def get_buffer_size(self) -> int:
        """Calculate buffer size based on ROI and image type."""
        pixels = self.roi_width * self.roi_height
        if self.img_type == ImgType.RGB24:
            return pixels * 3
        elif self.img_type == ImgType.RAW16:
            return pixels * 2
        else:  # RAW8, Y8
            return pixels
    
    def _buffer_to_array(self, buffer) -> np.ndarray:
        """Convert buffer to numpy array."""
        if self.img_type == ImgType.RAW16:
            arr = np.frombuffer(buffer, dtype=np.uint16)
            return arr.reshape((self.roi_height, self.roi_width))
        elif self.img_type == ImgType.RGB24:
            arr = np.frombuffer(buffer, dtype=np.uint8)
            return arr.reshape((self.roi_height, self.roi_width, 3))
        else:  # RAW8, Y8
            arr = np.frombuffer(buffer, dtype=np.uint8)
            return arr.reshape((self.roi_height, self.roi_width))
    
    # Video Mode Methods
    
    def start_video(self) -> int:
        """Start video capture mode."""
        if self.camera_id is None:
            return -1
        return self.dll.cam_start_video(self.camera_id)
    
    def stop_video(self) -> int:
        """Stop video capture mode."""
        if self.camera_id is None:
            return -1
        return self.dll.cam_stop_video(self.camera_id)
    
    def get_video_frame(self, wait_ms: int = 500) -> np.ndarray | None:
        """
        Get single frame from video mode.
        
        Args:
            wait_ms: Timeout in milliseconds
            
        Returns:
            numpy array or None on failure
        """
        if self.camera_id is None:
            return None
        
        buffer_size = self.get_buffer_size()
        buffer = (c_ubyte * buffer_size)()
        
        result = self.dll.cam_get_frame(self.camera_id, buffer, buffer_size, wait_ms)
        if result != 0:
            return None
        
        return self._buffer_to_array(buffer)
    
    # Snap Mode Methods
    
    def snap(self, is_dark: bool = False, timeout_ms: int = 30000) -> np.ndarray | None:
        """
        Take a single snap image.
        
        Args:
            is_dark: True for dark frame (closes mechanical shutter if available)
            timeout_ms: Maximum wait time in milliseconds
            
        Returns:
            numpy array or None on failure
        """
        if self.camera_id is None:
            print("Camera not initialized")
            return None
        
        buffer_size = self.get_buffer_size()
        buffer = (c_ubyte * buffer_size)()
        
        result = self.dll.cam_snap(
            self.camera_id,
            buffer,
            buffer_size,
            1 if is_dark else 0,
            timeout_ms
        )
        
        if result != 0:
            errors = {
                -1: "Invalid buffer",
                -2: "Failed to start exposure",
                -3: "Failed to get status",
                -4: "Exposure failed",
                -5: "Timeout",
                -6: "Failed to get data"
            }
            print(f"Snap failed: {errors.get(result, f'Unknown error {result}')}")
            return None
        
        return self._buffer_to_array(buffer)
    
    def start_exposure(self, is_dark: bool = False) -> int:
        """Start a long exposure (manual control)."""
        if self.camera_id is None:
            return -1
        return self.dll.cam_start_exposure(self.camera_id, 1 if is_dark else 0)
    
    def get_exposure_status(self) -> int:
        """
        Get current exposure status.
        
        Returns:
            ExpStatus.IDLE, WORKING, SUCCESS, or FAILED (-1 on error)
        """
        if self.camera_id is None:
            return -1
        status = c_int()
        result = self.dll.cam_get_exposure_status(self.camera_id, byref(status))
        if result != 0:
            return -1
        return status.value
    
    def stop_exposure(self) -> int:
        """Cancel current exposure."""
        if self.camera_id is None:
            return -1
        return self.dll.cam_stop_exposure(self.camera_id)
    
    def get_data_after_exp(self) -> np.ndarray | None:
        """Get image data after successful exposure."""
        if self.camera_id is None:
            return None
        
        buffer_size = self.get_buffer_size()
        buffer = (c_ubyte * buffer_size)()
        
        result = self.dll.cam_get_data_after_exp(self.camera_id, buffer, buffer_size)
        if result != 0:
            return None
        
        return self._buffer_to_array(buffer)
    
    # ============== Save Methods ==============
    
    def save_image(self, image: np.ndarray, filepath: str = None) -> bool:
        """
        Save image to file.
        
        Args:
            image: numpy array from snap/get_video_frame
            filepath: save path (opens dialog if None)
            
        Returns:
            True if saved successfully
        """
        if image is None:
            print("No image to save")
            return False
        
        if filepath is None:
            filepath = filedialog.asksaveasfilename(
                defaultextension=".png",
                filetypes=[
                    ("PNG files", "*.png"),
                    ("TIFF files", "*.tiff"),
                    ("JPEG files", "*.jpg"),
                    ("All files", "*.*")
                ],
                initialfile="snap_image.png"
            )
        
        if not filepath:
            print("Save cancelled")
            return False
        
        try:
            ext = Path(filepath).suffix.lower()
            
            if ext == ".fits":
                return self._save_fits(image, filepath)
            elif ext in [".tiff", ".tif"] and image.dtype == np.uint16:
                return self._save_tiff_16bit(image, filepath)
            else:
                img = Image.fromarray(image)
                img.save(filepath)
                print(f"Saved: {filepath}")
                return True
                
        except Exception as e:
            print(f"Save failed: {e}")
            return False
    
    def _save_tiff_16bit(self, image: np.ndarray, filepath: str) -> bool:
        """Save 16-bit TIFF."""
        try:
            img = Image.fromarray(image, mode='I;16')
            img.save(filepath)
            print(f"Saved 16-bit TIFF: {filepath}")
            return True
        except Exception as e:
            print(f"Failed to save 16-bit TIFF: {e}")
            return False
    
    def snap_and_save(self, filepath: str = None, is_dark: bool = False, timeout_ms: int = 30000) -> bool:
        """
        Snap an image and save it.
        
        Args:
            filepath: save path (opens dialog if None)
            is_dark: True for dark frame
            timeout_ms: Maximum wait time
            
        Returns:
            True if successful
        """
        image = self.snap(is_dark=is_dark, timeout_ms=timeout_ms)
        if image is None:
            return False
        return self.save_image(image, filepath)


# Test
if __name__ == "__main__":
    # Create camera instance (uses bin/asi_api.dll by default)
    camera = ASICamera()
    
    # Check for cameras
    num_cams = camera.get_num_cameras()
    print(f"Found {num_cams} camera(s)")
    
    if num_cams > 0:
        # Initialize with 1920x1080 ROI
        result = camera.init_camera(1920, 1080, roi_bin=1, img_type=ImgType.RAW8)
        
        if camera.is_connected:
            # Get ranges
            _, exp_min, exp_max = camera.get_exposure_range()
            _, gain_min, gain_max = camera.get_gain_range()
            print(f"Exposure range: {exp_min} - {exp_max} us")
            print(f"Gain range: {gain_min} - {gain_max}")
            
            # Set exposure to 100ms (100000 microseconds)
            camera.set_exposure(100000)
            camera.set_gain(0)
            
            # === Snap Mode Example ===
            print("\n--- Snap Mode ---")
            image = camera.snap(timeout_ms=30000)
            if image is not None:
                print(f"Captured: {image.shape}, dtype: {image.dtype}")
                camera.save_image(image, "snap_test.png")
            
            # === Or snap and save in one call ===
            # camera.snap_and_save("my_image.png")
            
            # === Video Mode Example ===
            print("\n--- Video Mode ---")
            camera.start_video()
            for i in range(5):
                frame = camera.get_video_frame(wait_ms=1000)
                if frame is not None:
                    print(f"Frame {i}: {frame.shape}")
            camera.stop_video()
            
            # Stop camera
            camera.stop_camera()