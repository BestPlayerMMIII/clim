#include "audio_player.h"
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

// Constructor: Initialize the audio player with a specific audio file path.
AudioPlayer::AudioPlayer(const std::string& audio_file_path)
    : temp_file_path(audio_file_path) {
#ifdef _WIN32
    ffplay_handle = nullptr; // Initialize the handle for Windows to null.
#endif
}

// Default constructor: Create an audio player without specifying an audio file.
AudioPlayer::AudioPlayer() {
#ifdef _WIN32
    ffplay_handle = nullptr; // Initialize the handle for Windows to null.
#endif
}

// Destructor: Ensure proper cleanup by stopping any ongoing audio playback.
AudioPlayer::~AudioPlayer() {
    stop_audio();
}

// Sets a new audio file path and ensures any previous playback is stopped before proceeding.
void AudioPlayer::set_audio_file(const std::string& audio_file_path) {
    stop_audio();  // Stop any playback in progress.
    temp_file_path = audio_file_path; // Update the audio file path.
}

// Starts audio playback using ffplay in a separate thread.
void AudioPlayer::play_audio() {
    if (temp_file_path.empty()) { // Ensure the audio file path is set.
        throw std::runtime_error("Audio file path is not set!");
    }

    stop_audio();  // Stop any ongoing playback to prevent conflicts.
    stop_requested = false; // Reset stop flag for the new playback session.

    // Launch a new thread to handle audio playback.
    player_thread = std::thread(&AudioPlayer::execute_ffplay, this);
}

// Stops audio playback gracefully by signaling the playback process/thread to terminate.
void AudioPlayer::stop_audio() {
    stop_requested = true; // Request playback termination.

#ifdef _WIN32
    if (ffplay_handle) { // If a process handle exists on Windows:
        TerminateProcess(ffplay_handle, 0); // Forcefully terminate the process.
        CloseHandle(ffplay_handle); // Release the process handle.
        ffplay_handle = nullptr; // Reset the handle to null.
    }
#else
    if (ffplay_pid > 0) { // If a valid process ID exists on POSIX systems:
        kill(ffplay_pid, SIGTERM); // Send termination signal to the ffplay process.
        ffplay_pid = -1; // Reset the process ID.
    }
#endif

    // Wait for the playback thread to join (complete execution).
    if (player_thread.joinable()) {
        player_thread.join();
    }
}

// Handles the execution of the ffplay process for audio playback.
void AudioPlayer::execute_ffplay() {
    try {
        std::string command; // Command string for launching ffplay.

#ifdef _WIN32
        // Create the ffplay command for Windows.
        command = "ffplay -nodisp -loglevel quiet -autoexit \"" + temp_file_path + "\"";

        STARTUPINFO si; // Structure for process startup information.
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi; // Structure to receive process information.
        ZeroMemory(&pi, sizeof(pi));

        // Create a new process to run the ffplay command.
        if (!CreateProcess(
                nullptr, const_cast<char*>(command.c_str()), nullptr, nullptr, FALSE,
                CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            throw std::runtime_error("Failed to start ffplay process on Windows.");
        }

        ffplay_handle = pi.hProcess; // Store the process handle for future management.

        // Monitor the process or stop request in a loop.
        while (WaitForSingleObject(pi.hProcess, 100) == WAIT_TIMEOUT) {
            if (stop_requested) { // Terminate the process if a stop is requested.
                TerminateProcess(pi.hProcess, 0);
                break;
            }
        }

        // Clean up the process and thread handles.
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        ffplay_handle = nullptr; // Reset the handle.

#else
        // Create the ffplay command for POSIX systems.
        command = "ffplay -nodisp -loglevel quiet -autoexit \"" + temp_file_path + "\"";
        ffplay_pid = fork(); // Fork the current process.

        if (ffplay_pid == 0) {
            // Child process: Execute ffplay using execlp.
            execlp("ffplay", "ffplay", "-nodisp", "-loglevel", "quiet", "-autoexit", temp_file_path.c_str(), (char*)nullptr);
            std::cerr << "Failed to start ffplay!\n"; // If execlp fails, print an error and exit.
            std::exit(EXIT_FAILURE);
        } else if (ffplay_pid < 0) {
            // Forking failed; throw an exception.
            throw std::runtime_error("Unable to create ffplay process!");
        }

        // Parent process: Monitor the stop signal.
        while (!stop_requested) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep briefly between checks.
        }

        // Send a termination signal to the ffplay process when stopping.
        if (ffplay_pid > 0) {
            kill(ffplay_pid, SIGTERM);
            ffplay_pid = -1; // Reset the process ID.
        }
#endif
    } catch (const std::exception& e) {
        // Handle exceptions and log any errors that occur during playback.
        std::cerr << "Exception in audio playback: " << e.what() << std::endl;
    } catch (...) {
        // Catch any unknown errors during playback.
        std::cerr << "Unknown error in audio playback thread." << std::endl;
    }
}

