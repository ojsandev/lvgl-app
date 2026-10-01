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

create_icon() {
    log "Creating application icon"

    cat > "$APPDIR/$APP_NAME.svg" <<'EOF'
<svg xmlns="http://www.w3.org/2000/svg" width="256" height="256" viewBox="0 0 256 256">
  <rect width="256" height="256" rx="48" fill="#222"/>
  <rect x="48" y="48" width="160" height="160" rx="24" fill="#7C3AED"/>
  <path d="M80 88h96v24h-36v56h-24v-56H80z" fill="#fff"/>
</svg>
EOF
}

create_desktop_entry() {
    log "Creating desktop entry"

    cat > "$APPDIR/$APP_NAME.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=$APP_NAME
Exec=$APP_NAME
Icon=$APP_NAME
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

verify_appimage() {
    log "Verifying AppImage"

    [[ -s "$OUTPUT" ]] || {
        echo "AppImage was not created: $OUTPUT" >&2
        return 1
    }

    file "$OUTPUT" | grep -q "ELF" || {
        echo "Generated file is not a valid AppImage ELF executable: $OUTPUT" >&2
        return 1
    }
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
    create_icon
    create_desktop_entry
    download_appimagetool
    build_appimage
    verify_appimage
    cleanup

    log "Created: $OUTPUT"
}

main "$@"
