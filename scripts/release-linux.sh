#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_NAME="my-lvgl-app"
APPDIR="$PROJECT_ROOT/AppDir"
OUTPUT="$PROJECT_ROOT/${APP_NAME}-${GITHUB_REF_NAME}-linux-x86_64.AppImage"

cd "$PROJECT_ROOT"

log() {
    printf "\n==> %s\n" "$1"
}

clean_previous_artifacts() {
    log "Cleaning previous Linux packaging artifacts"
    rm -rf "$APPDIR" "$PROJECT_ROOT/appimagetool" "$OUTPUT"
}

create_appdir() {
    log "Creating AppImage directory"
    mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/lib"
    cp "build/release/$APP_NAME" "$APPDIR/usr/bin/$APP_NAME"
}

copy_runtime_dependencies() {
    log "Copying runtime dependencies"

    while IFS= read -r lib; do
        [[ -f "$lib" ]] || continue

        case "$lib" in
            */libSDL2*.so*|*/libssl*.so*|*/libcrypto*.so*)
                cp -L "$lib" "$APPDIR/usr/lib/"
                ;;
        esac
    done < <(
        ldd "$APPDIR/usr/bin/$APP_NAME" |
            awk '{print $3}' |
            grep '^/' |
            sort -u
    )
}

create_apprun() {
    log "Creating AppRun"

    cat > "$APPDIR/AppRun" <<'EOF'
#!/bin/sh

    HERE="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
    export LD_LIBRARY_PATH="$HERE/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
    exec "$HERE/usr/bin/my-lvgl-app" "$@"
EOF

    chmod +x "$APPDIR/AppRun"
}

create_desktop_entry() {
    log "Creating desktop entry"

    cat > "$APPDIR/$APP_NAME.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=my-lvgl-app
Exec=my-lvgl-app
Terminal=false
Categories=Utility;
EOF
}

download_appimagetool() {
    log "Downloading appimagetool"

    curl -L --fail \
        -o "$PROJECT_ROOT/appimagetool" \
        https://github.com/AppImage/appimagetool/releases/latest/download/appimagetool-x86_64.AppImage

    chmod +x "$PROJECT_ROOT/appimagetool"
}

build_appimage() {
    log "Building AppImage"

    ARCH=x86_64 \
        "$PROJECT_ROOT/appimagetool" \
        "$APPDIR" \
        "$OUTPUT"
}

cleanup() {
    log "Cleaning temporary packaging files"
    rm -rf "$APPDIR" "$PROJECT_ROOT/appimagetool"
}

main() {
    clean_previous_artifacts
    create_appdir
    copy_runtime_dependencies
    create_apprun
    create_desktop_entry
    download_appimagetool
    build_appimage
    cleanup

    log "Created: $OUTPUT"
}

main "$@"
