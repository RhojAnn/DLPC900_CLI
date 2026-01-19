import tkinter as tk
from PIL import Image, ImageTk
import numpy as np

class VideoPanel(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        super().__init__(parent, bg="black", *args, **kwargs)
        
        # Create a label to display the video
        self.video_label = tk.Label(self, bg="black")
        self.video_label.pack(fill=tk.BOTH, expand=True)
        
        # Store the current image reference to prevent garbage collection
        self.current_image = None
    
    def update_frame(self, frame):
        """
        Update the video panel with a new frame
        
        Args:
            frame: numpy array containing the image data (grayscale)
        """
        if frame is None:
            return
        
        # Convert numpy array to PIL Image
        # For grayscale images
        if len(frame.shape) == 2:
            image = Image.fromarray(frame, mode='L')
        else:
            image = Image.fromarray(frame)
        
        # Resize to fit the panel while maintaining aspect ratio
        panel_width = self.winfo_width()
        panel_height = self.winfo_height()
        
        if panel_width > 1 and panel_height > 1:
            # Calculate scaling to fit
            img_width, img_height = image.size
            scale = min(panel_width / img_width, panel_height / img_height)
            new_width = int(img_width * scale)
            new_height = int(img_height * scale)
            
            image = image.resize((new_width, new_height), Image.Resampling.LANCZOS)
        
        # Convert to PhotoImage for tkinter
        photo = ImageTk.PhotoImage(image)
        
        # Update the label
        self.video_label.configure(image=photo)
        self.current_image = photo  # Keep a reference
    
    def clear(self):
        """Clear the video panel"""
        self.video_label.configure(image='')
        self.current_image = None
