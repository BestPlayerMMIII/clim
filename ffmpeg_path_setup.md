### How to Add FFmpeg's `bin` Directory to the System `Path` Variable

When you download FFmpeg directly from an official build (e.g., a ZIP file), you need to manually add the `bin` folder to your system's **`Path`** environment variable. This ensures that the FFmpeg tools (`ffmpeg`, `ffprobe`, etc.) can be accessed globally from any terminal or command line.

Below are detailed steps for Windows, macOS, and Linux:

---

#### **1. Windows**
1. **Extract FFmpeg**:
   - Download the FFmpeg ZIP file from the official FFmpeg website.
   - Extract it to a folder, such as `C:\ffmpeg\` (or any location of your choice).

2. **Locate the `bin` Directory**:
   - Inside the extracted folder, navigate to the `bin` directory, e.g., `C:\ffmpeg\bin`.

3. **Add to Environment Variables**:
   - Press `Win + R`, type `sysdm.cpl`, and hit Enter to open **System Properties**.
   - Go to the **Advanced** tab and click on **Environment Variables**.
   - In the "System variables" section, find and select the variable named **Path**. Click **Edit**.
   - Click **New** and add the full path to the FFmpeg `bin` directory (e.g., `C:\ffmpeg\bin`).
   - Click **OK** to save and close all dialogs.

4. **Verify**:
   - Open a **Command Prompt** or **PowerShell** and type:
     ```bash
     ffmpeg -version
     ```
   - If FFmpeg is correctly added, you'll see the version information.

---

#### **2. macOS**
1. **Download FFmpeg**:
   - Extract the FFmpeg build (e.g., using `tar` or double-click to unzip).

2. **Move the `bin` Directory**:
   - Optionally, place the FFmpeg folder in `/usr/local/` or another preferred directory. For example:
     ```bash
     sudo mv ffmpeg /usr/local/ffmpeg
     ```

3. **Add to `Path`**:
   - Open a terminal and edit your shell configuration file (`.zshrc` for Zsh or `.bashrc` for Bash):
     ```bash
     nano ~/.zshrc
     ```
   - Add the following line:
     ```bash
     export PATH="/usr/local/ffmpeg/bin:$PATH"
     ```
   - Save and exit (`Ctrl + O`, `Ctrl + X` in nano).

4. **Reload Configuration**:
   - Reload the shell configuration:
     ```bash
     source ~/.zshrc
     ```

5. **Verify**:
   - Run the command:
     ```bash
     ffmpeg -version
     ```
   - FFmpeg version details should appear.

---

#### **3. Linux**
1. **Download and Extract**:
   - Download the FFmpeg build and extract it:
     ```bash
     tar -xvf ffmpeg-release.tar.gz
     ```

2. **Move the `bin` Directory**:
   - Move FFmpeg to a suitable location, such as `/usr/local/ffmpeg`:
     ```bash
     sudo mv ffmpeg /usr/local/ffmpeg
     ```

3. **Add to `Path`**:
   - Open your shell configuration file (`~/.bashrc`, `~/.profile`, or `~/.zshrc`):
     ```bash
     nano ~/.bashrc
     ```
   - Add the following line:
     ```bash
     export PATH="/usr/local/ffmpeg/bin:$PATH"
     ```

4. **Reload Configuration**:
   - Reload the shell configuration:
     ```bash
     source ~/.bashrc
     ```

5. **Verify**:
   - Run:
     ```bash
     ffmpeg -version
     ```
   - FFmpeg should display its version information.

---

### Summary
By adding FFmpeg's `bin` folder to the `Path` environment variable, you make the FFmpeg tools available globally on your system.
This process is necessary when you download and extract FFmpeg manually, as opposed to installing it via package managers.
Follow the platform-specific steps to ensure FFmpeg runs seamlessly from any terminal or command line.
