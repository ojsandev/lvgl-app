#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_NAME="my-lvgl-app"
APP="$PROJECT_ROOT/dist/$APP_NAME.app"
OUTPUT="$PROJECT_ROOT/${APP_NAME}-${GITHUB_REF_NAME}-macos-arm64.dmg"

cd "$PROJECT_ROOT"

log() {
    printf "\n==> %s\n" "$1"
}

clean_previous_artifacts() {
    log "Cleaning previous macOS packaging artifacts"
    rm -rf "$PROJECT_ROOT/dist" "$OUTPUT"
}

create_app_bundle() {
    log "Creating application bundle"
    mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Frameworks"
    cp "build/release/$APP_NAME" "$APP/Contents/MacOS/$APP_NAME"
    chmod +x "$APP/Contents/MacOS/$APP_NAME"
}

remove_adhoc_signature() {
    log "Removing ad-hoc code signatures"

    # macOS/ld may produce an ad-hoc signature for the executable. An
    # unsigned GitHub release is preferable to shipping a malformed
    # signature that Gatekeeper reports as a damaged application.
    codesign --remove-signature "$APP/Contents/MacOS/$APP_NAME" 2>/dev/null || true
    codesign --remove-signature "$APP" 2>/dev/null || true
}

create_info_plist() {
    log "Creating Info.plist"
    local version="${GITHUB_REF_NAME#v}"
    cat > "$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>my-lvgl-app</string>
<key>CFBundleIdentifier</key><string>com.ojsandev.my-lvgl-app</string>
<key>CFBundleName</key><string>my-lvgl-app</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>CFBundleVersion</key><string>${version}</string>
<key>CFBundleShortVersionString</key><string>${version}</string>
</dict></plist>
EOF
}

find_brew_library() {
    local formula="$1"
    local pattern="$2"
    find "$(brew --prefix "$formula")/lib" -maxdepth 1 -name "$pattern" | head -n 1
}

bundle_library() {
    local library="$1"
    [[ -f "$library" ]] || return 0
    cp -L "$library" "$APP/Contents/Frameworks/"
    local name
    name="$(basename "$library")"
    install_name_tool -change "$library" "@loader_path/../Frameworks/$name" "$APP/Contents/MacOS/$APP_NAME" || true
}

bundle_runtime_dependencies() {
    log "Bundling runtime dependencies"
    bundle_library "$(find_brew_library sdl2 'libSDL2*.dylib')"
    bundle_library "$(find_brew_library openssl@3 'libssl*.dylib')"
    bundle_library "$(find_brew_library openssl@3 'libcrypto*.dylib')"
}

create_dmg_root() {
    log "Preparing DMG contents"
    mkdir -p "$PROJECT_ROOT/dist/dmg-root"
    cp -R "$APP" "$PROJECT_ROOT/dist/dmg-root/"
}

create_dmg() {
    log "Creating DMG"
    create-dmg \
        --volname "$APP_NAME" \
        --window-size 600 400 \
        --app-drop-link 450 200 \
        "$OUTPUT" \
        "$PROJECT_ROOT/dist/dmg-root/"
}

verify_dmg() {
    log "Verifying DMG"

    [[ -s "$OUTPUT" ]] || {
        echo "DMG was not created: $OUTPUT" >&2
        return 1
    }

    hdiutil verify "$OUTPUT"
    test -x "$APP/Contents/MacOS/$APP_NAME"

    if codesign --verify --deep --strict "$APP" 2>/dev/null; then
        echo "Unexpected valid code signature found on unsigned release"
        return 1
    fi
}

main() {
    clean_previous_artifacts
    create_app_bundle
    remove_adhoc_signature
    create_info_plist
    bundle_runtime_dependencies
    create_dmg_root
    create_dmg
    verify_dmg
    log "Created: $OUTPUT"
}

main "$@"
