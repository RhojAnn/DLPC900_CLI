"""
dmd_wrapper.py
Python wrapper for DLPC900 DMD control DLL
"""

import ctypes
from ctypes import c_int, c_uint, c_ubyte, c_char_p, POINTER, byref
from pathlib import Path
import os


class PatternMode:
    """DMD Pattern Modes"""
    DISABLE = 0
    SPLASH = 1
    VIDEO = 2
    OTF = 3


class PowerMode:
    """DMD Power Modes"""
    NORMAL = 0
    STANDBY = 1


class DMD:
    """Python wrapper for DLPC900 DMD DLL"""
    
    def __init__(self, dll_path: str = None):
        """
        Initialize the DMD wrapper.
        
        Args:
            dll_path: Path to the compiled dmd_api.dll (defaults to bin/dmd_api.dll)
        """
        if dll_path is None:
            dll_path = Path(__file__).parent.parent / "bin" / "dmd_api.dll"
        else:
            dll_path = Path(dll_path)
        
        # Add the DLL directory to the search path
        dll_dir = dll_path.parent.resolve()
        os.add_dll_directory(str(dll_dir))
        
        self.dll = ctypes.CDLL(str(dll_path.resolve()))
        self._define_functions()
        self._connected = False
    
    def _define_functions(self):
        """Define all DLL function signatures."""
        
        # ============== Connection ==============
        self.dll.dmd_is_connected.argtypes = []
        self.dll.dmd_is_connected.restype = c_int
        
        self.dll.dmd_connect.argtypes = []
        self.dll.dmd_connect.restype = c_int
        
        self.dll.dmd_disconnect.argtypes = []
        self.dll.dmd_disconnect.restype = c_int
        
        # ============== Status ==============
        self.dll.dmd_get_status.argtypes = [
            POINTER(c_ubyte), POINTER(c_ubyte), POINTER(c_ubyte),
            POINTER(c_ubyte), POINTER(c_ubyte)
        ]
        self.dll.dmd_get_status.restype = c_int
        
        self.dll.dmd_get_version.argtypes = [
            POINTER(c_uint), POINTER(c_uint), POINTER(c_uint), POINTER(c_uint)
        ]
        self.dll.dmd_get_version.restype = c_int
        
        self.dll.dmd_get_power_mode.argtypes = [POINTER(c_ubyte)]
        self.dll.dmd_get_power_mode.restype = c_int
        
        self.dll.dmd_set_standby.argtypes = []
        self.dll.dmd_set_standby.restype = c_int
        
        self.dll.dmd_set_normal.argtypes = []
        self.dll.dmd_set_normal.restype = c_int
        
        self.dll.dmd_toggle_idle.argtypes = []
        self.dll.dmd_toggle_idle.restype = c_int
        
        # ============== Pattern Mode ==============
        self.dll.dmd_set_otf_mode.argtypes = []
        self.dll.dmd_set_otf_mode.restype = c_int
        
        self.dll.dmd_set_disable_mode.argtypes = []
        self.dll.dmd_set_disable_mode.restype = c_int
        
        self.dll.dmd_get_pattern_mode.argtypes = [POINTER(c_int)]
        self.dll.dmd_get_pattern_mode.restype = c_int
        
        self.dll.dmd_clear_pattern.argtypes = []
        self.dll.dmd_clear_pattern.restype = c_int
        
        self.dll.dmd_show_tpg.argtypes = []
        self.dll.dmd_show_tpg.restype = c_int
        
        # ============== LED Control ==============
        self.dll.dmd_set_led_enables.argtypes = [c_int, c_int, c_int, c_int]
        self.dll.dmd_set_led_enables.restype = c_int
        
        self.dll.dmd_get_led_enables.argtypes = [
            POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)
        ]
        self.dll.dmd_get_led_enables.restype = c_int
        
        # ============== Image Display ==============
        self.dll.dmd_display_bmp.argtypes = [c_char_p]
        self.dll.dmd_display_bmp.restype = c_int
        
        self.dll.dmd_load_white.argtypes = []
        self.dll.dmd_load_white.restype = c_int
        
        self.dll.dmd_load_black.argtypes = []
        self.dll.dmd_load_black.restype = c_int
        
        self.dll.dmd_load_half.argtypes = []
        self.dll.dmd_load_half.restype = c_int

        self.dll.dmd_software_reset.argtypes = []
        self.dll.dmd_software_reset.restype = c_int
    
    # ============== Connection Methods ==============
    
    def connect(self) -> bool:
        """
        Connect to DMD device.
        
        Returns:
            True if connected successfully
        """
        result = self.dll.dmd_connect()
        if result == 0:
            self._connected = True
            print("DMD connected")
            return True
        print("Failed to connect to DMD")
        return False
    
    def disconnect(self) -> bool:
        """Disconnect from DMD device."""
        result = self.dll.dmd_disconnect()
        self._connected = False
        return result == 0
    
    def is_connected(self) -> bool:
        """Check if DMD is connected (uses internal state, not USB check)."""
        return self._connected
    
    
    @property
    def connected(self) -> bool:
        """Connection status property."""
        return self._connected
    
    # ============== Status Methods ==============
    
    def get_status(self) -> dict | None:
        """
        Get device status.
        
        Returns:
            Dictionary with hw, sys, main, dlpa, dmd status values
        """
        hw, sys, main, dlpa, dmd = c_ubyte(), c_ubyte(), c_ubyte(), c_ubyte(), c_ubyte()
        result = self.dll.dmd_get_status(byref(hw), byref(sys), byref(main), byref(dlpa), byref(dmd))
        if result != 0:
            return None
        return {
            'hardware': hw.value,
            'system': sys.value,
            'main': main.value,
            'dlpa200': dlpa.value,
            'dmd': dmd.value
        }
    
    def get_version(self) -> dict | None:
        """
        Get firmware version.
        
        Returns:
            Dictionary with app, api, sw_config, seq_config versions
        """
        app, api, swconfig, seqconfig = c_uint(), c_uint(), c_uint(), c_uint()
        result = self.dll.dmd_get_version(byref(app), byref(api), byref(swconfig), byref(seqconfig))
        if result != 0:
            return None
        
        def parse_version(v):
            return f"{(v >> 24) & 0xFF}.{(v >> 16) & 0xFF}.{v & 0xFFFF}"
        
        return {
            'app': parse_version(app.value),
            'api': parse_version(api.value),
            'sw_config': parse_version(swconfig.value),
            'seq_config': parse_version(seqconfig.value)
        }
    
    def get_power_mode(self) -> int:
        """
        Get current power mode.
        
        Returns:
            PowerMode.NORMAL (0), PowerMode.STANDBY (1), or -1 on error
        """
        mode = c_ubyte()
        result = self.dll.dmd_get_power_mode(byref(mode))
        if result != 0:
            return -1
        return mode.value
    
    def set_standby(self) -> bool:
        """Set DMD to standby mode."""
        return self.dll.dmd_set_standby() == 0
    
    def set_normal(self) -> bool:
        """Set DMD to normal mode."""
        return self.dll.dmd_set_normal() == 0
    
    def toggle_idle(self) -> bool:
        """Toggle DMD saver (idle) mode."""
        return self.dll.dmd_toggle_idle() == 0
    
    # ============== Pattern Mode Methods ==============
    
    def set_otf_mode(self) -> bool:
        """Switch to On-The-Fly pattern mode."""
        return self.dll.dmd_set_otf_mode() == 0
    
    def set_disable_mode(self) -> bool:
        """Disable pattern mode."""
        return self.dll.dmd_set_disable_mode() == 0
    
    def get_pattern_mode(self) -> int:
        """
        Get current pattern mode.
        
        Returns:
            PatternMode value (0=Disable, 1=Splash, 2=Video, 3=OTF)
        """
        mode = c_int()
        result = self.dll.dmd_get_pattern_mode(byref(mode))
        if result != 0:
            return -1
        return mode.value
    
    def clear_pattern(self) -> bool:
        """Stop current pattern display."""
        return self.dll.dmd_clear_pattern() == 0
    
    def show_test_pattern(self) -> bool:
        """Display test pattern generator (checkerboard)."""
        return self.dll.dmd_show_tpg() == 0
    
    # ============== LED Control ==============
    
    def set_led_enables(self, red: bool = True, green: bool = True, 
                        blue: bool = True, seq_ctrl: bool = True) -> bool:
        """
        Enable/disable LEDs.
        
        Args:
            red: Enable red LED
            green: Enable green LED
            blue: Enable blue LED
            seq_ctrl: Enable sequence control
        """
        return self.dll.dmd_set_led_enables(
            1 if seq_ctrl else 0,
            1 if red else 0,
            1 if green else 0,
            1 if blue else 0
        ) == 0
    
    def get_led_enables(self) -> dict | None:
        """
        Get LED enable status.
        
        Returns:
            Dictionary with seq_ctrl, red, green, blue status
        """
        seq, r, g, b = c_int(), c_int(), c_int(), c_int()
        result = self.dll.dmd_get_led_enables(byref(seq), byref(r), byref(g), byref(b))
        if result != 0:
            return None
        return {
            'seq_ctrl': bool(seq.value),
            'red': bool(r.value),
            'green': bool(g.value),
            'blue': bool(b.value)
        }
    
    # ============== Image Display ==============
    
    def display_bmp(self, filename: str) -> bool:
        """
        Load and display a 1-bit BMP file on the DMD.
        
        Args:
            filename: Path to the 1-bit BMP file
            
        Returns:
            True if successful
        """
        return self.dll.dmd_display_bmp(filename.encode('utf-8')) == 0
    
    def load_white(self) -> bool:
        """Display white pattern on DMD."""
        return self.dll.dmd_load_white() == 0
    
    def load_black(self) -> bool:
        """Display black pattern on DMD."""
        return self.dll.dmd_load_black() == 0
    
    def load_half(self) -> bool:
        """Display half white/half black pattern on DMD."""
        return self.dll.dmd_load_half() == 0

    def software_reset(self) -> bool:
        """Perform a software reset via the DLL."""
        return self.dll.dmd_software_reset() == 0

# ============== Usage Example ==============

if __name__ == "__main__":
    dmd = DMD()
    
    if dmd.connect():
        # Get info
        version = dmd.get_version()
        if version:
            print(f"Firmware: {version}")
        
        status = dmd.get_status()
        if status:
            print(f"Status: {status}")
        
        # Show test pattern
        print("Showing test pattern...")
        dmd.show_test_pattern()
        
        input("Press Enter to show white...")
        dmd.load_white()
        
        input("Press Enter to disconnect...")
        dmd.disconnect()
