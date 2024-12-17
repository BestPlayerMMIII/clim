#ifndef CLUSTER_DECODER_H
#define CLUSTER_DECODER_H

#include <vector>
#include <unordered_map>
#include <string>
#include "color.h"
#include "huffman_codebook.h"
#include "binary_reader.h"
#include "frame.h"

#ifndef BYTE_TYPE
typedef unsigned char byte;
#endif

/**
 * @class ClusterDecoder
 * @brief A class for decoding clusters of video frames.
 */
class ClusterDecoder {
public:
    /**
     * @brief Default constructor for an uninitialized instance.
     */
    ClusterDecoder() : width(0), height(0) {}

    /**
     * @brief Constructor to initialize the cluster decoder with dimensions.
     * @param width Width of the frames.
     * @param height Height of the frames.
     */
    ClusterDecoder(const size_t width, const size_t height);

    /**
     * @brief Decodes a cluster into a series of frames.
     * @param binary_data_reader The binary reader for input data.
     * @param index Reference to the current byte index.
     * @param number_of_frames_in_cluster Number of frames in the cluster.
     * @return A vector of decoded frames.
     */
    std::vector<Frame> decode_cluster(BinaryReader& binary_data_reader,
                                      size_t& index, size_t number_of_frames_in_cluster);

    /**
     * @brief Simulates decoding a cluster by updating the byte index.
     * @param binary_data_reader The binary reader for input data.
     * @param index Reference to the current byte index.
     * @param number_of_frames_in_cluster Number of frames in the cluster.
     */
    void pass_cluster(BinaryReader& binary_data_reader,
                      size_t& index, size_t number_of_frames_in_cluster);

private:
    size_t width, height;	///< Dimensions of the frames.

    /**
     * @brief Decodes the palette header.
     * @param data_reader The binary reader for input data.
     * @param index Reference to the current byte index.
     * @return A Huffman codebook for the color palette.
     */
    HuffmanCodebook<Color> decode_palette(BinaryReader& data_reader, size_t& index);

    /**
     * @brief Decodes a single frame.
     * @param data_reader The binary reader for input data.
     * @param index Reference to the current byte index.
     * @param palette The Huffman codebook for the palette.
     * @return A decoded frame.
     */
    FlatFrame decode_frame(BinaryReader& data_reader, size_t& index,
                           const HuffmanCodebook<Color>& palette);
};

#endif	// CLUSTER_DECODER_H

