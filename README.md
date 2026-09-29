# my-lvgl-app

Application built with **C++**, **LVGL**, **CMake**, **Conan** and **Ninja**.

The project provides a small and reproducible development workflow through CMake presets and a Bash build script.

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
./build.sh doctor
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
chmod +x dev.sh
```

### Debug Setup

To install dependencies, configure CMake and compile the Debug version:

```bash
./dev.sh setup
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
dev/debug/my-lvl-app
```

---

## Build Debug

If the Debug environment has already been configured:

```bash
./dev.sh build
```

This executes the Debug CMake preset:

```bash
cmake --build --preset debug
```

---

## Run Debug

To compile the application if necessary and execute it:

```bash
./dev.sh run
```

The script expects the executable at:

```text
build/debug/my-lvl-app
```

If the executable does not exist, it automatically builds the Debug configuration first.

---

## Release

To install Release dependencies, configure CMake and compile the Release version:

```bash
./dev.sh release
```

The resulting executable is:

```text
build/release/my-lvl-app
```

---

## Run Release

To run the Release version:

```bash
./dev.sh run-release
```

If the executable does not exist, the script automatically builds it first.

---

## Rebuild

To completely rebuild the Debug configuration:

```bash
./dev.sh rebuild
```

This removes:

```text
build/debug
```

and runs the complete Debug setup again.

For Release:

```bash
./dev.sh rebuild-release
```

---

## Clean

To remove all generated build artifacts:

```bash
./dev.sh clean
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
./build.sh doctor
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

Project root: /path/to/my-lvl-app
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
cd my-lvl-app

chmod +x dev.sh

./dev.sh doctor
./dev.sh setup
./dev.sh run
```

After making changes:

```bash
./dev.sh build
./dev.sh run
```

For a clean rebuild:

```bash
./dev.sh rebuild
./dev.sh run
```

---

## Command Reference

| Command                      | Description                          |
| ---------------------------- | ------------------------------------ |
| `./dev.sh setup`           | Configure and build Debug            |
| `./dev.sh build`           | Build Debug                          |
| `./dev.sh run`             | Build if necessary and run Debug     |
| `./dev.sh release`         | Configure and build Release          |
| `./dev.sh run-release`     | Build if necessary and run Release   |
| `./dev.sh rebuild`         | Clean and rebuild Debug              |
| `./dev.sh rebuild-release` | Clean and rebuild Release            |
| `./dev.sh clean`           | Remove all build artifacts           |
| `./dev.sh doctor`          | Diagnose the development environment |

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
./build.sh clean
./build.sh setup
```

For Debug only:

```bash
./build.sh rebuild
```

For Release only:

```bash
./build.sh rebuild-release
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

## License

Add the project license information here.
