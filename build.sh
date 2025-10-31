#!/usr/bin/env bash

set -euo pipefail

# Usage: ./build.sh [Release|Debug]
BUILD_TYPE="${1:-Release}"

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

mkdir -p "$BUILD_DIR"

echo "[build.sh] Configuring ($BUILD_TYPE)..."
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "[build.sh] Building..."
cmake --build "$BUILD_DIR" -- -j

# Sync runtime config to avoid missing models at run time
if [[ -f "$PROJECT_DIR/app/config.json" ]]; then
    cp -f "$PROJECT_DIR/app/config.json" "$BUILD_DIR/config.json"
    echo "[build.sh] Synced app/config.json -> build/config.json"
fi

echo "[build.sh] Done. Artifacts in: $BUILD_DIR"



