import numpy as np
from scipy.cluster.hierarchy import linkage, fcluster
from bitarray import bitarray
from typing import List, Tuple

from Converter.encoder.utils import align_to_byte


def calculate_frame_similarity(frame1: np.ndarray, frame2: np.ndarray) -> float:
    """
    Calculate the similarity between two frames based on their RGB pixel values.
    Using Mean Squared Error (MSE) as a dissimilarity measure.
    """
    return np.mean((frame1 - frame2) ** 2)


def compute_pairwise_distances(frames: List[np.ndarray]) -> List[float]:
    """
    Compute the pairwise distances (similarities) between consecutive frames.
    """
    distances = []
    for i in range(len(frames) - 1):
        distances.append(calculate_frame_similarity(frames[i], frames[i + 1]))
    return distances


def calculate_dynamic_clusters(distances: List[float], threshold: float = 1.0) -> int:
    """
    Dynamically determine the number of clusters based on the distances between frames.
    
    Args:
        distances: List of frame-to-frame distances (similarities).
        threshold: A threshold to determine the level of change between frames.
        
    Returns:
        The dynamically determined number of clusters.
    """
    # Calculate the standard deviation of the distances
    std_dev = np.std(distances)

    # Initialize the number of clusters to 1 (start with one cluster)
    n_clusters = 1
    
    # Count the number of large differences (changes above the threshold)
    threshold_dev = threshold * std_dev
    for distance in distances:
        # If the distance is more than threshold standard deviations
        if distance > threshold_dev:
            n_clusters += 1

    return n_clusters


def find_optimal_palette_segments(frames: List[List[List[Tuple[int, int, int]]]], target_max_palette_segments_percent: float = 1.0) -> List[int]:
    """
    Find the starting indices of optimal palette segments based on dynamic clustering.
    
    Args:
        frames: List of frames, where each frame is a matrix of RGB tuples.
        target_max_palette_segments_percent: 0.0 -> 1.0 percentage (0 is 0%, 1 is 100%) of the maximum palette size with respect to total number of frames
    
    Returns:
        A list of indices where new palettes should be applied.
    """
    if len(frames) < 2:  # Just one segment: it starts at index 0
        return [0]

    # Convert frames to NumPy arrays for efficient computation
    np_frames = [np.array(frame, dtype=np.float32).reshape(-1, 3) for frame in frames]

    # Compute pairwise distances (optimized: only between consecutive frames)
    distances = [np.linalg.norm(np_frames[i] - np_frames[i + 1], axis=1).mean()
                 for i in range(len(np_frames) - 1)]
    
    # Dynamically determine the number of clusters
    target_n_clusters = calculate_dynamic_clusters(distances)
    max_palette_segments_number = max(1, int(target_max_palette_segments_percent * len(frames)))
    target_n_clusters = min(target_n_clusters, max_palette_segments_number)

    # Use a condensed distance matrix directly
    condensed_distances = np.array([
        calculate_frame_similarity(np_frames[i], np_frames[j])
        for i in range(len(np_frames))
        for j in range(i + 1, len(np_frames))
    ])

    # Perform hierarchical clustering
    linkage_matrix = linkage(condensed_distances, method='ward')
    cluster_labels = fcluster(linkage_matrix, t=target_n_clusters, criterion='maxclust')

    # Identify the start of each cluster
    segment_starts = [0]  # Always start with the first frame
    for i in range(1, len(cluster_labels)):
        if cluster_labels[i] != cluster_labels[i - 1]:
            segment_starts.append(i)

    return segment_starts


def calculate_header_clustering(segment_starts: List[int], tot_number_of_frames: int) -> bytes:
    """
    Write the frames clustering info.

    Parameters:
    - segment_starts (List[int]): Palette change indices.

    Returns:
    - bytes: The binary representation of the frames clustering.
    """
    # note: store positive_value - 1 because it's nonsensical to store positive_value == 0
    clustering_header = bitarray(endian="big")
    
    # calculate info
    clusters_number = len(segment_starts)
    clusters_number_binary_length = clusters_number.bit_length()
    clusters_dimensions = []  # in number of frames
    for i in range(1, len(segment_starts)):
        clusters_dimensions.append(segment_starts[i] - segment_starts[i - 1])
    clusters_dimensions.append(tot_number_of_frames - segment_starts[-1])  # last cluster dimension
    max_clusters_dimensions = max(clusters_dimensions)
    max_clusters_dimensions_binary_length = max_clusters_dimensions.bit_length()
    
    # number of bits (in 5-bit binary) that will be used to represent the number of clusters
    clustering_header.extend(format(clusters_number_binary_length - 1, "05b"))
    
    # number of clusters (in clusters_number_binary_length-bit binary)
    clustering_header.extend(format(clusters_number - 1, f"0{clusters_number_binary_length}b"))
    
    # number of bits (in 5-bit binary) that will be used to represent the number of bits used to represent the number of frames present in each cluster
    clustering_header.extend(format(max_clusters_dimensions_binary_length - 1, "05b"))

    # write, for each cluster in order, the number of frames per cluster
    for cluster_dimension in clusters_dimensions:
        clustering_header.extend(format(cluster_dimension - 1, f"0{max_clusters_dimensions_binary_length}b"))
        
    # align to a byte boundary
    align_to_byte(clustering_header)

    return bytes(clustering_header)