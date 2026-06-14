#!/usr/bin/env bash

set -e

export WINEDEBUG=-all

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cmd="${1:-debug}"

if ! command -v cl &>/dev/null; then
  echo "error: 'cl' not found in PATH. Add MSVC bin/x64 to PATH."
  exit 1
fi

MSVC_ROOT="$(cd "$(dirname "$(which cl)")/../.." && pwd)"

ENGINE_SRC="$SCRIPT_DIR/engine/src"
TESTBED_SRC="$SCRIPT_DIR/testbed/src"

ENGINE_SRCS=$(find "$ENGINE_SRC" -name "*.cpp" \
  ! -name "*linux*" ! -name "*unix*" ! -name "*apple*" \
  | tr '\n' ' ')

build_common() {
  local build_dir="$1"
  shift
  local extra="$*"

  mkdir -p "$build_dir"

  echo "==> Compiling engine -> mist.dll"
  cl /nologo /EHs-c- /GR- /std:c++17 /LD /Zc:preprocessor \
    $extra \
    /I"$ENGINE_SRC" \
    /DMEXPORT /DMIST_ARENA_FREELIST \
    $ENGINE_SRCS \
    /Fo"$build_dir/obj/" \
    /Fd"$build_dir/mist.pdb" \
    /link \
      /OUT:"$build_dir/mist.dll" \
      /IMPLIB:"$build_dir/mist.lib" \
      /ignore:4099

  echo "==> Compiling testbed -> testbed.exe"
  cl /nologo /EHs-c- /GR- /std:c++17 /Zc:preprocessor \
    $extra \
    /I"$ENGINE_SRC" \
    /I"$TESTBED_SRC" \
    /DMIMPORT /DMIST_ARENA_FREELIST \
    "$TESTBED_SRC/main.cpp" \
    /Fo"$build_dir/" \
    /Fd"$build_dir/testbed.pdb" \
    /link \
      "$build_dir/mist.lib" \
      /OUT:"$build_dir/testbed.exe" \
      /SUBSYSTEM:WINDOWS \
      /ENTRY:wWinMainCRTStartup \
      /ignore:4099
}

case "$cmd" in
  debug)
    mkdir -p "$SCRIPT_DIR/build-win/Debug/obj"
    build_common "$SCRIPT_DIR/build-win/Debug" /MDd /Zi /Od /D_DEBUG
    ;;
  release)
    mkdir -p "$SCRIPT_DIR/build-win/Release/obj"
    build_common "$SCRIPT_DIR/build-win/Release" /MD /O2 /DNDEBUG
    ;;
  clean)
    rm -rf "$SCRIPT_DIR/build-win"
    echo "Cleaned build-win/"
    exit 0
    ;;
  run)
    DBG_CRT=$(find "$MSVC_ROOT/VC/Redist/MSVC" -type d -name "Microsoft.VC143.DebugCRT" -path "*/x64/*" | sort | tail -1)
    UCRT_DBG=$(find "$MSVC_ROOT/Windows Kits/10/bin" -type d -name "ucrt" -path "*/x64/*" | sort | tail -1)
    WINEPATH="$DBG_CRT;$UCRT_DBG" wine "$SCRIPT_DIR/build-win/Debug/testbed.exe"
    exit 0
    ;;
  *)
    echo "Usage: $0 [debug|release|clean|run]"
    exit 1
    ;;
esac

echo "==> Done."
echo "    build-win/Debug/mist.dll"
echo "    build-win/Debug/testbed.exe"
echo ""
echo "Run: $0 run"
