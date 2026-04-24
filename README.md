<p align="center">
  <img src="icon.svg" alt="Godot Logo" width="128" height="128" />
</p>

<h1 align="center">Godot Symbol Search</h1>

<p align="center">
  <b>Quick search/gotos for all symbols in currently active scripts.</b>
</p>

<p align="center">
  <!-- <a href="https://godotengine.org/asset-library/asset/xxxx">
    <img src="https://img.shields.io/badge/Godot%20Asset%20Lib-4.6%2B-478cbf?logo=godot-engine" alt="Godot Asset Library">
  </a> -->
  <img src="https://img.shields.io/github/license/travlee/godot-symbol-search" alt="License">
</p>

This is a plugin for the [Godot Game Engine](https://godotengine.org/) created with [godot-cpp](https://github.com/godotengine/godot-cpp) that provides quick searching/gotos for all symbols in the currently active script file using a **currently** hardcoded keyboard shortcut: `Ctrl + Shift + O`. This is a recreation of VSCode's `Go To Symbol` utility, improving workflow efficiency for developers who are familiar with this ... workflow. Mainly just a pain point for **me** working in Godot. Now solved.

---

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Features](#features)
- [Usage](#usage)
- [Installation](#installation)
  - [Godot Asset Library (recommended)](#godot-asset-library-recommended)
  - [Releases (easy)](#releases-easy)
  - [Manual (fun)](#manual-fun)
- [Technical Details](#technical-details)
- [Contributing](#contributing)
- [License](#license)

## Features

*   **Quick Symbol Search**: Instant access to all symbols in the current script using `Ctrl + Shift + O`.
*   **Fuzzy Matching**: Find symbols quickly by typing just a few characters. Embrace the lazy.
*   **Live Navigation**: The script editor jumps to the symbol's location as you navigate the search list, providing an immediate preview.
*   **Reset Cursor Position**: If you cancel the search (Esc), the cursor position resets to the starting line and column.
*   **Comprehensive Symbol Extraction**: Currently parses and categorizes `func`, `var`, `const`, `signal`, and `static` symbols.
*   **Detached Script Editor Support**: Works when script editor is popped out!
*   **Symbol Cursor Go-To**: Places the caret at the exact line and column of the selected symbol.
*   **High Performance**: Built with C++ (GDExtension) for near-instant indexing and filtering, even in massive script files.

## Usage

Once the plugin is enabled, you can use the following keyboard shortcuts:

*   **Trigger the Popup**:
    *   Press `Ctrl + Shift + O`.
    *   A popup will appear, displaying a list of symbols in your active script, sorted from top to bottom.

*   **Navigate the List**:
    *   Use the arrow keys to navigate symbols. The script editor will goto the selected symbol's line as you navigate about.

*   **Select a Symbol**:
    *   Press the `Enter` key.
    *   The editor will goto the line your selected symbol is on in the Script Editor.

## Installation

### Godot Asset Library (recommended)

1. Open Godot → **AssetLib** tab
2. Search **"[GodotSymbolSearch](https://godotengine.org/asset-library/asset/5035)"** → **Download**
3. In the install dialog, make sure **"Ignore asset root"** is **checked**, then click **Install**
4. **Project → Project Settings** → **Plugins** → Enable **Godot Symbol Search**
5. **Restart the Editor**

### Releases (easy)

Head to releases and download whatever version you want. I'll have files for Windows/Linux/Mac available there.

1. Download a release zip.
   * [Latest release](https://github.com/Travlee/godot-symbol-search/releases/latest)
   * [All releases](https://github.com/Travlee/godot-symbol-search/releases)
2. Extract to your project's addons dir: `../addons/godot-symbol-search`.
3. Enable the plugin:
   1. **Project** → **Project Settings** → **Plugins**
   2.   Click to enable **Godot Symbol Search**

### Manual (fun)
You'll have to compile it on your target platform and then enable it in your Godot project. This project requries CMake, GCC, and Ninja **my** preference recommended.

1.  **Clone the Repository**.
    ```bash
    cd godot-symbol-search/
    git clone https://github.com/travlee/godot-symbol-search.git .
    ```

2.  **Compiling**: This plugin is written in C++ using `godot-cpp`. You must compile the plugin for your specific OS and architecture using CMake. **I use Ninja as the build system, if you use Visual Studio, the output dir will differ so you'll have to combine them into the `addons/godot_symbol_search/` dir.**
    *   From the root of the project, configure the CMake build:
        ```bash
        cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release
        ```
    *   Build the plugin:
        ```bash
        cmake --build build --config Release
        ```
    *   After building, simply copy the `./godot/addons/godot_symbol_search/` dir to your project's `./addons/` dir.

3.  **Enable the Plugin in Godot**:
    *   **Project** → **Project Settings** → **Plugins**
    *   Click to enable **Godot Symbol Search**


## Technical Details

This plugin is implemented in C++, using Godot's **GDExtension** with [godot-cpp](https://github.com/godotengine/godot-cpp) bindings for blazingly fast performance and deep integration with the Godot editor. Also I wanted to.


## Contributing

If you encounter any bugs, have suggestions for new features, or would like to improve the existing code, please feel free to:

*  **Open an Issue**: Describe the bug or feature request in detail on the GitHub issue tracker.
*   **Submit a Pull Request**: Fork the repo, make some changes, and submit a PR.

## License

[MIT](LICENSE)
