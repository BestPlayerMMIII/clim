# CLIM Project: Command Line Interface Multimedia

Welcome to the **CLIM Project**, an experimental tool for converting multimedia files into a custom format (`.clim`) and playing them directly in the command line as colored "pixels". This project is an initial version, developed as a learning exercise in multimedia processing, compression, and command-line visualization. It is designed for experimentation and feedback.

---

## Features
1. **Converter**:
   - Converts multimedia files (e.g., MP4, AVI) into the `.clim` format.
   - Compresses data efficiently for lightweight command-line playback.

2. **Player**:
   - Displays `.clim` files in the terminal using background-colored characters.
   - Cross-platform support for Linux, macOS, and Windows.

3. **YouTube Downloader**
This project includes a Python script that uses the public library `yt_dlp` for downloading videos from YouTube.  

**Disclaimer:**  
- The library `yt_dlp` is distributed under the [Unlicense license](https://github.com/yt-dlp/yt-dlp/blob/master/LICENSE), allowing open and unrestricted use.  
- However, downloading copyrighted content without authorization may violate local or international laws.  
- As the creator of this project, I **do not condone** or support any illegal use of the software. Users are fully responsible for ensuring their actions comply with copyright and intellectual property laws in their jurisdiction.  

---

## Requirements

### General
- **FFmpeg**: Ensure it is installed and accessible from your terminal. Download it from [ffmpeg.org](https://ffmpeg.org).

### Python (Converter)
- Python 3.10+
- Install dependencies:
  ```bash
  pip install -r Converter/requirements.txt
  ```

### C++ (Player)
- A modern C++ compiler supporting C++17.
- Recommended build tools:
  - `Make` (Linux/macOS)
  - `Visual Studio` (Windows)
  - `CMake` (Cross-platform).

---

## Installation

### 1. Clone the Repository
```bash
git clone https://github.com/BestPlayer/clim.git
cd clim
```

### 2. Install Dependencies

#### Converter
```bash
pip install -r Converter/requirements.txt
```

#### Player
Compile the C++ code:
- Using `Make`:
  ```bash
  cd Player
  make
  ```
- Using `CMake`:
  ```bash
  cd Player
  mkdir build && cd build
  cmake ..
  make
  ```

---

## Usage

### Convert a Video File
```bash
python Converter/clim_converter/converter.py input.mp4 output.clim
```

### Play a `.clim` File
```bash
./Player/CLIM_Player output.clim
```

---

## Example Workflow
1. Download a video from YouTube:
   ```bash
   python Converter/yt_downloader.py "https://www.youtube.com/watch?v=example" -o video.mp4
   ```
2. Convert the downloaded video:
   ```bash
   python Converter/clim_converter/converter.py video.mp4 output.clim
   ```
3. Play the converted file in the terminal:
   ```bash
   ./Player/CLIM_Player output.clim
   ```

---

## Current Limitations
- This is an **early version** focused on experimentation.
- Optimizations for speed and quality are ongoing.
- Playback is designed for **low-resolution visuals** due to terminal constraints (default: 103x29 pixels).
- Error handling is minimal; feedback is welcome!

---

## Goals and Future Directions
This project is a starting point for exploring:
- Advanced video compression algorithms.
- Real-time playback optimizations.
- Expanding format support for the converter and player.

I encourage contributions and feedback as I continue to improve!

---

## License
This project is open-source under the CC BY-NC 4.0 License. See the [`LICENSE`](https://github.com/BestPlayerMMIII/clim/blob/main/LICENSE) file for details.

---

## Acknowledgments
Thanks to the developers of open-source tools like FFmpeg, yt-dlp, and the Python/C++ communities for their incredible resources and libraries.
