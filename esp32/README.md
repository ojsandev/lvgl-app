# ESP32-S3 Firmware

This directory builds firmware for the Waveshare ESP32-S3-Touch-LCD-7B using ESP-IDF 5.5.5. The target is `esp32s3`, with 16 MB flash and octal PSRAM.

## Activate ESP-IDF

Activate ESP-IDF before using `idf.py` or the project wrapper. On the supported Fish setup:

```fish
source ~/.espressif/tools/activate_idf_v5.5.5.fish
```

For Bash, Zsh, or another installation path, use the activation command supplied by that ESP-IDF installation. Confirm the active version:

```bash
idf.py --version
scripts/esp.sh doctor
```

The wrapper rejects versions other than 5.5.5.

## Build And Flash

Run these commands from the repository root after activating ESP-IDF:

```bash
scripts/esp.sh configure
scripts/esp.sh build
scripts/esp.sh flash --port /dev/cu.usbmodemXXXX
scripts/esp.sh monitor --port /dev/cu.usbmodemXXXX
```

On Linux, serial devices are typically `/dev/ttyACM0` or `/dev/ttyUSB0`. The combined command is:

```bash
scripts/esp.sh flash-monitor --port PORT
```

`configure` selects `esp32s3` and regenerates the ESP-IDF build system. It is required after a clean checkout or target change. `clean` runs `idf.py fullclean` and removes all local firmware build state.

## Configuration And Dependencies

- `sdkconfig.defaults` contains reproducible board defaults: 16 MB flash, large single-app partition, octal PSRAM, 16-bit LVGL color and C++ exceptions.
- `dependencies.lock` pins ESP-IDF 5.5.5 and LVGL 9.5.0.
- `main/idf_component.yml` declares managed ESP-IDF components.
- `.deps/` contains the C++ LVGL wrapper fetched at its pinned revision and is local build state.

The current firmware image uses the large single-app partition. It leaves substantially more room than the default single-app layout; use a custom partition table before adding OTA support.

## Module Composition

The firmware reuses the shared `common`, `domain` contracts and `app` components through `EXTRA_COMPONENT_DIRS`. `display` implements the platform-specific LVGL display and touch driver. `demo_usecases` supplies intentionally local demonstration data while ESP-IDF HTTP, image-cache and production data adapters are not implemented yet.

`main` is only the composition root. Do not add reusable UI, domain or platform code to it.

## IDEs

Open `esp32/` as a separate project root for ESP-IDF-aware tools. It has a different toolchain and compilation database from the desktop target. Run `scripts/esp.sh configure` before relying on `esp32/build/compile_commands.json`.

- VS Code: install Espressif IDF and activate/configure the local ESP-IDF extension for the machine.
- CLion: open `esp32/` separately and configure the ESP-IDF integration with version 5.5.5.
- Zed: invoke `scripts/esp.sh build` from a terminal/session where ESP-IDF is already active.
