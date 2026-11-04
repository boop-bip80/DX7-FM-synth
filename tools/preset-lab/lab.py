"""lab.py - render DX7 voices through the repo's msfa engine and analyse the audio."""
from __future__ import annotations
import os, subprocess, tempfile
from typing import Sequence, List
import numpy as np
import dx7

SR = 44100
HERE = os.path.dirname(os.path.abspath(__file__))
RENDERER = os.path.join(HERE, "build", "dx7render")


def render_voices(voices: Sequence[dx7.Voice], note=60, vel=100, hold=1.5, tail=1.0) -> np.ndarray:
    """Render up to any number of voices; returns float32 array (n_voices, samples)."""
    if not os.path.exists(RENDERER):
        raise RuntimeError("renderer missing - run tools/preset-lab/build_renderer.sh")
    out = []
    n = len(voices)
    for start in range(0, n, 32):
        chunk = list(voices[start:start + 32])
        real = len(chunk)
        while len(chunk) < 32:
            chunk.append(dx7.Voice())
        with tempfile.TemporaryDirectory() as td:
            syx = os.path.join(td, "c.syx")
            raw = os.path.join(td, "o.f32")
            with open(syx, "wb") as f:
                f.write(dx7.write_bank(chunk))
            subprocess.run([RENDERER, syx, raw, str(note), str(vel), str(hold), str(tail)], check=True)
            a = np.fromfile(raw, dtype=np.float32)
        a = a.reshape(32, -1)[:real]
        out.append(a)
    return np.concatenate(out, axis=0)


def db(x, floor=-120.0):
    return 20 * np.log10(np.maximum(x, 10 ** (floor / 20)))


def rms_env(a: np.ndarray, win=0.01) -> np.ndarray:
    """Windowed RMS envelope, one value per `win` seconds."""
    w = int(SR * win)
    n = (len(a) // w) * w
    return np.sqrt((a[:n].reshape(-1, w) ** 2).mean(axis=1))


def spectrum(a: np.ndarray, t0: float, dur: float):
    i0, n = int(t0 * SR), int(dur * SR)
    seg = a[i0:i0 + n] * np.hanning(n)
    mag = np.abs(np.fft.rfft(seg, n=max(n, 1))) / (n / 4)
    freqs = np.fft.rfftfreq(n, 1 / SR)
    return freqs, mag


def centroid(a: np.ndarray, t0: float, dur: float) -> float:
    f, m = spectrum(a, t0, dur)
    p = m ** 2
    return float((f * p).sum() / max(p.sum(), 1e-12))


def f0_estimate(a: np.ndarray, t0: float, dur: float, fmin=20.0, fmax=4000.0):
    """Autocorrelation pitch estimate (Hz) or None."""
    i0, n = int(t0 * SR), int(dur * SR)
    seg = a[i0:i0 + n].astype(np.float64)
    seg -= seg.mean()
    if np.abs(seg).max() < 1e-4:
        return None
    ac = np.correlate(seg, seg, mode="full")[n - 1:]
    lo, hi = int(SR / fmax), int(SR / fmin)
    hi = min(hi, len(ac) - 1)
    k = lo + int(np.argmax(ac[lo:hi]))
    return SR / k


def simple_voice(algo=32, **kw) -> dx7.Voice:
    v = dx7.Voice(algo=algo)
    for o in v.ops:
        o.ol = 0
    return v
