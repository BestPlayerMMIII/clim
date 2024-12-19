# Make - Installation and Troubleshooting

If you want to install Make or need help with troubleshooting, follow this guide.

---

## 1. Install Make

**Note**: this should resolve also the error `"make" is not recognized as an internal or external command operable program or batch file`

### On Windows
`make` is not included by default in Windows. Use one of the following methods:

#### Option A (preferred): Install Make via MinGW
1. **Download MinGW**:
   - Visit the [MinGW website](https://osdn.net/projects/mingw/) and download the installer.
2. **Install MinGW**:
   - During installation, select the "MinGW Developer Toolkit" option, which includes `mingw32-make`.
3. **Add MinGW to the PATH**:
   - Press `Win + R`, type `sysdm.cpl`, and press Enter.
   - Go to the **Advanced** tab > **Environment Variables**.
   - Under "System Variables", find `Path` and click **Edit**.
   - Add the path to the MinGW `bin` directory (e.g., `C:\MinGW\bin`).
4. **Test Installation**:
   - Open a **new** Command Prompt and run:
     ```bash
     mingw32-make --version
     ```
   - Use `mingw32-make` instead of `make` if necessary.

#### Option B: Use `make` via WSL (Windows Subsystem for Linux)
1. **Install WSL**:
   - Open PowerShell as Administrator and run:
     ```powershell
     wsl --install
     ```
     This will install WSL and a default Linux distribution (e.g., Ubuntu).
2. **Install `make` in WSL**:
   - Open the WSL terminal and run:
     ```bash
     sudo apt update
     sudo apt install build-essential
     ```
3. **Run Make Commands**:
   - Use the WSL terminal to navigate to your project directory and execute `make`.

#### Option C: Install via Chocolatey
1. Install Chocolatey:
   - Open PowerShell as Administrator and run:
     ```powershell
     Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
     ```
2. Install Make:
   - Run:
     ```powershell
     choco install make
     ```
3. Verify Installation:
   - Open a new Command Prompt or PowerShell and run:
     ```bash
     make --version
     ```

---

### On Linux
`make` is usually available as part of standard development tools. Install it using your package manager:

- **Ubuntu/Debian**:
  ```bash
  sudo apt update
  sudo apt install build-essential
  ```
- **Fedora**:
  ```bash
  sudo dnf groupinstall "Development Tools"
  ```
- **Arch**:
  ```bash
  sudo pacman -S base-devel
  ```

Verify installation:
```bash
make --version
```

---

### On macOS
`make` is included with the Xcode Command Line Tools.

1. Install the tools:
   ```bash
   xcode-select --install
   ```
2. Verify installation:
   ```bash
   make --version
   ```

## 2. Verify if Make is installed correctly
Open your terminal or command prompt and run:  
```bash  
make --version  
```  
The command should now be recognized, displaying the Make version.

**Still some errors?**

Do not quit here if other errors are shown!

Please try using [CMake](cmake.md) instead: it is cross-platform and should resolve any pending errors.