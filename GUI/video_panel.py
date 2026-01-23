import tkinter as tk
from PIL import Image, ImageTk
import numpy as np
import threading
import queue
import time
import contextlib


class VideoPanel(tk.Frame):
    def __init__(self, parent, *args, **kwargs):
        super().__init__(parent, bg="black", *args, **kwargs)
        
        self.camera = None
        self.is_streaming = False
        self.update_interval = 16  # ~60 FPS (in ms, if 30 FPS use 33ms)
        
        self.canvas = tk.Canvas(self, bg="black", highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)
        
        self.placeholder_text = self.canvas.create_text(
            0, 0, text="No Camera Feed", fill="gray", font=("Arial", 16)
        )
        
        self._current_image = None
        self._image_id = None

        self.frame_queue = queue.Queue(maxsize=1)  
        self._capture_thread = None
        self._stop_event = threading.Event()
        self._last_canvas_size = (0, 0)

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
        # Start capture thread; the thread will start camera video mode
        self._stop_event.clear()
        if not self._capture_thread or not self._capture_thread.is_alive():
            self._capture_thread = threading.Thread(target=self._capture_loop, daemon=True)
            self._capture_thread.start()

        self.is_streaming = True
        self.canvas.itemconfig(self.placeholder_text, state="hidden")
        # Start UI updater
        self._update_frame()
        return True
    
    def stop_stream(self):
        """Stop the video stream."""
        self.is_streaming = False

        # Stop capture thread
        self._stop_event.set()
        if self._capture_thread and self._capture_thread.is_alive():
            self._capture_thread.join(timeout=0.2)

        if self.camera is not None and self.camera.is_connected:
            try:
                self.camera.stop_video()
            except Exception:
                pass

        # Show placeholder
        self.canvas.itemconfig(self.placeholder_text, state="normal")

        # Clear current image and queue
        if self._image_id is not None:
            self.canvas.delete(self._image_id)
            self._image_id = None
        self._current_image = None
        with contextlib.suppress(Exception):
            while True:
                self.frame_queue.get_nowait()
    
    def _update_frame(self):
        """Fetch and display the next frame."""
        if not self.is_streaming:
            return

        # Try to get the latest frame from the queue without blocking
        frame = None
        with contextlib.suppress(queue.Empty):
            # Drain queue to get the most recent frame
            while True:
                frame = self.frame_queue.get_nowait()

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
                # Use a cheaper resampling filter for per-frame resizing
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
        
        return image.resize((new_width, new_height), Image.Resampling.BILINEAR)

    def _capture_loop(self):
        """Background thread: capture frames from camera and keep latest in queue."""
        try:
            # Attempt to start camera video mode from background thread
            try:
                res = self.camera.start_video()
            except Exception as e:
                res = -1
                print(f"start_video error: {e}")

            if res != 0:
                # notify UI and exit capture loop
                try:
                    self.after(0, lambda: print(f"Failed to start video mode: {res}"))
                except Exception:
                    pass
                return

            while not self._stop_event.is_set():
                if not self.camera or not getattr(self.camera, 'is_connected', False):
                    time.sleep(0.05)
                    continue

                # Small wait to avoid blocking too long in the capture thread
                frame = None
                try:
                    frame = self.camera.get_video_frame(wait_ms=10)
                except Exception:
                    frame = None

                if frame is None:
                    # Sleep briefly to avoid a tight loop when no frames available
                    time.sleep(0.005)
                    continue

                # Put latest frame into queue, dropping older one if necessary
                try:
                    self.frame_queue.put_nowait(frame)
                except queue.Full:
                    with contextlib.suppress(Exception):
                        _ = self.frame_queue.get_nowait()
                    with contextlib.suppress(Exception):
                        self.frame_queue.put_nowait(frame)

        except Exception as e:
            print(f"Capture thread error: {e}")
    
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
