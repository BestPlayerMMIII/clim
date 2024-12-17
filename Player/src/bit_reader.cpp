#include "bit_reader.h"
#include <stdexcept>

// Constructor initializes the bit reader with a reference to the data reader
// and the starting bit index.
BitReader::BitReader(BinaryReader& _data_reader, const size_t next_bit_index)
    : data_reader(_data_reader), byte_index(next_bit_index / 8), bit_index(next_bit_index % 8) {}

// Reads the next `n` bits and returns them as a 64-bit unsigned integer.
uint64_t BitReader::read_bits(size_t n) {
    if (n > 64) {
        throw std::invalid_argument("Cannot read more than 64 bits at a time.");
    }

    uint64_t result = 0;

    // Loop to read `n` bits one at a time.
    for (size_t i = 0; i < n; ++i) {
        if (is_at_end()) {
            throw std::out_of_range("Attempt to read beyond the end of the data.");
        }

        // Extract the next bit and shift it into the result.
        result = (result << 1) | ((data_reader.get_byte(byte_index) >> (7 - bit_index)) & 1);

        // Move to the next bit in the data.
        increment_bit_index();
    }

    return result;
}

// Reads a single bit and returns it as a boolean.
bool BitReader::read_bit_bool() {
    // Extract the current bit as a boolean value.
    bool result = ((data_reader.get_byte(byte_index) >> (7 - bit_index)) & 1);

    // Move to the next bit.
    increment_bit_index();
    return result;
}

// Reads `n` bits and returns them as a string of '0' and '1'.
std::string BitReader::read_bit_string(size_t n) {
    std::string bit_string;

    // Loop to read `n` bits one at a time.
    for (size_t i = 0; i < n; ++i) {
        if (is_at_end()) {
            throw std::out_of_range("Attempt to read beyond the end of the data.");
        }

        // Append '1' or '0' to the string based on the current bit value.
        char bit = ((data_reader.get_byte(byte_index) >> (7 - bit_index)) & 1) ? '1' : '0';
        bit_string.push_back(bit);

        // Move to the next bit.
        increment_bit_index();
    }

    return bit_string;  // Return the constructed string of bits.
}

// Aligns the bit reader to the next byte boundary and returns the updated byte index.
size_t BitReader::align_to_byte() {
    if ((bit_index % 8) != 0) {
        // Move to the next byte if the bit index is not aligned.
        bit_index = 0;
        byte_index++;
    }
    return byte_index;
}

// Checks if the bit reader has reached the end of the data.
bool BitReader::is_at_end() const {
    return byte_index >= data_reader.size();  // True if the byte index exceeds the data size.
}

