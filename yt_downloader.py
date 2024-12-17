import yt_dlp
import argparse

VIDEO_EXTENSION = 'mp4'

def download_video(youtube_url, output_file, format_option):
    ydl_opts = {
        'merge_output_format': VIDEO_EXTENSION,     # Merge audio and video in VIDEO_EXTENSION format
        'outtmpl': output_file,                     # Save with specified name
    }
    if format_option is not None:
        ydl_opts['format'] = format_option          # User-defined format

    try:
        with yt_dlp.YoutubeDL(ydl_opts) as ydl:
            ydl.download([youtube_url])
            print("Download complete!")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Download videos from YouTube")
    parser.add_argument('url', type=str, help="YouTube video URL")
    parser.add_argument('-o', '--output', type=str, default='%(title)s.%(ext)s', help=f"Output filename without extension (default: {{youtube-video-title}}.{VIDEO_EXTENSION})")
    parser.add_argument('-f', '--format', type=str, default=None, help=("Download format option (default: chosen by yt-dlp)."))

    args = parser.parse_args()

    download_video(args.url, args.output, args.format)
