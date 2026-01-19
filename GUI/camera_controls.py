import tkinter as tk
from tkinter import messagebox
from tkinter import filedialog
import sys
import os

# Add parent directory to path to import the wrapper
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

try:
    from test_asi_wrapper import ASICameraWrapper
    WRAPPER_AVAILABLE = True
except (ImportError, FileNotFoundError) as e:
    print(f"Warning: ASI Camera wrapper not available: {e}")
    WRAPPER_AVAILABLE = False

class CameraControls(tk.Frame):
    def __init__(self, parent, video_panel=None, *args, **kwargs):
        super().__init__(parent, borderwidth=2, relief="groove", *args, **kwargs)
        self.label = tk.Label(self, text="Camera Controls", font=("Arial", 12, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=5)
        
        self.video_panel = video_panel
        self.camera = None
        self.video_running = False
        self.video_job = None
        
        # Initialize camera wrapper if available
        if WRAPPER_AVAILABLE:
            try:
                self.camera = ASICameraWrapper()
                self.camera_available = True
            except FileNotFoundError:
                self.camera_available = False
                messagebox.showwarning("Camera DLL Not Found", 
                                     "asi_wrapper.dll not found. Please build it using build_asi_wrapper.bat")
        else:
            self.camera_available = False

        self.create_init_section()
        self.create_camera_mode_section()
        self.create_roi_section()
        self.create_position_section()
        self.create_exposure_section()
        self.create_gain_section()
        self.create_snapshot_save_section()
    
    def create_init_section(self):
        """Create camera initialization section"""
        init_frame = tk.Frame(self)
        init_frame.pack(anchor="w", padx=5, pady=5)
        
        self.init_btn = tk.Button(init_frame, text="Initialize Camera", 
                                  command=self.on_init_camera,
                                  bg="blue",
                                  fg="white",
                                  font=("Arial", 10))
        self.init_btn.pack(side="left", padx=5)
        
        self.camera_status = tk.Label(init_frame, text="Not Initialized", 
                                      font=("Arial", 9), fg="red")
        self.camera_status.pack(side="left", padx=5)


    def create_camera_mode_section(self):
        self.label = tk.Label(self, text="Camera Mode", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=5)

        # Radio buttons for mode selection
        self.mode_var = tk.StringVar(value="Video")
        radio1 = tk.Radiobutton(self, text="Video", variable=self.mode_var, value="Video", 
                               command=self.on_mode_select, state=tk.DISABLED)
        radio2 = tk.Radiobutton(self, text="Snapshot", variable=self.mode_var, value="Snapshot", 
                               command=self.on_mode_select, state=tk.DISABLED)
        radio1.pack(anchor="sw", padx=5, pady=2)
        radio2.pack(anchor="sw", padx=5, pady=2)
        
        self.mode_radio1 = radio1
        self.mode_radio2 = radio2

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

    def on_init_camera(self):
        """Initialize the camera"""
        if not self.camera_available or self.camera is None:
            messagebox.showerror("Error", "Camera wrapper not available")
            return
        
        # Get ROI settings
        try:
            width = int(self.roi_width_var.get())
            height = int(self.roi_height_var.get())
        except ValueError:
            messagebox.showerror("Error", "Invalid ROI dimensions")
            return
        
        # Initialize camera
        if self.camera.init_camera(roi_width=width, roi_height=height):
            self.camera_status.config(text="Initialized", fg="green")
            # Enable mode selection
            self.mode_radio1.config(state=tk.NORMAL)
            self.mode_radio2.config(state=tk.NORMAL)
            self.init_btn.config(state=tk.DISABLED)
            messagebox.showinfo("Success", "Camera initialized successfully!")
        else:
            messagebox.showerror("Error", "Failed to initialize camera. Check connection.")

    def on_mode_select(self):
        """Handle mode selection change"""
        mode = self.mode_var.get()
        
        if mode == "Snapshot":
            self.save_btn.pack(anchor="sw", padx=5, pady=5)
            # Stop video if running
            if self.video_running:
                self.stop_video()
        else:  # Video mode
            self.save_btn.pack_forget()
            # Start video if not running
            if not self.video_running and self.camera and self.camera.camera_id is not None:
                self.start_video()

    def start_video(self):
        """Start video streaming"""
        if not self.camera or self.camera.camera_id is None:
            return
        
        if self.camera.start_video_mode():
            self.video_running = True
            self.update_video_frame()
    
    def stop_video(self):
        """Stop video streaming"""
        self.video_running = False
        if self.video_job:
            self.after_cancel(self.video_job)
            self.video_job = None
        
        if self.camera and self.camera.camera_id is not None:
            self.camera.stop_video_mode()
    
    def update_video_frame(self):
        """Update video frame in a loop"""
        if not self.video_running:
            return
        
        # Get frame from camera
        frame = self.camera.get_frame(wait_ms=100)
        
        if frame is not None and self.video_panel:
            self.video_panel.update_frame(frame)
        
        # Schedule next update (approx 30 FPS)
        self.video_job = self.after(33, self.update_video_frame)

    def on_exposure_change(self, value):
        """Handle exposure slider change"""
        self.exposure_var.set(value)
        # Apply exposure if camera is initialized
        if self.camera and self.camera.camera_id is not None:
            # Convert slider value to actual exposure value
            # Slider: -100 to 100, map to reasonable exposure range
            # This is a simple linear mapping - adjust as needed
            exposure_val = int((float(value) + 100) * 500)  # Maps to 0-100000 microseconds
            self.camera.set_exposure(exposure_val, auto=False)

    def on_gain_change(self, value):
        """Handle gain slider change"""
        self.gain_var.set(value)
        # Apply gain if camera is initialized
        if self.camera and self.camera.camera_id is not None:
            # Convert slider value to actual gain value
            # Slider: -100 to 100, map to 0-200 gain range
            gain_val = int((float(value) + 100) * 1)  # Maps to 0-200
            self.camera.set_gain(gain_val, auto=False)

    def on_save_snapshot(self):
        """Capture and save a snapshot"""
        if not self.camera or self.camera.camera_id is None:
            messagebox.showerror("Error", "Camera not initialized")
            return
        
        filename = filedialog.asksaveasfilename(
            defaultextension=".png",
            initialfile="snap_image.png",
            filetypes=[("PNG Files", "*.png"), ("All Files", "*.*")]
        )
        
        if filename:
            # Capture snapshot
            frame = self.camera.capture_snapshot(timeout_ms=5000)
            
            if frame is not None:
                # Save using PIL
                from PIL import Image
                img = Image.fromarray(frame, mode='L')
                img.save(filename)
                
                # Display in video panel
                if self.video_panel:
                    self.video_panel.update_frame(frame)
                
                messagebox.showinfo("Success", f"Snapshot saved to {filename}")
            else:
                messagebox.showerror("Error", "Failed to capture snapshot")
    
    def cleanup(self):
        """Cleanup camera resources"""
        if self.video_running:
            self.stop_video()
        
        if self.camera and self.camera.camera_id is not None:
            self.camera.stop_camera()
    