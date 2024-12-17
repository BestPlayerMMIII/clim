#include "clim_decoder.h"
#include "bit_reader.h"
#include "filesystem.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

CLIMDecoder::CLIMDecoder(const std::string& file_path, const std::string& audio_extraction_folder)
    : file_path(file_path), audio_extraction_folder(audio_extraction_folder), next_byte_index(0),
	current_cluster_index(0), cluster_starting_frame(0) {
    try {
        // Setup the reader of the binary file
        encoded_file_reader.setup(file_path, (1 << 16), (1 << 8));
    	/* Read header info mode: byte M = m7 m6 m5 m4 m3 m2 m1 m0
		- `m0` always equals 1.
		- `m1`:
			- 0 for standard format:
		  		M:			Mode (1 B) |
		  		[Start Standard Format Header]
				W:			Width (2 B) |  up to 65536 pixels in width
				H:			Height (2 B) |  up to 65536 pixels in height
				MSBF:		MilliSeconds Between Frames (2 B) |  from 0.015 fps to 1000 fps
				IFBA:		Index of the First Byte used for Audio (5 B) | up to 1024 GiB of video data only
		  		[End Standard Format Header]
		  		[Start Standard Format Content]
				CH:			Clustering Header (variable # B) |
				clusters:	cluster0 | cluster1 | ... | clusterN |
				audio:		audio in binary mp3 format (starting from Byte with index IFBA)
		  		[End Standard Format Content]
			- 1 for other formats (to implement)
		- `m2->m7` are undefined for now.
		*/
    	BitReader bit_reader(encoded_file_reader, next_byte_index * 8);
    	bit_reader.read_bits(6);  // m7 -> m2
    	bool m1 = bit_reader.read_bit_bool();
    	if (!bit_reader.read_bit_bool()) {  // m0 is not 1: impossible <-> error for sure
    		throw std::runtime_error("error while reading header: invalid CLIM format");
		}
		next_byte_index = bit_reader.align_to_byte();
    	if (m1) {  // not standard format
    		// eventually implement other formats
    		throw std::runtime_error("format not yet implemented");
		}
		else {  // read standard format header
			decode_standard_format_header();
		}
		// Extract audio
		extract_audio();
    	// Setup cluster decoder
    	cluster_decoder = ClusterDecoder(info.width, info.height);
        // Decode the clustering header
        decode_clustering_header();
        // Save current index as the start of clusters
        first_cluster_starting_byte_index = next_byte_index;
    } catch (const std::exception& e) {
        std::cerr << "Error during initialization: " << e.what() << "\n";
        throw;  // Rethrow exception for caller to handle
    }
}

CLIMDecoder::~CLIMDecoder() {
	namespace fs = std::filesystem;
	// delete created audio file
	if (!fs::delete_file(audio_extraxtion_filepath)) {
		std::cerr << "unable to delete the audio file: " + audio_extraxtion_filepath;
	}
	// now delete audio extraction folder if it's empty
	// (if it's not empty probably there are other instances of CLIMDecoder using it!)
	if (fs::is_directory_empty(audio_extraction_folder)) {
		fs::delete_directory(audio_extraction_folder);
	}
}

void CLIMDecoder::decode_standard_format_header() {
    // Initialize bit reader
    BitReader reader(encoded_file_reader, next_byte_index * 8);
    
	// Read Width (2 Bytes)
	info.width = reader.read_bits(16);
    // std::cout << "W = " << info.width << "\n";
	
	// Read Height (2 Bytes)
	info.height = reader.read_bits(16);
    // std::cout << "H = " << info.height << "\n";
	
	// Read MilliSeconds Between Frames (2 Bytes)
	info.milliseconds_between_frames = reader.read_bits(16);
    // std::cout << "MSBF = " << info.milliseconds_between_frames << "\n";
	
	// Read Index of the First Byte used for Audio (5 B)
	info.index_first_byte_audio = reader.read_bits(40);
    // std::cout << "IFBA = " << info.index_first_byte_audio << "\n";
	
    // Align to byte (and save at which bit index to start considering the content after the header)
    next_byte_index = reader.align_to_byte();
}

