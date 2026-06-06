#!/usr/bin/env bash

set -e

BUILD_TYPE=${1:-Debug}

case "$BUILD_TYPE" in
  Debug|Release|RelWithDebInfo) ;;
  *) echo "Unknown build type: $BUILD_TYPE. Use Debug, Release, or RelWithDebInfo."; exit 1 ;;
esac

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)

echo "==> Building engine [$BUILD_TYPE]"
cmake -B "$ROOT_DIR/engine/build" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -S "$ROOT_DIR/engine"
cmake --build "$ROOT_DIR/engine/build"

echo "==> Building testbed [$BUILD_TYPE]"
cmake -B "$ROOT_DIR/testbed/build" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -S "$ROOT_DIR/testbed"
cmake --build "$ROOT_DIR/testbed/build"

echo "==> Done. Output in engine/build/$BUILD_TYPE and testbed/build/$BUILD_TYPE"
