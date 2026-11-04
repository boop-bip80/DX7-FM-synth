"""audit.py - render every voice through the msfa engine, measure it, and balance loudness."""
from __future__ import annotations
import math
from typing import Dict, List, Sequence
import numpy as np
import dx7, lab

SR = lab.SR
HOLD, TAIL = 1.6, 1.2
TARGET_RMS = 0.085          # steady-state 'body' loudness (-21.4 dBFS) ~ one carrier at level 99
PEAK_LIMIT = 0.70


def _win_rms(a: np.ndarray, win: float) -> np.ndarray:
    return lab.rms_env(a, win)


def _stft_bands(a: np.ndarray, n_slices=12, n_bands=24, dur=2.4):
    """Log-band dB spectrogram used as a timbre/envelope fingerprint."""
    n = int(dur * SR)
    a = a[:n] if len(a) >= n else np.pad(a, (0, n - len(a)))
    win = 4096
    # time slices spaced roughly logarithmically so the attack gets resolution
    edges = np.geomspace(0.01, dur, n_slices + 1)
    edges[0] = 0.0
    edges[-1] = dur
    freqs = np.fft.rfftfreq(win, 1 / SR)
    bands = np.geomspace(60, 14000, n_bands + 1)
    out = np.full((n_slices, n_bands), -100.0)
    for i in range(n_slices):
        t0 = edges[i]
        t1 = max(edges[i + 1], t0 + 0.012)
        seg = a[int(t0 * SR):int(t1 * SR)]
        if len(seg) < 64:
            continue
        seg = seg[:win]
        w = np.hanning(len(seg))
        spec = np.abs(np.fft.rfft(seg * w, n=win)) ** 2 / max(len(seg), 1)
        for b in range(n_bands):
            m = (freqs >= bands[b]) & (freqs < bands[b + 1])
            p = spec[m].sum() if m.any() else 0.0
            out[i, b] = 10 * math.log10(p + 1e-12)
    return out


def analyze(a: np.ndarray, voice: dx7.Voice, key: int = 60) -> Dict:
    m: Dict = {}
    pk = float(np.abs(a).max())
    m["peak"] = pk
    m["dc"] = float(abs(a.mean()))
    env = _win_rms(a, 0.005)
    denv = lab.db(env, -100)
    m["silent"] = pk < 0.004
    m["clipped"] = pk >= 0.985
    if m["silent"]:
        m.update(rms_body=0.0, attack_ms=0, t20=None, sus_db=-100, rel_ms=0, cent_early=0, cent_late=0, flat=0, f0_err=None)
        return m
    ipk = int(denv.argmax())
    m["t_peak_ms"] = ipk * 5
    # onset = first window within 40 dB of the peak
    ion = int(np.argmax(denv > denv[ipk] - 40))
    iatt = int(np.argmax(denv > denv[ipk] - 3))
    m["attack_ms"] = max(0, (iatt - ion) * 5)
    after = denv[ipk:]
    idx = np.where(after < denv[ipk] - 20)[0]
    m["t20"] = float(idx[0] * 0.005) if len(idx) else None
    hold_i = int(HOLD / 0.005)
    seg_before = denv[max(0, hold_i - 12):hold_i]
    m["sus_db"] = float(seg_before.mean() - denv[ipk])
    rel = denv[hold_i:]
    base = seg_before.mean()
    idx = np.where(rel < base - 30)[0]
    m["rel_ms"] = float(idx[0] * 5) if len(idx) else None
    w50 = _win_rms(a[: int(HOLD * SR)], 0.05)
    m["rms_body"] = float(np.percentile(w50, 92)) if len(w50) else 0.0
    m["cent_early"] = lab.centroid(a, 0.03, 0.12)
    m["cent_late"] = lab.centroid(a, 0.5, 0.25) if HOLD > 0.75 else m["cent_early"]
    # spectral flatness 80..12k on 0.05-0.35 s
    f, mag = lab.spectrum(a, 0.04, 0.3)
    band = (f > 80) & (f < 12000)
    p = mag[band] ** 2 + 1e-14
    m["flat"] = float(np.exp(np.log(p).mean()) / p.mean())
    # pitch accuracy
    exp_note = key + voice.transpose - 24
    f_exp = 440.0 * 2 ** ((exp_note - 69) / 12)
    f0 = lab.f0_estimate(a, 0.15, 0.2, fmin=max(20.0, f_exp / 2.5), fmax=min(8000.0, f_exp * 2.5)) if f_exp < 3000 else None
    m["f0_err"] = float(1200 * math.log2(f0 / f_exp)) if f0 else None
    m["f_exp"] = f_exp
    return m


