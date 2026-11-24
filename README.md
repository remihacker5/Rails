![Alt](resources/images/menu/icon.png)

# Rails 

[![C++14](https://img.shields.io/badge/C++-14-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Raylib 5.0](https://img.shields.io/badge/Raylib-5.0-000000?logo=raylib&logoColor=white)](https://www.raylib.com/)
[![Platforms](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS%20%7C%20Web-blue)](#-building)

## 📝 Description

<!-- Add a description of the game here. -->

## 🗺️ Roadmap

<!-- Add planned features and development milestones here. -->

## 🔨 Build from source

Rails is built with C++14 and Raylib 5.0. From the project root, install
the required C++ compiler and Raylib development files for your platform, then
follow the instructions below.

### 🐧 Linux

Install Raylib 5.0 or newer, a C++ compiler, GNU Make, and the OpenGL/X11
development libraries required by Raylib.

```sh
make
./rails
```

### 🪟 Windows

Install MinGW-w64 and Raylib for the desktop build. The Makefile is configured
for the following default paths:

- Raylib: `C:/raylib/raylib`
- w64devkit: `C:/raylib/w64devkit/bin`

Update `RAYLIB_PATH` and `COMPILER_PATH` in the Makefile if Raylib or the
toolchain is installed elsewhere.

```powershell
mingw32-make
.\rails
```

### 🍎 macOS

Install Xcode command-line tools, GNU Make, and Raylib for macOS. The Makefile
uses `clang++` and `/opt/homebrew` for Apple Silicon or an equivalent custom
Raylib installation.

```sh
make
./rails
```

### 🌐 Web

A web build requires Emscripten and a Raylib build for HTML5. Configure
`EMSDK_PATH`, `RAYLIB_PATH`, and the Makefile web options for your installed
toolchain, then run:

```sh
make PLATFORM=PLATFORM_WEB
```

### 🛠️ Debug build

Create a debug build on any desktop platform with:

```sh
make BUILD_MODE=DEBUG
```

The debug executable is named `rails-debug` on Linux and macOS and
`rails-debug.exe` on Windows.

In VS Code, the provided run and debug configurations also build from the
project root, so the `resources/` assets are available at runtime. Install the
recommended C/C++ extension (`ms-vscode.cpptools`) for debugging.

## 🙏 Credits
- **Raylib**: C/C++ Game engine made by Raysan5
- **Libresprite**: Used for art and sprite creation.
- **Overpass-api**: Used for railway design. (Rail map based on Leeds, England)
