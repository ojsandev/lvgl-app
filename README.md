# my-lvgl-app

Application built with **C++**, **LVGL 9.5**, **CMake**, **Conan** and **Ninja**, with a desktop development workflow and an **ESP32-S3 firmware target** based on **ESP-IDF 5.5.5**.

The desktop application uses CMake presets and a Bash build script. The embedded firmware lives in `esp32/` and is built with Espressif's `idf.py` tooling.

---

## Requirements

The following tools are required:

* Git
* CMake
* Ninja
* Conan
* Clang++

The setup script verifies that all required tools are available before configuring the project.

You can check your environment with:

```bash
scripts/dev.sh doctor
```

Example:

```text
==> Diagnose

==> Checking tools

git:    git version ...
cmake:  cmake version ...
ninja:  ...
conan:  Conan version ...
clang:  Apple/LLVM clang version ...
```

---

## Conan

The project uses Conan to manage its dependencies.

The development profile is:

```text
.conan/profiles/macos-clang26
```

The same profile is used for both the host and build contexts:

```bash
-pr:h=.conan/profiles/macos-clang26
-pr:b=.conan/profiles/macos-clang26
```

Dependencies are installed separately for Debug and Release configurations.

### Debug

```bash
conan install . \
    -of=build/debug \
    -pr:h=.conan/profiles/macos-clang26 \
    -pr:b=.conan/profiles/macos-clang26 \
    -s build_type=Debug \
    --build=missing
```

### Release

```bash
conan install . \
    -of=build/release \
    -pr:h=.conan/profiles/macos-clang26 \
    -pr:b=.conan/profiles/macos-clang26 \
    -s build_type=Release \
    --build=missing
```

Normally, you do not need to run these commands manually. The project script handles them automatically.

---

## Building

The recommended way to work with the project is through the build script.

Make sure the script is executable:

```bash
chmod +x scripts/dev.sh
```

### Debug Setup

To install dependencies, configure CMake and compile the Debug version:

```bash
scripts/dev.sh setup
```

This performs:

```text
Check tools
    ↓
Check project files
    ↓
Install Conan dependencies
    ↓
Configure CMake
    ↓
Build application
```

The resulting executable is:

```text
build/debug/my-lvgl-app
```

---

## Build Debug

If the Debug environment has already been configured:

```bash
scripts/dev.sh build
```

This executes the Debug CMake preset:

```bash
cmake --build --preset debug
```

---

## Run Debug

To compile the application if necessary and execute it:

```bash
scripts/dev.sh run
```

The script expects the executable at:

```text
build/debug/my-lvgl-app
```

If the executable does not exist, it automatically builds the Debug configuration first.

---

## Release

To install Release dependencies, configure CMake and compile the Release version:

```bash
scripts/dev.sh release
```

The resulting executable is:

```text
build/release/my-lvgl-app
```

---

## Run Release

To run the Release version:

```bash
scripts/dev.sh run-release
```

If the executable does not exist, the script automatically builds it first.

---

## Rebuild

To completely rebuild the Debug configuration:

```bash
scripts/dev.sh rebuild
```

This removes:

```text
build/debug
```

and runs the complete Debug setup again.

For Release:

```bash
scripts/dev.sh rebuild-release
```

---

## Clean

To remove all generated build artifacts:

```bash
scripts/dev.sh clean
```

This removes the entire:

```text
build/
```

directory.

After cleaning, the next setup will reinstall/configure the required build artifacts.

---

## Diagnostics

If something is not working, start with:

```bash
scripts/dev.sh doctor
```

The command checks:

1. Required tools
2. Required project files
3. Conan profile
4. CMake presets
5. Git working tree

It also displays the project root and active Conan profile.

Example:

```text
==> Diagnose

==> Checking tools
...

Project root: /path/to/my-lvgl-app
Conan profile: .conan/profiles/macos-clang26

CMake presets:
...

Git status:
...
```

This is the recommended first step when troubleshooting the development environment.

---

## CMake Presets

The project uses CMake presets instead of manually specifying build directories and compiler settings.

Available presets can be inspected with:

```bash
cmake --list-presets
```

The build script currently uses:

```text
debug
release
```

Configure Debug manually:

```bash
cmake --preset debug
```

Build Debug manually:

```bash
cmake --build --preset debug
```

Configure Release manually:

```bash
cmake --preset release
```

Build Release manually:

```bash
cmake --build --preset release
```

Using the presets keeps the build configuration consistent between developers and avoids long command lines.

---

## ESP32-S3 Firmware

The repository also contains an ESP-IDF firmware target in `esp32/` for the **Waveshare ESP32-S3-Touch-LCD-7B** board (ESP32-S3, 16 MB flash and 8 MB PSRAM).

### Firmware stack

- **ESP-IDF:** 5.5.5
- **Target:** `esp32s3`
- **LVGL:** 9.5.0, resolved through the ESP-IDF Component Manager
- **C++ LVGL wrapper:** `aptumfr/lv`, fetched by CMake FetchContent
- **Build system:** CMake and Ninja, orchestrated by `idf.py`
- **Application UI:** reuses the project's UI code from `app/ui`
- **Display and touch:** ESP32-specific implementation under `esp32/components/display/`

The embedded project is separate from the desktop CMake/Conan workflow. Run firmware commands from the `esp32/` directory.

### Install ESP-IDF

