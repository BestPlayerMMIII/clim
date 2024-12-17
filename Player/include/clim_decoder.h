#ifndef CLIM_DECODER_H
#define CLIM_DECODER_H

#include <string>
#include <vector>
#include "binary_reader.h"
#include "bit_reader.h"
#include "cluster_decoder.h"
#include "audio_player.h"

/**
 * @struct StandardFormatInfo
 * @brief Holds metadata about the CLIM video format.
 */
struct StandardFormatInfo {
    uint16_t width;							///< Width of the video in pixels.
    uint16_t height;						///< Height of the video in pixels.
    uint16_t milliseconds_between_frames;	///< Frame duration in milliseconds.
    uint64_t index_first_byte_audio;		///< Starting byte index of the audio data.

    /**
     * @brief Calculates the frames per second (FPS).
     * @return The FPS value.
     */
    double fps() const {
        return 1000.0 / milliseconds_between_frames;
    }
};

/**
 * @class CLIMDecoder
 * @brief Decoder for the CLIM video format.
 */
class CLIMDecoder {
public:
    /**
     * @brief Constructor to initialize the CLIMDecoder.
     * @param file_path Path to the CLIM file.
     * @param audio_extraction_folder Folder for extracting audio files.
     */
    CLIMDecoder(const std::string& file_path, const std::string& audio_extraction_folder = "./.audio_extraction/");

    /**
     * @brief Destructor for CLIMDecoder.
     */
    ~CLIMDecoder();

    /**
     * @brief Retrieves frames for the next cluster.
     * @param frames Output parameter to hold the decoded frames.
     * @return True if frames were successfully retrieved, false otherwise.
     */
    bool get_next_cluster_frames(std::vector<Frame>& frames);

    /**
     * @brief Sets the cluster for a specific frame index.
     * @param frame_index The frame index to set the cluster for.
     * @return True if successful, false otherwise.
     */
    bool set_cluster_for_frame(size_t frame_index);

    /**
     * @brief Gets the metadata about the video.
     * @return A StandardFormatInfo structure.
     */
    StandardFormatInfo get_info() const {
		return info;
	}

    /**
     * @brief Gets the total number of frames in the video.
     * @return The total frame count.
     */
    size_t get_total_frames() const {
		return total_frames;
	}

    /**
     * @brief Gets the total number of clusters in the video.
     * @return The total cluster count.
     */
    size_t get_total_clusters() const {
		return total_clusters;
	}

    /**
     * @brief Gets the index of the first frame in the current cluster.
     * @return The index of the first frame.
     */
    size_t get_cluster_starting_frame() const {
		return cluster_starting_frame;
	}

    /**
     * @brief Gets the file path of the extracted audio.
     * @return The audio extraction file path.
     */
    std::string get_audio_extraxtion_filepath() const {
		return audio_extraxtion_filepath;
	}

private:
    /**
     * @brief Decodes the clustering header from the file.
     */
    void decode_clustering_header();

    /**
     * @brief Decodes the standard format header.
     */
    void decode_standard_format_header();

    /**
     * @brief Extracts audio from the file.
     */
    void extract_audio();

    const std::string AUDIO_EXTENSION = "--audio" + AUDIO_FORMAT;	///< Audio file extension.
    std::string file_path;											///< Path to the CLIM file.
    std::string audio_extraction_folder;							///< Folder for extracted audio.
    std::string audio_extraxtion_filepath;							///< Path to the extracted audio file.

    BinaryReader encoded_file_reader;	///< Reader for the encoded file data.
    ClusterDecoder cluster_decoder;		///< Decoder for cluster data.

    StandardFormatInfo info;	///< Metadata about the video.
    size_t next_byte_index;		///< Next byte to decode.

    std::vector<size_t> cluster_dimensions;	///< Dimensions of each cluster.
    size_t total_frames;					///< Total number of frames.
    size_t total_clusters;					///< Total number of clusters.

    size_t first_cluster_starting_byte_index;	///< First byte index of the first cluster.
    size_t current_cluster_index;				///< Index of the current cluster.
    size_t cluster_starting_frame;				///< Index of the first frame in the current cluster.
};

#endif	// CLIM_DECODER_H

