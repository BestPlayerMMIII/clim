#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <string>
#include <thread>
#include <atomic>

const std::string AUDIO_FORMAT = ".mp3";

/**
 * @class AudioPlayer
 * @brief A utility class for playing audio files using ffplay.
 */
class AudioPlayer {
public:
    /**
     * @brief Destructor for AudioPlayer.
     */
    ~AudioPlayer();

    /**
     * @brief Constructor to initialize the audio player with a file path.
     * @param audio_file_path The path to the audio file to be played.
     */
    explicit AudioPlayer(const std::string& audio_file_path);

    /**
     * @brief Constructor to create a non-initialized AudioPlayer.
     */
    AudioPlayer();
    
    /**
     * @brief Constructor to initialize the audio player with a copy of another AudioPlayer object.
     * @param other The other audio player to copy.
     */
	AudioPlayer(const AudioPlayer& other) {
	    temp_file_path = other.temp_file_path;	// Copies the temporary file path
	}
	
    /**
     * @brief Copy assignment operator from another AudioPlayer object.
     * @param other The other audio player to copy.
     */
	AudioPlayer& operator=(const AudioPlayer& other) {
	    if (this != &other) {	// Prevent self-assignment
	        temp_file_path = other.temp_file_path;
	    }
	    return *this;
	}


    /**
     * @brief Sets the path to the audio file to be played.
     * @param audio_file_path The path to the audio file to be set.
     */
    void set_audio_file(const std::string& audio_file_path);

    /**
     * @brief Plays the audio file set in the AudioPlayer instance.
     */
    void play_audio();

    /**
     * @brief Stops the audio playback.
     */
    void stop_audio();

private:
    void execute_ffplay();	///< Executes the ffplay process based on the current OS.

    std::string temp_file_path;					///< Temporary file path for the audio to be played.
    std::thread player_thread;					///< Thread for managing audio playback.
    std::atomic<bool> stop_requested{false};	///< Atomic flag to signal playback termination.

#ifdef _WIN32
    void* ffplay_handle;	///< Handle to the ffplay process on Windows.
#else
    pid_t ffplay_pid = -1;	///< PID of the ffplay process on POSIX systems.
#endif
};

#endif	// AUDIO_PLAYER_H