def render_all(voices: Sequence[dx7.Voice], vel=100) -> np.ndarray:
    """Render each voice at its own test key (grouped so the engine runs once per distinct key)."""
    out = [None] * len(voices)
    by_key: Dict[int, List[int]] = {}
    for i, v in enumerate(voices):
        by_key.setdefault(int(v.meta.get("key", 60)), []).append(i)
    for key, idxs in by_key.items():
        arr = lab.render_voices([voices[i] for i in idxs], note=key, vel=vel, hold=HOLD, tail=TAIL)
        for j, i in enumerate(idxs):
            out[i] = arr[j]
    return np.array(out)


def _shift_carriers(v: dx7.Voice, delta_db: float):
    for n in v.meta.get("carriers", []):
        o = v.op(n)
        if o.ol == 0:
            continue
        o.ol = dx7.level_for_db(min(0.0, dx7.level_db(o.ol) + delta_db))


def autolevel(voices: Sequence[dx7.Voice], iters=3):
    """Shift all carriers of each voice so its body loudness hits the target (never above level 99)."""
    for _ in range(iters):
        audio = render_all(voices)
        changed = 0
        for v, a in zip(voices, audio):
            tgt = TARGET_RMS * 10 ** (v.meta.get("gain_db", 0.0) / 20.0)
            mode = v.meta.get("lvl", "body")
            body = float(np.percentile(_win_rms(a[: int(HOLD * SR)], 0.05), 92))
            # percussive/short sounds have a small 'body' RMS but a big peak: judge them by the larger of
            # body RMS and peak/4 so they are not driven into the limiter chasing an RMS they can't reach
            cur = max(body, float(np.abs(a).max()) / 4.0)
            if cur < 1e-5:
                continue
            delta = 20 * math.log10(tgt / cur)
            # never let peak exceed limit
            pk = float(np.abs(a).max())
            if pk * 10 ** (delta / 20) > PEAK_LIMIT:
                delta = 20 * math.log10(PEAK_LIMIT / pk)
            if delta > 0:
                # never push the loudest carrier past level 99: clipping each carrier separately would erase the
                # relative balance the recipe designed (e.g. noise vs. tone in a snare)
                top = max((dx7.level_db(v.op(n).ol) for n in v.meta.get("carriers", []) if v.op(n).ol), default=0.0)
                delta = min(delta, -top)
            if abs(delta) >= 0.4:
                _shift_carriers(v, delta)
                changed += 1
        if not changed:
            break


def run(voices: Sequence[dx7.Voice], do_level=True, verbose=False):
    if do_level:
        autolevel(voices)
    audio = render_all(voices)
    mets = []
    for v, a in zip(voices, audio):
        mets.append(analyze(a, v, int(v.meta.get("key", 60))))
    return audio, mets


