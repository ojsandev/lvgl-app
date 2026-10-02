#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_NAME="my-lvgl-app"
VERSION="${VERSION:-${GITHUB_REF_NAME:-dev}}"
APP="$PROJECT_ROOT/dist/$APP_NAME.app"
DMG_ROOT="$PROJECT_ROOT/dist/dmg-root"
OUTPUT="$PROJECT_ROOT/${APP_NAME}-${VERSION}-macos-arm64.dmg"

cd "$PROJECT_ROOT"

log() {
    printf "\n==> %s\n" "$1"
}

clean_previous_artifacts() {
    log "Cleaning previous macOS packaging artifacts"
    rm -rf "$PROJECT_ROOT/dist" "$OUTPUT"
}

require_command() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "Required command not found: $1" >&2
        return 1
    }
}

check_tools() {
    log "Checking macOS packaging tools"

    require_command codesign
    require_command create-dmg
    require_command hdiutil
    require_command install_name_tool
    require_command otool
}

create_app_bundle() {
    log "Creating application bundle"
    mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Frameworks"
    cp "build/release/$APP_NAME" "$APP/Contents/MacOS/$APP_NAME"
    chmod +x "$APP/Contents/MacOS/$APP_NAME"
}

create_info_plist() {
    log "Creating Info.plist"
    local version="${VERSION#v}"
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

is_system_library() {
    case "$1" in
        /System/Library/*|/usr/lib/*) return 0 ;;
        *) return 1 ;;
    esac
}

bundle_dependencies_for() {
    local target="$1"
    local library

    while IFS= read -r library; do
        [[ -n "$library" ]] || continue
        is_system_library "$library" && continue

        if [[ ! -f "$library" ]]; then
            echo "Missing non-system runtime dependency: $library" >&2
            return 1
        fi

        local name="${library##*/}"
        local bundled_library="$APP/Contents/Frameworks/$name"

        if [[ ! -e "$bundled_library" ]]; then
            cp -L "$library" "$bundled_library"
        fi

        install_name_tool -change "$library" "@loader_path/../Frameworks/$name" "$target"
    done < <(otool -L "$target" | awk 'NR > 1 { print $1 }')
}

bundle_runtime_dependencies() {
    log "Bundling runtime dependencies"

    bundle_dependencies_for "$APP/Contents/MacOS/$APP_NAME"

    # A copied dylib can have dependencies of its own. Repeat until no new
    # libraries are added, which also avoids relying on the build machine's Homebrew.
    local bundled_count=0
    local previous_count=-1
    while [[ "$bundled_count" -ne "$previous_count" ]]; do
        previous_count="$bundled_count"
        while IFS= read -r library; do
            bundle_dependencies_for "$library"
        done < <(find "$APP/Contents/Frameworks" -type f -maxdepth 1)
        bundled_count="$(find "$APP/Contents/Frameworks" -type f -maxdepth 1 | wc -l | tr -d ' ')"
    done
}

sign_bundle() {
    log "Applying ad-hoc signature"

    # An ad-hoc signature is local and needs neither an Apple Developer
    # account nor a certificate. It does not provide notarization.
    codesign --force --deep --sign - "$APP"
}

create_dmg_root() {
    log "Preparing DMG contents"
    mkdir -p "$DMG_ROOT"
    cp -R "$APP" "$DMG_ROOT/"
    cat > "$DMG_ROOT/README.txt" <<'EOF'
This application is ad-hoc signed and is not notarized by Apple.

If macOS blocks the first launch, Control-click the application, choose Open,
and confirm the prompt. This does not require an Apple Developer account.
EOF
}

create_dmg() {
    log "Creating DMG"
    create-dmg \
        --volname "$APP_NAME" \
        --window-size 600 400 \
        --app-drop-link 450 200 \
        "$OUTPUT" \
        "$DMG_ROOT/"
}

verify_dmg() {
    log "Verifying DMG"

    [[ -s "$OUTPUT" ]] || {
        echo "DMG was not created: $OUTPUT" >&2
        return 1
    }

    hdiutil verify "$OUTPUT"
    test -x "$APP/Contents/MacOS/$APP_NAME"
    codesign --verify --deep --strict --verbose=2 "$APP"
    codesign -dvvv "$APP" 2>&1 | grep -q 'Signature=adhoc'
    if otool -L "$APP/Contents/MacOS/$APP_NAME" | awk 'NR > 1 { print $1 }' |
        grep -Ev '^(/System/Library/|/usr/lib/|@loader_path/../Frameworks/)' | grep -q .; then
        echo "Bundle retains unresolved non-system dependencies" >&2
        return 1
    fi
}

main() {
    check_tools
    clean_previous_artifacts
    create_app_bundle
    create_info_plist
    bundle_runtime_dependencies
    sign_bundle
    create_dmg_root
    create_dmg
    verify_dmg
    log "Created: $OUTPUT"
}

main "$@"
