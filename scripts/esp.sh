#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ESP_PROJECT_DIR="$PROJECT_ROOT/esp32"
EXPECTED_IDF_VERSION="5.5.5"

die() {
    printf 'ERROR: %s\n' "$1" >&2
    exit 1
}

require_idf() {
    command -v idf.py >/dev/null 2>&1 || die "idf.py not found; activate ESP-IDF ${EXPECTED_IDF_VERSION} first"

    local version
    version="$(idf.py --version)"
    [[ "$version" == *"v${EXPECTED_IDF_VERSION}"* ]] || die "ESP-IDF ${EXPECTED_IDF_VERSION} is required; active: ${version}"
}

run_idf() {
    require_idf
    (
        cd "$ESP_PROJECT_DIR"
        idf.py "$@"
    )
}

usage() {
    cat <<'EOF'
Usage: scripts/esp.sh <command> [idf.py options]

Commands:
  doctor                  Validate the active ESP-IDF installation
  configure               Select ESP32-S3 and regenerate CMake files
  build                   Build firmware
  flash [options]         Flash firmware
  monitor [options]       Open serial monitor
  flash-monitor [options] Flash firmware and open serial monitor
  clean                   Remove all ESP-IDF build state
EOF
}

case "${1:-}" in
    doctor)
        require_idf
        printf 'ESP-IDF project: %s\n' "$ESP_PROJECT_DIR"
        ;;
    configure)
        run_idf set-target esp32s3
        run_idf reconfigure
        ;;
    build)
        run_idf build
        ;;
    flash|monitor)
        command="$1"
        shift
        run_idf "$command" "$@"
        ;;
    flash-monitor)
        shift
        run_idf flash monitor "$@"
        ;;
    clean)
        run_idf fullclean
        ;;
    *)
        usage
        exit 1
        ;;
esac
