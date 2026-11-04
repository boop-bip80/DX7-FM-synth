# 80s LA Studio - 1,024 DX7 voices

**1,024 voices in 32 DX7 32-voice SysEx cartridges**, organised the way a well-stocked Los Angeles studio of the 1980s would have been: keyboards, basses, brass, strings, pads, leads, mallets, guitars, voices, a full drum-and-percussion room, sound effects and rhythmic/motion patches. Every voice is a full six-operator DX7 patch (all 155 voice parameters) stored in the standard DX7 32-voice bulk-dump format, so the files should also open in any DX7-compatible editor or hardware (only tested here in the msfa engine this synth uses).

## Loading them

1. **Library:** open the cartridge library (bars icon, `Ctrl+L`) and press **LOAD**, or drag a `.syx` onto the browser.
2. **Permanent install:** copy the `80s-LA-Studio` folder into the app's `Cartridges` directory and it appears in the library tree.
   * macOS: `~/Library/Application Support/DigitalSuburban/Dexed/Cartridges`
   * Windows: `%APPDATA%\DigitalSuburban\Dexed\Cartridges`
   * Linux: `${XDG_DATA_HOME:-~/.local/share}/DigitalSuburban/Dexed/Cartridges`
   or run `python3 tools/preset-lab/install_presets.py` from a checkout (it finds the right folder for you).

## Layout

| Folder | Cartridges | Voices | What is in it |
| --- | ---: | ---: | --- |
| `01-Keys` | 5 | 160 | The control-room keyboards: tine and reed electric pianos, FM bell pianos, acoustic grands, clavs, harpsichords, drawbar/pipe/combo organs and glassy digital keys. |
| `02-Bass` | 3 | 96 | FM thump basses, slap/pluck/fretless/upright basses, resonant and pulsing synth basses, octave-jumping disco bass and sub-bass. |
| `03-Brass-and-Winds` | 3 | 96 | Wide brass sections and stabs, solo trumpets, horns and saxes, flutes, whistles, clarinets, oboes and ethnic winds. |
| `04-Strings` | 2 | 64 | Lush string ensembles, tremolo strings, solo violin/viola/cello, pizzicato and orchestral hits. |
| `05-Pads` | 3 | 96 | Warm analog-style pads, glass and digital pads, filter sweeps, drones and evolving atmospheres. |
| `06-Leads` | 3 | 96 | Saw, square and sync leads, expressive flute/sax/vocal leads, plucked hook leads and bell leads. |
| `07-Mallets-and-Bells` | 2 | 64 | Marimba, xylophone, vibes, glockenspiel, celesta, tubular and church bells, music boxes, kalimba, gongs and steel pans. |
| `08-Plucks-and-Guitars` | 2 | 64 | Nylon, steel, clean, chorused and muted guitars, harps, koto, banjo, mandolin, sitar and other world strings. |
| `09-Voices` | 1 | 32 | Choirs on six vowels, breathy and whispered voices, FM 'ooh/aah' vocals. |
| `10-Drums-and-Percussion` | 5 | 160 | Kicks, toms, snares, claps, rimshots, hats, cymbals, shakers, Latin and world percussion, timpani, taiko, gongs and electronic kit pieces. |
| `11-Sound-FX` | 2 | 64 | Risers, impacts, whooshes, lasers, sirens, phones, weather, engines, power-ups and computer bleeps. |
| `12-Sequence-and-Motion` | 1 | 32 | Two-note trills, gated/chopped pads, octave-hop pads, scanners and sample-and-hold arps. |

Cartridges are numbered `LA01` to `LA32` in studio order; voice slots 1-32 inside each cartridge are the 32 front-panel voice buttons. The full voice-by-voice list is in [`doc/80S_LA_STUDIO_PRESETS.md`](../../doc/80S_LA_STUDIO_PRESETS.md), and `catalog.json` carries the same information for scripts.

## Playing notes

* **Basses** are transposed down an octave in the voice (TRANSPOSE = 12), so play them where a bass guitar lives (C1-C3). Pitched toms and timpani follow the keyboard.
* **Snares, hats, cymbals, claps, metallic percussion (cowbell, claves, triangle...) and noise FX** use fixed-frequency operators: they sound the same on every key, so any key triggers the same hit. Congas, bongos and other skin drums are ratio-mode and follow the keyboard.
* **Kicks, toms and timpani** are pitch-swept, ratio-mode voices that track the keyboard; the natural kick range is around C3.
* **Velocity** drives *brightness* (modulator level) harder than loudness on the keys, bass, mallet and pluck voices - play them soft and hard. Organs and many sustained or atmospheric voices respond only gently to velocity.
* **Vibrato and tremolo** come from the voice's own LFO and fade in after a short delay where an instrument would do that. Use the mod wheel / aftertouch assignments for extra expression.
* Loudness is balanced across the library: a note at velocity 100 on any voice peaks well below full scale (median -15 dBFS), so layered chords have headroom.

## How the library was made and checked

Voices were *designed in physical units* (envelope times in ms and seconds, modulation depth as FM index, vibrato in cents, pitch sweeps in semitones) with a small generator in [`tools/preset-lab`](../../tools/preset-lab), whose unit conversions were measured on this repository's own msfa engine. Every voice was then rendered offline through that same engine and checked: audible, unclipped, level-balanced, in tune, not a duplicate of another voice, and free of clipping or dropouts at low, middle and high keys (two octaves either side of each voice's test note) and at soft, medium and hard velocity. These are objective checks on rendered audio; they are not a substitute for playing the sounds, and taste will differ - treat the library as a large, consistent set of starting points and edit freely.
