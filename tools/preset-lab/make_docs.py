#!/usr/bin/env python3
"""make_docs.py - write the human-facing catalog from presets/80s-LA-Studio/catalog.json."""
import json, os
from collections import OrderedDict

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
ROOT = os.path.join(REPO, "presets", "80s-LA-Studio")
cat = json.load(open(os.path.join(ROOT, "catalog.json")))

SECTION_BLURB = OrderedDict([
    ("KEYS", "The control-room keyboards: tine and reed electric pianos, FM bell pianos, acoustic grands, clavs, harpsichords, drawbar/pipe/combo organs and glassy digital keys."),
    ("BASS", "FM thump basses, slap/pluck/fretless/upright basses, resonant and pulsing synth basses, octave-jumping disco bass and sub-bass."),
    ("BRASS & WINDS", "Wide brass sections and stabs, solo trumpets, horns and saxes, flutes, whistles, clarinets, oboes and ethnic winds."),
    ("STRINGS", "Lush string ensembles, tremolo strings, solo violin/viola/cello, pizzicato and orchestral hits."),
    ("PADS", "Warm analog-style pads, glass and digital pads, filter sweeps, drones and evolving atmospheres."),
    ("LEADS", "Saw, square and sync leads, expressive flute/sax/vocal leads, plucked hook leads and bell leads."),
    ("MALLETS & BELLS", "Marimba, xylophone, vibes, glockenspiel, celesta, tubular and church bells, music boxes, kalimba, gongs and steel pans."),
    ("PLUCKS & GUITARS", "Nylon, steel, clean, chorused and muted guitars, harps, koto, banjo, mandolin, sitar and other world strings."),
    ("VOICES", "Choirs on six vowels, breathy and whispered voices, FM 'ooh/aah' vocals."),
    ("DRUMS", "Kicks, toms, snares, claps, rimshots, hats, cymbals, shakers, Latin and world percussion, timpani, taiko, gongs and electronic kit pieces."),
    ("FX", "Risers, impacts, whooshes, lasers, sirens, phones, weather, engines, power-ups and computer bleeps."),
    ("SEQUENCE", "Two-note trills, gated/chopped pads, octave-hop pads, scanners and sample-and-hold arps."),
])

total = sum(len(b["voices"]) for b in cat)
bysec = OrderedDict()
for b in cat:
    bysec.setdefault(b["section"], []).append(b)


def slug(b):
    return os.path.basename(b["file"])


# ---------------------------------------------------------------------------------------------------
# presets/80s-LA-Studio/README.md  (short guide that lives next to the files)
# ---------------------------------------------------------------------------------------------------
L = []
L.append("# 80s LA Studio - 1,024 DX7 voices\n")
L.append(f"**{total:,} voices in {len(cat)} DX7 32-voice SysEx cartridges**, organised the way a well-stocked Los Angeles studio of the 1980s would "
         "have been: keyboards, basses, brass, strings, pads, leads, mallets, guitars, voices, a full drum-and-percussion room, sound effects and "
         "rhythmic/motion patches. Every voice is a full six-operator DX7 patch (all 155 voice parameters) stored in the standard DX7 32-voice bulk-dump "
         "format, so the files should also open in any DX7-compatible editor or hardware (tested in the msfa engine this synth uses; cartridge LA01 also loads in the real editor in the CI screenshots, but nothing has been tried on hardware).\n")
L.append("## Loading them\n")
L.append("**Built in:** every build of the app (standalone, VST3, AU, CLAP; Windows, macOS, Linux) carries this whole library and unpacks it into the "
         "`Cartridges/80s-LA-Studio` folder the first time it starts - nothing to install. The unpack only happens when that folder does not exist, so "
         "anything you edit, add or delete there is never overwritten; delete the folder to get the original set back on the next launch. "
         "To load files by hand (for example from a checkout, or on a hardware-compatible editor):\n")
L.append("1. **Library:** open the cartridge library (bars icon, `Ctrl+L`) and press **LOAD**, or drag a `.syx` onto the browser.\n"
         "2. **Permanent install:** copy the `80s-LA-Studio` folder into the app's `Cartridges` directory and it appears in the library tree.\n"
         "   * macOS: `~/Library/Application Support/DigitalSuburban/Dexed/Cartridges`\n"
         "   * Windows: `%APPDATA%\\DigitalSuburban\\Dexed\\Cartridges`\n"
         "   * Linux: `${XDG_DATA_HOME:-~/.local/share}/DigitalSuburban/Dexed/Cartridges`\n"
         "   or run `python3 tools/preset-lab/install_presets.py` from a checkout (it finds the right folder for you).\n")
