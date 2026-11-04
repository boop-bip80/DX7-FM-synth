# DX7 FM Synth — keyboard shortcuts

The shortcuts below are implemented by the JUCE editor. They use **Ctrl**, including on macOS (they are not automatically remapped to ⌘ Command).

| Shortcut | Action |
| --- | --- |
| `Ctrl` + `1` … `6` | Open the **Overview** tab and focus operator strip 1 … 6 |
| `Ctrl` + `Shift` + `1` … `6` | Switch operator 1 … 6 on or off |
| `Ctrl` + `G` | Focus the algorithm / feedback controls |
| `Ctrl` + `E` | Fold or unfold the advanced workspace |
| `Ctrl` + `←` / `→` | Previous / next voice in the loaded cartridge |
| `Ctrl` + `L` | Open the cartridge library |
| `Ctrl` + `P` | Open the settings dialog (MIDI, controllers, tuning, UI scaling) |
| `Esc` | Close the cartridge library |
| `Shift` + `↑` / `↓` | Move a focused knob or slider in larger steps (about 10% of its range) |
| `←` / `→` on the LFO wave selector | Choose the previous / next LFO waveform |

## Mouse gestures

| Gesture | Action |
| --- | --- |
| Click any control, then move **DATA ENTRY** or press **−1 / +1** | Edit the parameter shown on the front-panel display, like the hardware's data-entry workflow |
| Click an operator in the algorithm chart | Select that operator (Overview strip and Envelopes page follow) |
| Right-click an operator in the algorithm chart | Mute or unmute that operator |
| Drag an envelope point (Envelopes tab) | Up/down sets the level, left/right sets the rate; hold `Shift` to change only the level |
| Click a mini envelope on an operator strip | Jump to that operator's envelope page |
| Scroll over the preset name | Step through the cartridge's voices |
| Double-click a knob | Return it to its default value |
| Right-click a knob or switch | MIDI CC learn / unlearn, send the voice to a DX7; on operators also copy and paste operator or envelope values |

The shortcuts work when the plugin editor receives the keystroke. A host DAW may intercept certain combinations before the plugin sees them. Click the plugin to establish keyboard focus first.

The on-screen keyboard can be played with a pointer or MIDI controller. It is not a replacement for DAW MIDI routing.

Accessibility: use `Tab`/`Shift` + `Tab` to move through focusable controls, and use the arrow keys on focused knobs and sliders. Focused controls draw an accent outline. Operating systems and DAW hosts may vary.
