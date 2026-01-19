import tkinter as tk
from PIL import Image, ImageTk
import numpy as np


class VideoPanel(tk.Frame):
    """Panel for displaying live camera video feed."""
    
    def __init__(self, parent, *args, **kwargs):
        super().__init__(parent, bg="black", *args, **kwargs)
        
        self.camera = None
        self.is_streaming = False
        self.update_interval = 33  # ~30 FPS (milliseconds)
        
        # Canvas for displaying video
        self.canvas = tk.Canvas(self, bg="black", highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)
        
        # Placeholder text
        self.placeholder_text = self.canvas.create_text(
            0, 0, text="No Camera Feed", fill="gray", font=("Arial", 16)
        )
        
        # Store current image reference (prevents garbage collection)
        self._current_image = None
        self._image_id = None
        
        # Bind resize event
        self.canvas.bind("<Configure>", self._on_resize)
    
    def _on_resize(self, event):
        """Center placeholder text on resize."""
        self.canvas.coords(self.placeholder_text, event.width // 2, event.height // 2)
    
    def set_camera(self, camera):
        """
        Set the camera instance to use for video.
        
        Args:
            camera: ASICamera instance
        """
        self.camera = camera
    
    def start_stream(self):
        """Start the video stream."""
        if self.camera is None or not self.camera.is_connected:
            print("Cannot start stream: camera not connected")
            return False
        
        # Start camera video mode
        result = self.camera.start_video()
        if result != 0:
            print(f"Failed to start video mode: {result}")
            return False
        
        self.is_streaming = True
        self.canvas.itemconfig(self.placeholder_text, state="hidden")
        self._update_frame()
        return True
    
    def stop_stream(self):
        """Stop the video stream."""
        self.is_streaming = False
        
        if self.camera is not None and self.camera.is_connected:
            self.camera.stop_video()
        
        # Show placeholder
        self.canvas.itemconfig(self.placeholder_text, state="normal")
        
        # Clear current image
        if self._image_id is not None:
            self.canvas.delete(self._image_id)
            self._image_id = None
        self._current_image = None
    
    def _update_frame(self):
        """Fetch and display the next frame."""
        if not self.is_streaming or self.camera is None:
            return
        
        # Get frame from camera
        frame = self.camera.get_video_frame(wait_ms=100)
        
        if frame is not None:
            self._display_frame(frame)
        
        # Schedule next update
        self.after(self.update_interval, self._update_frame)
    
    def _display_frame(self, frame: np.ndarray):
        """
        Convert and display a numpy frame on the canvas.
        
        Args:
            frame: numpy array from camera (grayscale or RGB)
        """
        try:
            # Convert to PIL Image
            if frame.ndim == 2:
                # Grayscale
                image = Image.fromarray(frame, mode='L')
            else:
                # RGB
                image = Image.fromarray(frame, mode='RGB')
            
            # Resize to fit canvas while maintaining aspect ratio
            canvas_width = self.canvas.winfo_width()
            canvas_height = self.canvas.winfo_height()
            
            if canvas_width > 1 and canvas_height > 1:
                image = self._resize_to_fit(image, canvas_width, canvas_height)
            
            # Convert to Tkinter PhotoImage
            self._current_image = ImageTk.PhotoImage(image)
            
            # Display on canvas
            if self._image_id is None:
                self._image_id = self.canvas.create_image(
                    canvas_width // 2, canvas_height // 2,
                    image=self._current_image, anchor="center"
                )
            else:
                self.canvas.itemconfig(self._image_id, image=self._current_image)
                self.canvas.coords(self._image_id, canvas_width // 2, canvas_height // 2)
                
        except Exception as e:
            print(f"Error displaying frame: {e}")
    
    def _resize_to_fit(self, image: Image.Image, max_width: int, max_height: int) -> Image.Image:
        """Resize image to fit within bounds while maintaining aspect ratio."""
        img_width, img_height = image.size
        
        # Calculate scaling factor
        scale = min(max_width / img_width, max_height / img_height)
        
        new_width = int(img_width * scale)
        new_height = int(img_height * scale)
        
        return image.resize((new_width, new_height), Image.Resampling.LANCZOS)
    
    def display_single_frame(self, frame: np.ndarray):
        """
        Display a single frame (for snapshot preview).
        
        Args:
            frame: numpy array to display
        """
        self.canvas.itemconfig(self.placeholder_text, state="hidden")
        self._display_frame(frame)
    
    def clear(self):
        """Clear the display."""
        if self._image_id is not None:
            self.canvas.delete(self._image_id)
            self._image_id = None
        self._current_image = None
        self.canvas.itemconfig(self.placeholder_text, state="normal")
