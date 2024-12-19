# CLIM File Format Documentation

The CLIM file format is a binary format designed to store compressed video efficiently, balancing storage space and computational performance. The file is structured into **mode**, **header**, **clustered video content**, and optional **audio data**, following a standard layout.


## Binary Structure of the CLIM File

### 1. Mode Byte `M` (1 byte)
   - The first byte specifies the **mode** of the file.
   - **Structure** of `M` is - from the most to the least significant digit - **`m7` `m6` `m5` `m4` `m3` `m2` `m1` `m0`**, where:
     - **`m0`**: Always `1`, indicating a valid file.
     - **`m1`**: Format type:
       - `0`: Standard format (documented here).
       - `1`: Reserved for future formats.
     - **`m2-m7`**: Undefined, reserved for future use.


### 2. Standard Format Header (11 bytes)
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


### 3. Clustered Video Content
   - **Clustering Header (`CH`)**:
     - A variable-length header describing the structure and metadata of the clusters that follow.
   - **Clusters**:
     - The clustering process begins by analyzing each video frame, which is initially represented as a two-dimensional grid of pixels. These frames are flattened into a one-dimensional sequence (a `FlatFrame`) for more efficient processing. 
     - The flattened pixel data is then analyzed to determine the most frequently occurring colors. The clustering algorithm selects a maximum of 255 unique colors per cluster.
       \[[Why?](#why-to-use-a-maximum-of-255-pixels-per-cluster)\]
     - Colors are prioritized based on their frequency of occurrence in the frame. The most common colors are included in the cluster, while less frequent colors are approximated or replaced by the closest match within the selected palette.
     - The resulting cluster palette is stored alongside the compressed frame data, allowing for reconstruction during playback.
     - Each cluster represents a portion of video data compressed using one of the following methods:
       - **Huffman**: Directly compresses pixel sequences based on frequency, associating unique sequences of bits (`codes`) to the color identification in the palette (`values`).
         More frequent values ​​are associated with shorter codes, to optimize the required storage space without losing further information.
       - **Run-Length Encoding (RLE)**: Compresses repeated pixel sequences as `(pixel, count)`.
       - **RLE + Huffman**: Compresses the lengths of repeated sequences using Huffman, storing these instead of direct counts.
      

### 4. Audio Data
   - Stored in binary MP3 format, starting at the byte index specified by `IFBA`.


## Compression and Processing

### FlatFrame and Encoding
Frames are converted into a flat representation (FlatFrame), transforming a 2D matrix into a 1D sequence:
- Pixels are processed **row by row**, left to right.
- This simplifies the compression process and ensures compatibility with methods like RLE.

### Huffman and RLE Methods
The encoding dynamically selects the most efficient compression method based on the frame content:
1. **Huffman Only**: Optimized for data with frequent patterns.
2. **RLE Only**: Effective for frames with many repeated pixel sequences.
3. **RLE + Huffman**: Combines the advantages of both methods, compressing run lengths with Huffman for additional efficiency.


## Why to use a maximum of 255 Pixels per Cluster?

### Technical Motivation
- **Byte Alignment**
   - A maximum of 255 pixels allows each cluster length to be represented with a single byte, reducing overhead.
- **Processing Efficiency**
   - Larger clusters increase decoding complexity and memory usage.
   - Smaller clusters result in too many headers, causing unnecessary overhead.
   - Using a single-byte index (0–255) simplifies coding logic and ensures compatibility with lightweight decoding systems.

### Physiological and Perceptual Considerations

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

### Practical Conclusion

For a typical user viewing a video at **103x29 resolution**, a palette with up to **255 colors** is probably almost indistinguishable from one with millions of colors.
Increasing the number of colors further adds computational overhead without enhancing visual perception or immersion.
