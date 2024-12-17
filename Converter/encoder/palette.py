import numpy as np
from sklearn.cluster import KMeans
from bitarray import bitarray
from collections import Counter
from tqdm import tqdm as progressbar
from typing import List, Dict, Tuple

from Converter.encoder.utils import align_to_byte


def generate_palettes_frequencies(
    palettes: List[List[Tuple[int, int, int]]], 
    pixel_indices: List[List[List[List[int]]]]
) -> List[Dict[Tuple[int, int, int], int]]:
    """
    Generate palettes and frequencies for each segment based on pixel indices.

    Parameters:
    - palettes: List of palettes, where each palette is a list of RGB tuples.
    - pixel_indices: List of pixel index mappings for each frame in each segment.

    Returns:
    - A list of frequency dictionaries, one for each segment, where each dictionary maps an RGB tuple to its frequency.
    """
    frequencies = []

    # Iterate over each segment
    for palette, segment_indices in zip(palettes, pixel_indices):
        # Flatten all indices in the current segment
        all_indices = [index for frame in segment_indices for row in frame for index in row]
        
        # Count occurrences of each index
        index_counts = Counter(all_indices)
        
        # Map counts to the corresponding colors in the palette
        color_frequencies = {tuple(palette[i]): count for i, count in index_counts.items()}
        
        # Add the frequency dictionary for this segment
        frequencies.append(color_frequencies)
    
    return frequencies


def calculate_header_palette(huffman_codes: Dict[Tuple[int, int, int], str]) -> bytes:
    """
    Write the palette and Huffman codes to a binary format with optimized storage.

    Parameters:
    - huffman_codes (Dict[Tuple[int, int, int], str]): Huffman codes for each color in the palette.

    Returns:
    - bytes: The binary representation of the palette and codes.
    """
    binary_data = bytearray()
    huffman_lengths = []
    huffman_bitstream = bitarray(endian="big")

    palette = huffman_codes.keys()

    # Write the number of colors
    binary_data.append(len(palette) - 1)  # Note: length - 1 because length == 0 is nonsensical and will never be so

    # Write the colors (3 bytes per color)
    for color in palette:
        binary_data.extend(color)

        # Calculate the length of the Huffman code
        huffman_code = huffman_codes[color]
        huffman_lengths.append(len(huffman_code))

        # Add the Huffman code to the bitstream
        huffman_bitstream.extend(huffman_code)

    # Pack Huffman lengths (3 bits each)
    length_bits = bitarray(endian="big")
    for length in huffman_lengths:
        length_bits.extend(format(length - 1, "03b"))  # Convert length to 3-bit binary.
        # Note: length - 1 because length is in [1; 8] and 8 cannot be represented with 3 bits

    # Align length_bits to a byte boundary
    align_to_byte(length_bits)

    # Add length bits to binary data
    binary_data.extend(length_bits.tobytes())

    # Align Huffman bitstream to a byte boundary
    align_to_byte(huffman_bitstream)

    # Add Huffman bitstream to binary data
    binary_data.extend(huffman_bitstream.tobytes())

    return bytes(binary_data)


def create_palette_for_clusters(
    cluster_start_indices: List[int],
    frames: List[List[List[Tuple[int]]]],
    max_colors_per_palette: int,
    desc: str | None = None
) -> Tuple[List[List[Tuple[int]]], List[List[List[List[int]]]]]:
    """
    Create optimal palettes for clusters and return pixel indices in the respective palettes.

    Parameters:
    - cluster_start_indices (List[int]): Start indices for each cluster.
    - frames (List[List[List[Tuple[int]]]]): 3D list of frames with RGB tuples. 
      NOTE: frames pixels will be set equal to the most similar color in the palette.
    - max_colors_per_palette (int): Max number of colors per palette.
    - desc (str): Description to show as a label in the progressbar (default is None).

    Returns:
    - Tuple[List[List[Tuple[int]]], List[List[List[List[int]]]]]:
      - Palettes for each cluster.
      - Pixel indices in palettes for each frame.
    """
    def calculate_palette_and_indices(cluster_pixels, frame_pixels, max_colors):
        """Helper function to create palette and remap pixel indices."""
        # Find unique pixels and count occurrences
        unique_pixels, counts = np.unique(cluster_pixels, axis=0, return_counts=True)

        # Apply KMeans clustering
        kmeans = KMeans(n_clusters=min(max_colors, len(unique_pixels)), random_state=42)
        kmeans.fit(unique_pixels, sample_weight=counts)
        labels = kmeans.predict(unique_pixels)
        palette = np.round(kmeans.cluster_centers_).astype(int).tolist()

        # Map each unique pixel to its closest color in the palette
        pixel_to_palette = {tuple(unique_pixels[i]): labels[i] for i in range(len(unique_pixels))}

        # Generate the remapped pixel indices for all frames
        remapped_frames = [
            [[pixel_to_palette[tuple(pixel)] for pixel in row] for row in frame]
            for frame in frame_pixels
        ]

        # Update frames in place to reflect palette-mapped colors
        for frame in frame_pixels:
            for i, row in enumerate(frame):
                for j, _ in enumerate(row):
                    row[j] = tuple(palette[remapped_frames[frame_pixels.index(frame)][i][j]])

        return palette, remapped_frames

    # Final outputs
    palettes = []
    pixel_indices = []

    # Iterate through clusters
    for cluster_index in progressbar(range(len(cluster_start_indices)), desc=desc):
        start_idx = cluster_start_indices[cluster_index]
        end_idx = cluster_start_indices[cluster_index + 1] if cluster_index + 1 < len(cluster_start_indices) else len(frames)

        # Gather all pixels in the cluster
        cluster_frames = frames[start_idx:end_idx]
        cluster_pixels = np.vstack([
            np.vstack(frame) for frame in cluster_frames
        ])  # Flatten all frames into a single array of pixels

        # Calculate palette and remap pixel indices
        palette, remapped_frames = calculate_palette_and_indices(cluster_pixels, cluster_frames, max_colors_per_palette)

        # Append results to final output
        palettes.append(palette)
        pixel_indices.append(remapped_frames)

    return palettes, pixel_indices

