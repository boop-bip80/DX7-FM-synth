#!/usr/bin/env python3
"""
install_presets.py - copy the 80s LA Studio cartridges into the synth's Cartridges folder.

  python3 tools/preset-lab/install_presets.py            # auto-detect the folder for this OS
  python3 tools/preset-lab/install_presets.py --dest DIR # or choose one
  python3 tools/preset-lab/install_presets.py --dry-run
"""
import argparse, os, shutil, sys

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
SRC = os.path.join(REPO, "presets", "80s-LA-Studio")


def default_dest():
    if sys.platform == "darwin":
        base = os.path.expanduser("~/Library/Application Support/DigitalSuburban/Dexed")
    elif sys.platform.startswith("win"):
        base = os.path.join(os.environ.get("APPDATA", os.path.expanduser("~")), "DigitalSuburban", "Dexed")
    else:
        base = os.path.join(os.environ.get("XDG_DATA_HOME") or os.path.expanduser("~/.local/share"), "DigitalSuburban", "Dexed")
    return os.path.join(base, "Cartridges")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--dest", help="Cartridges folder (default: auto-detected)")
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()
    dest = os.path.join(a.dest or default_dest(), "80s-LA-Studio")
    n = 0
    for d, _, fs in os.walk(SRC):
        for f in fs:
            if not f.endswith(".syx"):
                continue
            rel = os.path.relpath(os.path.join(d, f), SRC)
            target = os.path.join(dest, rel)
            print(("would copy " if a.dry_run else "copy ") + rel)
            if not a.dry_run:
                os.makedirs(os.path.dirname(target), exist_ok=True)
                shutil.copyfile(os.path.join(d, f), target)
            n += 1
    print(f"{n} cartridges {'would be ' if a.dry_run else ''}installed to {dest}")


if __name__ == "__main__":
    main()
