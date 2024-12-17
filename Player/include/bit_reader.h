#ifndef BIT_READER_H
#define BIT_READER_H

#include <vector>
#include <string>
#include <cstdint>
#include "binary_reader.h"

/**
 * @class BitReader
 * @brief A utility class for reading bits from a binary reader.
 */
class BitReader {
public:
    /**
     * @brief Constructor to initialize the bit reader.
     * @param data The BinaryReader instance to read from.
     * @param next_bit_index The starting bit index.
     */
    BitReader(BinaryReader& data, size_t next_bit_index = 0);

    /**
     * @brief Reads the specified number of bits as an integer.
     * @param n Number of bits to read.
     * @return The bits read as an unsigned 64-bit integer.
     */
    uint64_t read_bits(size_t n);

    /**
     * @brief Reads a single bit as a boolean.
     * @return The bit value as a boolean.
     */
    bool read_bit_bool();

    /**
     * @brief Reads the specified number of bits as a string of '0's and '1's.
     * @param n Number of bits to read.
     * @return A string representing the bits.
     */
    std::string read_bit_string(size_t n);

    /**
     * @brief Aligns the reader to the next byte boundary.
     * @return The index of the first non-read byte.
     */
    size_t align_to_byte();

    /**
     * @brief Gets the current bit index.
     * @return The bit index.
     */
    inline size_t get_bit_index() const {
        return bit_index;
    }

    /**
     * @brief Gets the current bit index and byte index.
     * @param _byte_index Output parameter to hold the byte index.
     * @return The bit index.
     */
    inline size_t get_bit_index(size_t& _byte_index) const {
        _byte_index = byte_index;
        return bit_index;
    }

    /**
     * @brief Checks if the reader has reached the end of the stream.
     * @return True if at the end of the stream, false otherwise.
     */
    bool is_at_end() const;

private:
    /**
     * @brief Increments the bit index and adjusts the byte index if necessary.
     */
    inline void increment_bit_index() {
        bit_index++;
        if (bit_index == 8) {
            bit_index = 0;
            byte_index++;
        }
    }

    BinaryReader& data_reader;	///< Reference to the BinaryReader instance.
    size_t byte_index = 0;		///< Current byte index.
    size_t bit_index = 0;		///< Current bit index within the byte.
};

#endif	// BIT_READER_H

