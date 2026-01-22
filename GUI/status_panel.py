import tkinter as tk
from tkinter import messagebox

class StatusPanel(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        import ctypes
        import os
        super().__init__(parent, borderwidth=2, relief="groove", *args, **kwargs)
        self.status_label = tk.Label(self, text="Status", font=("Arial", 10, "bold"))
        self.status_label.pack(anchor="nw", padx=3, pady=3)

        self.dmd_row = tk.Frame(self)
        self.dmd_row.pack(anchor="nw", padx=3, pady=2)
        self.dmd_status = tk.Label(self.dmd_row, text="DMD: Disconnected", fg="red")
        self.dmd_status.pack(side="left")

        self.reset_dmd_callback = None
        self.reset_btn = tk.Button(self.dmd_row, text="Reset DMD", command=self._on_reset_dmd, padx=6, pady=2)
        self.reset_btn.pack(side="left", padx=6)

        self.camera_status = tk.Label(self, text="Camera: Disconnected", fg="red")
        self.camera_status.pack(anchor="nw", padx=3, pady=2)

    def set_dmd(self, dmd):
        """
        Set the DMD instance to use for control.

        Args:
            dmd: DMD instance
        """
        self.dmd = dmd


    def set_camera_status(self, message: str, connected: bool):
        """Update camera status display"""
        if connected:
            self.camera_status.config(text=f"Camera: {message}", fg="green")
        else:
            self.camera_status.config(text=f"Camera: {message}", fg="red")
    
    def set_dmd_status(self, message: str, connected: bool):
        """Update DMD status display"""
        if connected:
            self.dmd_status.config(text=f"DMD: {message}", fg="green")
        else:
            self.dmd_status.config(text=f"DMD: {message}", fg="red")

    def _on_reset_dmd(self):
        """Software reset the DMD"""
        if self.dmd and self.dmd.connected:
            self.dmd.software_reset()
            self.dmd.set_disable_mode()
            self.dmd.set_otf_mode()
            messagebox.showinfo("Info", "DMD has been reset.")
        else:
            messagebox.showinfo("Error", "DMD not connected.")