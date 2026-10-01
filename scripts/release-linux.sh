#!/usr/bin/env bash
set -euo pipefail
PROJECT_ROOT="$(cd "$(dirname "\${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"
APPDIR="$PROJECT_ROOT/AppDir"
OUTPUT="my-lvgl-app-\${GITHUB_REF_NAME}-linux-x86_64.AppImage"
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/lib"
cp build/release/my-lvgl-app "$APPDIR/usr/bin/my-lvgl-app"
while IFS= read -r lib; do
  [[ -f "$lib" ]] || continue
  case "$lib" in */libSDL2*.so*|*/libssl*.so*|*/libcrypto*.so*) cp -L "$lib" "$APPDIR/usr/lib/";; esac
done < <(ldd "$APPDIR/usr/bin/my-lvgl-app" | awk '{print $3}' | grep '^/' | sort -u)
cat > "$APPDIR/AppRun" <<'EOF'
#!/bin/sh
HERE="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
export LD_LIBRARY_PATH="$HERE/usr/lib\${LD_LIBRARY_PATH:+:\$LD_LIBRARY_PATH}"
exec "$HERE/usr/bin/my-lvgl-app" "$@"
EOF
chmod +x "$APPDIR/AppRun"
cat > "$APPDIR/my-lvgl-app.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=my-lvgl-app
Exec=my-lvgl-app
Terminal=false
Categories=Utility;
EOF
curl -L --fail -o appimagetool https://github.com/AppImage/appimagetool/releases/latest/download/appimagetool-x86_64.AppImage
chmod +x appimagetool
ARCH=x86_64 ./appimagetool "$APPDIR" "$OUTPUT"
rm -rf "$APPDIR" appimagetool