def modfeat(a: np.ndarray, t0=0.35, t1=1.45) -> np.ndarray:
    """Modulation fingerprint: how loudness (tremolo/gating) and pitch (vibrato) move over the sustain.
    8 amplitude-modulation bands (0.5-32 Hz, dB re mean envelope) + 6 pitch-modulation bands (1-10 Hz, cents)."""
    seg = a[int(t0 * SR):int(t1 * SR)].astype(np.float64)
    out = np.full(14, -40.0)
    env = _win_rms(seg, 0.005)
    if len(env) < 64 or env.mean() < 1e-4:
        return out
    e = 20 * np.log10(np.maximum(env, 1e-6))
    e = e - np.convolve(e, np.ones(51) / 51, mode="same")      # remove slow trend (0.25 s)
    e = e[25:-25] * np.hanning(len(e) - 50)
    sp = np.abs(np.fft.rfft(e, n=1024)) / max(len(e), 1)
    fr = np.fft.rfftfreq(1024, 0.005)
    edges = np.geomspace(0.5, 32, 9)
    for b in range(8):
        m = (fr >= edges[b]) & (fr < edges[b + 1])
        out[b] = 20 * np.log10(sp[m].max() + 1e-3) if m.any() else -40.0
    # pitch track
    f = []
    w = int(0.09 * SR)
    for i in range(0, len(seg) - w, int(0.025 * SR)):
        s = seg[i:i + w] - seg[i:i + w].mean()
        if np.abs(s).max() < 1e-4:
            f.append(np.nan)
            continue
        ac = np.correlate(s, s, mode="full")[w - 1:]
        lo, hi = int(SR / 2000), min(int(SR / 40), w - 1)
        k = lo + int(np.argmax(ac[lo:hi]))
        f.append(SR / k)
    f = np.array(f, dtype=np.float64)
    ok = ~np.isnan(f)
    if ok.sum() > 20:
        c = 1200 * np.log2(f[ok] / np.median(f[ok]))
        c = np.clip(c, -150, 150)
        c = c - c.mean()
        sp2 = np.abs(np.fft.rfft(c * np.hanning(len(c)), n=256)) / max(len(c), 1)
        fr2 = np.fft.rfftfreq(256, 0.025)
        edges2 = np.geomspace(1.0, 10.0, 7)
        for b in range(6):
            m = (fr2 >= edges2[b]) & (fr2 < edges2[b + 1])
            out[8 + b] = 20 * np.log10(sp2[m].max() + 1e-2) if m.any() else -40.0
    return np.maximum(out, -40.0)


def fingerprints(audio: np.ndarray):
    spec = np.maximum(np.array([_stft_bands(a).ravel() for a in audio]), -90.0)
    mod = np.array([modfeat(a) for a in audio])
    return spec, mod


def nearest_pairs(fp, thresh: float, mod_thresh: float = 4.0):
    """Pairs that are spectrally AND temporally (modulation-wise) alike."""
    spec, mod = fp
    n = len(spec)
    pairs = []
    for i in range(n):
        d = np.abs(spec[i + 1:] - spec[i]).mean(axis=1)
        dm = np.abs(mod[i + 1:] - mod[i]).mean(axis=1)
        for j in np.where((d < thresh) & (dm < mod_thresh))[0]:
            pairs.append((i, i + 1 + int(j), float(d[j])))
    return sorted(pairs, key=lambda x: x[2])


def fmt(m: Dict) -> str:
    def g(k, f="{:.0f}"):
        v = m.get(k)
        return "-" if v is None else f.format(v)
    return (f"pk={m['peak']:.2f} rms={g('rms_body','{:.3f}')} att={g('attack_ms')}ms t20={g('t20','{:.2f}')}s "
            f"sus={g('sus_db','{:.0f}')}dB rel={g('rel_ms')}ms cent={g('cent_early')}/{g('cent_late')}Hz "
            f"flat={g('flat','{:.3f}')} f0err={g('f0_err')}c" + (" SILENT" if m['silent'] else "") + (" CLIP" if m['clipped'] else ""))


def range_check(voices, notes=(-24, 0, 24), vels=(40, 100, 127)):
    """Render every voice at low/mid/high keys and soft/medium/hard velocity.  Returns (flags, table) where flags
    lists silence/clipping problems and table[i] = dict of peak per (note offset, vel)."""
    flags = []
    table = [dict() for _ in voices]
    for off in notes:
        for vel in vels:
            if off == 0 and vel == 100:
                continue
            by_key = {}
            for i, v in enumerate(voices):
                k = int(v.meta.get("key", 60)) + off
                k = max(24, min(108, k))
                by_key.setdefault(k, []).append(i)
            for k, idxs in by_key.items():
                arr = lab.render_voices([voices[i] for i in idxs], note=k, vel=vel, hold=0.9, tail=0.5)
                for j, i in enumerate(idxs):
                    pk = float(np.abs(arr[j]).max())
                    table[i][(off, vel)] = pk
                    if pk >= 0.985:
                        flags.append(f"CLIP   {voices[i].name:10s} key{off:+d} vel{vel} peak={pk:.2f}")
                    if pk < 0.0015 and vel >= 100:
                        flags.append(f"SILENT {voices[i].name:10s} key{off:+d} vel{vel} peak={pk:.4f}")
    return flags, table
