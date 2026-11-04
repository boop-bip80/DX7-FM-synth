#!/usr/bin/env python3
"""
build_presets.py - generate the 80s LA Studio preset library.

  python3 build_presets.py            # audit + write presets/ and doc catalog
  python3 build_presets.py --audit    # audit only, no files written

Pipeline: build every voice from the recipes -> render through the repo's msfa engine -> balance loudness
(carrier levels only, never above 99) -> audit (silence / clipping / pitch / near-duplicates) -> write SysEx.
"""
import argparse, importlib, json, os, re, sys, time
from collections import Counter
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import dx7, audit

ORDER = ["keys", "bass", "brass_winds", "strings_pads", "leads_mallets", "plucks_vox", "drums", "fx_seq"]
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
OUT_DIR = os.path.join(REPO, "presets", "80s-LA-Studio")

SECTION_DIR = {"KEYS": "01-Keys", "BASS": "02-Bass", "BRASS & WINDS": "03-Brass-and-Winds", "STRINGS": "04-Strings",
               "PADS": "05-Pads", "LEADS": "06-Leads", "MALLETS & BELLS": "07-Mallets-and-Bells",
               "PLUCKS & GUITARS": "08-Plucks-and-Guitars", "VOICES": "09-Voices", "DRUMS": "10-Drums-and-Percussion",
               "FX": "11-Sound-FX", "SEQUENCE": "12-Sequence-and-Motion"}
PITCHED = {"keys", "bass", "brass", "strings", "pad", "lead", "mallet", "pluck", "vox", "wind"}
# 'bell' and 'seq' voices are deliberately inharmonic or pitch-modulated, so the autocorrelation pitch check does
# not apply to them; pitched leads that use a fixed-frequency modulator (sync sweeps) are exempt by name.
PITCH_EXEMPT = {"SYNC LEAD", "SYNC SWEEP", "SYNC BITE", "SYNC DEEP", "STAR LEAD", "PRISM", "PRISM PAD", "SYNC HOOK"}


def tags_for(m, v):
    """Short human-readable descriptors derived from the measured render and the voice's own settings."""
    t = []
    if m["silent"]:
        return t
    sus, att = m["sus_db"], m["attack_ms"]
    if sus < -35:
        t.append("percussive" if (m["t20"] is not None and m["t20"] < 1.0) else "decaying")
    elif sus < -12:
        t.append("decaying")
    else:
        t.append("sustained")
    t.append("instant attack" if att < 8 else "fast attack" if att < 30 else "soft attack" if att < 120 else
             "slow attack" if att < 400 else "very slow attack")
    cat = v.meta.get("cat", "")
    if cat not in ("drum", "fx") and m.get("f_exp"):
        r = m["cent_early"] / m["f_exp"]
        t.append("pure" if r < 1.5 else "warm" if r < 3.5 else "bright" if r < 8 else "very bright")
    if v.pms and v.lfo_pmd:
        t.append("vibrato")
    if v.lfo_amd and any(o.ams for o in v.ops):
        t.append("tremolo/gated")
    if any(l != 50 for l in v.peg_levels[:3]) or v.peg_levels[3] != 50:
        t.append("pitch sweep")
    return t


def collect():
    banks = []
    for m in ORDER:
        mod = importlib.import_module("banks." + m)
        banks.extend(mod.BANKS)
    return banks


