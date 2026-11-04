# preset-lab

Generator, renderer and auditor for the **80s LA Studio** voice library in [`presets/80s-LA-Studio`](../../presets/80s-LA-Studio).
Pure Python 3 (numpy for analysis, PIL optional for spectrogram sheets) plus one small C++ program built from this repository's own `source/msfa` engine.

```bash
tools/preset-lab/build_renderer.sh          # builds build/dx7render from source/msfa (g++, no JUCE needed)
python3 tools/preset-lab/build_presets.py   # recipes -> render -> balance -> audit -> write .syx + catalog.json  (~2 min)
python3 tools/preset-lab/make_docs.py       # regenerate presets README and doc/80S_LA_STUDIO_PRESETS.md
python3 tools/preset-lab/verify_presets.py  # re-read every shipped .syx and re-render it through the engine
python3 tools/preset-lab/install_presets.py # copy the cartridges into the synth's Cartridges folder by hand
python3 tools/preset-lab/make_bundle.py     # repack assets/la_studio_presets.zip, the copy compiled into every build
```

**After changing any voice, run `build_presets.py` and then `make_bundle.py` and commit the new `assets/la_studio_presets.zip`.** CI fails the Linux build (`make_bundle.py --check`) if the compiled-in bundle differs from `presets/`.

`build_presets.py --audit` runs every check without writing files; add `--range` to also render every voice at low/middle/high keys and soft/medium/hard velocity.

## Layout

| File | Role |
| --- | --- |
| `dx7.py` | DX7 voice model, algorithm topology (simulated from `fm_core.cc`), bit-exact pack/unpack and cartridge writer. Round-trips all 1,056 factory voices in `assets/builtin_pgm.zip`. |
| `units.py` | Musical units to DX7 numbers: envelope times, FM index, vibrato cents, LFO delay. Every table was **measured on the engine** (`calibrate*.py`). |
| `studio.py` | Small sound-design DSL: `P.car()` / `P.mod()` with dB envelopes (`shape`), FM index, tremolo in dB, pitch EG in semitones. |
| `recipes/` | Instrument builders (EPs, organs, basses, brass, strings, pads, leads, mallets, plucks, winds, voices, drums, FX, sequence). |
| `banks/` | The 32 cartridges: each voice is one builder call with named parameters. |
| `renderer/`, `build_renderer.sh` | Offline renderer using the real `Dx7Note`/`FmCore` engine, honouring voice transpose exactly like `DexedAudioProcessor`. |
| `audit.py`, `lab.py`, `viz.py` | Rendering, loudness balancing, silence/clip/pitch checks, spectral + modulation fingerprints for near-duplicate detection, spectrogram sheets. |
| `make_bundle.py` | Packs the 32 cartridges into `assets/la_studio_presets.zip` (deterministic), which `assets/CMakeLists.txt` embeds in the binary and `DexedAudioProcessor::resolvAppDir()` unpacks on first launch. `--check` verifies bundle == `presets/`. |
| `audit_bank.py` | Audition one bank: `python3 audit_bank.py keys 0` prints metrics and writes `build/bank_sheet.png`. |

## What the measurements established

* A sine carrier at output level 99 peaks at 0.125 FS; levels step 0.75 dB; modulation index is 4π at level 99 and falls 0.75 dB per step.
* EG fall speed is `0.0638 * (4 + (q & 3)) * 2^(q >> 2)` dB/s with `q = rate*41 >> 6`; attack times were tabulated.
* Amp-mod depth is very non-linear (AMS 1 tops out at 2.7 dB peak-to-peak, AMS 3 reaches full gating), vibrato depth is linear in PMD per PMS, and the LFO delay parameter was tabulated.
* The square LFO starts in its low state in this engine, so gated sounds begin one half-cycle late (the clap recipe compensates).
* Dense, noise-like spectra come from chains of fixed-frequency operators at maximum index with feedback; the frequency sets in `recipes/drums.py` were found by searching for spectral flatness (`search_noise.py`).
* Fixed-frequency operators ignore the pitch EG and key tracking, which is what makes sync-style sweeps and key-independent drums possible.

## Audit rules enforced by `build_presets.py`

32 voices per cartridge; names are 10 printable ASCII characters and unique across the library; no silent or clipped voice at the test note; no DC offset above 0.03; in-tune pitched voices (octave/fifth/third relationships and a short list of deliberately inharmonic voices excepted); no pair of voices that is spectrally *and* modulation-wise nearly identical; loudness balanced by carrier level only (the loudest carrier never exceeds level 99).

The checks are objective measurements of rendered audio. They catch broken or redundant voices; they do not judge taste.
