import tkinter as tk
from tkinter import messagebox
from tkinter import filedialog

class CameraControls(tk.Frame):
    def __init__(self, parent, camera=None, video_panel=None, *args, **kwargs):
        super().__init__(parent, borderwidth=2, relief="groove", *args, **kwargs)
        
        self.camera = camera
        self.video_panel = video_panel
        
        self.label = tk.Label(self, text="Camera Controls", font=("Arial", 12, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=5)

        self.create_camera_mode_section()
        self.create_roi_section()
        self.create_position_section()
        self.create_exposure_section()
        self.create_gain_section()
        self.create_snapshot_save_section()
        
        self.status_panel_ref = None  # Reference to status panel
        self._health_check_id = None  # For periodic health check
        self._was_connected = False   # Track previous connection state
    
    def auto_connect(self, status_panel=None):
        """Automatically connect to camera on startup."""
        self.status_panel_ref = status_panel
        
        if not self.camera:
            self._update_camera_status("No camera instance", False)
            return False
        
        num_cams = self.camera.get_num_cameras()
        if num_cams == 0:
            self._update_camera_status("No cameras detected", False)
            self._start_health_check()
            return False
        
        # Get ROI from input fields
        try:
            width = int(self.roi_width_var.get())
            height = int(self.roi_height_var.get())
        except (ValueError, AttributeError):
            width, height = 1080, 1080
        
        result = self.camera.init_camera(width, height)
        
        if result == 0:
            self._update_camera_status("Connected", True)
            self._was_connected = True
            self._update_control_ranges()
            
            # Start video stream
            if self.video_panel:
                self.video_panel.start_stream()
            
            # Start periodic health check
            self._start_health_check()
            return True
        else:
            self._update_camera_status(f"Connection failed: {result}", False)
            self._start_health_check()
            return False
    
    def _start_health_check(self, interval_ms: int = 2000):
        """Start periodic camera health check."""
        if self._health_check_id:
            self.after_cancel(self._health_check_id)
        self._check_camera_health()
    
    def _check_camera_health(self):
        """Check if camera is still connected."""
        if not self.camera:
            self._health_check_id = self.after(2000, self._check_camera_health)
            return
        
        num_cams = self.camera.get_num_cameras()
        is_connected = num_cams > 0 and self.camera.is_connected
        
        # Detect disconnect
        if self._was_connected and not is_connected:
            self._was_connected = False
            self._update_camera_status("Disconnected", False)
            if self.video_panel:
                self.video_panel.stop_stream()
        
        # Detect reconnect
        elif not self._was_connected and num_cams > 0:
            self._update_camera_status("Reconnecting...", False)
            try:
                width = int(self.roi_width_var.get())
                height = int(self.roi_height_var.get())
            except (ValueError, AttributeError):
                width, height = 1080, 1080
            
            result = self.camera.init_camera(width, height)
            if result == 0:
                self._was_connected = True
                self._update_camera_status("Connected", True)
                self._update_control_ranges()
                if self.video_panel and self.mode_var.get() == "Video":
                    self.video_panel.start_stream()
        
        self._health_check_id = self.after(2000, self._check_camera_health)
    
    def stop_health_check(self):
        """Stop the periodic health check."""
        if self._health_check_id:
            self.after_cancel(self._health_check_id)
            self._health_check_id = None
    
    def _update_camera_status(self, message: str, connected: bool):
        """Update camera status in status panel."""
        if self.status_panel_ref:
            self.status_panel_ref.set_camera_status(message, connected)

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

        self.roi_width_var = tk.StringVar(value="8288")
        self.roi_width_text = tk.Entry(roi_frame, textvariable=self.roi_width_var, width=8)
        self.roi_width_text.pack(side="left", padx=5)

        tk.Label(roi_frame, text="Height:", font=("Arial", 10)).pack(side="left", padx=5)
        self.roi_height_var = tk.StringVar(value="5644")
        self.roi_height_text = tk.Entry(roi_frame, textvariable=self.roi_height_var, width=8)
        self.roi_height_text.pack(side="left", padx=5)

        # Apply ROI button
        self.apply_roi_btn = tk.Button(roi_frame, text="Apply ROI", command=self.on_apply_roi)
        self.apply_roi_btn.pack(side="left", padx=10)

        # ROI min/max label (underneath)
        self.roi_range_label = tk.Label(self, text="", font=("Arial", 8), fg="gray")
        self.roi_range_label.pack(anchor="w", padx=10, pady=(0, 5))

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

        # Apply Position button
        self.apply_pos_btn = tk.Button(pos_frame, text="Apply Position", command=self.on_apply_position)
        self.apply_pos_btn.pack(side="left", padx=10)

        # Position min/max label (underneath)
        self.pos_range_label = tk.Label(self, text="", font=("Arial", 8), fg="gray")
        self.pos_range_label.pack(anchor="w", padx=10, pady=(0, 5))

    def create_exposure_section(self):
        self.label = tk.Label(self, text="Exposure (μs)", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=(15, 5))

        # Exposure slider and text box
        exp_frame = tk.Frame(self)
        exp_frame.pack(anchor="w", padx=5, pady=5)
        tk.Label(exp_frame, text="Exposure:", font=("Arial", 10)).pack(side="left", padx=5)
        self.exposure_slider = tk.Scale(exp_frame, from_=1000, to=1000000, orient="horizontal", length=200, command=self.on_exposure_slider_change)
        self.exposure_slider.pack(side="left", padx=5)
        self.exposure_var = tk.StringVar(value="1000")
        self.exposure_text = tk.Entry(exp_frame, textvariable=self.exposure_var, width=10)
        self.exposure_text.pack(side="left", padx=5)
        self.exposure_text.bind("<Return>", self.on_exposure_text_change)
        self.exposure_text.bind("<FocusOut>", self.on_exposure_text_change)

        # Exposure min/max label (underneath)
        self.exposure_range_label = tk.Label(self, text="", font=("Arial", 8), fg="gray")
        self.exposure_range_label.pack(anchor="w", padx=10, pady=(0, 5))

    def create_gain_section(self):
        self.label = tk.Label(self, text="Gain", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=(15, 5))

        # Gain slider and text box
        gain_frame = tk.Frame(self)
        gain_frame.pack(anchor="w", padx=5, pady=5)
        tk.Label(gain_frame, text="Gain:", font=("Arial", 10)).pack(side="left", padx=5)
        self.gain_slider = tk.Scale(gain_frame, from_=0, to=100, orient="horizontal", length=200, command=self.on_gain_slider_change)
        self.gain_slider.pack(side="left", padx=5)
        self.gain_var = tk.StringVar(value="0")
        self.gain_text = tk.Entry(gain_frame, textvariable=self.gain_var, width=6)
        self.gain_text.pack(side="left", padx=5)
        self.gain_text.bind("<Return>", self.on_gain_text_change)
        self.gain_text.bind("<FocusOut>", self.on_gain_text_change)

        # Gain min/max label (underneath)
        self.gain_range_label = tk.Label(self, text="", font=("Arial", 8), fg="gray")
        self.gain_range_label.pack(anchor="w", padx=10, pady=(0, 5))

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
        """Handle video/snapshot mode change."""
        mode = self.mode_var.get()
        
        if mode == "Snapshot":
            self.save_btn.pack(anchor="sw", padx=5, pady=5)
            # Stop video stream
            if self.video_panel:
                self.video_panel.stop_stream()
        else:
            self.save_btn.pack_forget()
            # Start video stream if connected
            if self.camera and self.camera.is_connected and self.video_panel:
                self.video_panel.start_stream()

    def on_exposure_slider_change(self, value):
        """Update exposure from slider."""
        self.exposure_var.set(value)
        if self.camera and self.camera.is_connected:
            self.camera.set_exposure(int(value))
    
    def on_exposure_text_change(self, event=None):
        """Update exposure from text entry."""
        try:
            value = int(self.exposure_var.get())
            # Clamp to slider range
            min_val = int(self.exposure_slider.cget("from"))
            max_val = int(self.exposure_slider.cget("to"))
            value = max(min_val, min(max_val, value))
            self.exposure_slider.set(value)
            self.exposure_var.set(str(value))
            if self.camera and self.camera.is_connected:
                self.camera.set_exposure(value)
        except ValueError:
            pass  # Invalid input, ignore

    def on_gain_slider_change(self, value):
        """Update gain from slider."""
        self.gain_var.set(value)
        if self.camera and self.camera.is_connected:
            self.camera.set_gain(int(value))
    
    def on_gain_text_change(self, event=None):
        """Update gain from text entry."""
        try:
            value = int(self.gain_var.get())
            # Clamp to slider range
            min_val = int(self.gain_slider.cget("from"))
            max_val = int(self.gain_slider.cget("to"))
            value = max(min_val, min(max_val, value))
            self.gain_slider.set(value)
            self.gain_var.set(str(value))
            if self.camera and self.camera.is_connected:
                self.camera.set_gain(value)
        except ValueError:
            pass  # Invalid input, ignore

    def on_apply_roi(self):
        """Apply ROI settings."""
        if not self.camera or not self.camera.is_connected:
            messagebox.showerror("Error", "Camera not connected")
            return
        
        try:
            width = int(self.roi_width_var.get())
            height = int(self.roi_height_var.get())
        except ValueError:
            messagebox.showerror("Error", "Invalid ROI values")
            return
        
        result = self.camera.set_ROI(width, height)
        if result != 0:
            messagebox.showerror("Error", f"Failed to set ROI: error {result}")
            return
        
        print(f"ROI applied: {width}x{height}")

    def on_apply_position(self):
        """Apply position settings."""
        if not self.camera or not self.camera.is_connected:
            messagebox.showerror("Error", "Camera not connected")
            return
        
        try:
            pos_x = int(self.pos_x_var.get())
            pos_y = int(self.pos_y_var.get())
        except ValueError:
            messagebox.showerror("Error", "Invalid position values")
            return
        
        result = self.camera.set_pos(pos_x, pos_y)
        if result != 0:
            messagebox.showerror("Error", f"Failed to set position: error {result}")
            return
        
        print(f"Position applied: ({pos_x}, {pos_y})")

    def on_save_snapshot(self):
        """Capture and save a snapshot."""
        if not self.camera or not self.camera.is_connected:
            messagebox.showerror("Error", "Camera not connected")
            return
        
        # Stop video if running
        if self.video_panel:
            self.video_panel.stop_stream()
        
        # Capture snapshot
        image = self.camera.snap(timeout_ms=30000)
        
        if image is None:
            messagebox.showerror("Error", "Failed to capture snapshot")
            return
        
        # Show preview
        if self.video_panel:
            self.video_panel.display_single_frame(image)
        
        # Save dialog
        filename = filedialog.asksaveasfilename(
            defaultextension=".png",
            initialfile="snap_image.png",
            filetypes=[("PNG Files", "*.png"), ("TIFF Files", "*.tiff"), ("All Files", "*.*")]
        )
        if filename:
            self.camera.save_image(image, filename)
            messagebox.showinfo("Save Snapshot", f"Snapshot saved to {filename}")
    
    def on_connect(self):
        """Connect to camera."""
        if not self.camera:
            messagebox.showerror("Error", "No camera instance")
            return
        
        # Get ROI from input fields
        try:
            width = int(self.roi_width_var.get())
            height = int(self.roi_height_var.get())
        except ValueError:
            width, height = 1080, 1080
        
        result = self.camera.init_camera(width, height)
        
        if result == 0:
            self.status_label.config(text="Connected", fg="green")
            self.connect_btn.config(state="disabled")
            self.disconnect_btn.config(state="normal")
            
            # Update exposure/gain ranges and position
            self._update_control_ranges()
            
            # Start video if in video mode
            if self.mode_var.get() == "Video" and self.video_panel:
                self.video_panel.start_stream()
        else:
            messagebox.showerror("Error", f"Failed to connect: error {result}")
    
    def on_disconnect(self):
        """Disconnect from camera."""
        if self.video_panel:
            self.video_panel.stop_stream()
        
        if self.camera:
            self.camera.stop_camera()
        
        self.status_label.config(text="Not connected", fg="gray")
        self.connect_btn.config(state="normal")
        self.disconnect_btn.config(state="disabled")
    
    def _update_control_ranges(self):
        """Update slider ranges and min/max hints based on camera capabilities."""
        if not self.camera or not self.camera.is_connected:
            return

        # Update exposure range
        _, exp_min, exp_max = self.camera.get_exposure_range()
        self.exposure_slider.config(from_=exp_min, to=exp_max)
        _, current_exp, _ = self.camera.get_exposure()
        self.exposure_slider.set(current_exp)
        self.exposure_var.set(str(current_exp))
        self.exposure_range_label.config(text=f"Exposure: [{exp_min} - {exp_max}] μs")

        # Update gain range
        _, gain_min, gain_max = self.camera.get_gain_range()
        self.gain_slider.config(from_=gain_min, to=gain_max)
        _, current_gain, _ = self.camera.get_gain()
        self.gain_slider.set(current_gain)
        self.gain_var.set(str(current_gain))
        self.gain_range_label.config(text=f"Gain: [{gain_min} - {gain_max}]")

        # Update ROI and position min/max hints
        try:
            min_w, max_w, min_h, max_h = self.camera.get_dimension_range()
        except Exception:
            min_w = min_h = 0
            max_w = max_h = 0

        # ROI min/max label
        self.roi_range_label.config(text=f"ROI width: [{min_w}-{max_w}], height: [{min_h}-{max_h}]")

        # Position min/max label (max pos depends on ROI size)
        try:
            roi_w = int(self.roi_width_var.get())
            roi_h = int(self.roi_height_var.get())
        except Exception:
            roi_w = roi_h = 0
        max_x = max(0, max_w - roi_w)
        max_y = max(0, max_h - roi_h)
        self.pos_range_label.config(text=f"Position X: [0-{max_x}], Y: [0-{max_y}]")

    