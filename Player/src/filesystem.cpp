#include "filesystem.h"

// Custom filesystem namespace

namespace std {
    namespace filesystem {

        // FILES

        // Function to check if a file exists at the specified path.
        bool file_exists(const std::string& file_path) {
            // Attempt to open the file in read mode.
            std::ifstream file_check(file_path);
            bool exists = file_check.good();  // File exists if it can be opened successfully.
            if (exists) {
                file_check.close();  // Close the file if it exists.
            }
            return exists;
        }

        // Function to delete a file at the specified path.
        bool delete_file(const std::string& file_path) {
            // Use std::remove to delete the file and return whether the operation was successful.
            return (std::remove(file_path.c_str()) == 0);
        }

        // DIRECTORIES

        // Function to create directories recursively from the given path.
        bool create_directories(const std::string& path) {
            size_t pos = 0;
            std::string current_path;
            bool success = true;

            // Iterate through each component of the path to create directories step-by-step.
            while (pos < path.length()) {
                // Find the next '/' or '\' character, indicating the end of a directory name.
                pos = path.find_first_of("/\\", pos + 1);

                // Extract the current sub-path from the beginning to the current position.
                current_path = path.substr(0, pos);
                if (current_path.empty()) continue;  // Skip empty path components.

#ifdef _WIN32
                // On Windows, use _mkdir to create the directory. Ignore errors if the directory already exists.
                if (_mkdir(current_path.c_str()) != 0 && errno != EEXIST) {
                    success = false;
                    break;
                }
#else
                // On POSIX systems, use mkdir with full permissions (0777). Ignore errors for existing directories.
                if (mkdir(current_path.c_str(), 0777) != 0 && errno != EEXIST) {
                    success = false;
                    break;
                }
#endif
            }

            return success;  // Return true if all directories were created successfully.
        }

        // Function to check if a directory exists at the specified path.
        bool directory_exists(const std::string& dir_path) {
#ifdef _WIN32
            // On Windows, use GetFileAttributes to check for directory attributes.
            DWORD dwAttrib = GetFileAttributesA(dir_path.c_str());
            return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
            // On POSIX systems, use stat to check if the path points to a directory.
            struct stat info;
            return (stat(dir_path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
#endif
        }

        // Function to check if a directory is empty.
        bool is_directory_empty(const std::string& dir_path) {
#ifdef _WIN32
            // On Windows, use FindFirstFile/FindNextFile to iterate through directory contents.
            WIN32_FIND_DATAA find_file_data;
            HANDLE hFind = FindFirstFileA((dir_path + "\\*").c_str(), &find_file_data);

            if (hFind == INVALID_HANDLE_VALUE) {
                return false;  // Return false if the directory cannot be opened.
            }

            bool is_empty = true;
            do {
                // Ignore "." and ".." entries which represent the current and parent directories.
                std::string file_name = find_file_data.cFileName;
                if (file_name != "." && file_name != "..") {
                    is_empty = false;
                    break;
                }
            } while (FindNextFileA(hFind, &find_file_data) != 0);

            FindClose(hFind);  // Close the handle after iteration.
            return is_empty;
#else
            // On POSIX systems, use opendir/readdir to iterate through directory contents.
            DIR* dir = opendir(dir_path.c_str());
            if (!dir) {
                return false;  // Return false if the directory cannot be opened.
            }

            bool is_empty = true;
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                // Ignore "." and ".." entries.
                std::string file_name = entry->d_name;
                if (file_name != "." && file_name != "..") {
                    is_empty = false;
                    break;
                }
            }

            closedir(dir);  // Close the directory stream after iteration.
            return is_empty;
#endif
        }

        // Function to delete a directory and its contents recursively.
        bool delete_directory(const std::string& dir_path) {
#ifdef _WIN32
            // On Windows, use a system command to delete the directory and its contents.
            std::string command = "rmdir /s /q \"" + dir_path + "\"";
            return system(command.c_str()) == 0;  // Return true if the command was successful.
#else
            // On POSIX systems, use opendir/readdir to iterate and remove directory contents.
            DIR* dir = opendir(dir_path.c_str());
            if (!dir) {
                return false;  // Return false if the directory cannot be opened.
            }

            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string entry_name = entry->d_name;

                // Skip "." and ".." entries which represent the current and parent directories.
                if (entry_name == "." || entry_name == "..") {
                    continue;
                }

                std::string full_path = dir_path + "/" + entry_name;

                if (entry->d_type == DT_DIR) {
                    // If the entry is a directory, delete it recursively.
                    if (!delete_directory(full_path)) {
                        closedir(dir);
                        return false;
                    }
                } else {
                    // If the entry is a file, delete it.
                    if (std::remove(full_path.c_str()) != 0) {
                        closedir(dir);
                        return false;
                    }
                }
            }

            closedir(dir);  // Close the directory stream.

            // Remove the now-empty directory.
            if (rmdir(dir_path.c_str()) != 0) {
                return false;
            }

            return true;  // Return true if the directory was successfully deleted.
#endif
        }

        // Function to ensure a directory exists, creating it if necessary.
        bool ensure_directory_existence(const std::string& dir_path) {
            if (!directory_exists(dir_path)) {  // Check if the directory exists.
                return create_directories(dir_path);  // Create it if it does not exist.
            } else {
                return true;  // Return true if it already exists.
            }
        }

        // Function to ensure a directory is removed, deleting it if it exists.
        bool ensure_directory_removal(const std::string& dir_path) {
            if (directory_exists(dir_path)) {  // Check if the directory exists.
                return delete_directory(dir_path);  // Delete it if it exists.
            } else {
                return true;  // Return true if the directory does not exist.
            }
        }

    }  // namespace filesystem
}  // namespace std

