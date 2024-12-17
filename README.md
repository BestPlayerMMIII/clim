# CLIM Project: Command Line Interface Multimedia

Welcome to the **CLIM Project**, an experimental tool for converting multimedia files into a custom format (`.clim`) and playing them directly in the command line as colored "pixels" (spaces). This project is an initial version, developed as a learning exercise in multimedia processing, compression, and command-line visualization. It is designed for experimentation and feedback.


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
- As the creator of this project, I **do not condone or support** any illegal use of the software. Users are fully responsible for ensuring their actions comply with copyright and intellectual property laws in their jurisdiction.  


## Requirements

### General
- **FFmpeg**: Ensure it is installed and accessible from your terminal. Download it from [ffmpeg.org](https://ffmpeg.org).
It is used for the Converter (both audio and video) and for the Player (just for the audio).

### Python (Converter)
- Python 3.10+

### C++ (Player)
- The player supports C++11 and requires g++ to compile the provided Makefile.
It has been tested successfully on Windows 10 and should work across all platforms where g++ is available.


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
Compile the C++ code. To build the project using `Make`:
```bash
cd Player
make
```

---

## Usage

Note: the commands are meant to be executed from the root folder `clim`.

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

---

## Example Workflow
1. Download a video from YouTube:
   ```bash
   python yt_downloader.py https://www.youtube.com/watch?v=BBJa32lCaaY -o videos/video
   ```
   A file "video.mp4" should appear in the folder "videos".

2. Convert the downloaded video:
   ```bash
   python -m Converter.main videos/video.mp4 clims/video.clim
   ```
   A file "video.clim" should appear in the folder "clims".

3. Play the converted file in the terminal:
   ```bash
   "./Player/player" --loop clims/video.clim
   ```

**Note**:

- to interrupt the video execution press ctrl+C;
- if you want the video to start in another CLI add `start` before the command:
   ```bash
   start ./Player/player --loop clims/video.clim
   ```

**Expected result for the example**

You got rickrolled by a CLIM file in a Command Line Interface (before GTA 6).

---

## CLIM File Format Documentation

The CLIM file format is a binary format designed to store compressed video efficiently, balancing storage space and computational performance. The file is structured into **mode**, **header**, **clustered video content**, and optional **audio data**, following a standard layout.


### Binary Structure of the CLIM File

#### 1. Mode Byte `M` (1 byte)
   - The first byte specifies the **mode** of the file.
   - Structure of `M`: `m7 m6 m5 m4 m3 m2 m1 m0`:
     - **`m0`**: Always `1`, indicating a valid file.
     - **`m1`**: Format type:
       - `0`: Standard format (documented here).
       - `1`: Reserved for future formats.
     - **`m2-m7`**: Undefined, reserved for future use.


#### 2. Standard Format Header (11 bytes)
   - **Width (`W`, 2 bytes)**:
     - Frame width in pixels, stored as an unsigned 16-bit integer.
     - Maximum supported width: **65,536 pixels**.
   - **Height (`H`, 2 bytes)**:
     - Frame height in pixels, stored as an unsigned 16-bit integer.
     - Maximum supported height: **65,536 pixels**.
   - **Milliseconds Between Frames (`MSBF`, 2 bytes)**:
     - Specifies the time between frames in milliseconds, as an unsigned 16-bit integer.
     - Frame rate range: from **0.015 FPS** ($` \frac{1000\; ms}{2^{16}\; msbf} `$) to **1,000 FPS** ($` \frac{1000\; ms}{1\; msbf} `$).
   - **Index of the First Byte for Audio (`IFBA`, 5 bytes)**:
     - Specifies the byte index where audio data begins.
     - Up to **1,024 GiB of video data** before the audio section starts.


#### 3. **Clustered Video Content**
   - **Clustering Header (`CH`)**:
     - A variable-length header describing the structure and metadata of the clusters that follow.
   - **Clusters**:
     - Each cluster represents a portion of video data compressed using one of the following methods:
       - **Huffman Coding**: Directly compresses pixel sequences based on frequency.
       - **Run-Length Encoding (RLE)**: Compresses repeated pixel sequences as `(pixel, count)`.
       - **RLE + Huffman**: Compresses the lengths of repeated sequences using Huffman, storing these instead of direct counts.
      

#### 3. **Audio Data**
   - Stored in binary MP3 format, starting at the byte index specified by `IFBA`.


### Compression and Processing

#### FlatFrame and Encoding
Frames are converted into a flat representation (FlatFrame), transforming a 2D matrix into a 1D sequence:
- Pixels are processed **row by row**, left to right.
- This simplifies the compression process and ensures compatibility with methods like RLE.

#### Huffman and RLE Methods
The encoding dynamically selects the most efficient compression method based on the frame content:
1. **Huffman Only**: Optimized for data with frequent patterns.
2. **RLE Only**: Effective for frames with many repeated pixel sequences.
3. **RLE + Huffman**: Combines the advantages of both methods, compressing run lengths with Huffman for additional efficiency.

---

### Why to use a maximum of 255 Pixels per Cluster?

#### Optimal Cluster Size: 255 Pixels
- **Technical Rationale**:
  - **Byte Alignment**: A maximum of 255 pixels allows each cluster length to be represented with a single byte, reducing overhead.
  - **Processing Efficiency**:
    - Larger clusters increase decoding complexity and memory usage.
    - Smaller clusters result in too many headers, causing unnecessary overhead.
  - **Compression Balance**:
    - Tests show that 255 pixels per cluster strikes the best balance between compression efficiency and decoding speed.
- **Experimental Validation**:
  - Simulations on a variety of video data confirm that this size minimizes storage while maintaining fast access times.
- **Computational Simplicity**:
  - Using a single-byte index (0–255) simplifies coding logic and ensures compatibility with lightweight decoding systems.


### Perception of Color in Humans

#### Physiological Limits
- The human retina has three types of cones sensitive to different wavelengths: red, green, and blue.
- Humans can perceive approximately **10 million colors**, but not all simultaneously (this range is the chromatic gamut).

#### Just Noticeable Difference (JND)
- Under ideal conditions, humans can detect differences as small as **2% in luminance or hue**.
- At low resolutions, spatial details limit perceivable color variations.

#### Resolution Example: 103x29 (2,987 pixels)
- In such a restricted space, the human eye cannot discern all theoretically possible color differences.
- Limited spatial resolution reduces the effective range of perceivable chromatic variations.


### Psycho-Perceptual Factors

#### Peripheral Vision
- Humans are more sensitive to **luminance variations** (light and dark) than **chrominance** (color), especially in peripheral vision.
- For low-resolution videos, distinct but limited color palettes improve visual clarity and immersion.

#### Reduced Impact of Wide Color Gamut
- Technologies like HDR and extended RGB color spaces support millions of colors.
- However, at low resolutions, their utility diminishes significantly.


### Evidence-Based Recommendation

#### Optimal Palette Size
- A palette of **200–256 colors** is sufficient for most human applications at low resolutions (e.g., 103x29).
- This balance ensures:
  - Immersion and clarity.
  - Computational efficiency.

### Practical Conclusion
For a typical user viewing a video at **103x29 resolution**, a palette with up to **256 colors** is probably almost indistinguishable from one with millions of colors. Increasing the number of colors further adds computational overhead without enhancing visual perception or immersion.

#### to be continued?

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

### Planned Features for Future Releases

1. **Playback Controls**
   - Implementation of essential playback functions such as:
     - **Rewind**: Navigate backward within the video.
     - **Fast-Forward**: Skip ahead in the video timeline.
     - **Stop**: Pause and terminate playback as needed.

2. **Enhanced Compression**
   - Improve video compression techniques for better storage efficiency and faster encoding and decoding.

3. **Removal of FFmpeg Dependency for Audio**
   - Develop a proprietary audio decoding solution to eliminate reliance on FFmpeg.
   - Ensure full compatibility and flexibility for integrating audio.

4. **Reusable Player Framework**
   - Package the player as a library or API for integration into other projects.
   - Enable developers to leverage the player’s features within their applications.

---

### Legal Disclaimer

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

---

## License
This project is open-source under the CC BY-NC 4.0 License. See the [`LICENSE`](https://github.com/BestPlayerMMIII/clim/blob/main/LICENSE) file for details.
The license provides a general framework for usage, but the disclaimer above ensures additional legal protection for the developer in any unforeseen situations. 

---

## Acknowledgments
Thanks to the developers of open-source tools like FFmpeg, yt-dlp, and the Python/C++ communities for their incredible resources and libraries.
