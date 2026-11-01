# DX7 FM Synth — UI/UX redesign specification

This is an independent visual redesign of the **Dexed-derived** DX7 editor. The goal is to make six-operator FM programming approachable without losing the original hardware's parameter model, SysEx workflow, or musicians' muscle memory. Arturia's DX7 V is a quality/usability reference, **not** a source of assets, logos, or code; this project is not affiliated with Arturia or Yamaha.

## Design principles

1. **Signal flow first.** Six numbered operator modules remain visible together. The FM algorithm diagram should be legible at a glance, with operator states and envelope behavior easy to inspect.
2. **Tactility without visual noise.** A restrained graphite/navy chassis, warm labels, sea-glass parameter accents, and amber activity indicators create a recognizable instrument rather than a generic app form.
3. **Read before adjusting.** Every critical parameter gets a descriptive label, a readable current state, an obvious hit area, and contrasting focus/hover/active states.
4. **Preserve the real instrument.** Buttons and knobs must remain wired to existing JUCE parameters; a beautifully drawn non-functional control is a regression. The engine, patch format, MIDI behavior, 32 DX7 algorithms, and six-operator editing semantics are not redesigned here.
5. **Fast for experienced players.** Retain shortcuts for operator focus/toggling, global settings, cartridge library and parameter dialog. Make program selection and keyboard performance immediately discoverable.
6. **Window and host awareness.** Check zoom settings, compact/keyboard-hidden layout, high-DPI rendering, focus traversal, and DAW resizing. Do not rely on a single hard-coded desktop resolution in usability validation.

## Visual system

| Role | Color | Meaning |
| --- | --- | --- |
| Primary chassis | `#101820` | Quiet background and depth |
| Raised panels | `#17242D` | Operator and global grouping |
| Primary text | `#E8E6DA` | Labels and readable parameter values |
| Edit accent | `#55D7C7` | Active controls, parameter feedback and focus |
| Signal / activity | `#F2B563` | Operator activity and warm status detail |

Use subtle strokes and carefully controlled illumination for visual hierarchy. Do not use Arturia brand elements or their proprietary photographs. Prefer high-resolution JUCE vector drawing so details stay crisp at different zoom factors.

## Interaction map

| Region | User task | Regression check |
| --- | --- | --- |
| Header / program area | Know the loaded instrument and current patch | Patch selection and program name stay synchronized with processor |
| Six operator cards | Change FM ratio, output, keyboard scaling and envelope | All operator controls still write their native DX7 parameters and update on preset load |
| Algorithm display | Understand the carrier/modulator routing | All 32 selections remain accessible; feedback is preserved |
| Global / performance section | Adjust LFO, transpose, portamento and global parameters | Settings stay host-automatable and saved with program/host state where applicable |
| Cartridge & settings | Import/export SysEx, manage programs and MIDI preferences | Existing cartridge dialogs, drag/drop and parameter dialog continue working |
| Keyboard | Audition sounds directly | MIDI keyboard triggers notes and can be hidden without clipping other sections |

## Completion checklist

The following are **verification gates**, not claims that all are complete:

- [x] Linux native standalone compiles and launches using the redesigned JUCE editor — [final screenshot build/run 41267462749](https://github.com/boop-bip80/DX7-FM-synth/actions/runs/41267462749) for source `28e6b2d`.
- [x] Windows standalone and plug-in build job passes on GitHub Actions — [final multi-platform build run 41267462752](https://github.com/boop-bip80/DX7-FM-synth/actions/runs/41267462752) for `28e6b2d`; **host runtime not independently tested**.
- [x] macOS standalone and plug-in build job passes on GitHub Actions — [same final build run](https://github.com/boop-bip80/DX7-FM-synth/actions/runs/41267462752); **host runtime and code-signing acceptance not independently tested**.
- [x] An actual Linux standalone window was captured under Xvfb and committed to `docs/screenshots/dx7-main.png` by the UI screenshot workflow (1202 × 870; final capture run above, bot commit `bd8933d`).
- [x] README embeds the verified main screenshot and links to the current build/release artifacts.
- [x] The `UI-Nightly` rolling release was republished from the same source SHA `28e6b2d`, with five packages and a successful [final publish job](https://github.com/boop-bip80/DX7-FM-synth/actions/runs/41267462752).
- [x] Operator and performance detail crops from the live Linux application are committed alongside the full screenshot ([final capture run 41267462749](https://github.com/boop-bip80/DX7-FM-synth/actions/runs/41267462749), [screenshot directory](https://github.com/boop-bip80/DX7-FM-synth/tree/master/docs/screenshots)).
- [ ] All six operator panels and the algorithm diagram are legible at the default zoom level.
- [ ] Program browsing, SysEx import/export, operator switches and keyboard shortcuts work manually.
- [ ] Rendering is checked with keyboard shown and hidden, at available zoom settings.
- [ ] Plugin controls remain linked to DSP and persist correctly in a DAW session.
- [ ] Focus, keyboard operation, contrast and target sizes receive a manual accessibility review.

The workflows provide compile/build and screenshot evidence; they do not by themselves prove audio fidelity, accessibility, or DAW integration. Those require explicit manual or automated functional tests.
