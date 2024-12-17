import os
import shutil
import subprocess
import heapq
import numpy as np
from bitarray import bitarray
from typing import Dict, TypeVar, List, Tuple

_T = TypeVar('_T')

### FILE MANAGEMENT

def ensure_directory_exists(dirpath: str) -> None:
    """Ensure that a directory exists."""
    os.makedirs(dirpath, exist_ok=True)

def remove_directory(dirpath: str) -> None:
    """Remove a directory and its contents, if it exists."""
    shutil.rmtree(dirpath, ignore_errors=True)

def remove_file(filepath: str) -> None:
    """Remove a file if it exists."""
    if os.path.exists(filepath):
        os.remove(filepath)

def get_filename_without_extension(filename: str) -> str:
    """Return the filename without its extension."""
    return os.path.splitext(filename)[0]

### AUDIO EXTRACTION

def extract_audio(video_path: str, output_audio_path: str, bitrate: str = "64k", speed_percentage: float = 1.0) -> bool:
    """
    Extract audio from a video file and adjust its playback speed.
    
    Parameters:
        video_path (str): Path to the input video file.
        output_audio_path (str): Path to save the extracted audio file.
        bitrate (str): Audio bitrate (default is "64k").
        speed_percentage (float): Playback speed adjustment factor (default is 1.0).
        
    Returns:
        bool: True if extraction was successful, False otherwise.
    """
    if speed_percentage <= 0:
        raise ValueError("Speed percentage must be greater than 0.")
    
    # Ensure output directory exists
    ensure_directory_exists(os.path.dirname(output_audio_path))

    # Adjust atempo (allowed values: 0.5 <= atempo <= 2.0)
    speed_factor = 1 / speed_percentage
    if not (0.5 <= speed_factor <= 2.0):
        raise ValueError("Speed factor must be between 0.5 and 2.0 due to ffmpeg limitations.")

    command = [
        'ffmpeg', '-i', video_path,
        "-filter:a", f"atempo={speed_factor}", 
        "-b:a", bitrate,
        '-q:a', '0', '-map', 'a', output_audio_path,
        '-y'  # Overwrite output file if it exists
    ]

    try:
        subprocess.run(command, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if os.path.exists(output_audio_path):
            return True
        return False
    except subprocess.CalledProcessError:
        print(f"Error: Failed to extract audio from '{video_path}'.")
        return False

### BIT ARRAY

def align_to_byte(bit_array: bitarray) -> bitarray:
    """Align a bitarray to the nearest byte boundary."""
    if len(bit_array) % 8 != 0:
        bit_array.extend("0" * (8 - (len(bit_array) % 8)))
    return bit_array

### HUFFMAN CODING

def huffman_coding(frequencies: Dict[_T, int], max_length: int = None) -> Dict[_T, str]:
    """Generate Huffman codes based on frequencies with an optional max length."""
    if not frequencies:
        raise ValueError("Frequencies dictionary is empty.")
    elif len(frequencies) == 1:
        return {list(frequencies.keys())[0]: '0'}

    # Filter out invalid frequencies
    valid_frequencies = {k: v for k, v in frequencies.items() if v > 0}
    if not valid_frequencies:
        raise ValueError("No valid frequencies found (all are zero or negative).")

    # Build Huffman Tree
    heap = [[weight, [symbol, ""]] for symbol, weight in valid_frequencies.items()]
    heapq.heapify(heap)

    while len(heap) > 1:
        lo = heapq.heappop(heap)
        hi = heapq.heappop(heap)
        for pair in lo[1:]:
            pair[1] = "0" + pair[1]
        for pair in hi[1:]:
            pair[1] = "1" + pair[1]
        heapq.heappush(heap, [lo[0] + hi[0]] + lo[1:] + hi[1:])

    huffman_dict = {symbol: code for symbol, code in heapq.heappop(heap)[1:]}
    return _enforce_max_length(huffman_dict, max_length) if max_length else huffman_dict

def _enforce_max_length(huffman_dict: Dict[_T, str], max_length: int) -> Dict[_T, str]:
    """Ensure Huffman codes do not exceed the max allowable length."""
    if all(len(code) <= max_length for code in huffman_dict.values()):
        return huffman_dict

    sorted_items = sorted(huffman_dict.items(), key=lambda x: len(x[1]))
    new_codes = {}
    current_code = 0

    for symbol, _ in sorted_items:
        bin_code = format(current_code, 'b').zfill(max_length)
        new_codes[symbol] = bin_code
        current_code += 1
    return new_codes

### COLOR UTILITIES


def color_euclidean_distance_sqr(c1: Tuple[int, int, int], c2: Tuple[int, int, int]) -> float:
    """Calculate the squared Euclidean distance between two RGB colors."""
    return sum((a - b) ** 2 for a, b in zip(c1, c2))

MAX_COLOR_DISTANCE_SQR = color_euclidean_distance_sqr((255, 255, 255), (0, 0, 0))

def color_is_similar(c1: Tuple[int, int, int], c2: Tuple[int, int, int], threshold_percentage_sqr: float = 0.0625) -> bool:
    """Check if two colors are similar based on a threshold."""
    threshold_distance = MAX_COLOR_DISTANCE_SQR * threshold_percentage_sqr
    return color_euclidean_distance_sqr(c1, c2) <= threshold_distance

def average_color(colors: List[Tuple[int, int, int]], weights: List[int]) -> Tuple[int, int, int]:
    """Calculate the weighted average color."""
    avg = lambda channel: int(round(np.average([color[channel] for color in colors], weights=weights)))
    return (avg(0), avg(1), avg(2))

def find_closest_color(pixel: Tuple[int, int, int], palette: List[Tuple[int, int, int]]) -> int:
    """Find the index of the closest color in the palette."""
    distances = [color_euclidean_distance_sqr(pixel, color) for color in palette]
    return int(np.argmin(distances))