Install ESP-IDF **v5.5.5** and its required tools by following the [official ESP-IDF v5.5.5 Getting Started guide](https://docs.espressif.com/projects/esp-idf/en/v5.5.5/esp32/get-started/index.html). The [ESP-IDF Installation Manager](https://dl.espressif.com/dl/eim/) can be used to install the framework and tools.

For the current macOS setup, ESP-IDF was installed at:

```text
~/.espressif/v5.5.5/esp-idf
```

Activate the environment in **Fish** with:

```fish
source ~/.espressif/tools/activate_idf_v5.5.5.fish
```

If you use another shell or install ESP-IDF in a different location, use the activation command generated by your ESP-IDF installation.

Verify that the CLI is available:

```bash
idf.py --version
```

### Configure and build the firmware

From the repository root:

```bash
cd esp32
idf.py set-target esp32s3
idf.py reconfigure
idf.py build
```

The first configuration downloads/resolves the components declared in `main/idf_component.yml`. The resolved versions are recorded in `dependencies.lock`.

To configure options interactively:

```bash
idf.py menuconfig
```

### Flash and monitor

Connect the board over USB, then run from `esp32/`:

```bash
idf.py flash monitor
```

If multiple serial devices are connected, specify the board's serial port:

```bash
idf.py -p PORT flash monitor
```

Replace `PORT` with the appropriate serial port for your system. To build without flashing, use `idf.py build`.

### Firmware project layout

```text
esp32/
├── CMakeLists.txt
├── dependencies.lock
├── sdkconfig.defaults
├── main/
│   ├── CMakeLists.txt
│   ├── idf_component.yml
│   ├── main.cpp
│   └── ESP32TestUseCases.h
└── components/
    ├── app_ui/   # Reuses UI code from app/ui
    ├── common/   # Shared logger implementation
    ├── display/  # ESP32 display and touch drivers
    └── lv_cpp/   # C++ LVGL wrapper component
```

Generated files such as `esp32/build/`, `esp32/managed_components/`, `esp32/.deps/` and the machine-specific `esp32/sdkconfig` are build/local state and should not be committed. Keep `esp32/sdkconfig.defaults` and `esp32/dependencies.lock` under version control so other developers can reproduce the firmware configuration and dependency versions.

### Current status

The ESP-IDF target has been **built successfully**. The current `esp32/main/ESP32TestUseCases.h` provides fake/test character data (Tanjiro and Nezuko) for exercising the UI; it is not yet the production data/API integration. Flash the board and run the serial monitor to validate the firmware on the physical device.

---

## Formatting

The project uses `.clang-format` for C/C++ source formatting.

To format an individual file:

```bash
clang-format -i --style=file:.clang-format src/main.cpp
```

To format the entire project:

```bash
find . \
  -type d \( -name build -o -name third_party -o -name vendor \) -prune -o \
  -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) \
  -print0 | xargs -0 clang-format -i --style=file:.clang-format
```

---

## Typical Development Workflow

For a fresh checkout:

```bash
git clone <repository>
cd my-lvgl-app

scripts/dev.sh doctor
scripts/dev.sh setup
scripts/dev.sh run
```

After making changes:

```bash
scripts/dev.sh build
scripts/dev.sh run
```

For a clean rebuild:

```bash
scripts/dev.sh rebuild
scripts/dev.sh run
```

---

## Command Reference

| Command                      | Description                          |
| ---------------------------- | ------------------------------------ |
| `scripts/dev.sh setup`           | Configure and build Debug            |
| `scripts/dev.sh build`           | Build Debug                          |
| `scripts/dev.sh run`             | Build if necessary and run Debug     |
| `scripts/dev.sh release`         | Configure and build Release          |
| `scripts/dev.sh run-release`     | Build if necessary and run Release   |
| `scripts/dev.sh rebuild`         | Clean and rebuild Debug              |
| `scripts/dev.sh rebuild-release` | Clean and rebuild Release            |
| `scripts/dev.sh clean`           | Remove all build artifacts           |
| `scripts/dev.sh doctor`          | Diagnose the development environment |

---

## Troubleshooting

### Conan profile not found

If you see:

```text
ERROR: Conan profile not found: .conan/profiles/macos-clang26
```

verify that the profile exists:

```bash
ls -la .conan/profiles/
```

The expected profile is:

```text
.conan/profiles/macos-clang26
```

---

### `lv_conf.h` not found

If you see:

```text
ERROR: lv_conf.h not found
```

make sure `lv_conf.h` exists in the project root:

```bash
ls -la lv_conf.h
```

---

### Required command not found

If `doctor` reports something such as:

```text
ERROR: Command not found: conan
```

install the missing dependency and make sure it is available through your `PATH`.

You can verify it with:

```bash
command -v conan
```

The same approach can be used for:

```text
git
cmake
ninja
clang++
```

---

### Build directory is corrupted

Remove the build artifacts and recreate the configuration:

```bash
scripts/dev.sh clean
scripts/dev.sh setup
```

For Debug only:

```bash
scripts/dev.sh rebuild
```

For Release only:

```bash
scripts/dev.sh rebuild-release
```

---

## Development Philosophy

The project aims to keep the development workflow simple and reproducible:

```text
Source Code
     │
     ▼
   Conan
     │
     ▼
  CMake
     │
     ▼
  Ninja
     │
     ▼
   Clang
     │
     ▼
 my-lvl-app
```

The build script provides a single entry point for the most common development operations while CMake presets maintain the actual build configuration.

---

## 📄 License

This project is licensed under the **GNU General Public License v3.0** - see the [LICENSE](LICENSE) file for details.

### Permissions & Conditions
* **Commercial use, modification, and distribution** are allowed.
* **Source code disclosure** is mandatory if you distribute modified versions.
* **Same license**: Any derivatives or modifications must be released under the GPLv3 license as well.


