# CMake - Installation and Troubleshooting

If you want to Install CMake or to solve "cmake is not recognized as an internal or external command operable program or batch file" Error, please follow this guide.

---

CMake is a cross-platform build system generator that simplifies project builds. Follow these steps to set up CMake on your system:


## Step 1: Install CMake

### On Windows
1. **Download CMake**:
   - Visit the official [CMake download page](https://cmake.org/download/).
   - Download the installer for Windows.
2. **Run the Installer**:
   - During installation, select the option to "Add CMake to the system PATH for all users."
   - Complete the installation process.
3. **Verify Installation**:
   - Open a Command Prompt or PowerShell and type:
     ```bash
     cmake --version
     ```
   - If the version number appears, CMake is installed and available in your PATH.

### On Linux
1. **Install Using Package Manager**:
   - **Ubuntu/Debian**:
     ```bash
     sudo apt update
     sudo apt install cmake
     ```
   - **Fedora**:
     ```bash
     sudo dnf install cmake
     ```
   - **Arch**:
     ```bash
     sudo pacman -S cmake
     ```
2. **Verify Installation**:
   - Run:
     ```bash
     cmake --version
     ```

### On macOS
1. **Install Using Homebrew**:
   - If you don’t have Homebrew installed, first install it:
     ```bash
     /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
     ```
   - Then install CMake:
     ```bash
     brew install cmake
     ```
2. **Verify Installation**:
   - Run:
     ```bash
     cmake --version
     ```


## Step 2: Configure the Project with CMake

1. **Check the Project Directory**:
   - Ensure you have a `CMakeLists.txt` file in the `Player` directory. This file defines the project and its build configuration.

2. **Create a Build Directory**:
   - It’s a good practice to build your project in a separate directory to keep files organized.
   - Create a `build` directory. From the `Player` directory:
     ```bash
     mkdir build
     cd build
     ```

3. **Run CMake Configuration**:
   - Inside the `build` directory, run:
     ```bash
     cmake ..
     ```
   - This will generate the build files for your project. CMake will automatically detect your platform and create appropriate build scripts.

---

### Step 3: Build the Project

1. **Use the Generated Build Files**:
   - Try to build the files. Cross-platform:
     ```bash
     make
     ```
     > Remember to use `mingw32-make` instead of `make` if you [installed Make with MinGW](not_recognized_make.md#option-a-preferred-install-make-via-mingw).
   - On Windows with Visual Studio:
     - Open the generated `.sln` file in Visual Studio.
     - Build the project from within the IDE.

2. **Locate the Executable**:
   - Once the build process is complete, the compiled binary files should be located in the `Player/build` directory, and `Player/player.exe` file should appear.

---

### Step 4: Troubleshooting

- **"CMake is not recognized"**:
  - Ensure that CMake is installed and added to the system PATH.
  - On Windows, reopen the terminal after installation to refresh the PATH variable.

- **Missing Compiler**:
  - Ensure you have a C++ compiler installed:
    - **Windows**: Install [MinGW](not_recognized_make.md#option-a-preferred-install-make-via-mingw) or Visual Studio.
    - **Linux/macOS**: Install GCC or Clang via your package manager.

- **CMake Errors**:
  - Ensure all dependencies are installed and accessible.
