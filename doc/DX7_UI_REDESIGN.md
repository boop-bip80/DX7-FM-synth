# DX7 FM Synth — UI/UX design specification

This is an independent redesign of the **Dexed-derived** DX7 editor. It takes the *interaction model* of modern hardware emulations such as Arturia's DX7 V as its reference: a classic front panel that you can play like the original keyboard, plus an expandable advanced workspace for deep editing.

The reference is used for **layout and workflow ideas only**. Arturia's artwork, photographs, logos, fonts, code and product names are not used. Every element in this editor is drawn with JUCE vector primitives from original code in this repository. The project is not affiliated with or endorsed by Arturia or Yamaha.

## Information architecture

| Level | Region | What it is for |
| --- | --- | --- |
| 1 | **Upper toolbar** | Main menu, cartridge library, preset name with voice list, previous/next voice, store voice, settings |
| 2 | **Front panel** | Play and audition: volume, data entry with −1/+1, voice LED and two-line LCD, mode switches (cartridge, store, init, function), 32 voice-select membrane switches, operator on/off, algorithm and feedback steppers |
| 3 | **Advanced workspace** (fold-out) | Deep editing in four tabs: Overview, Envelopes, Modulation, Output |
| 4 | **Keyboard** | Audition with the mouse; can be hidden |
| 5 | **Lower status bar** | Name and value of the control under the pointer, panic (all notes off), keyboard toggle |

### Advanced workspace tabs

| Tab | Contents |
| --- | --- |
| **Overview** | Clickable algorithm chart (click selects, right-click mutes), algorithm and feedback knobs, oscillator key sync, and six operator strips. Each strip has a power switch, activity meter, frequency readout, ratio/fixed switch, coarse/fine/detune, level, velocity, AM sensitivity, key rate scaling and a mini envelope |
| **Envelopes** | OP 1–6 and PITCH selector; a large envelope with draggable points, eight rate/level knobs; keyboard scaling graph with break point, left/right curves and depths, rate scaling, output level and velocity |
| **Modulation** | LFO waveform selector, live shape preview with delay ramp, speed/delay/pitch depth/amp depth/pitch sensitivity, key sync; mod wheel, foot, breath and aftertouch ranges with pitch/amp/EG-bias routing; pitch bend range and step; portamento and glissando; mono mode |
| **Output** | Filter response curve with cutoff and resonance; output level and meter; master tune and transpose; engine resolution (Modern, Mark I, OPL series); mono mode and scale transposition |

## Design principles

1. **Two levels of depth.** The front panel covers playing and quick edits; the advanced workspace holds the full parameter set. Folding the workspace away leaves a compact classic instrument.
2. **The hardware workflow still works.** Touch any control and the LCD shows its name and value; the data-entry fader and −1/+1 buttons then edit it, like the original's select-then-adjust model.
3. **Signal flow first.** The algorithm chart is interactive and colour-codes each operator. The same colours run through the operator strips, envelope pages and keyboard-scaling graphs.
4. **Read before adjusting.** Every knob caption turns into a live value readout while it is hovered or dragged, and the status bar repeats it.
5. **Preserve the real instrument.** Every control is bound to an existing APVTS parameter: host automation, MIDI learn, presets and SysEx keep working. The engine, patch format, 32 algorithms and six-operator semantics are unchanged.
6. **Window and host awareness.** Everything is vector-drawn and scales with the UI zoom setting. The window height follows the workspace and keyboard toggles, and both preferences persist.

## Visual system

| Role | Colour | Use |
| --- | --- | --- |
| Chassis | `#342B26` → `#231D1A` | Front-panel surface with fine horizontal grain |
| Panel print | `#EADFC6` / `#B6A68B` | Silk-screen style labels and rules |
| Membrane switches | maroon `#93323D`, mint `#78BFA6`, cream `#DCCFB2`, violet `#74639A` | Mode, voice, data-entry and operator switches |
| LCD | `#BCCD55` → `#98AF38`, ink `#1D2610` | Two-line, 20-column backlit display |
| LED | `#FF4433` | Voice, algorithm and feedback numbers |
| Workspace | `#1F2023`, cards `#2A2B2F`–`#313236` | Charcoal advanced area |
| Accent | `#5AD4A6` | Selection, value arcs, open-workspace tab |
| Operator colours | mint, sky, amber, lilac, rose, lime | OP 1 … OP 6 |

## Interaction map and regression checks

| Region | User task | Regression check |
| --- | --- | --- |
| Toolbar / preset field | Know and change the current voice | Name, number and cartridge stay synchronised with the processor |
| Voice-select switches | One-click voice recall | The lit switch and LED follow host program changes |
| Data entry | Hardware-style editing | Fader and LCD follow the last-touched parameter and host automation |
| Operator strips / Envelopes | Shape operators | Each control writes its native DX7 parameter and updates when a preset loads |
| Algorithm chart | Understand and change routing | All 32 algorithms render; click selects, right-click toggles the operator switch |
| Modulation / Output | Performance and engine setup | Controls stay host-automatable and match the settings dialog |
| Cartridge library & settings | Import/export SysEx, MIDI preferences | Existing dialogs, drag-and-drop and file choosers keep working |
| Keyboard | Audition | MIDI keyboard triggers notes and can be hidden without clipping |

## Completion checklist

These are **verification gates**, not claims that everything is complete:

- [x] The redesigned editor compiles and launches as a Linux standalone, and real Xvfb screenshots of all four tabs and the folded classic view are captured by `.github/workflows/capture-ui.sh`, which clicks the real tabs.
- [x] Scripted Xvfb interaction check: voice select, data entry +1, an envelope point drag, algorithm-chart operator mute, the cartridge library and the settings dialog all respond as intended.
- [x] Windows, macOS and Linux builds pass on GitHub Actions for `5be0097` ([build run 41875529732](https://github.com/boop-bip80/DX7-FM-synth/actions/runs/41875529732)), and UI-Nightly was republished from that commit. Host runtime is **not** independently tested.
- [x] The screenshot workflow recaptured every tab on GitHub's runner for `5be0097` ([run 41875529385](https://github.com/boop-bip80/DX7-FM-synth/actions/runs/41875529385)).
- [ ] Program browsing, SysEx import/export and keyboard shortcuts are checked manually in a DAW.
- [ ] Rendering is checked at every UI zoom setting, with the keyboard and workspace both shown and hidden.
- [ ] Plug-in controls remain linked to DSP and persist correctly in a DAW session.
- [ ] Focus order, keyboard operation, contrast and target sizes receive a manual accessibility review.

The workflows provide compile, build and screenshot evidence. They do not prove audio fidelity, accessibility or DAW integration on their own; those need explicit manual or automated functional tests.
