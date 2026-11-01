#!/usr/bin/env bash
# Captures the actual JUCE standalone editor under Xvfb. Not a mockup.
set -Eeuo pipefail

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 /path/to/Dexed /path/to/screenshot.png" >&2
  exit 2
fi

standalone="$1"
destination="$2"
[[ -x "$standalone" ]] || { echo "Missing executable: $standalone" >&2; exit 1; }

if ! xwininfo -root >/dev/null 2>&1; then
  echo "X11 display ${DISPLAY:-unset} is unavailable; cannot capture a real editor frame." >&2
  exit 1
fi

mkdir -p "$(dirname "$destination")"
log_path="${RUNNER_TEMP:-/tmp}/dx7-screenshot-standalone.log"
"$standalone" >"$log_path" 2>&1 &
app_pid=$!

cleanup() {
  kill "$app_pid" 2>/dev/null || true
  wait "$app_pid" 2>/dev/null || true
}
trap cleanup EXIT

window_id=""
for _ in $(seq 1 50); do
  if ! kill -0 "$app_pid" 2>/dev/null; then
    echo 'Standalone exited before the editor appeared:' >&2
    sed -n '1,100p' "$log_path" >&2
    exit 1
  fi

  # Avoid mistaking a small alert or splash screen for the plugin editor.
  for candidate in $(xdotool search --onlyvisible --name 'Dexed|DX7' 2>/dev/null || true); do
    geometry="$(xdotool getwindowgeometry --shell "$candidate" 2>/dev/null || true)"
    width="$(printf '%s\n' "$geometry" | sed -n 's/^WIDTH=//p')"
    height="$(printf '%s\n' "$geometry" | sed -n 's/^HEIGHT=//p')"
    if [[ "$width" =~ ^[0-9]+$ && "$height" =~ ^[0-9]+$ ]] && (( width >= 700 && height >= 450 )); then
      window_id="$candidate"
      break
    fi
  done

  [[ -n "$window_id" ]] && break
  sleep 1
done

if [[ -z "$window_id" ]]; then
  echo 'Timed out waiting for the editor. No screenshot published.' >&2
  sed -n '1,100p' "$log_path" >&2
  xwininfo -root -tree >&2 || true
  exit 1
fi

sleep 3 # let fonts, panels and meters finish painting
import -silent -window "$window_id" -strip "$destination"

read -r image_width image_height <<<"$(identify -format '%w %h' "$destination")"
if (( image_width < 700 || image_height < 450 )); then
  echo "Refusing incomplete ${image_width}x${image_height} screenshot." >&2
  exit 1
fi

echo "Captured live editor: $destination (${image_width}x${image_height})"
