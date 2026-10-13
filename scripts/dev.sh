#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

APP_NAME="my-lvgl-app"

case "$(uname -s)" in
    Darwin)
        DEFAULT_CONAN_PROFILE=".conan/profiles/macos-clang26"
        ;;
    Linux)
        DEFAULT_CONAN_PROFILE=".conan/profiles/linux-gcc"
        ;;
    *)
        DEFAULT_CONAN_PROFILE=""
        ;;
esac

CONAN_PROFILE="${CONAN_PROFILE:-$DEFAULT_CONAN_PROFILE}"

log() {
    printf '\n==> %s\n' "$1"
}

die() {
    printf '\nERROR: %s\n' "$1" >&2
    exit 1
}

require_command() {
    command -v "$1" >/dev/null 2>&1 || die "Command not found: $1"
}

check_tools() {
    log "Checking tools"

    require_command git
    require_command cmake
    require_command ninja
    require_command conan
    require_command clang++

    echo "git:    $(git --version)"
    echo "cmake:  $(cmake --version | head -n 1)"
    echo "ninja:  $(ninja --version)"
    echo "conan:  $(conan --version)"
    echo "clang:  $(clang++ --version | head -n 1)"
}

check_files() {
    log "Checking project files"

    [[ -f lv_conf.h ]] || die "lv_conf.h not found"
    [[ -n "$CONAN_PROFILE" ]] || die "Unsupported host OS: $(uname -s)"
    [[ -f "$CONAN_PROFILE" ]] || die "Conan profile not found: $CONAN_PROFILE"
}

install_dependencies() {
    local build_type="$1"
    local build_dir="build/$(echo "$build_type" | tr '[:upper:]' '[:lower:]')"

    log "Installing Conan dependencies ($build_type)"

    conan install . \
        -of="$build_dir" \
        -pr:h="$CONAN_PROFILE" \
        -pr:b="$CONAN_PROFILE" \
        -s build_type="$build_type" \
        --build=missing
}

configure() {
    local preset="$1"

    log "Configuring CMake ($preset)"

    cmake --preset "$preset"
}

build() {
    local preset="$1"

    log "Compiling $APP_NAME ($preset)"

    cmake --build --preset "$preset"
}

run_app() {
    local preset="$1"
    local build_dir="build/$preset"

    log "Executing $APP_NAME ($preset)"

    [[ -x "$build_dir/$APP_NAME" ]] || {
        echo "Executable not found. Compiling..."
        build "$preset"
    }

    "./$build_dir/$APP_NAME"
}

test_debug() {
    check_tools
    check_files

    [[ -f build/debug/conan_toolchain.cmake ]] || die "Debug dependencies are not configured; run '$0 setup' first"

    log "Building and running deterministic desktop tests"
    cmake --build --preset debug
    ctest --test-dir build/debug --output-on-failure -LE network
}

clean() {
    log "Cleaning build artifacts"

    rm -rf build
}

setup_debug() {
    check_tools
    check_files
    install_dependencies "Debug"
    configure "debug"
    build "debug"
}

setup_release() {
    check_tools
    check_files
    install_dependencies "Release"
    configure "release"
    build "release"
}

rebuild_debug() {
    rm -rf build/debug
    setup_debug
}

rebuild_release() {
    rm -rf build/release
    setup_release
}

doctor() {
    log "Diagnose"

    check_tools
    check_files

    echo
    echo "Project root: $PROJECT_ROOT"
    echo "Conan profile: $CONAN_PROFILE"

    echo
    echo "CMake presets:"
    cmake --list-presets

    echo
    echo "Git status:"
    git status --short
}

usage() {
    cat <<EOF

Usage:

  $0 setup
  $0 build
  $0 run
  $0 test

  $0 release
  $0 run-release

  $0 rebuild
  $0 rebuild-release

  $0 clean
  $0 doctor
EOF
}

main() {
    case "${1:-}" in
        setup)
            setup_debug
            ;;

        build)
            build "debug"
            ;;

        run)
            run_app "debug"
            ;;

        test)
            test_debug
            ;;

        release)
            setup_release
            ;;

        run-release)
            run_app "release"
            ;;

        rebuild)
            rebuild_debug
            ;;

        rebuild-release)
            rebuild_release
            ;;

        clean)
            clean
            ;;

        doctor)
            doctor
            ;;

        *)
            usage
            exit 1
            ;;
    esac
}

main "$@"
