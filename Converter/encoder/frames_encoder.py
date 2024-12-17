from typing import List, Dict, Tuple

from Converter.encoder.palette import (
    generate_palettes_frequencies,
    calculate_header_palette,
    create_palette_for_clusters
)
from Converter.encoder.clustering import find_optimal_palette_segments
from Converter.encoder.encoding_method_selector import calculate_best_encoding
from Converter.encoder.scanners import scan_frame_line_by_line
from Converter.encoder.utils import huffman_coding
from Converter.encoder.preferences import get_preferences

Preferences = get_preferences()


def encode_frames(frames: List[List[List[Tuple[int, int, int]]]]) -> Tuple[List[int], bytes]:
    """
    Encodes a list of frames into cluster metadata and encoded frame bytes.

    Args:
        frames (List[List[List[Tuple[int, int, int]]]]): List of frames, where each frame is a 2D grid of RGB tuples.

    Returns:
        Tuple[List[int], bytes]: 
            - List of cluster start indices.
            - Encoded bytes representing the frame data and cluster information.
    """
    print("   start - Finding frame cluster boundaries...")  # Debug
    cluster_start_indices = find_optimal_palette_segments(frames)

    palettes, pixel_indices = create_palette_for_clusters(
        cluster_start_indices, 
        frames, 
        Preferences.MAX_COLORS_PER_PALETTE,
        desc="   done - Generating palettes and pixel mappings"  # Debug
    )

    print("   done - Encoding each cluster...")  # Debug
    encoded_frames = encode_clusters(
        palettes, pixel_indices, cluster_start_indices, frames
    )

    print("   done")  # Debug
    return cluster_start_indices, bytes(encoded_frames)


def encode_clusters(
    palettes: List[List[Tuple[int, int, int]]],
    pixel_indices: List[List[int]],
    cluster_start_indices: List[int],
    frames: List[List[List[Tuple[int, int, int]]]]
) -> bytearray:
    """
    Encodes all clusters into a byte array.

    Args:
        palettes (List[List[Tuple[int, int, int]]]): Palettes for each cluster.
        pixel_indices (List[List[int]]): Pixel indices for each cluster.
        cluster_start_indices (List[int]): Cluster boundaries.
        frames (List[List[List[Tuple[int, int, int]]]]): List of frames.

    Returns:
        bytearray: Encoded frames as a byte array.
    """
    encoded_frames = bytearray()
    frames_count = len(frames)
    max_color_index_bits = Preferences.MAX_COLORS_PER_PALETTE.bit_length()

    frequencies_list = generate_palettes_frequencies(palettes, pixel_indices)

    for cluster_index, frequencies in enumerate(frequencies_list):
        # Step 1: Generate Huffman codes and header
        huffman_codes = huffman_coding(frequencies, max_length=max_color_index_bits)
        encoded_frames.extend(calculate_header_palette(huffman_codes))

        # Step 2: Encode each frame in the cluster
        start_idx = cluster_start_indices[cluster_index]
        end_idx = cluster_start_indices[cluster_index + 1] if cluster_index + 1 < len(cluster_start_indices) else frames_count

        for frame_index in range(start_idx, end_idx):
            encoded_frames.extend(
                encode_frame(frames[frame_index], huffman_codes)
            )

    return encoded_frames


def encode_frame(frame: List[List[Tuple[int, int, int]]], huffman_codes: dict) -> bytes:
    """
    Encodes a single frame using RLE and Huffman codes.

    Args:
        frame (List[List[Tuple[int, int, int]]]): Frame to encode as a 2D grid of RGB tuples.
        huffman_codes (dict): Huffman codes for encoding the frame.

    Returns:
        bytes: Encoded frame as a byte array.
    """
    flat_frame = scan_frame_line_by_line(frame)
    rle_indices, huffman_codebook = generate_rle_indices_and_codebook(huffman_codes, flat_frame)
    encoding_result, _ = calculate_best_encoding(rle_indices, huffman_codebook)
    return encoding_result["all"]


def generate_rle_indices_and_codebook(
    huffman_codes: Dict[Tuple[int, int, int], str],
    flat_frame: List[Tuple[int, int, int]]
) -> Tuple[List[Tuple[int, int]], Dict[int, str]]:
    """
    Transform a dictionary of Huffman codes (indexed by RGB tuples) into RLE indices and a Huffman codebook.

    Args:
        huffman_codes (Dict[Tuple[int, int, int], str]): Huffman codes indexed by RGB tuples.
        flat_frame (List[Tuple[int, int, int]]): Pixels of the frame to process (scanned line by line or in another order).

    Returns:
        Tuple[List[Tuple[int, int]], Dict[int, str]]:
            - rle_indices: List of tuples (codebook index, consecutive count).
            - huffman_codebook: Huffman codebook with integer indices.
    """
    # Create mapping from RGB tuples to indices and the Huffman codebook
    colors_indices = {rgb: i for i, rgb in enumerate(huffman_codes)}
    huffman_codebook = dict(enumerate(huffman_codes.values()))

    # Initialize variables for RLE
    rle_indices = []
    current_index, current_count = None, 0

    # Iterate through pixels and generate RLE indices
    for pixel in flat_frame:
        index = colors_indices[pixel]
        if index == current_index:
            current_count += 1
        else:
            if current_index is not None:
                rle_indices.append((current_index, current_count))
            current_index, current_count = index, 1

    # Append the final block if it exists
    if current_index is not None:
        rle_indices.append((current_index, current_count))

    return rle_indices, huffman_codebook
