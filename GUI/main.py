import tkinter as tk
from status_panel import StatusPanel
from dmd_controls import DMDControls
from camera_controls import CameraControls
from video_panel import VideoPanel
from asi_wrapper import ASICamera, ImgType
from dmd_wrapper import DMD
import ctypes
import os

window = tk.Tk()
window.title("Microscopy Control Panel")
window.geometry("800x400")
window.resizable(True, True)

# ---------------- Layout ----------------
window.rowconfigure(0, weight=0, minsize=100)
window.rowconfigure(1, weight=0, minsize=100)
window.rowconfigure(2, weight=1, minsize=150)
window.columnconfigure(0, weight=0, minsize=400)
window.columnconfigure(1, weight=3)

# ---------------- UI ----------------
status_panel = StatusPanel(window)
status_panel.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
status_panel.grid_propagate(False)
status_panel.config(width=400)

control_panel = tk.Frame(window)
control_panel.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
control_panel.grid_propagate(False)
control_panel.config(width=400)

video_panel = VideoPanel(window)
video_panel.grid(row=0, column=1, rowspan=3, sticky="nsew")

# ---------------- Globals  ----------------
camera = None
dmd = None
camera_controls = None
dmd_controls = None

# ---------------- Init ----------------
def init_hardware():
    global camera, dmd, camera_controls, dmd_controls

    from asi_wrapper import ASICamera
    from dmd_wrapper import DMD

    camera = ASICamera()
    dmd = DMD()

    status_panel.set_dmd(dmd)
    video_panel.set_camera(camera)

    dmd_controls = DMDControls(window, dmd=dmd, status_panel=status_panel)
    dmd_controls.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
    dmd_controls.grid_propagate(False)
    dmd_controls.config(width=400)

    camera_controls = CameraControls(window, camera, video_panel)
    camera_controls.grid(row=2, column=0, sticky="nsew", padx=5, pady=5)
    camera_controls.grid_propagate(False)
    camera_controls.config(width=400)

    window.after(100, camera_controls.auto_connect, status_panel)
    window.after(200, dmd_controls.auto_connect)

def keep_video_aspect(event=None):
    grid_info = window.grid_bbox(1, 0)
    size = min(grid_info[2], grid_info[3])
    video_panel.config(width=size, height=size)

def enable_resize_bind():
    window.bind('<Configure>', keep_video_aspect)

def on_closing():
    if camera_controls:
        camera_controls.stop_health_check()
    if dmd_controls:
        dmd_controls.stop_health_check()
    video_panel.stop_stream()
    if camera and camera.is_connected:
        camera.stop_camera()
    if dmd and dmd.connected:
        dmd.disconnect()
    window.destroy()

window.protocol("WM_DELETE_WINDOW", on_closing)

window.after(0, window.update_idletasks)
window.after(100, init_hardware)            
window.after(300, enable_resize_bind)

window.mainloop()