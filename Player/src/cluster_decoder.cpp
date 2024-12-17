#include "cluster_decoder.h"
#include "bit_reader.h"
#include <iostream>
#include <stdexcept>  // exceptions

ClusterDecoder::ClusterDecoder(const size_t width, const size_t height)
: width(width), height(height) {}

std::vector<Frame> ClusterDecoder::decode_cluster(BinaryReader& binary_data_reader,
size_t& index, size_t number_of_frames_in_cluster) {
    std::vector<Frame> frames(number_of_frames_in_cluster);
    
    // Step 1: Decode palette header
    HuffmanCodebook<Color> palette = decode_palette(binary_data_reader, index);

    // Step 2: Decode cluster flat frames and transform them into frames
    FlatFrame flat_frame;
    for (size_t i = 0; i < number_of_frames_in_cluster; ++i) {
        flat_frame = decode_frame(binary_data_reader, index, palette);
        frames[i] = flat_frame.to_frame(width, height);
    }

    return frames;
}

void ClusterDecoder::pass_cluster(BinaryReader& binary_data_reader, size_t& index, size_t number_of_frames_in_cluster) {
    // Step 1: Decode palette header
    HuffmanCodebook<Color> palette = decode_palette(binary_data_reader, index);

    // Step 2: Decode cluster flat frames
    for (size_t i = 0; i < number_of_frames_in_cluster; ++i) {
        decode_frame(binary_data_reader, index, palette);
    }
}

HuffmanCodebook<Color> ClusterDecoder::decode_palette(BinaryReader& data_reader, size_t& index) {
    HuffmanCodebook<Color> palette;
    std::unordered_map<byte, Color> colors;

    // Read the number of colors in the palette
    byte num_colors = data_reader.get_byte(index++) + 1;
    
    // Read colors
    for (byte i = 0; i < num_colors; ++i) {
        byte r = data_reader.get_byte(index++);
        byte g = data_reader.get_byte(index++);
        byte b = data_reader.get_byte(index++);
        colors[i] = {r, g, b};
    }
    
    // Read the huffman codes length (each in 3-bit binary)
    byte num_bits_huffman_codes[num_colors];
    BitReader bit_reader(data_reader, index * 8);  // index [Byte] * 8 <=> index [bit]
    for (byte i = 0; i < num_colors; ++i) {
        num_bits_huffman_codes[i] = bit_reader.read_bits(3) + 1;
    }
    
    // after huffman codes length need to align to byte
    index = bit_reader.align_to_byte();
    
    // Read the huffman codes (dynamic length each)
    std::string code;
    for (byte i = 0; i < num_colors; ++i) {
    	code = bit_reader.read_bit_string(num_bits_huffman_codes[i]);
    	palette.insert(code, colors[i]);
    }
    
    // update index to the next aligned byte
    index = bit_reader.align_to_byte();
    
    return palette;
}

FlatFrame ClusterDecoder::decode_frame(BinaryReader& data_reader, size_t& index,
const HuffmanCodebook<Color>& palette) {
    FlatFrame frame;
	size_t frame_dimension = width * height;

    // Step 1: Read encoding method from header
    BitReader bit_reader(data_reader, index * 8); // Initialize BitReader at the current bit position
    bool is_rle = bit_reader.read_bits(1); // First bit indicates if RLE is used
    bool uses_huffman = is_rle ? bit_reader.read_bits(1) : false; // Second bit (if RLE) indicates if Huffman is used
    size_t rle_bit_length = 0;
    
    HuffmanCodebook<size_t> rle_huffman_codebook;
    
    //* DEBUG */ std::cout << "frame@" << index << " -> " << (is_rle?(uses_huffman?"RLE + HUFFMAN":"RLE"):"HUFFMAN") << "\n";

    // Step 2: Process header for RLE if applicable
    if (is_rle) {
        if (uses_huffman) {  // Read the Huffman codebook for RLE counts
            // Number of bits to represent number of codes in the RLE Huffman codebook
            size_t max_num_codes = bit_reader.read_bits(4);
            // Number of codes in the RLE Huffman codebook
            size_t num_codes = bit_reader.read_bits(max_num_codes);
            
            // Length in bits of the max value encountered in the codebook
            size_t max_value_bits = bit_reader.read_bits(4);

            for (size_t i = 0; i < num_codes; ++i) {
                size_t count = bit_reader.read_bits(max_value_bits) + 1;  // RLE count
                size_t code_length = bit_reader.read_bits(4) + 1;  // Length in bits of the Huffman code
                std::string code = bit_reader.read_bit_string(code_length);
                rle_huffman_codebook.insert(code, count);
            }
        } else {  // Direct RLE bit length
            rle_bit_length = bit_reader.read_bits(5) + 1; // RLE bit length
        }
    }

    // Step 3: Decode pixel data
    while (frame.pixels.size() < frame_dimension) {
        if (is_rle) {
            if (uses_huffman) {
                // Decode palette index using Huffman, then RLE using Huffman codebook
                // 1. code (to color)
                std::string palette_code = "";
                while (!palette.contains(palette_code)) {
                    palette_code += bit_reader.read_bit_string(1);
                    if (palette_code.length() > 8) {
                    	std::cerr << "Invalid palette:\n" << palette;
                    	throw std::runtime_error("palette code not found during RLE+Huffman decoding");
					}
                }
                Color color = palette.at(palette_code);
                // 2. code (to count)
                std::string count_code = "";
                while (!rle_huffman_codebook.contains(count_code)) {
                    count_code += bit_reader.read_bit_string(1);
                    if (count_code.length() > 16) {
                    	std::cerr << "Invalid RLE+Huffman codebook for counts:\n" << rle_huffman_codebook;
                    	throw std::runtime_error("count code not found during RLE+Huffman decoding");
					}
                }
                size_t count = rle_huffman_codebook.at(count_code);
                // Append decoded pixels
                for (size_t i = 0; i < count; ++i) {
                    frame.pixels.push_back(color);
                }
            } else {
                // Decode RLE directly
                // 1. code (to color)
                std::string palette_code = "";
                while (!palette.contains(palette_code)) {
                    palette_code += bit_reader.read_bit_string(1);
                    if (palette_code.length() > 8) {
                    	std::cerr << "Invalid palette:\n" << palette;
                    	throw std::runtime_error("palette code not found during RLE decoding");
					}
                }
                Color color = palette.at(palette_code);
                // 2. count
                size_t count = bit_reader.read_bits(rle_bit_length) + 1;
                // Append decoded pixels
                for (size_t i = 0; i < count; ++i) {
                    frame.pixels.push_back(color);
                }
            }
        } else {
            // Decode pixel data using Huffman only
            // code (to color)
            std::string palette_code = "";
            while (!palette.contains(palette_code)) {
                palette_code += bit_reader.read_bit_string(1);
                    if (palette_code.length() > 8) {
                    	std::cerr << "Invalid palette:\n" << palette;
                    	throw std::runtime_error("palette code not found during Huffman decoding");
					}
            }
            // Append decoded pixel
            frame.pixels.push_back(palette.at(palette_code));
        }
    }

    // Update index to reflect the current byte-aligned position in the BitReader
    index = bit_reader.align_to_byte();
    return frame;
}

