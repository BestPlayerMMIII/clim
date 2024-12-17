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
pip install -r requirements.txt
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

The CLIM file format is a binary format designed to store compressed video efficiently, balancing storage space and computational performance. The file is structured into **mode**, **header**, **clustered video content**, and optional **audio data**, following a standard layout.


### Binary Structure of the CLIM File

#### 1. Mode Byte `M` (1 byte)
   - The first byte specifies the **mode** of the file.
   - **Structure** of `M` is - from the most to the least significant digit - **`m7` `m6` `m5` `m4` `m3` `m2` `m1` `m0`**, where:
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
     - Up to **1 TiB of video data** before the audio section starts.


#### 3. Clustered Video Content
   - **Clustering Header (`CH`)**:
     - A variable-length header describing the structure and metadata of the clusters that follow.
   - **Clusters**:
     - The clustering process begins by analyzing each video frame, which is initially represented as a two-dimensional grid of pixels. These frames are flattened into a one-dimensional sequence (a `FlatFrame`) for more efficient processing. 
     - The flattened pixel data is then analyzed to determine the most frequently occurring colors. The clustering algorithm selects a maximum of 255 unique colors per cluster.
       \[[Why?](https://github.com/BestPlayerMMIII/clim?tab=readme-ov-file#why-to-use-a-maximum-of-255-pixels-per-cluster)\]
     - Colors are prioritized based on their frequency of occurrence in the frame. The most common colors are included in the cluster, while less frequent colors are approximated or replaced by the closest match within the selected palette.
     - The resulting cluster palette is stored alongside the compressed frame data, allowing for reconstruction during playback.
     - Each cluster represents a portion of video data compressed using one of the following methods:
       - **Huffman**: Directly compresses pixel sequences based on frequency, associating unique sequences of bits (`codes`) to the color identification in the palette (`values`).
         More frequent values ​​are associated with shorter codes, to optimize the required storage space without losing further information.
       - **Run-Length Encoding (RLE)**: Compresses repeated pixel sequences as `(pixel, count)`.
       - **RLE + Huffman**: Compresses the lengths of repeated sequences using Huffman, storing these instead of direct counts.
      

#### 4. Audio Data
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


### Why to use a maximum of 255 Pixels per Cluster?

#### Technical Motivation
- **Byte Alignment**
   - A maximum of 255 pixels allows each cluster length to be represented with a single byte, reducing overhead.
- **Processing Efficiency**
   - Larger clusters increase decoding complexity and memory usage.
   - Smaller clusters result in too many headers, causing unnecessary overhead.
   - Using a single-byte index (0–255) simplifies coding logic and ensures compatibility with lightweight decoding systems.

#### Physiological and Perceptual Considerations

  The human visual system relies on three types of retinal cones sensitive to red, green, and blue wavelengths, enabling the perception of approximately **10 million distinct colors** within the chromatic gamut.
  However, this perception is context-dependent, as humans cannot differentiate all colors simultaneously. Under ideal conditions, differences as small as **2% in luminance or hue** can be detected.
  
  Sensitivity to **luminance** (light and dark variations) is significantly greater than to **chrominance** (color differences), particularly in peripheral vision.
  For low-resolution videos, a limited yet distinct color palette enhances clarity and visual immersion without overwhelming the viewer's perceptual limits.
  
  In low-resolution videos with small pixel counts, the human eye struggles to discern **minor variations** in color, particularly when individual frames are displayed for only a fraction of a second.
  For example, at the default frame rate of 12 FPS, each frame is visible for **less than 0.084 seconds**, further reducing the viewer's ability to perceive subtle differences in color intensity or hue.
  
  The maximum Euclidean distance between colors in RGB space is approximately 442 (calculated as $` \sqrt{255^2 + 255^2 + 255^2} `$), representing the largest possible color difference.
  A difference of 3 units in each of the red, green, and blue channels constitutes **less than 1.2%** of this maximum distance.
  For small frames, where spatial detail is already limited, such minimal differences are **imperceptible** to the average viewer.

  Furthermore, in dynamic video content, the rapid succession of frames creates a **smoothing effect** in the brain, making it nearly **impossible to notice these fine distinctions**.
  As a result, for practical purposes in this format, a highly optimized and reduced color palette does not negatively impact perceived visual quality.
  Instead, it enhances performance and compression efficiency while maintaining the viewer's immersive experience.

**Practical Conclusion**

For a typical user viewing a video at **103x29 resolution**, a palette with up to **255 colors** is probably almost indistinguishable from one with millions of colors.
Increasing the number of colors further adds computational overhead without enhancing visual perception or immersion.

---

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
This project is open-source under the CC BY-NC 4.0 License. See the [`LICENSE`](https://github.com/BestPlayerMMIII/clim/blob/main/LICENSE) file for details.
The license provides a general framework for usage, but the disclaimer above ensures additional legal protection for the developer in any unforeseen situations. 


## Acknowledgments
Thanks to the developers of open-source tools like FFmpeg, yt-dlp, and the Python/C++ communities for their incredible resources and libraries.
