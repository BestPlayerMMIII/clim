import os
import tempfile
import subprocess
import cv2
import numpy as np
from tqdm import tqdm as progressbar
from typing import List, Tuple, Iterator


class VideoFramer:
    """
    Processes the input video with specified options and yields an interator to get the frames chunk by chunk.
    """

    def __init__(
        self, 
        video_path: str, 
        target_width: int, 
        target_height: int, 
        target_fps: float, 
        preprocess_video: bool
    ) -> None:
        """
        Args:
            video_path (str): Path to the input video file.
            target_width (int): Target video width.
            target_height (int): Target video height.
            target_fps (int): Target frames per second.
            preprocess_video (bool): Whether to preprocess the video before conversion.
                This will speed up the process of getting the frames chunk by chunk.
        """
        self.video_path = video_path
        self.target_width = target_width
        self.target_height = target_height
        self.target_fps = target_fps
        self.preprocess_video = preprocess_video
        self.temp_video_path = None  # Temporary file for the preprocessed video

        if preprocess_video:
            self._preprocess_video()
            self.capture = cv2.VideoCapture(self.temp_video_path)
        else:
            self.capture = cv2.VideoCapture(video_path)

        if not self.capture.isOpened():
            raise ValueError(f"Unable to open video file: {video_path}")

        # Extract metadata from the processed or original video
        self.original_fps = self.capture.get(cv2.CAP_PROP_FPS)
        self.original_frame_count = int(self.capture.get(cv2.CAP_PROP_FRAME_COUNT))

        # Total frames in the target video sequence
        self.total_target_frames = self.original_frame_count if preprocess_video else int(
            self.original_frame_count * self.target_fps / self.original_fps
        )
        self.frame_step = 1 if preprocess_video else self.original_fps / self.target_fps

    def _preprocess_video(self) -> None:
        """
        Preprocess the video using ffmpeg to create a temporary copy with the target resolution and FPS.
        """
        temp_file = tempfile.NamedTemporaryFile(delete=False, suffix=".webm")
        self.temp_video_path = temp_file.name
        temp_file.close()

        ffmpeg_command = [
            "ffmpeg", "-y",  # ensure file override without asking
            "-i", self.video_path,
            "-vf", f"scale={self.target_width}:{self.target_height}:flags=neighbor",  # INTER_NEAREST
            "-r", str(self.target_fps),
            "-c:v", "libvpx",  # Codec VP8/VP9
            "-b:v", "10M",  # Bitrate
            "-an",  # Disable audio
            self.temp_video_path
        ]

        result = subprocess.run(ffmpeg_command) #, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if result.returncode != 0:
            raise RuntimeError(f"FFmpeg preprocessing failed: {result.stderr.decode('utf-8')}")

    def _convert_to_rgb_grid(self, frame: np.ndarray) -> List[List[Tuple[int, int, int]]]:
        """Convert a resized BGR frame to a 2D grid of RGB tuples."""
        return [
            [(int(pixel[2]), int(pixel[1]), int(pixel[0])) for pixel in row]
            for row in frame
        ]

    def get_frame_RGB(self, frame_index: int) -> List[List[Tuple[int, int, int]]]:
        """
        Extract a specific frame as a grid of RGB tuples.

        Parameters:
            frame_index (int): The index of the target frame in the sequence.

        Returns:
            List[List[Tuple[int, int, int]]]: A 2D grid of RGB tuples.
        """
        original_frame_index = frame_index if self.preprocess_video else int(frame_index * self.frame_step)
        self.capture.set(cv2.CAP_PROP_POS_FRAMES, original_frame_index)

        # Read and process the frame
        success, frame = self.capture.read()
        if not success or frame is None:
            return np.zeros((self.target_height, self.target_width, 3), dtype=np.uint8).tolist()

        resized_frame = frame if self.preprocess_video else cv2.resize(
            frame, (self.target_width, self.target_height), interpolation=cv2.INTER_NEAREST
        )
        return self._convert_to_rgb_grid(resized_frame)

    def get_frame_RGB_chunks(self, chunk_size: int) -> Iterator[List[List[List[Tuple[int, int, int]]]]]:
        """
        Yield chunks of frames as grids of RGB tuples.

        Parameters:
            chunk_size (int): Number of frames in each chunk.

        Yields:
            List[List[List[Tuple[int, int, int]]]]: A chunk of frames, each represented as a 2D grid of RGB tuples.
        """
        frame_index = 0
        frames_left = self.total_target_frames

        while frames_left > 0:
            chunk = []
            for _ in progressbar(range(min(chunk_size, frames_left)), desc=" Retrieving chunk frames"):
                chunk.append(self.get_frame_RGB(frame_index))
                frame_index += 1
            yield chunk
            frames_left -= len(chunk)

    def calculate_number_of_chunks(self, chunk_size: int) -> int:
        """
        Calculate the number of chunks needed to process all frames.

        Parameters:
            chunk_size (int): Number of frames in each chunk.

        Returns:
            int: Total number of chunks.
        """
        return (self.total_target_frames + chunk_size - 1) // chunk_size  # Efficient ceiling division

    def __del__(self):
        """Release video capture and delete the temporary video file (if created)."""
        if hasattr(self, 'capture') and self.capture.isOpened():
            self.capture.release()
        if hasattr(self, 'temp_video_path') and self.temp_video_path and os.path.exists(self.temp_video_path):
            os.remove(self.temp_video_path)
