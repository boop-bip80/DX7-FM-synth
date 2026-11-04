#!/usr/bin/env python3
"""
make_bundle.py - pack the 80s LA Studio cartridges into assets/la_studio_presets.zip.

That zip is compiled into every build as BinaryData (assets/CMakeLists.txt) and unpacked into the user's
Cartridges folder on first launch (DexedAudioProcessor::resolvAppDir in source/PluginData.cpp).

  python3 tools/preset-lab/make_bundle.py            # (re)write assets/la_studio_presets.zip
  python3 tools/preset-lab/make_bundle.py --check    # exit 1 unless the zip holds exactly the .syx files in presets/

The zip is written deterministically (sorted entries, fixed timestamps).  --check compares file *contents*, not zip bytes,
so it does not depend on the zlib version that produced the archive.
"""
import argparse, os, sys, zipfile

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
PRESETS = os.path.join(REPO, "presets")
SRC = os.path.join(PRESETS, "80s-LA-Studio")
OUT = os.path.join(REPO, "assets", "la_studio_presets.zip")
EXPECTED_CARTRIDGES = 32


def collect():
    files = {}
    for d, _, fs in os.walk(SRC):
        for f in fs:
            if f.endswith(".syx"):
                full = os.path.join(d, f)
                files[os.path.relpath(full, PRESETS).replace(os.sep, "/")] = open(full, "rb").read()
    return dict(sorted(files.items()))


def build():
    files = collect()
    if len(files) != EXPECTED_CARTRIDGES:
        sys.exit(f"expected {EXPECTED_CARTRIDGES} cartridges under {SRC}, found {len(files)}")
    for name, data in files.items():
        if len(data) != 4104 or data[:6] != bytes([0xF0, 0x43, 0x00, 0x09, 0x20, 0x00]) or data[-1] != 0xF7:
            sys.exit(f"{name} is not a DX7 32-voice cartridge")
    with zipfile.ZipFile(OUT, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for name, data in files.items():
            zi = zipfile.ZipInfo(name, date_time=(2026, 1, 1, 0, 0, 0))
            zi.compress_type = zipfile.ZIP_DEFLATED
            zi.external_attr = 0o644 << 16
            z.writestr(zi, data)
    print(f"wrote {os.path.relpath(OUT, REPO)}: {len(files)} cartridges, {os.path.getsize(OUT)} bytes")


def check():
    want = collect()
    if not os.path.exists(OUT):
        sys.exit(f"missing {OUT}; run make_bundle.py")
    with zipfile.ZipFile(OUT) as z:
        bad = z.testzip()
        if bad:
            sys.exit(f"corrupt entry in bundle: {bad}")
        have = {n: z.read(n) for n in z.namelist() if not n.endswith("/")}
    problems = []
    for n in sorted(set(want) | set(have)):
        if n not in have:
            problems.append(f"missing from bundle: {n}")
        elif n not in want:
            problems.append(f"in bundle but not in presets/: {n}")
        elif have[n] != want[n]:
            problems.append(f"differs from presets/: {n}")
    if len(want) != EXPECTED_CARTRIDGES:
        problems.append(f"presets/ holds {len(want)} cartridges, expected {EXPECTED_CARTRIDGES}")
    if problems:
        print("assets/la_studio_presets.zip is out of date:\n  " + "\n  ".join(problems))
        print("run: python3 tools/preset-lab/make_bundle.py")
        sys.exit(1)
    print(f"bundle OK: all {len(have)} cartridges ({len(have) * 32} voices) match presets/")


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true")
    check() if ap.parse_args().check else build()
