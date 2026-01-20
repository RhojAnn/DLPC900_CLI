import tkinter as tk
from tkinter import messagebox

class DMDControls(tk.Frame):
    def __init__(self, parent, dmd=None, status_panel=None, *args, **kwargs):
        super().__init__(parent, borderwidth=2, relief="groove", *args, **kwargs)
        self.dmd = dmd
        self.status_panel = status_panel
        self._health_check_id = None
        
        self.label = tk.Label(self, text="DMD Controls", font=("Arial", 12, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=5)

        self.create_power_mode_section()

        # Button grid for pattern selection
        self.label = tk.Label(self, text="Pattern Selection", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=(15, 5))
        self.create_button_grid(10, 10)

        stop_btn = tk.Button(self, text="Stop Pattern", 
                            command=self.stop_pattern, 
                            bg="red", 
                            fg="white",
                            font=("Arial", 10))
        stop_btn.pack(anchor="sw", padx=5, pady=5)
        
        # Test pattern buttons frame
        test_frame = tk.Frame(self)
        test_frame.pack(anchor="sw", padx=5, pady=5)
        
        test_checker_btn = tk.Button(test_frame, text="Test Checkerboard", 
                            command=self.show_checkerboard, 
                            bg="blue", 
                            fg="white",
                            font=("Arial", 10))
        test_checker_btn.pack(side="left", padx=2)
        
        test_white_btn = tk.Button(test_frame, text="Test White", 
                            command=self.show_white, 
                            bg="white", 
                            fg="black",
                            font=("Arial", 10))
        test_white_btn.pack(side="left", padx=2)
        
        test_black_btn = tk.Button(test_frame, text="Test Black", 
                            command=self.show_black, 
                            bg="black", 
                            fg="white",
                            font=("Arial", 10))
        test_black_btn.pack(side="left", padx=2)
   
    def create_power_mode_section(self):
        self.label = tk.Label(self, text="Power Mode", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=5)

        # Radio buttons for power mode selection
        self.mode_var = tk.StringVar(value="Normal")
        radio1 = tk.Radiobutton(self, text="Normal", 
                                variable=self.mode_var, 
                                value="Normal", 
                                command=self.on_mode_select)
        radio2 = tk.Radiobutton(self, text="Standby (Recommended for longer rest periods)", 
                                variable=self.mode_var, value="Standby", 
                                command=self.on_mode_select)
        radio1.pack(anchor="sw", padx=5, pady=2)
        radio2.pack(anchor="sw", padx=5, pady=2)

    def on_mode_select(self):
        """Handle power mode change and update radio selection to match DMD mode"""
        if not self.dmd or not self.dmd.connected:
            messagebox.showwarning("DMD Not Connected", "Please connect to DMD first.")
            return

        requested_mode = self.mode_var.get()
        try:
            if requested_mode == "Normal":
                if self.dmd.set_normal():
                    print("DMD set to Normal mode")
                else:
                    messagebox.showerror("Error", "Failed to set Normal mode")
            elif requested_mode == "Standby":
                if self.dmd.set_standby():
                    print("DMD set to Standby mode")
                else:
                    messagebox.showerror("Error", "Failed to set Standby mode")

            self.after(1000, self._update_power_mode_display)
        except Exception as e:
            messagebox.showerror("Error", f"Power mode change failed: {e}")
            self.after(1000, self._update_power_mode_display)
    
    def auto_connect(self):
        """Auto-connect to DMD on startup."""
        if not self.dmd:
            print("No DMD instance provided")
            return
        
        try:
            if self.dmd.connect():
                if self.status_panel:
                    self.status_panel.set_dmd_status("Connected", True)
                # Update power mode display
                self._update_power_mode_display()
                # Start health check
                # self.start_health_check()  # Commented out for now
            else:
                if self.status_panel:
                    self.status_panel.set_dmd_status("Connection failed", False)
        except Exception as e:
            print(f"DMD auto-connect error: {e}")
            if self.status_panel:
                self.status_panel.set_dmd_status(f"Error: {e}", False)
    
    def _update_power_mode_display(self):
        """Update the radio buttons to reflect current DMD power mode."""
        if not self.dmd or not self.dmd.connected:
            return
        try:
            mode = self.dmd.get_power_mode()
            if mode == 0:
                self.mode_var.set("Normal")
            elif mode == 1:
                self.mode_var.set("Standby")
            else:
                self.mode_var.set("")
        except Exception as e:
            print(f"Failed to get power mode: {e}")
            self.mode_var.set("")
    
    def start_health_check(self):
        """Start periodic health check for DMD connection."""
        self._check_dmd_health()
    
    def stop_health_check(self):
        """Stop the periodic health check."""
        if self._health_check_id:
            self.after_cancel(self._health_check_id)
            self._health_check_id = None
    
    def _check_dmd_health(self):
        """Check DMD connection status periodically."""
        if not self.dmd:
            return
        
        try:
            was_connected = self.dmd._connected
            
            if was_connected:
                # Use get_power_mode() as a lightweight heartbeat
                # Returns -1 on error (connection lost)
                mode = self.dmd.get_power_mode()
                if mode < 0:
                    # Connection lost
                    self.dmd._connected = False
                    if self.status_panel:
                        self.status_panel.set_dmd_status("Disconnected", False)
            else:
                # Not connected, try to connect
                if self.dmd.connect():
                    if self.status_panel:
                        self.status_panel.set_dmd_status("Connected", True)
                    self._update_power_mode_display()
        except Exception as e:
            print(f"DMD health check error: {e}")
            self.dmd._connected = False
            if self.status_panel:
                self.status_panel.set_dmd_status("Error", False)
        
        # Schedule next check in 2 seconds
        self._health_check_id = self.after(2000, self._check_dmd_health)

    def create_button_grid(self, rows=10, cols=10):
        grid_frame = tk.Frame(self)
        grid_frame.pack(padx=5, pady=5)
        
        self.grid_buttons = {}
        self.selected_button = None
        
        for row in range(rows):
            for col in range(cols):
                btn = tk.Button(grid_frame, width=3, height=1, 
                              command=lambda r=row, c=col: self.on_button_select(r, c))
                btn.grid(row=row, column=col, padx=2, pady=2)
                self.grid_buttons[(row, col)] = btn

    def on_button_select(self, row, col):
        # Deselect previous button
        if self.selected_button:
            self.grid_buttons[self.selected_button].config(bg='SystemButtonFace', relief='raised')
        
        # Select new button
        self.grid_buttons[(row, col)].config(bg='black', relief='sunken')
        self.selected_button = (row, col)

    def stop_pattern(self):
        # Deselect the currently selected button
        if self.selected_button:
            self.grid_buttons[self.selected_button].config(bg='SystemButtonFace', relief='raised')
            self.selected_button = None
        
        if self.dmd and self.dmd.connected:
            if self.dmd.clear_pattern():
                print("Pattern stopped")
            else:
                messagebox.showerror("Error", "Failed to stop pattern")
        else:
            messagebox.showwarning("DMD Not Connected", "Please connect to DMD first.")
    
    # The following is for testing patterns
    def show_checkerboard(self):
        """Display checkerboard test pattern on DMD."""
        if not self.dmd or not self.dmd.connected:
            messagebox.showwarning("DMD Not Connected", "Please connect to DMD first.")
            return
        
        try:
            if self.dmd.show_test_pattern():
                print("Checkerboard pattern displayed")
            else:
                messagebox.showerror("Error", "Failed to display test pattern")
        except Exception as e:
            messagebox.showerror("Error", f"Test pattern failed: {e}")
    
    def show_white(self):
        """Display white test pattern on DMD."""
        if not self.dmd or not self.dmd.connected:
            messagebox.showwarning("DMD Not Connected", "Please connect to DMD first.")
            return
        
        try:
            if self.dmd.load_white():
                print("White pattern displayed")
            else:
                messagebox.showerror("Error", "Failed to display white pattern")
        except Exception as e:
            messagebox.showerror("Error", f"White pattern failed: {e}")
    
    def show_black(self):
        """Display black test pattern on DMD."""
        if not self.dmd or not self.dmd.connected:
            messagebox.showwarning("DMD Not Connected", "Please connect to DMD first.")
            return
        
        try:
            if self.dmd.load_black():
                print("Black pattern displayed")
            else:
                messagebox.showerror("Error", "Failed to display black pattern")
        except Exception as e:
            messagebox.showerror("Error", f"Black pattern failed: {e}")

    # end of testing patterns