def slug(s):
    return re.sub(r"[^A-Za-z0-9]+", "-", s).strip("-")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--audit", action="store_true", help="audit only")
    ap.add_argument("--dup", type=float, default=1.0, help="near-duplicate spectral threshold (mean dB)")
    ap.add_argument("--range", action="store_true", help="also render every voice across keys and velocities")
    args = ap.parse_args()
    t0 = time.time()
    banks = collect()
    assert len(banks) == 32, f"expected 32 banks, got {len(banks)}"
    voices, where = [], []
    for bi, (sec, title, vs) in enumerate(banks):
        assert len(vs) == 32, f"bank {bi+1} '{title}' has {len(vs)} voices"
        for vi, v in enumerate(vs):
            voices.append(v)
            where.append((bi, vi))
    names = [v.name.strip().upper() for v in voices]
    c = Counter(names)
    problems = []
    for n, k in c.items():
        if k > 1:
            problems.append(f"duplicate name {n!r} x{k}")
    for v in voices:
        if len(v.name) > 10:
            problems.append(f"name too long: {v.name!r}")
        if not all(32 <= ord(ch) < 127 for ch in v.name):
            problems.append(f"non-ASCII name: {v.name!r}")
    print(f"collected {len(voices)} voices in {len(banks)} banks ({time.time()-t0:.0f}s)")

    audio, mets = audit.run(voices)
    print(f"rendered + balanced ({time.time()-t0:.0f}s)")

    bad = []
    for i, (v, m) in enumerate(zip(voices, mets)):
        tag = f"{where[i][0]+1:02d}.{where[i][1]+1:02d} {v.name}"
        if m["silent"]:
            bad.append(f"SILENT   {tag}")
        if m["clipped"] or m["peak"] > 0.8:
            bad.append(f"HOT      {tag} peak={m['peak']:.2f}")
        if m.get("dc", 0) > 0.03:
            bad.append(f"DC       {tag} dc={m['dc']:.3f}")
        cat = v.meta.get("cat", "")
        e = m.get("f0_err")
        if cat in PITCHED and e is not None and v.name.strip() not in PITCH_EXEMPT:
            r = abs(e) % 1200
            r = min(r, 1200 - r)
            # allow octave/fifth/third relationships (sub-octaves, inharmonic bells etc.), flag only odd offsets
            if r > 60 and min(abs(abs(e) - 702), abs(abs(e) - 1902), abs(abs(e) - 386), abs(abs(e) - 1586)) > 60:
                bad.append(f"PITCH?   {tag} err={e:.0f}c")
        if m["rms_body"] < 0.004 and m["peak"] < 0.1:
            bad.append(f"QUIET    {tag} rms={m['rms_body']:.4f} peak={m['peak']:.2f}")
    fp = audit.fingerprints(audio)
    pairs = audit.nearest_pairs(fp, args.dup)
    print(f"audit done ({time.time()-t0:.0f}s)")
    print("\n== name/format problems:", len(problems))
    for p in problems:
        print("  ", p)
    print("== level/pitch/silence flags:", len(bad))
    for b in bad:
        print("  ", b)
    print(f"== near-duplicate pairs (spectral < {args.dup} dB and similar modulation):", len(pairs))
    for i, j, d in pairs:
        print(f"   {where[i][0]+1:02d}.{where[i][1]+1:02d} {voices[i].name:10s} ~ {where[j][0]+1:02d}.{where[j][1]+1:02d} {voices[j].name:10s} d={d:.2f}")
    if args.range:
        rflags, rtab = audit.range_check(voices)
        print(f"== keyboard/velocity range flags: {len(rflags)} ({time.time()-t0:.0f}s)")
        for r in rflags[:60]:
            print("   ", r)
        # velocity response summary: how much louder is vel 127 than vel 40 at the test key?
        resp = []
        for t in rtab:
            if (0, 127) in t and (0, 40) in t and t[(0, 40)] > 1e-5:
                resp.append(20 * np.log10(t[(0, 127)] / t[(0, 40)]))
        resp = np.array(resp)
        print(f"== velocity response (vel127 vs vel40 peak): median {np.median(resp):+.1f} dB, "
              f"p10 {np.percentile(resp,10):+.1f}, p90 {np.percentile(resp,90):+.1f}; flat (<1 dB): {(resp<1).sum()} voices")
    rms = np.array([m["rms_body"] for m in mets])
    pk = np.array([m["peak"] for m in mets])
    print(f"== loudness: body RMS median {np.median(rms):.3f} (p5 {np.percentile(rms,5):.3f}, p95 {np.percentile(rms,95):.3f}); peak median {np.median(pk):.2f} max {pk.max():.2f}")
    print("== algorithms used:", dict(sorted(Counter(v.algo for v in voices).items())))
    if problems or args.audit:
        sys.exit(1 if problems else 0)

    # ---- write -------------------------------------------------------------------------------------
    gi = {w: i for i, w in enumerate(where)}
    os.makedirs(OUT_DIR, exist_ok=True)
    catalog = []
    for bi, (sec, title, vs) in enumerate(banks):
        d = os.path.join(OUT_DIR, SECTION_DIR[sec])
        os.makedirs(d, exist_ok=True)
        fn = f"LA{bi+1:02d}_{slug(title)}.syx"
        raw = dx7.write_bank(vs)
        with open(os.path.join(d, fn), "wb") as f:
            f.write(raw)
        # verify by reading back
        back = dx7.read_bank(raw)
        assert [v.name for v in back] == [dx7.sanitize_name(v.name) for v in vs]
        catalog.append(dict(bank=bi + 1, section=sec, title=title, file=f"{SECTION_DIR[sec]}/{fn}",
                            voices=[dict(slot=k + 1, name=v.name.strip(), algo=v.algo, fb=v.fb, transpose=v.transpose,
                                         cat=v.meta.get("cat", ""),
                                         tags=tags_for(mets[gi[(bi, k)]], v)) for k, v in enumerate(vs)]))
    with open(os.path.join(OUT_DIR, "catalog.json"), "w") as f:
        json.dump(catalog, f, indent=1)
    print(f"wrote {len(banks)} cartridges / {len(voices)} voices to {OUT_DIR} ({time.time()-t0:.0f}s)")


if __name__ == "__main__":
    main()
