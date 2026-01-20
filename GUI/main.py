
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
window.title("GUI Test")
window.geometry("800x400")
window.resizable(True, True)

camera = ASICamera()
dmd = DMD()

# Configure grid layout
window.rowconfigure(0, weight=0, minsize=100)
window.rowconfigure(1, weight=0, minsize=80)
window.rowconfigure(2, weight=1, minsize=150)
window.columnconfigure(0, weight=0, minsize=400) 
window.columnconfigure(1, weight=3)

# Status panel
status_panel = StatusPanel(window)
status_panel.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
status_panel.grid_propagate(False)
status_panel.config(width=400)

# Control panel for switching between Camera and DMD controls
control_panel = tk.Frame(window)
control_panel.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
control_panel.grid_propagate(False)
control_panel.config(width=400)

# Video feed area
video_panel = VideoPanel(window)
video_panel.grid(row=0, column=1, rowspan=3, sticky="nsew")
video_panel.set_camera(camera)


def auto_connect_camera():
    """Auto-connect camera after window is ready."""
    camera_controls.auto_connect(status_panel)


def auto_connect_dmd():
    """Auto-connect DMD after window is ready."""
    dmd_controls.auto_connect()


def show_camera_controls():
    camera_controls.grid()
    dmd_controls.grid_remove()

def show_dmd_controls():
    camera_controls.grid_remove()
    dmd_controls.grid()

dmd_btn = tk.Button(control_panel, text="DMD Controls", command=show_dmd_controls, bg="lightgrey", font=("Arial", 10))
dmd_btn.pack(side="left", padx=5, pady=5)

camera_btn = tk.Button(control_panel, text="Camera Controls", command=show_camera_controls, bg="lightgrey", font=("Arial", 10))
camera_btn.pack(side="left", padx=5, pady=5)

# DMD controls
dmd_controls = DMDControls(window, dmd=dmd, status_panel=status_panel)
dmd_controls.grid(row=2, column=0, sticky="nsew", padx=5, pady=5)
dmd_controls.grid_propagate(False)
dmd_controls.config(width=400)

# Camera controls
camera_controls = CameraControls(window, camera, video_panel)
camera_controls.grid(row=2, column=0, sticky="nsew", padx=5, pady=5)
camera_controls.grid_propagate(False)
camera_controls.config(width=400)
camera_controls.grid_remove()  # Hidden by default


# Function to maintain 1:1 aspect ratio for video_panel
def keep_video_aspect(event=None):
    grid_info = window.grid_bbox(1, 0)
    cell_width = grid_info[2]
    cell_height = grid_info[3]
    size = min(cell_width, cell_height)
    video_panel.config(width=size, height=size)


# Bind the resize event to keep the aspect ratio
window.bind('<Configure>', keep_video_aspect)


def toggle_fullscreen(event=None):
    is_fullscreen = window.attributes('-fullscreen')
    window.attributes('-fullscreen', not is_fullscreen)


def on_closing():
    """Cleanup when window is closed."""
    camera_controls.stop_health_check()
    dmd_controls.stop_health_check()
    video_panel.stop_stream()
    if camera.is_connected:
        camera.stop_camera()
    if dmd.connected:
        dmd.disconnect()
    window.destroy()


window.bind('<F11>', toggle_fullscreen)
window.protocol("WM_DELETE_WINDOW", on_closing)

# Auto-connect camera after window is fully loaded
window.after(100, auto_connect_camera)

# Auto-connect DMD after window is fully loaded
window.after(200, auto_connect_dmd)

window.mainloop()