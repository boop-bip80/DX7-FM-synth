#!/usr/bin/env bash
# Builds an offline renderer from the *repo's own* msfa engine (source/msfa) so that
# every generated voice can be auditioned numerically without JUCE or a DAW.
# Output: tools/preset-lab/build/dx7render
set -euo pipefail
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC="$HERE/../../source/msfa"
OUT="$HERE/build"
rm -rf "$OUT/engine"; mkdir -p "$OUT/engine/msfa" "$OUT/engine/debugger"
cp "$SRC"/* "$OUT/engine/msfa/"
cp "$HERE/renderer/stubs/Logger.h" "$OUT/engine/debugger/Logger.h"
CXX="${CXX:-g++}"
"$CXX" -O2 -std=c++17 -I"$OUT/engine" -I"$HERE/renderer/stubs" -I"$OUT/engine/msfa" \
  "$OUT"/engine/msfa/{exp2,sin,freqlut,lfo,env,pitchenv,fm_op_kernel,fm_core,dx7note}.cc \
  "$OUT/engine/msfa/porta.cpp" "$HERE/renderer/stubs/tuning_std.cc" "$HERE/renderer/render.cc" \
  -o "$OUT/dx7render"
echo "built $OUT/dx7render"
