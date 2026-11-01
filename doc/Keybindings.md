# DX7 FM Synth — keyboard shortcuts

The shortcuts below are implemented by the JUCE editor. They generally use **Ctrl**, including on macOS (they are not automatically remapped to ⌘ Command).

| Shortcut | Action |
| --- | --- |
| `Ctrl` + `1` … `6` | Focus operator 1 … 6 |
| `Ctrl` + `Shift` + `1` … `6` | Enable or disable operator 1 … 6 |
| `Ctrl` + `G` | Focus global controls |
| `Ctrl` + `L` | Open the cartridge / SysEx library manager |
| `Ctrl` + `P` | Open the parameter and MIDI settings dialog |
| `Esc` | Dismiss the cartridge manager |
| `Shift` + `↑` / `↓` | Move a focused slider in larger steps (approximately 10% of its range) |

The shortcuts work when the plugin editor receives the keystroke. A host DAW may intercept certain combinations before the plugin sees them. Click the plugin to establish keyboard focus first.

The on-screen keyboard can be played with a pointer or MIDI controller. It is not a replacement for DAW MIDI routing.

Accessibility: use `Tab`/`Shift` + `Tab` to move through focusable controls, and use the arrow keys on focused sliders and controls where JUCE supports them. Visible focused-control styling is part of the UI redesign; individual operating systems and DAW hosts may vary.
