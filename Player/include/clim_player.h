#ifndef CLIM_PLAYER_H
#define CLIM_PLAYER_H

#include <vector>
#include <chrono>
#include "frame.h"
#include "clim_decoder.h"
#include "audio_player.h"

/**
 * @class CLIMPlayer
 * @brief A class for playing CLIM video files with synchronized audio.
 */
class CLIMPlayer {
public:
    /**
     * @brief Constructor to initialize the player.
     * @param root_folder The root folder path.
     * @param file_path_from_root Path to the file relative to the root folder.
     */
    CLIMPlayer(const std::string& root_folder, const std::string& file_path_from_root);

    /**
     * @brief Destructor for CLIMPlayer.
     */
    ~CLIMPlayer();

    /**
     * @brief Starts playback of frames and audio.
     * @param outs The output stream for rendering.
     * @param loop Whether to loop playback.
     */
    void play(std::ostream& outs, const bool loop = false);

private:
    size_t destructor_fid;	///< Unique ID for cleanup functions.

    // Memory
    double fps;				///< Desired frames per second.
    size_t frame_time_ms;	///< Time in milliseconds between frames.
    size_t width, height;	///< Frame dimensions.
    CLIMDecoder decoder;	///< Decoder for extracting frames.

    // Video
    FrameRenderer renderer;				///< Renderer for drawing frames.
    std::vector<Frame> frame_buffer;	///< Buffer for storing frames.
    size_t buffer_threshold;			///< Threshold for fetching new frames.

    // Audio
    AudioPlayer music;	///< Audio player for playing the soundtrack.

    /**
     * @brief Fetches the next set of frames from the decoder.
     * @return True if frames were successfully fetched, false otherwise.
     */
    bool fetch_next_frames();

    /**
     * @brief Starts the playback of CLIM content.
     * @param outs The output stream for rendering.
     */
    void play_clim(std::ostream& outs);
};

#endif	// CLIM_PLAYER_H

