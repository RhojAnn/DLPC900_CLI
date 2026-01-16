import tkinter as tk
from tkinter import messagebox
from tkinter import filedialog

class CameraControls(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        super().__init__(parent, borderwidth=2, relief="groove", *args, **kwargs)
        self.label = tk.Label(self, text="Camera Controls", font=("Arial", 12, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=5)

        self.create_camera_mode_section()
        self.create_roi_section()
        self.create_position_section()
        self.create_exposure_section()
        self.create_gain_section()
        self.create_snapshot_save_section()

    def create_camera_mode_section(self):
        self.label = tk.Label(self, text="Camera Mode", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=5)

        # Radio buttons for power mode selection
        self.mode_var = tk.StringVar(value="Video")
        radio1 = tk.Radiobutton(self, text="Video", variable=self.mode_var, value="Video", command=self.on_mode_select)
        radio2 = tk.Radiobutton(self, text="Snapshot", variable=self.mode_var, value="Snapshot", command=self.on_mode_select)
        radio1.pack(anchor="sw", padx=5, pady=2)
        radio2.pack(anchor="sw", padx=5, pady=2)

    def create_roi_section(self):
        self.label = tk.Label(self, text="ROI Section", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=(15, 5))

        # ROI Width and Height
        roi_frame = tk.Frame(self)
        roi_frame.pack(anchor="w", padx=5, pady=5)
        
        tk.Label(roi_frame, text="Width:", font=("Arial", 10)).pack(side="left", padx=5)
        self.roi_width_var = tk.StringVar(value="640")
        self.roi_width_text = tk.Entry(roi_frame, textvariable=self.roi_width_var, width=8)
        self.roi_width_text.pack(side="left", padx=5)
        
        tk.Label(roi_frame, text="Height:", font=("Arial", 10)).pack(side="left", padx=5)
        self.roi_height_var = tk.StringVar(value="480")
        self.roi_height_text = tk.Entry(roi_frame, textvariable=self.roi_height_var, width=8)
        self.roi_height_text.pack(side="left", padx=5)

    def create_position_section(self):
        self.label = tk.Label(self, text="Position", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=(15, 5))

        # Position X and Y
        pos_frame = tk.Frame(self)
        pos_frame.pack(anchor="w", padx=5, pady=5)
        
        tk.Label(pos_frame, text="X:", font=("Arial", 10)).pack(side="left", padx=5)
        self.pos_x_var = tk.StringVar(value="0")
        self.pos_x_text = tk.Entry(pos_frame, textvariable=self.pos_x_var, width=8)
        self.pos_x_text.pack(side="left", padx=5)
        
        tk.Label(pos_frame, text="Y:", font=("Arial", 10)).pack(side="left", padx=5)
        self.pos_y_var = tk.StringVar(value="0")
        self.pos_y_text = tk.Entry(pos_frame, textvariable=self.pos_y_var, width=8)
        self.pos_y_text.pack(side="left", padx=5)

    def create_exposure_section(self):
        self.label = tk.Label(self, text="Exposure", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=(15, 5))

        # Exposure slider and text box
        exp_frame = tk.Frame(self)
        exp_frame.pack(anchor="w", padx=5, pady=5)
        tk.Label(exp_frame, text="Exposure:", font=("Arial", 10)).pack(side="left", padx=5)
        self.exposure_slider = tk.Scale(exp_frame, from_=-100, to=100, orient="horizontal", length=200, command=self.on_exposure_change)
        self.exposure_slider.pack(side="left", padx=5)
        self.exposure_var = tk.StringVar(value="0")
        self.exposure_text = tk.Entry(exp_frame, textvariable=self.exposure_var, width=5)
        self.exposure_text.pack(side="left", padx=5)

    def create_gain_section(self):
        self.label = tk.Label(self, text="Gain", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=(15, 5))

        # Gain slider and text box
        gain_frame = tk.Frame(self)
        gain_frame.pack(anchor="w", padx=5, pady=5)
        tk.Label(gain_frame, text="Gain:", font=("Arial", 10)).pack(side="left", padx=5)
        self.gain_slider = tk.Scale(gain_frame, from_=-100, to=100, orient="horizontal", length=200, command=self.on_gain_change)
        self.gain_slider.pack(side="left", padx=5)
        self.gain_var = tk.StringVar(value="0")
        self.gain_text = tk.Entry(gain_frame, textvariable=self.gain_var, width=5)
        self.gain_text.pack(side="left", padx=5)

    def create_snapshot_save_section(self):
        # Save button for snapshot mode (initially hidden)
        self.save_btn = tk.Button(self, text="Save Snapshot", 
                                   command=self.on_save_snapshot,
                                   bg="green",
                                   fg="white",
                                   font=("Arial", 10))
        self.save_btn.pack(anchor="sw", padx=5, pady=5)
        self.save_btn.pack_forget()  # Hide by default

    def on_mode_select(self):
        # TODO: Add logic to handle video/snapshot mode change here
        if self.mode_var.get() == "Snapshot":
            self.save_btn.pack(anchor="sw", padx=5, pady=5)
        else:
            self.save_btn.pack_forget()
        messagebox.showinfo("Mode Selected", f"You selected: {self.mode_var.get()}")

    def on_exposure_change(self, value):
        # TODO: Add logic to handle exposure change here
        self.exposure_var.set(value)

    def on_gain_change(self, value):
        # TODO: Add logic to handle gain change here
        self.gain_var.set(value)

    def on_save_snapshot(self):
        filename = filedialog.asksaveasfilename(
            defaultextension=".png",
            initialfile="snap_image.png",
            filetypes=[("PNG Files", "*.png"), ("All Files", "*.*")]
        )
        if filename:
            # TODO: Save the frame from video feed to the filename
            messagebox.showinfo("Save Snapshot", f"Snapshot saved to {filename}")
    