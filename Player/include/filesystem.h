#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
#else
    #include <dirent.h>
    #include <unistd.h>
#endif

namespace std {
    namespace filesystem {

        /**
         * @brief Checks if a file exists.
         * @param file_path The path to the file.
         * @return True if the file exists, false otherwise.
         */
        bool file_exists(const std::string& file_path);

        /**
         * @brief Deletes a file.
         * @param file_path The path to the file.
         * @return True if the file was successfully deleted, false otherwise.
         */
        bool delete_file(const std::string& file_path);

        /**
         * @brief Creates directories recursively.
         * @param path The path to the directory.
         * @return True if directories were created successfully, false otherwise.
         */
        bool create_directories(const std::string& path);

        /**
         * @brief Checks if a directory exists.
         * @param dir_path The path to the directory.
         * @return True if the directory exists, false otherwise.
         */
        bool directory_exists(const std::string& dir_path);

        /**
         * @brief Checks if a directory is empty.
         * @param dir_path The path to the directory.
         * @return True if the directory is empty, false otherwise.
         */
        bool is_directory_empty(const std::string& dir_path);

        /**
         * @brief Deletes a directory recursively.
         * @param dir_path The path to the directory.
         * @return True if the directory was successfully deleted, false otherwise.
         */
        bool delete_directory(const std::string& dir_path);

        /**
         * @brief Ensures a directory exists, creating it if necessary.
         * @param dir_path The path to the directory.
         * @return True if the directory exists or was created successfully, false otherwise.
         */
        bool ensure_directory_existence(const std::string& dir_path);

        /**
         * @brief Ensures a directory is removed, deleting it if it exists.
         * @param dir_path The path to the directory.
         * @return True if the directory was successfully removed, false otherwise.
         */
        bool ensure_directory_removal(const std::string& dir_path);

    }	// namespace filesystem
}	// namespace std

#endif	// FILESYSTEM_H

