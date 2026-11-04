#!/usr/bin/env python3
"""
verify_presets.py - independently validate the shipped .syx cartridges.

Checks, for every file under presets/80s-LA-Studio:
  * exact DX7 32-voice cartridge framing (F0 43 00 09 20 00 ... checksum F7, 4104 bytes)
  * checksum matches
  * every voice parameter is inside its legal DX7 range
  * 10-character printable-ASCII names, unique across the whole library, none left as INIT VOICE
  * catalog.json agrees with the files
  * each cartridge renders audible, unclipped audio through the repo's own msfa engine (needs build_renderer.sh)
"""
import json, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
import dx7

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
ROOT = os.path.join(REPO, "presets", "80s-LA-Studio")


def legal(v: dx7.Voice):
    errs = []
    for n, o in enumerate(v.ops, 1):
        for name, val, hi in (("rate", o.rates, 99), ("level", o.levels, 99)):
            if any(not 0 <= x <= hi for x in val):
                errs.append(f"OP{n} {name}")
        for name, val, hi in (("bp", o.bp, 99), ("ld", o.ld, 99), ("rd", o.rd, 99), ("lc", o.lc, 3), ("rc", o.rc, 3), ("rs", o.rs, 7),
                              ("ams", o.ams, 3), ("kvs", o.kvs, 7), ("ol", o.ol, 99), ("mode", o.mode, 1), ("coarse", o.coarse, 31),
                              ("fine", o.fine, 99), ("det", o.det, 14)):
            if not 0 <= val <= hi:
                errs.append(f"OP{n} {name}={val}")
    for name, val, hi in (("algo", v.algo - 1, 31), ("fb", v.fb, 7), ("oks", v.oks, 1), ("lfs", v.lfo_speed, 99), ("lfd", v.lfo_delay, 99),
                          ("pmd", v.lfo_pmd, 99), ("amd", v.lfo_amd, 99), ("lks", v.lfo_sync, 1), ("lfw", v.lfo_wave, 5), ("pms", v.pms, 7),
                          ("transpose", v.transpose, 48)):
        if not 0 <= val <= hi:
            errs.append(f"{name}={val}")
    if any(not 0 <= x <= 99 for x in v.peg_rates + v.peg_levels):
        errs.append("pitch EG")
    return errs


def main():
    files = []
    for d, _, fs in os.walk(ROOT):
        files += [os.path.join(d, f) for f in fs if f.endswith(".syx")]
    files.sort()
    if not files:
        print("no .syx files found under", ROOT)
        return 1
    bad = 0
    names = {}
    total = 0
    for fp in files:
        raw = open(fp, "rb").read()
        rel = os.path.relpath(fp, ROOT)
        try:
            vs = dx7.read_bank(raw)          # raises on framing / checksum problems
        except Exception as e:
            print(f"FAIL {rel}: {e}")
            bad += 1
            continue
        for i, v in enumerate(vs):
            total += 1
            e = legal(v)
            nm = v.name
            if e:
                print(f"FAIL {rel} #{i+1} {nm!r}: out of range {e}")
                bad += 1
            if len(nm) != 10 or not all(32 <= ord(c) < 127 for c in nm):
                print(f"FAIL {rel} #{i+1}: bad name {nm!r}")
                bad += 1
            if nm.strip() in ("", "INIT VOICE"):
                print(f"FAIL {rel} #{i+1}: unnamed voice")
                bad += 1
            key = nm.strip().upper()
            if key in names:
                print(f"FAIL duplicate name {key!r}: {names[key]} and {rel} #{i+1}")
                bad += 1
            names[key] = f"{rel} #{i+1}"
    cat_path = os.path.join(ROOT, "catalog.json")
    if os.path.exists(cat_path):
        cat = json.load(open(cat_path))
        n_cat = sum(len(b["voices"]) for b in cat)
        if n_cat != total or len(cat) != len(files):
            print(f"FAIL catalog.json lists {len(cat)} banks / {n_cat} voices but disk has {len(files)} / {total}")
            bad += 1
        for b in cat:
            vs = dx7.read_bank(open(os.path.join(ROOT, b["file"]), "rb").read())
            if [x.name.strip() for x in vs] != [x["name"] for x in b["voices"]]:
                print(f"FAIL catalog names differ for {b['file']}")
                bad += 1
    # audibility through the real engine
    try:
        import lab
        for fp in files:
            vs = dx7.read_bank(open(fp, "rb").read())
            for v in vs:
                v.meta["key"] = 60
            arr = lab.render_voices(vs, note=60, vel=100, hold=4.0, tail=0.3)   # slow risers need a few seconds to speak
            pk = np.abs(arr).max(axis=1)
            for i, p in enumerate(pk):
                if p < 0.004:
                    print(f"FAIL {os.path.relpath(fp, ROOT)} #{i+1} {vs[i].name!r}: silent at C3 (peak {p:.4f})")
                    bad += 1
                if p >= 0.985:
                    print(f"FAIL {os.path.relpath(fp, ROOT)} #{i+1} {vs[i].name!r}: clipped (peak {p:.2f})")
                    bad += 1
    except RuntimeError as e:
        print("skipped engine audibility check:", e)
    print(f"{len(files)} cartridges, {total} voices, {len(names)} unique names, {bad} problems")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
