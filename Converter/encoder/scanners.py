from typing import List, Tuple


def scan_frame_line_by_line(frame: List[List[Tuple[int, int, int]]]) -> List[Tuple[int, int, int]]:
    """
    Flattens a frame line by line.

    Parameters:
        - frame: List[List[Tuple[int, int, int]]] -> Frame represented as a matrix of RGB pixels.

    Returns:
        - List[Tuple[int, int, int]] -> Flattened list of RGB pixels, scanned line by line.
    """
    flattened_frame = []
    for row in frame:
        flattened_frame.extend(row)
    return flattened_frame