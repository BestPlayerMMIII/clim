import os
from bitarray import bitarray
from typing import List, Tuple, Iterator

from Converter.encoder.frames_encoder import encode_frames
from Converter.encoder.clustering import calculate_header_clustering
from Converter.encoder.utils import (
    ensure_directory_exists,
    remove_directory,
    get_filename_without_extension,
    extract_audio,
)
from Converter.encoder.preferences import get_preferences

Preferences = get_preferences()


def create_standard_format_header(width: int, height: int, fps: float) -> bitarray:
    """
    Create the initial part of the CLIM file, including Mode, Width, Height, MSBF, and placeholder for IFBA.

    Args:
        width (int): Video width in pixels.
        height (int): Video height in pixels.
        fps (float): Frames per second.

    Returns:
        bitarray: The standard format header as a bitarray.
    """
    clim_header = bitarray(endian="big")

    # Mode (8 bits: m7-m2 = 0, m1 = 0 for standard, m0 = 1)
    clim_header.extend("00000001")

    # Width (2 Bytes)
    clim_header.extend(format(width, "016b"))

    # Height (2 Bytes)
    clim_header.extend(format(height, "016b"))

    # MilliSeconds Between Frames (2 Bytes)
    msbf = int(round(1000 / fps))
    clim_header.extend(format(msbf, "016b"))

    return clim_header


def process_chunks(
    frame_chunks_iterator: Iterator[List[List[List[Tuple[int, int, int]]]]],
    number_of_chunks: int,
    video_name: str,
    chunk_dir: str,
) -> Tuple[int, List[str], List[int]]:
    """
    Process video frame chunks, encode them, and store them temporarily.

    Args:
        frame_chunks_iterator (Iterator): Iterator yielding video frame chunks.
        number_of_chunks (int): Total number of chunks.
        video_name (str): Name of the video file.
        chunk_dir (str): Directory to store chunk files.

    Returns:
        Tuple[int, List[str], List[int]]: Updated IFBA, list of chunk file paths, and cluster start indices.
    """
    multichunks_cluster_start_indices = [0]
    IFBA = 1 + (2 + 2 + 2 + 5)  # Mode + (Standard Format Header)
    chunk_filepaths = []

    for chunk_idx, chunk in enumerate(frame_chunks_iterator):
        print(f" Encoding chunk #{chunk_idx + 1}/{number_of_chunks}...")

        # Encode chunk and calculate cluster indices
        cluster_start_indices, encoded_frames = encode_frames(chunk)
        offset = multichunks_cluster_start_indices[-1]
        for cluster_start_index in cluster_start_indices[1:]:
            multichunks_cluster_start_indices.append(cluster_start_index + offset)
        multichunks_cluster_start_indices.append(len(chunk) + offset)

        # Save encoded frames to a temporary file
        chunk_filepath = os.path.join(chunk_dir, f"{video_name}--chunk_{chunk_idx}.bin")
        chunk_filepaths.append(chunk_filepath)
        with open(chunk_filepath, mode="wb") as chunk_file:
            chunk_file.write(encoded_frames)

        # Update IFBA with the size of the current chunk
        IFBA += len(encoded_frames)

    return IFBA, chunk_filepaths, multichunks_cluster_start_indices


def extract_and_attach_audio(
    video_filepath: str, clim_filepath: str, fps: float
) -> None:
    """
    Extract audio from the video file, process its playback speed, and append it to the CLIM file.

    Args:
        video_filepath (str): Path to the video file.
        clim_filepath (str): Path to the CLIM file being created.
        multimedia_target_name (str): Base name for temporary audio files.
        fps (float): Frames per second of the video.
    """
    # Video pure name
    multimedia_target_name = get_filename_without_extension(video_filepath)

    # Calculate audio speed factor based on FPS approximation
    audio_speed_factor = round(fps) / fps

    try:
        # Temporary folder for audio extraction
        hidden_audio_dir = "./.audio_extraction/"
        remove_directory(hidden_audio_dir)
        ensure_directory_exists(hidden_audio_dir)

        # Extract audio
        audio_filepath = os.path.join(
            hidden_audio_dir, multimedia_target_name + Preferences.AUDIO_EXTRACTION_EXTENSION
        )
        
        if extract_audio(video_filepath, audio_filepath, bitrate="8k", speed_percentage=audio_speed_factor):
            # Append audio to CLIM file
            with open(clim_filepath, mode="ab") as clim_file:
                with open(audio_filepath, mode="rb") as audio_file:
                    clim_file.write(audio_file.read())

    finally:
        # Cleanup temporary audio directory
        remove_directory(hidden_audio_dir)


def encode_video_to_clim(
    video_filepath: str,
    clim_filepath: str,
    frame_chunks_iterator: Iterator[List[List[List[Tuple[int, int, int]]]]],
    number_of_chunks: int,
    width: int,
    height: int,
    fps: float,
) -> None:
    """
    Main function to encode a video into CLIM format.

    Args:
        video_filepath (str): Path to the input video file.
        clim_filepath (str): Path to the output CLIM file.
        frame_chunks_iterator (Iterator): Iterator of frame chunks.
        number_of_chunks (int): Total number of frame chunks.
        width (int): Video width in pixels.
        height (int): Video height in pixels.
        fps (float): Frames per second.
    """
    # Create standard format header
    clim_file_content = create_standard_format_header(width, height, fps)

    try:
        # Create temporary directory for chunks
        chunk_dir = "./.video_chunks/"
        remove_directory(chunk_dir)
        ensure_directory_exists(chunk_dir)

        # Process video frame chunks
        IFBA, chunk_filepaths, cluster_indices = process_chunks(
            frame_chunks_iterator, number_of_chunks, os.path.basename(video_filepath), chunk_dir
        )

        # Calculate clustering header
        cluster_indices, frame_count = cluster_indices[:-1], cluster_indices[-1]
        header_clustering = calculate_header_clustering(cluster_indices, frame_count)
        IFBA += len(header_clustering)

        # Update standard format header with IFBA
        clim_file_content.extend(format(IFBA, "040b"))
        clim_file_content.frombytes(header_clustering)

        # Write chunks and header to CLIM file
        with open(clim_filepath, mode="wb") as clim_file:
            clim_file.write(clim_file_content)
            for chunk_filepath in chunk_filepaths:
                with open(chunk_filepath, mode="rb") as chunk_file:
                    clim_file.write(chunk_file.read())

    finally:
        # Cleanup temporary chunks directory
        remove_directory(chunk_dir)

    # Attach audio to CLIM file
    extract_and_attach_audio(video_filepath, clim_filepath, fps)

    print(f"CLIM file created successfully: {clim_filepath}")
