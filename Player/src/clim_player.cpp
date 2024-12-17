#include "clim_player.h"
#include "exit_handler.h"
#include <iostream>
#include <thread>  // For std::this_thread::sleep_for

CLIMPlayer::CLIMPlayer(const std::string& root_folder, const std::string& file_path_from_root)
    : decoder(root_folder + file_path_from_root) {
    // Retreive information from decoder
    StandardFormatInfo info = decoder.get_info();
    width = info.width;
    height = info.height;
    frame_time_ms = info.milliseconds_between_frames;
    // Setup utils
    fps = info.fps();  // Get calculated frames per second
    buffer_threshold = 2 * fps;  // Trigger when fewer than 2 FPS frames remain
    renderer = FrameRenderer(width, height);  // setup frame renderer
    // Setup audio player. Note: decoder has already extracted audio when previously initialized
    music = AudioPlayer(decoder.get_audio_extraxtion_filepath());  
    // Add the player destructor as a function to be executed at the end of the program
    destructor_fid = ExitHandler::add_cleanup_function([this]() {
        this->~CLIMPlayer();
    });
}

CLIMPlayer::~CLIMPlayer() {
	ExitHandler::remove_cleanup_function(destructor_fid);  // do not call this function anymore at the end of the program (already destroyed)
	music.stop_audio();  // first: stop audio, to ensure ability to delete audio file
	// then: default call to all the destructors for CLIMPlayer instances
}

bool CLIMPlayer::fetch_next_frames() {
    // Fetch the next cluster of frames from the decoder
    std::vector<Frame> new_frames;
    bool fetched_frames = decoder.get_next_cluster_frames(new_frames);
    if (fetched_frames) {
        frame_buffer.insert(frame_buffer.end(), new_frames.begin(), new_frames.end());
    }
    return fetched_frames;
}

void CLIMPlayer::play_clim(std::ostream& outs) {
    using namespace std::chrono;
    
    bool all_read = false;
    // populate initial buffer upto 2 buffer thresholds
    while ((!all_read) && (frame_buffer.size() <= 2 * buffer_threshold)) {
        all_read = all_read || (!fetch_next_frames());
    }

    auto start_time = steady_clock::now(); // Initial playback loop time
    
    // start audio
    music.play_audio();

	// Perform frames rendering
	while (!frame_buffer.empty()) {
	    
	    // Get frame to render from the FIFO buffer
	    Frame current_frame = frame_buffer.front();
	    frame_buffer.erase(frame_buffer.begin());  // Remove the frame from the buffer
	
	    // Display the frame
	    // outs << "\033[2J";  // Clear the screen
	    outs << "\033[H";  // Set the cursor to the top-left position
	    outs << renderer.render_frame(current_frame);  // Output the frame
	
	    // Calculate the target time for the next iteration to start
	    auto target_next_frame_time = start_time + milliseconds(frame_time_ms);
	    auto now = steady_clock::now();
	
	    // Calculate the remaining time to reach the next frame
	    int time_to_wait_ms = duration_cast<milliseconds>(target_next_frame_time - now).count();
	
	    if (time_to_wait_ms > 0) {
	        // Wait for the remaining time
	        std::this_thread::sleep_for(milliseconds(time_to_wait_ms));
	    }
	
	    // Update start_time to the target time, regardless of any accumulated delay
	    start_time = target_next_frame_time;
	
	    // Trigger fetching new frames if the buffer is running low
	    if (!all_read && frame_buffer.size() <= buffer_threshold) {
	        all_read = all_read || !fetch_next_frames();
	    }
	}
}


void CLIMPlayer::play(std::ostream& outs, const bool loop) {
	play_clim(outs);
	while (loop) {
		decoder.set_cluster_for_frame(0);
		play_clim(outs);
	}
}


