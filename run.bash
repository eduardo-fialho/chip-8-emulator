#!/usr/bin/env bash
set -Eeuo pipefail

ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

if ! command -v cmake >/dev/null 2>&1; then
    echo "ERROR: CMake was not found. Install CMake and ensure it is on PATH." >&2
    exit 1
fi

cmake -S . -B build
cmake --build build --config Release

EXE="$ROOT/build/bin/Release/chip8"
if [[ ! -x "$EXE" ]]; then
    echo "ERROR: Build completed but the expected executable was not found: $EXE" >&2
    exit 1
fi

if [[ $# -ge 1 ]]; then
    exec "$EXE" "$1"
else
    echo
    echo "Build successful: $EXE"
    echo 'To run a ROM, use: ./run.bash "roms/your-rom.ch8"'
fi