L.append("## Layout\n")
L.append("| Folder | Cartridges | Voices | What is in it |\n| --- | ---: | ---: | --- |")
for sec, bs in bysec.items():
    folder = os.path.dirname(bs[0]["file"])
    L.append(f"| `{folder}` | {len(bs)} | {sum(len(b['voices']) for b in bs)} | {SECTION_BLURB[sec]} |")
L.append("")
L.append("Cartridges are numbered `LA01` to `LA32` in studio order; voice slots 1-32 inside each cartridge are the 32 front-panel voice buttons. "
         "The full voice-by-voice list is in [`doc/80S_LA_STUDIO_PRESETS.md`](../../doc/80S_LA_STUDIO_PRESETS.md), and `catalog.json` carries the same "
         "information for scripts.\n")
L.append("## Playing notes\n")
L.append("* **Basses** are transposed down an octave in the voice (TRANSPOSE = 12), so play them where a bass guitar lives (C1-C3). "
         "Pitched toms and timpani follow the keyboard.\n"
         "* **Snares, hats, cymbals, claps, metallic percussion (cowbell, claves, triangle...) and noise FX** use fixed-frequency operators: they sound the same on every key, so any key triggers the same hit. Congas, bongos and other skin drums are ratio-mode and follow the keyboard.\n"
         "* **Kicks, toms and timpani** are pitch-swept, ratio-mode voices that track the keyboard; the natural kick range is around C3.\n"
         "* **Velocity** drives *brightness* (modulator level) harder than loudness on the keys, bass, mallet and pluck voices - play them soft and hard. "
         "Organs and many sustained or atmospheric voices respond only gently to velocity.\n"
         "* **Vibrato and tremolo** come from the voice's own LFO and fade in after a short delay where an instrument would do that. "
         "Use the mod wheel / aftertouch assignments for extra expression.\n"
         "* Loudness is balanced across the library: a note at velocity 100 on any voice peaks well below full scale (median -15 dBFS), so layered chords have headroom.\n")
L.append("## How the library was made and checked\n")
L.append("Voices were *designed in physical units* (envelope times in ms and seconds, modulation depth as FM index, vibrato in cents, pitch sweeps in "
         "semitones) with a small generator in [`tools/preset-lab`](../../tools/preset-lab), whose unit conversions were measured on this repository's own msfa engine. "
         "Every voice was then rendered offline through that same engine and checked: audible, unclipped, level-balanced, in tune, not a duplicate of another voice, "
         "and free of clipping or dropouts at low, middle and high keys (two octaves either side of each voice's test note) and at soft, medium and hard velocity. "
         "These are objective checks on rendered audio; they are not a substitute for playing the sounds, and taste will differ - treat the library as a large, "
         "consistent set of starting points and edit freely.\n")
open(os.path.join(ROOT, "README.md"), "w").write("\n".join(L))

# ---------------------------------------------------------------------------------------------------
# doc/80S_LA_STUDIO_PRESETS.md  (full catalog)
# ---------------------------------------------------------------------------------------------------
D = []
D.append("# 80s LA Studio preset catalog\n")
D.append(f"{total:,} voices in {len(cat)} cartridges under [`presets/80s-LA-Studio`](../presets/80s-LA-Studio). "
         "See the [folder guide](../presets/80s-LA-Studio/README.md) for how to load them. "
         "`Alg` is the DX7 algorithm (1-32); *character* tags are measured from a render at C3, velocity 100.\n")
D.append("## Contents\n")
D.append("| Cartridge | Section | Title | Voices |\n| --- | --- | --- | ---: |")
for b in cat:
    anchor = f"la{b['bank']:02d}"
    D.append(f"| [LA{b['bank']:02d}](#{anchor}) | {b['section'].title()} | {b['title']} | {len(b['voices'])} |")
D.append("")
for sec, bs in bysec.items():
    D.append(f"## {sec.title()}\n")
    D.append(SECTION_BLURB[sec] + "\n")
    for b in bs:
        D.append(f'<a id="la{b["bank"]:02d}"></a>')
        D.append(f"### LA{b['bank']:02d} - {b['title']}\n")
        D.append(f"`{b['file']}`\n")
        D.append("| Slot | Voice | Alg | Character |\n| ---: | --- | ---: | --- |")
        for v in b["voices"]:
            D.append(f"| {v['slot']} | {v['name']} | {v['algo']} | {', '.join(v['tags'])} |")
        D.append("")
open(os.path.join(REPO, "doc", "80S_LA_STUDIO_PRESETS.md"), "w").write("\n".join(D))
print("wrote README.md and doc/80S_LA_STUDIO_PRESETS.md")
