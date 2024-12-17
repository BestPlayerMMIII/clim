#include <iostream>
#include <string>
#include "clim_player.h"

using namespace std;

const string DEFAULT_CLIM_FILEPATH = "../clim/video.clim";

int main(int argc, char* argv[]) {
    try {
    	bool loop = false;
        string clim_file = DEFAULT_CLIM_FILEPATH;
        
        if (argc > 1 && argc <= 3) {  // Or (if provided)
	        for (int i = 1; i < argc; ++i) {
	            std::string arg = argv[i];
	            if (arg == "--loop") {
	                loop = true;  // Enable loop for optional arg "--loop"
	            } else {
	                clim_file = arg;  // Desired filepath
	            }
	        }
        }
        
		// Create a CLIMPlayer instance and play the desired file
        CLIMPlayer player("", clim_file);
        player.play(cout, loop);

    } catch (const exception& e) {  // catch all the error/exceptions
        cerr << "Fatal error: " << e.what() << "\n"
        	 << "Remember to use the correct syntax: player [--loop] input\n"
        	 << "  [--loop]: loop the video. Optional.\n"
        	 << "  input: is the clim file path to show. Required (default is " + DEFAULT_CLIM_FILEPATH +"\n"
        	 << "Press any key to continue...\n";
		getchar();
        return 1;
    }

    return 0;
}

