import tkinter as tk

class StatusPanel(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        import ctypes
        import os
        super().__init__(parent, borderwidth=2, relief="groove", *args, **kwargs)
        self.status_label = tk.Label(self, text="Status", font=("Arial", 12, "bold"))
        self.status_label.pack(anchor="nw", padx=5, pady=5)
        self.dmd_status = tk.Label(self, text="DMD: Disconnected", fg="red")
        self.dmd_status.pack(anchor="nw", padx=5, pady=2)
        self.camera_status = tk.Label(self, text="Camera: Disconnected", fg="red")
        self.camera_status.pack(anchor="nw", padx=5, pady=2)
    
    def set_camera_status(self, message: str, connected: bool):
        """Update camera status display."""
        if connected:
            self.camera_status.config(text=f"Camera: {message}", fg="green")
        else:
            self.camera_status.config(text=f"Camera: {message}", fg="red")
    
    def set_dmd_status(self, message: str, connected: bool):
        """Update DMD status display."""
        if connected:
            self.dmd_status.config(text=f"DMD: {message}", fg="green")
        else:
            self.dmd_status.config(text=f"DMD: {message}", fg="red")

'''
        # DLL loading and connection checking logic
        self.DLL_PATH = os.path.abspath("./your_dmd_status.dll")  # Change to your DLL name
        self.dmd_lib = None
        if os.path.exists(self.DLL_PATH):
            self.dmd_lib = ctypes.CDLL(self.DLL_PATH)
            self.dmd_lib.is_dmd_connected.restype = ctypes.c_int

        self.update_dmd_status()

    def update_dmd_status(self):
        if self.dmd_lib is not None:
            try:
                connected = bool(self.dmd_lib.is_dmd_connected())
                if connected:
                    self.dmd_status.config(text="DMD: Connected", fg="green")
                else:
                    self.dmd_status.config(text="DMD: Disconnected", fg="red")
            except Exception:
                self.dmd_status.config(text="DMD: Error", fg="orange")
        else:
            self.dmd_status.config(text="DMD: DLL not found", fg="orange")

'''