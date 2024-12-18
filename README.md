# CLIM Project: Command Line Interface Multimedia

This is the **CLIM Project**, an experimental tool for converting multimedia files into a custom format (`.clim`) and playing them directly in the command line as colored "pixels" (spaces). This project is an initial version, developed as a learning exercise in multimedia processing, compression, command-line visualization and complex project management. It is designed for experimentation and feedback.


## Features
1. **Converter**:
   - Converts multimedia files (e.g., MP4, AVI) into the `.clim` format.
   - Compresses data efficiently for lightweight command-line playback.

2. **Player**:
   - Displays `.clim` files in the terminal using background-colored space characters.
   - Cross-platform support for Linux, macOS, and Windows.

3. **YouTube Downloader**
This project includes a Python script that uses the public library `yt_dlp` for downloading videos from YouTube.  

**Disclaimer:**  
- The library `yt_dlp` is distributed under the [Unlicense license](https://github.com/yt-dlp/yt-dlp/blob/master/LICENSE), allowing open and unrestricted use.  
- However, downloading copyrighted content without authorization may violate local or international laws.  
- As the creator of this project, I **do not condone or support** any illegal use of the software. Users are fully responsible for ensuring their actions comply with copyright and intellectual property laws in their jurisdiction.  


## Requirements

### General
- **FFmpeg**: Ensure it is installed and accessible from your terminal.
   - **Download**
      - download it from the official site [ffmpeg.org](https://ffmpeg.org)
      - **OR** directly download the build from GitHub: [Latest FFmpeg Release](https://github.com/BtbN/FFmpeg-Builds/releases/tag/latest)
        and follow this guide on how to [add FFmpeg's bin Directory to the System Path Variable](docs/ffmpeg_path_setup.md).
   
   - It is used for the Converter (both audio and video) and for the Player (just for the audio).

### Python (Converter)
- Python 3.10+

### C++ (Player)
- The player supports C++11 and requires g++ and Make or CMake to compile with the provided Makefile or CMakeLists.txt.

Recommended build tools:
- Make (Linux/macOS)
- Visual Studio (Windows)
- CMake (Cross-platform)

It has been tested successfully on Windows 10 and Windows 11 and should work across all platforms.


## Installation

### 1. Clone the Repository
```bash
git clone https://github.com/BestPlayer/clim.git
cd clim
```

### 2. Install Dependencies

#### Converter
Make sure you have [Python 3.10 or higher](https://www.python.org/downloads/) installed.

Then - from the terminal or cmd - type this command inside the cloned repository folder `clim`:
```bash
pip install -r requirements.txt
```

#### Player
Compile the C++ code.

To build the project using `Make` (Linux/macOS):
```bash
cd Player
make
```

If some errors are shown, please follow the [make troubleshooting guide](docs/troubleshooting/not_recognized_make.md).

To build the project using `CMake` (Cross-platform: Windows/Linux/macOS):
```bash
cd Player
mkdir build && cd build
cmake ..
make
```

> Remember to use `mingw32-make` instead of `make` if you [installed Make with MinGW](docs/troubleshooting/not_recognized_make.md#option-a-preferred-install-make-via-mingw) for Windows.

If some errors are shown, please follow the [cmake troubleshooting guide](docs/troubleshooting/not_recognized_cmake.md).


---

## Usage

Note: the following commands are meant to be executed from the root folder `clim`.

### 1. Download a video from YouTube (optional)

**Usage**

```bash
yt_downloader.py [-h] [-o OUTPUT] [-f FORMAT] url

positional arguments:
  url                   YouTube video URL

options:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        Output filename without extension (default: {video-title}.mp4)
  -f FORMAT, --format FORMAT
                        Download format option (default: chosen by yt-dlp).
```

**Note**: the extension ".mp4" will be added at the end of the provided video name.


### 2. Convert a Video File to CLIM format

**Usage**

```bash
python -m Converter.main [-h] [--width WIDTH] [--height HEIGHT] [--fps FPS] [--chunk-size CHUNK_SIZE]
                              [--preprocess | --no-preprocess] input output

positional arguments:
  input                 Path to the input video file.
  output                Path to the output CLIM file.

options:
  -h, --help                 show this help message and exit
  --width WIDTH              Target video width (default: 103).
  --height HEIGHT            Target video height (default: 29).
  --fps FPS                  Target frames per second (default: 12).
  --chunk-size CHUNK_SIZE    Max chunk size for encoding (default: 256).
  --preprocess               Enable video preprocessing (default: enabled).
  --no-preprocess            Disable video preprocessing.
```


### 3. Play a `.clim` File in the CLI

**Usage**

```bash
"./Player/player" [--loop] input

positional arguments:
  input                 Path to the input file in CLIM format.

options:
  --loop LOOP           Enable video loop (default: enabled).
```


## Example Workflow
**1. Download a video from YouTube**:
   ```bash
   python yt_downloader.py https://www.youtube.com/watch?v=BBJa32lCaaY -o videos/video
   ```
   A file "video.mp4" should appear in the folder "videos".

**2. Convert the downloaded video**:
   ```bash
   python -m Converter.main videos/video.mp4 clims/video.clim
   ```
   A file "video.clim" should appear in the folder "clims".

**3. Play the converted file in the terminal**:
   ```bash
   "./Player/player" --loop clims/video.clim
   ```

**Note**:

- to interrupt the video execution press `ctrl+C`;
- if you want the video to start in another CLI, add `start` before the command:
   ```bash
   start ./Player/player --loop clims/video.clim
   ```

**Expected result for the example**

You got rickrolled by a CLIM file in a Command Line Interface (before GTA 6).

---

## CLIM File Format Documentation
For detailed information on the implementation of the CLIM file format, please refer to the [clim_format](docs/clim_format.md) file.


## Current Limitations
- This is an **early version** focused on experimentation.
- Speed ​​and quality optimizations will be considered for future versions of this project.
- Playback is designed for **low-resolution visuals** due to terminal constraints (default: 103x29 pixels).
- Error handling is minimal; feedback is welcome!


## Goals and Future Directions
This project is a starting point for exploring:
- Advanced video compression algorithms.
- Real-time playback optimizations.
- Expanding format support for the converter and player.

I encourage contributions and feedback as I continue to improve!

### Planned Features for Future Releases

1. **Playback Controls**
   - Implementation of essential playback functions such as:
     - **Rewind**: Navigate backward within the video.
     - **Fast-Forward**: Skip ahead in the video timeline.
     - **Stop**: Pause and restart playback as needed.
       > Note: while it is technically possible to pause playback by clicking on the terminal window with the mouse (which stops video output) and then pressing Enter to resume, this is not an intended feature.
       > During this "pause", the audio continues to play in the background.

2. **Enhanced Compression**
   - Improve video compression techniques for better storage efficiency and, most importantly, faster encoding - even for higher resolutions.

3. **Removal of FFmpeg Dependency for Audio**
   - Develop a proprietary audio decoding solution to eliminate reliance on FFmpeg for the CLIM Player.
   - Ensure full compatibility and flexibility for integrating audio.

4. **Reusable Player Framework**
   - Package the Player as a library for integration into other projects.
  
5. **Subtitle Support**
   - Add support for embedding and displaying subtitles in standard formats such as SRT or VTT.
   - Ensure synchronization with video playback and allow user customization (e.g., font size, color).
  
6. **Subtitle Support**
   - Add support for embedding and displaying sub


---

### Disclaimer

This software is provided "as is," without any express or implied warranties, including but not limited to warranties of merchantability, fitness for a particular purpose, or non-infringement. By using this application, you agree to the following terms:

1. **No Liability for Damages**:
   - The developer assumes no responsibility for any damages, direct or indirect, caused by the use of this application.
   - This includes, but is not limited to, damages to hardware, software, data loss, or any other issues resulting from proper or improper use of the application.

2. **No Warranty**:
   - The application is distributed without guarantees of performance, compatibility, or functionality.
   - Use at your own risk, and ensure thorough testing before deploying in critical environments.

3. **Third-Party Dependencies**:
   - Any issues caused by third-party libraries, including but not limited to FFmpeg, fall outside the scope of the developer's liability.

4. **User Responsibility**:
   - It is the user’s responsibility to ensure the legality of using this software in their region.
   - The developer is not liable for violations of copyright, licensing, or other regulations caused by the misuse of this application.

5. **No Legal Support**:
   - The developer does not provide legal or technical support for situations arising from disputes, damages, or other claims related to the use of this software.


## License
This project is open-source under the CC BY-NC 4.0 License. See the [`LICENSE`](LICENSE) file for details.
The license provides a general framework for usage, but the disclaimer above ensures additional legal protection for the developer in any unforeseen situations. 


## Acknowledgments
Thanks to the developers of open-source tools like FFmpeg, yt-dlp, and the Python/C++ communities for their incredible resources and libraries.
