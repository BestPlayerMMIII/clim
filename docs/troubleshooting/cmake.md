# CMake - Installation and Troubleshooting

If you want to Install CMake or for troubleshooting, please follow this guide.

---

CMake is a cross-platform build system generator that simplifies project builds. Follow these steps to set up CMake on your system:


## Install CMake

**Note**: this should resolve also the error `"cmake" is not recognized as an internal or external command operable program or batch file`

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


## Configure the Project with CMake

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


## Build the Project

1. **Use the Generated Build Files**:
   - Try to build the files. Cross-platform:
     ```bash
     make
     ```
     > Remember to use `mingw32-make` instead of `make` if you [installed Make with MinGW](make.md#option-a-preferred-install-make-via-mingw).
   - On Windows with Visual Studio:
     - Open the generated `.sln` file in Visual Studio.
     - Build the project from within the IDE.

2. **Locate the Executable**:
   - Once the build process is complete, the compiled binary files should be located in the `Player/build` directory, and `Player/player.exe` file should appear.

---

## Troubleshooting

- **"CMake is not recognized"**:
  - Ensure that CMake is installed and added to the system PATH.
  - On Windows, reopen the terminal after installation to refresh the PATH variable.

- **Missing Compiler**:
  - Ensure you have a C++ compiler installed:
    - **Windows**: Install [MinGW](make.md#option-a-preferred-install-make-via-mingw) or Visual Studio.
    - **Linux/macOS**: Install GCC or Clang via your package manager.

- **CMake Errors**:
  - Ensure all dependencies are installed and accessible.

The solutions to some possible problems are listed below.

---

### Solve Error `"cmake" is not recognized as an internal or external command operable program or batch file`
To resolve this error follow the [installation steps above](#install-cmake).

---

### Solve Error `CMake Error at CMakeLists.txt:2 (project): Running 'nmake' '-?' failed with: no such file or directory`

1. **`nmake` is not installed or accessible.**
   - `nmake` is Microsoft's build tool, typically bundled with Visual Studio. CMake defaults to using `nmake` when generating for "NMake Makefiles".
2. **CMake cannot find a C++ compiler.**
   - This means no compiler (like MSVC, GCC, or Clang) is properly set up or detected by CMake.

**Steps to Resolve**

#### 1. **Ensure a Compiler is Installed**
   > **Note**: Choose either MinGW **or** Visual Studio. If you have already installed one, proceed directly to step 2 and 3 corresponding to your selected option.
   - **Install MinGW (Minimalist GNU for Windows)**:
     - Download MinGW from [MinGW-w64](https://www.mingw-w64.org/).
     - Ensure `gcc.exe` and `g++.exe` are in your `PATH`.
   - **Install Visual Studio**:
     - Download and install Visual Studio from [here](https://visualstudio.microsoft.com/).
     - During installation, select the following workloads:
       - **Desktop development with C++** (this includes `nmake` and the MSVC compiler).

   - After installation, verify:
     ```bash
     gcc --version
     g++ --version
     ```


#### 2. **Ensure CMake is Installed**
   - Ensure CMake is installed and available in your system's `PATH` ([how to install CMake](#install-cmake)).
   - Verify with:
     ```bash
     cmake --version
     ```


#### 3. **Choose a Build System in CMake**

- CMake automatically tries to use `nmake` for "NMake Makefiles". If you don't want this (or don't have `nmake`), explicitly tell CMake to use an alternative generator.

##### Using MinGW Makefiles (Preferred for MinGW users)
1. Create a `build` directory:
   ```bash
   mkdir build
   cd build
   ```
2. Run CMake with the MinGW Makefiles generator:
   ```bash
   cmake -G "MinGW Makefiles" ..
   ```
3. Build the project:
   ```bash
   mingw32-make
   ```

##### Using Visual Studio (Preferred for Visual Studio users)
1. Run CMake with the Visual Studio generator:
   ```bash
   cmake -G "Visual Studio 16 2019" ..
   ```
2. Open the generated `.sln` file in Visual Studio.
3. Build the project in Visual Studio.

##### Using Ninja (Optional, Requires Ninja Installed)
1. Install Ninja via [Ninja's website](https://ninja-build.org/) or package managers.
2. Run CMake with the Ninja generator:
   ```bash
   cmake -G "Ninja" ..
   ```
3. Build with:
   ```bash
   ninja
   ```


#### 4. Set the Compiler Explicitly (If Needed)
If CMake still can't find your compiler, you may need to specify it manually. Add the following flags when running `cmake`:

- For MinGW:
  ```bash
  cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
  ```

- For MSVC:
  ```bash
  cmake -G "NMake Makefiles" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe ..
  ```
