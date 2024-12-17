#include "exit_handler.h"

// Initialize the static members
bool ExitHandler::initialized = false;
std::unordered_map<size_t, cleanup_function_t> ExitHandler::cleanup_functions;
size_t ExitHandler::current_id = 0;
bool ExitHandler::is_cleaning_up = false;

size_t ExitHandler::add_cleanup_function(cleanup_function_t func) {
	initialize();
	
    size_t id = current_id++;
    cleanup_functions[id] = func;
    return id;
}

void ExitHandler::remove_cleanup_function(size_t id) {
	if (is_cleaning_up) {  // it's wrong to remove functions to execute at the end when the end has already begun
		return;
	}
    cleanup_functions.erase(id);
}


void ExitHandler::initialize() {
	if (initialized) {
		return;
	} else {
		// begin initialization
	#ifdef _WIN32
	    if (!SetConsoleCtrlHandler(onCloseHandler, TRUE)) {
	        std::cerr << "Error: Unable to register the close handler on Windows." << std::endl;
	        exit(1);
	    }
	#else
	    // Register signal handlers for POSIX systems
	    signal(SIGTERM, onSignal); // Termination signal
	    signal(SIGINT, onSignal);  // Interrupt signal (CTRL+C)
	    signal(SIGHUP, onSignal);  // Hangup signal
	#endif
		// end initialization
		initialized = true;
	}
}


#ifdef _WIN32
BOOL WINAPI ExitHandler::onCloseHandler(DWORD event) {
    if (event == CTRL_CLOSE_EVENT || event == CTRL_C_EVENT) {  // Handle both close and Ctrl+C
        execute_cleanup_functions();
        return TRUE;  // Signal handled
    }
    return FALSE;  // Pass event to the system
}
#else
void ExitHandler::onSignal(int signal) {
    execute_cleanup_functions();
    exit(0);  // Safe termination
}
#endif

void ExitHandler::execute_cleanup_functions() {
	if (is_cleaning_up) return;  // Prevent recursive calls
    is_cleaning_up = true;
    
    for (std::unordered_map<size_t, cleanup_function_t>::const_iterator it = cleanup_functions.begin(); 
         it != cleanup_functions.end(); ++it) {
        try {
            it->second();  // execute function
        } catch (const std::exception& e) {
            std::cerr << "Exception during cleanup: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown exception during cleanup." << std::endl;
        }
    }
}

