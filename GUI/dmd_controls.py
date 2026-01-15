import tkinter as tk
from tkinter import messagebox

class DMDControls(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        super().__init__(parent, borderwidth=2, relief="groove", *args, **kwargs)
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
   
    def create_power_mode_section(self):
        self.label = tk.Label(self, text="Power Mode", font=("Arial", 11, "bold"))
        self.label.pack(anchor="sw", padx=5, pady=5)

        # Radio buttons for power mode selection
        self.mode_var = tk.StringVar(value="Normal")
        radio1 = tk.Radiobutton(self, text="Normal", variable=self.mode_var, value="Normal", command=self.on_mode_select)
        radio2 = tk.Radiobutton(self, text="Standby (Recommended for longer rest periods)", variable=self.mode_var, value="Standby", command=self.on_mode_select)
        radio3 = tk.Radiobutton(self, text="Idle (Recommended for shorter rest periods)", variable=self.mode_var, value="Idle", command=self.on_mode_select)
        radio1.pack(anchor="sw", padx=5, pady=2)
        radio2.pack(anchor="sw", padx=5, pady=2)
        radio3.pack(anchor="sw", padx=5, pady=2)

    def on_mode_select(self):
        # TODO: Add logic to handle power mode change here and standby delay
        messagebox.showinfo("Mode Selected", f"You selected: {self.mode_var.get()}")

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
        messagebox.showinfo("Pattern Stopped", "DMD pattern has been stopped.")
