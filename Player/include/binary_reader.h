#ifndef BINARY_READER_H
#define BINARY_READER_H

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdint>

#ifndef BYTE_TYPE
typedef unsigned char byte;
#endif

/**
 * @class BinaryReader
 * @brief A utility class for reading binary files in chunks with optional overlap.
 */
class BinaryReader {
public:
    /**
     * @brief Default constructor for an uninitialized instance.
     */
    BinaryReader() = default;

    /**
     * @brief Constructor to initialize the binary reader with file path and chunk settings.
     * @param file_path Path to the binary file.
     * @param chunk_size Size of each chunk to read from the file.
     * @param overlap_size Overlap between consecutive chunks.
     */
    BinaryReader(const std::string& file_path, size_t chunk_size, size_t overlap_size);

    /**
     * @brief Destructor for BinaryReader.
     */
    ~BinaryReader();

    /**
     * @brief Sets up the binary reader with a new file path and chunk settings.
     * @param file_path Path to the binary file.
     * @param chunk_size Size of each chunk to read from the file.
     * @param overlap_size Overlap between consecutive chunks.
     */
    void setup(const std::string& file_path, size_t chunk_size, size_t overlap_size);

    /**
     * @brief Gets a byte at the specified index in the file.
     * @param index The index of the byte to retrieve.
     * @return The byte value at the specified index.
     * @throws std::out_of_range if the index is out of bounds.
     */
    uint8_t get_byte(size_t index);

    /**
     * @brief Gets the total size of the file in bytes.
     * @return The total file size.
     */
    size_t size() const;

private:
    /**
     * @brief Loads a chunk of the file into the buffer.
     * @param start_index The starting index of the chunk to load.
     */
    void load_chunk(size_t start_index);

    std::string file_path;        	///< Path to the binary file.
    std::ifstream file_stream;    	///< Input file stream for reading binary data.
    std::vector<byte> buffer;     	///< Buffer holding the current chunk of data.
    size_t file_size = 0;         	///< Total size of the binary file.
    size_t chunk_size = 0;        	///< Size of each chunk to read.
    size_t overlap_size = 0;      	///< Overlap between consecutive chunks.
    size_t current_file_index = 0;	///< Current position in the file.
};

#endif	// BINARY_READER_H

