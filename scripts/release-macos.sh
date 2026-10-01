#!/usr/bin/env bash
set -euo pipefail
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"
rm -rf dist "my-lvgl-app-${GITHUB_REF_NAME}-macos-arm64.dmg"
APP="dist/my-lvgl-app.app"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Frameworks"
cp build/release/my-lvgl-app "$APP/Contents/MacOS/my-lvgl-app"
chmod +x "$APP/Contents/MacOS/my-lvgl-app"
cat > "$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>my-lvgl-app</string>
<key>CFBundleIdentifier</key><string>com.ojsandev.my-lvgl-app</string>
<key>CFBundleName</key><string>my-lvgl-app</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>CFBundleVersion</key><string>${GITHUB_REF_NAME#v}</string>
<key>CFBundleShortVersionString</key><string>${GITHUB_REF_NAME#v}</string>
</dict></plist>
EOF
SDL2_LIB="$(find "$(brew --prefix sdl2)/lib" -maxdepth 1 -name 'libSDL2*.dylib' | head -n 1)"
SSL_LIB="$(find "$(brew --prefix openssl@3)/lib" -maxdepth 1 -name 'libssl*.dylib' | head -n 1)"
CRYPTO_LIB="$(find "$(brew --prefix openssl@3)/lib" -maxdepth 1 -name 'libcrypto*.dylib' | head -n 1)"
for lib in "$SDL2_LIB" "$SSL_LIB" "$CRYPTO_LIB"; do
  [[ -f "$lib" ]] || continue
  cp -L "$lib" "$APP/Contents/Frameworks/"
  base="$(basename "$lib")"
  install_name_tool -change "$lib" "@loader_path/../Frameworks/$base" "$APP/Contents/MacOS/my-lvgl-app" || true
done
mkdir -p dist/dmg-root
cp -R "$APP" dist/dmg-root/
create-dmg --volname "my-lvgl-app" --window-size 600 400 --app-drop-link 450 200 "my-lvgl-app-${GITHUB_REF_NAME}-macos-arm64.dmg" dist/dmg-root/
