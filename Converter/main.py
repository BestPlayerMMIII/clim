import argparse
import time

from Converter.encoder.clim_encoder import encode_video_to_clim
from Converter.video.framer import VideoFramer
from Converter.encoder.preferences import get_preferences


def convert_video(input_path, output_path, width, height, fps, preprocess, chunk_size):
    """
    Converts a video to the CLIM format with specified options.

    Args:
        input_path (str): Path to the input video file.
        output_path (str): Path to save the output CLIM file.
        width (int): Target video width.
        height (int): Target video height.
        fps (int): Target frames per second.
        preprocess (bool): Whether to preprocess the video before conversion.
        chunk_size (int): Maximum chunk size for encoding.
    """
    try:
        # Initialize preferences
        preferences = get_preferences()
        preferences.set_target_dimensions(width, height)
        preferences.set_fps(fps)
        preferences.set_preprocess_video(preprocess)
        preferences.set_chunk_size(chunk_size)

        # Start conversion timer
        start_time = time.time()

        # Create a video framer for extracting frames
        print(f"Processing video: {input_path}")
        framer = VideoFramer(input_path, width, height, fps, preprocess_video=preprocess)

        # Encode video to CLIM format
        print(f"Converting video to CLIM format: {output_path}")
        encode_video_to_clim(
            input_path,
            output_path,
            framer.get_frame_RGB_chunks(chunk_size),
            framer.calculate_number_of_chunks(chunk_size),
            width,
            height,
            fps
        )

        # Display conversion success and time
        elapsed_time = time.time() - start_time
        print(f"Conversion complete! Elapsed time: {elapsed_time:.2f} seconds")

    except Exception as e:
        print(f"Error during conversion: {e}")


def parse_arguments():
    """
    Parses command-line arguments for the video conversion script.

    Returns:
        argparse.Namespace: Parsed arguments.
    """
    preferences = get_preferences()

    parser = argparse.ArgumentParser(description="Convert a video file to CLIM format.")
    
    # Required arguments
    parser.add_argument("input", type=str, help="Path to the input video file.")
    parser.add_argument("output", type=str, help="Path to the output CLIM file.")
    
    # Optional arguments with defaults from preferences
    parser.add_argument("--width", type=int, default=preferences.TARGET_WIDTH,
                        help=f"Target video width (default: {preferences.TARGET_WIDTH}).")
    parser.add_argument("--height", type=int, default=preferences.TARGET_HEIGHT,
                        help=f"Target video height (default: {preferences.TARGET_HEIGHT}).")
    parser.add_argument("--fps", type=int, default=preferences.TARGET_FPS,
                        help=f"Target frames per second (default: {preferences.TARGET_FPS}).")
    parser.add_argument("--chunk-size", type=int, default=preferences.MAX_CHUNK_SIZE,
                        help=f"Max chunk size for encoding (default: {preferences.MAX_CHUNK_SIZE}).")
    
    # Preprocessing flag with mutually exclusive options
    preprocess_group = parser.add_mutually_exclusive_group()
    preprocess_group.add_argument("--preprocess", action="store_true", default=preferences.PREPROCESS_VIDEO,
                                   help="Enable video preprocessing (default: enabled).")
    preprocess_group.add_argument("--no-preprocess", action="store_false", dest="preprocess",
                                   help="Disable video preprocessing.")
    
    return parser.parse_args()


if __name__ == '__main__':
    # Parse command-line arguments
    args = parse_arguments()

    # Execute the video conversion
    convert_video(
        input_path=args.input,
        output_path=args.output,
        width=args.width,
        height=args.height,
        fps=args.fps,
        preprocess=args.preprocess,
        chunk_size=args.chunk_size
    )
