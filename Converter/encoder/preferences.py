
class Preferences:
    def __init__(self):
        # CLIM Settings
        self.TARGET_WIDTH = 103
        self.TARGET_HEIGHT = 29
        self.TARGET_FPS = 12
        self.MAX_CHUNK_SIZE = 256
        self.PREPROCESS_VIDEO = True
        # Stable CLIM Settings
        self.AUDIO_EXTRACTION_EXTENSION = ".mp3"
        self.MAX_COLORS_PER_PALETTE = 255


    def set_target_dimensions(self, 
        width: int = None, 
        height: int = None, 
        aspect_ratio: float = 32 / 9
    ):
        """
        Set target dimensions (width and height) for the video output.

        Parameters:
            - width (int, optional): The desired width. Defaults to None.
            - height (int, optional): The desired height. Defaults to None.
            - aspect_ratio (float, optional): The aspect ratio (width/height) to use if one dimension is missing. Defaults to 32/9.

        Behavior:
            - If both width and height are None, defaults are used: width=103, height=29.
            - If only one dimension is provided, the other is calculated using the aspect_ratio.
            - If both dimensions are provided, they are set directly, ignoring aspect_ratio.
        """
        if width is None and height is None:
            # Both dimensions missing, use defaults
            self.TARGET_WIDTH = 103
            self.TARGET_HEIGHT = 29
        elif width is None:
            # Calculate width from height and aspect ratio
            self.TARGET_WIDTH = int(round(height * aspect_ratio))
            self.TARGET_HEIGHT = height
        elif height is None:
            # Calculate height from width and aspect ratio
            self.TARGET_WIDTH = width
            self.TARGET_HEIGHT = int(round(width / aspect_ratio))
        else:
            # Both dimensions provided, use them directly
            self.TARGET_WIDTH = width
            self.TARGET_HEIGHT = height


    def set_fps(self, fps: float = 12):
        """
        Set target framerate (fps) for the video output.

        Parameters:
            - fps (float, optional): The desired fps. Defaults to 12.
        """
        self.TARGET_FPS = fps


    def set_chunk_size(self, chunk_size: int = 256):
        """
        Set target maximum chunk size (chunk_size) for the video encoding.

        Parameters:
            - chunk_size (int, optional): The desired fps. Defaults to 256.
        """
        self.MAX_CHUNK_SIZE = chunk_size


    def set_preprocess_video(self, preprocess: bool = True):
        """
        Set whether to preprocess the video before conversion.

        Parameters:
            - preprocess (bool, optional): The desired behavior. Defaults to True (preprocess enabled).

        Behavior:
            - True: fast chunk retrieval. Relies on ffmpeg.
            - Fase: slow chunk retrieval. Relies on functions in this code [refer to framer.py module] that uses OpenCV as cv2.
        """
        self.PREPROCESS_VIDEO = preprocess


# Singleton
_preferences = Preferences()

def get_preferences():
    """Return preferences singleton"""
    return _preferences
