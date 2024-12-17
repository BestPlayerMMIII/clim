#include "binary_reader.h"

void BinaryReader::setup(const std::string& file_path, size_t chunk_size, size_t overlap_size) {
    // member initializers
	this->file_path = file_path;
	this->chunk_size = chunk_size;
	this->overlap_size = overlap_size;
    
    // Open the file stream
    file_stream.open(file_path, std::ios::binary | std::ios::ate);
    if (!file_stream.is_open()) {
        throw std::runtime_error("Unable to open file: " + file_path);
    }

    // Determine file size
    file_size = static_cast<size_t>(file_stream.tellg());
    file_stream.seekg(0, std::ios::beg);

    // Initialize the buffer
    buffer.resize(chunk_size);
    load_chunk(0);  // Load the first chunk
}

BinaryReader::~BinaryReader() {
    if (file_stream.is_open()) {
        file_stream.close();
    }
}

void BinaryReader::load_chunk(size_t start_index) {
    if (start_index >= file_size) {
        throw std::out_of_range("Start index exceeds file size");
    }

    // Calculate how much to read
    size_t read_size = std::min(chunk_size, file_size - start_index);

    // Move file pointer and read into buffer
    file_stream.seekg(start_index, std::ios::beg);
    if (!file_stream.read(reinterpret_cast<char*>(buffer.data()), read_size)) {
        throw std::runtime_error("Failed to read file chunk");
    }

    // Update the current file index
    current_file_index = start_index;
}

uint8_t BinaryReader::get_byte(size_t index) {
    // Check if the requested byte is outside the current chunk
    if (index < current_file_index || index >= current_file_index + buffer.size()) {
        // Calculate the start of the new chunk
        size_t new_start = (index < overlap_size) ? 0 : index - overlap_size;

        // Load the new chunk
        load_chunk(new_start);
    }

    // Return the byte from the buffer
    return buffer[index - current_file_index];
}

size_t BinaryReader::size() const {
    return file_size;
}

