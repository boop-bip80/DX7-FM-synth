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

# Park the pointer outside the editor so no hover state or tooltip is captured.
xdotool mousemove 0 0 2>/dev/null || true
sleep 3 # let fonts, panels and meters finish painting

capture() {
  local output="$1"
  import -silent -window "$window_id" -strip "$output"
  read -r image_width image_height <<<"$(identify -format '%w %h' "$output")"
  if (( image_width < 700 || image_height < 250 )); then
    echo "Refusing incomplete ${image_width}x${image_height} screenshot: $output" >&2
    exit 1
  fi
  echo "Captured live editor: $output (${image_width}x${image_height})"
}

capture "$destination"
shots_dir="$(dirname "$destination")"

# The editor is 1200 logical pixels wide. Window chrome (the standalone title
# bar and border) is measured from the captured frame, so clicks land on the
# real controls regardless of the wrapper's decoration size.
#   toolbar 44 px, front panel 214 px, advanced tab bar at y 6..36 within the
#   advanced workspace; tabs are 124 px wide on a 128 px pitch from x = 12.
full_height=796   # toolbar + front panel + advanced + keyboard + status bar
border_x=$(( (image_width - 1200) / 2 ))
offset_y=$(( image_height - full_height - border_x ))
if (( border_x < 0 || offset_y < 0 )); then
  echo "Unexpected editor geometry ${image_width}x${image_height}; skipping tab captures." >&2
  exit 1
fi

click_editor() {
  local x="$1" y="$2"
  xdotool mousemove --window "$window_id" $(( border_x + x )) $(( offset_y + y )) click 1
  sleep 0.4
  xdotool mousemove 0 0 2>/dev/null || true
  sleep 1.6
}

tab_y=$(( 44 + 214 + 21 ))
for entry in "1:envelopes" "2:modulation" "3:output"; do
  index="${entry%%:*}"
  name="${entry#*:}"
  click_editor $(( 12 + index * 128 + 62 )) "$tab_y"
  capture "$shots_dir/dx7-$name.png"
done
click_editor $(( 12 + 62 )) "$tab_y"   # back to OVERVIEW

# Fold the advanced workspace away to show the classic front panel alone,
# then unfold it again so the saved preference is left as it was.
click_editor 1171 $(( 44 + 107 ))
capture "$shots_dir/dx7-classic.png"
click_editor 1171 $(( 44 + 107 ))

echo "All live captures complete."
