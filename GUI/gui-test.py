
import tkinter as tk
from status_panel import StatusPanel
from dmd_controls import DMDControls
from video_panel import VideoPanel
import ctypes
import os

window = tk.Tk()
window.title("GUI Test")
window.geometry("800x400")
window.resizable(True, True)

# Configure grid layout
window.rowconfigure(0, weight=0)
window.rowconfigure(1, weight=1)
window.columnconfigure(0, weight=1, minsize=200) 
window.columnconfigure(1, weight=3)

# Status panel
status_panel = StatusPanel(window)
status_panel.grid(row=0, column=0, sticky="nw", padx=5, pady=5)


# Bottom left DMD controls
dmd_controls = DMDControls(window)
dmd_controls.grid(row=1, column=0, sticky="sw", padx=5, pady=5)


# Video feed area
video_panel = VideoPanel(window)
video_panel.grid(row=0, column=1, rowspan=2, sticky="nsew")


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


window.bind('<F11>', toggle_fullscreen)


window.mainloop()