#ifndef EXIT_HANDLER_H
#define EXIT_HANDLER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#include <cstdlib>
#endif

/**
 * @typedef cleanup_function_t
 * @brief Type alias for cleanup functions to execute on exit.
 */
using cleanup_function_t = std::function<void()>;

/**
 * @class ExitHandler
 * @brief A static utility class to handle cleanup operations at program termination.
 */
class ExitHandler {
public:
    /**
     * @brief Registers a cleanup function to execute on program termination.
     * @param func The cleanup function.
     * @return A unique ID for the cleanup function.
     */
    static size_t add_cleanup_function(cleanup_function_t func);

    /**
     * @brief Removes a cleanup function by its ID.
     * @param id The unique ID of the function.
     */
    static void remove_cleanup_function(size_t id);

private:
    static bool initialized;													///< Whether the class has been initialized.
    static std::unordered_map<size_t, cleanup_function_t> cleanup_functions;	///< Registered cleanup functions.
    static size_t current_id;													///< Counter for assigning unique IDs.
    static bool is_cleaning_up;													///< Whether cleanup is in progress.

    /**
     * @brief Initializes the ExitHandler.
     */
    static void initialize();

#ifdef _WIN32
    static BOOL WINAPI onCloseHandler(DWORD event);	///< Handles Windows console events.
#else
    static void onSignal(int signal);				///< Handles Unix signals.
#endif

    /**
     * @brief Executes all registered cleanup functions.
     */
    static void execute_cleanup_functions();
};

#endif	// EXIT_HANDLER_H