void CLIMDecoder::decode_clustering_header() {
    // Initialize bit reader
    BitReader reader(encoded_file_reader, next_byte_index * 8);

    // Read the number of bits used to represent the number of clusters (5 bits)
    uint32_t clusters_number_binary_length = reader.read_bits(5) + 1;

    // Read the number of clusters (clusters_number_binary_length bits)
    total_clusters = reader.read_bits(clusters_number_binary_length) + 1;

    // Read the number of bits used to represent the number of frames in each cluster (5 bits)
    uint32_t max_clusters_dimensions_binary_length = reader.read_bits(5) + 1;

    // Read the dimensions of each cluster and calculate total frames from cluster dimensions
    total_frames = 0;
    cluster_dimensions = std::vector<size_t>(total_clusters);
    for (size_t i = 0; i < total_clusters; ++i) {
        cluster_dimensions[i] = reader.read_bits(max_clusters_dimensions_binary_length) + 1;
        total_frames += cluster_dimensions[i];
    }

    // Align to byte (and save at which bit index to start considering the content after the header)
    next_byte_index = reader.align_to_byte();
}

void CLIMDecoder::extract_audio() {
    namespace fs = std::filesystem;
    
	// first ensure audio extraction folder existence
	fs::ensure_directory_existence(audio_extraction_folder);
	
    // Find the first available file name
    std::string output_file_path;
    size_t num = 0;
    do {
        output_file_path = audio_extraction_folder + std::to_string(num) + AUDIO_EXTENSION;
        num++;
    } while (fs::file_exists(output_file_path));
    
    // Open the output audio file in binary mode
    std::ofstream audio_file(output_file_path, std::ios::binary);
    if (!audio_file.is_open()) {
    	throw std::runtime_error("unable to open the output file: " + output_file_path);
    }
    // if opened correctly: save the path in this instance of the class
    audio_extraxtion_filepath = output_file_path;

    // Extract and write audio data to the output file
    size_t index_byte_audio = info.index_first_byte_audio;
    while (index_byte_audio < encoded_file_reader.size()) {
        // Retrieve a byte from the encoded file reader and write it to the audio file
        char byte = encoded_file_reader.get_byte(index_byte_audio);
        audio_file.put(byte);
        index_byte_audio++;
    }

    // Close the file after writing
    audio_file.close();
    
    if (!audio_file) {
    	throw std::runtime_error("writing to the audio file failed");
    }
}


bool CLIMDecoder::get_next_cluster_frames(std::vector<Frame>& frames) {
    if (current_cluster_index >= total_clusters) {
        return false;
    }

    try {
        frames = cluster_decoder.decode_cluster(encoded_file_reader, next_byte_index, cluster_dimensions[current_cluster_index]);
        // Move to the next cluster
		cluster_starting_frame += cluster_dimensions[current_cluster_index];  // update starting frame index
		current_cluster_index++;  // update cluster index
    } catch (const std::exception& e) {
        std::cerr << "Error while decoding cluster: " << e.what() << "\n";
        throw;  // Rethrow exception for caller to handle
    }

    return true;
}


// Reset to desired frame index the next extraction
bool CLIMDecoder::set_cluster_for_frame(const size_t frame_index) {
    if (frame_index >= total_frames) {
        return false;
    }
    
    try {
    	size_t byte_index = first_cluster_starting_byte_index;
	    // retrieve cluster index and relative frame to the cluster
	    cluster_starting_frame = 0;
	    current_cluster_index = 0;
	    while (cluster_starting_frame + cluster_dimensions[current_cluster_index] < frame_index) {
	    	cluster_decoder.pass_cluster(encoded_file_reader, byte_index, cluster_dimensions[current_cluster_index]);
	    	cluster_starting_frame += cluster_dimensions[current_cluster_index];
	    	current_cluster_index++;
		}
		next_byte_index = byte_index;
    } catch (const std::exception& e) {
        std::cerr << "Error while retrieving cluster for frame with index " << frame_index << ": " << e.what() << "\n";
        throw;  // Rethrow exception for caller to handle
    }
	
	return true;
}

