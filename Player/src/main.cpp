#include <iostream>
#include <string>
#include "clim_player.h"

using namespace std;

int main(int argc, char* argv[]) {
    try {
    	bool loop = false;
        string clim_filepath = "";  // Not-set clim filepath
        
        if (argc > 1 && argc <= 3) {  // Or (if provided)
	        for (int i = 1; i < argc; ++i) {
	            std::string arg = argv[i];
	            if (arg == "--loop") {
	                loop = true;  // Enable loop for optional arg "--loop"
	            } else {
	                clim_filepath = arg;  // Desired filepath
	            }
	        }
        }
        
        if (clim_filepath == "") {  // Explicitly require an input CLIM file
        	cout << "Provide the path to the CLIM file you want to play (e.g. clims/video.clim)\n > ";
        	cin >> clim_filepath;
		}
        
		// Create a CLIMPlayer instance and play the desired file
        CLIMPlayer player("", clim_filepath);
        player.play(cout, loop);

    } catch (const exception& e) {  // catch all the error/exceptions
        cerr << "Fatal error: " << e.what() << "\n"
        	 << "Remember to use the correct syntax and to provide a valid CLIM file as input.\n"
			 << "Syntax: player [--loop] [input]\n"
        	 << "  [--loop]: loop the video. Optional.\n"
        	 << "  [input]: is the clim file path to show. Optional (but it will be required when starting the program if not provided).\n"
        	 << "Press any key to continue...\n";
		getchar();
        return 1;
    }

    return 0;
}

