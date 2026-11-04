"""
studio.py - a small sound-design DSL on top of dx7.py / units.py.

A patch is described with carriers and modulators using musical units:
  * envelopes in dB / seconds       (shape(...))
  * modulation depth as FM index    (beta, radians; 4*pi is the DX7 maximum)
  * pitch sweeps in semitones/secs  (P.peg)
  * vibrato in cents, LFO in Hz     (P.lfo)
The result is an ordinary dx7.Voice.
"""
from __future__ import annotations
import math
from typing import Optional, Sequence, Tuple, Union
import dx7
import units as U

CARRIER_HEADROOM_DB = -6.0     # carriers start 6 dB under full scale so audit.autolevel can raise as well as lower
CURVES = {"-lin": 0, "-exp": 1, "+exp": 2, "+lin": 3}
WAVES = {"tri": 0, "saw-dn": 1, "saw-up": 2, "sq": 3, "sin": 4, "sh": 5}


def _seg_rate(l_from: int, l_to: int, secs: float) -> int:
    if l_to == l_from:
        return 99
    if l_to < l_from:
        db = 0.75 * (dx7.level_units(l_from) - dx7.level_units(l_to))
        return U.fall_rate(db / max(secs, 0.002))
    return U.atk_rate(max(secs, 0.001) * 1000)


def shape(att: float = 2.0, pts: Sequence[Tuple[float, float]] = (), rel: float = 0.3,
          peak_db: float = 0.0, end_db: Optional[float] = None):
    """Build a 4-stage EG from dB targets.

    att   - attack time in ms (to the peak)
    pts   - up to two (db, seconds) stages after the peak; db is relative to the op's full level,
            seconds is how long that segment takes
    rel   - release time (key up) in seconds, T60-style (time to fall 60 dB)
    end_db- level the release falls to (default silent)
    returns (rates[4], levels[4])
    """
    if len(pts) > 2:
        raise ValueError("EG only has two post-attack stages")
    l1 = dx7.level_for_db(peak_db) if peak_db < 0 else 99
    rates, levels = [U.atk_rate(att)], [l1]
    prev = l1
    for db, secs in pts:
        if db <= -60:
            # 'decay to silence': -60 dB is read as 'T60 = secs', the envelope runs on down to level 0
            l = 0
            rates.append(U.t60_rate(secs))
        else:
            l = dx7.level_for_db(db)
            rates.append(_seg_rate(prev, l, secs))
        levels.append(l)
        prev = l
    while len(rates) < 3:
        rates.append(99)
        levels.append(prev)
    l4 = 0 if end_db is None else dx7.level_for_db(end_db)
    rates.append(U.t60_rate(rel))
    levels.append(l4)
    return rates, levels


def organ_env(att=4.0, rel=0.06):
    return shape(att, (), rel)


# measured peak-to-peak tremolo depth (dB) for AMD = 10,25,40,60,80,99  (calibrate_am.py)
_AM_TABLE = {1: (0.5, 0.7, 1.0, 1.4, 2.0, 2.7), 2: (0.6, 1.0, 1.6, 2.6, 4.2, 6.4), 3: (0.9, 2.5, 5.7, 15.1, 38.5, 98.0)}
_AM_AMD = (10, 25, 40, 60, 80, 99)


def am_params(depth_db: float):
    """(ams, amd) producing roughly `depth_db` peak-to-peak tremolo on an operator."""
    if depth_db <= 0:
        return 0, 0
    for ams in (1, 2, 3):
        tab = _AM_TABLE[ams]
        if depth_db <= tab[-1] or ams == 3:
            break
    d = min(depth_db, tab[-1])
    for i in range(len(tab) - 1):
        if d <= tab[i + 1]:
            lo, hi = tab[i], tab[i + 1]
            f = 0.0 if hi == lo else (d - lo) / (hi - lo)
            return ams, int(round(_AM_AMD[i] + f * (_AM_AMD[i + 1] - _AM_AMD[i])))
    return ams, 99


def _rs_delta(midinote: int, rs: int) -> int:
    """Engine's ScaleRate(): extra 'qrate' steps added to every EG rate of an operator."""
    x = min(31, max(0, midinote // 3 - 7))
    return (rs * x) >> 3


def _comp_rates(rates, rs: int, midinote: int):
    """Lower EG rates so that, *after* the engine adds its key-rate scaling at `midinote`, the segment
    times are what the recipe asked for.  (Rate 99 stays instantaneous.)"""
    d = _rs_delta(midinote, rs)
    if d <= 0:
        return list(rates)
    out = []
    for r in rates:
        if r >= 99:
            out.append(r)
            continue
        q = max(0, ((r * 41) >> 6) - d)
        # smallest rate whose qrate == q
        rr = 0
        while rr < 99 and ((rr * 41) >> 6) < q:
            rr += 1
        out.append(rr)
    return out


class P:
    """Patch builder."""

    def __init__(self, name: str, algo: int, *, tr: int = 24, fb: int = 0, key: int = 60, cat: str = ""):
        self.v = dx7.Voice(name=name, algo=algo, fb=fb, transpose=tr)
        for o in self.v.ops:
            o.ol = 0
            o.rates, o.levels = [99, 99, 99, 99], [99, 99, 99, 0]
        self.t = dx7.topo(algo)
        self.v.meta.update(key=key, cat=cat, carriers=list(self.t.carriers))
        self.v.oks = 1
        self.ref_note = key + tr - 24      # sounding MIDI note that envelope times are designed at

    # -- operators -------------------------------------------------------------------------------
    def _freq(self, o: dx7.Op, ratio, fx):
        if fx is not None:
            o.mode = 1
            o.coarse, o.fine = dx7.hz_to_fixed(fx)
        else:
            o.mode = 0
            if isinstance(ratio, tuple):
                o.coarse, o.fine = ratio
            else:
                o.coarse, o.fine = dx7.ratio_to_coarse_fine(float(ratio))

    def car(self, n: int, ratio=1.0, db: float = 0.0, env=None, *, det: int = 0, kvs: int = 2,
            ams: int = 0, rs: int = 0, fx: Optional[float] = None, ks=None):
        """Configure operator n as an audible partial. `db` is its level relative to the loudest carrier."""
        if n not in self.t.carriers:
            raise ValueError(f"{self.v.name}: OP{n} is not a carrier in algorithm {self.v.algo}")
        o = self.v.op(n)
        self._freq(o, ratio, fx)
        o.ol = dx7.level_for_db(min(db, 0.0) + CARRIER_HEADROOM_DB)
        o.rates, o.levels = (env or shape())
        o.det = 7 + det
        o.kvs, o.ams, o.rs = kvs, ams, rs
        o.rates = _comp_rates(o.rates, rs, self.ref_note)
        self._ks(o, ks)
        return self

    def mod(self, n: int, ratio=1.0, beta: float = 2.0, env=None, *, det: int = 0, kvs: int = 3,
            ams: int = 0, rs: int = 0, fx: Optional[float] = None, ks=None):
        """Configure operator n as a modulator with peak FM index `beta` (radians)."""
        if n in self.t.carriers:
            raise ValueError(f"{self.v.name}: OP{n} is a carrier in algorithm {self.v.algo}")
        o = self.v.op(n)
        self._freq(o, ratio, fx)
        o.ol = U.ol_for_beta(beta)
        o.rates, o.levels = (env or shape())
        o.det = 7 + det
        o.kvs, o.ams, o.rs = kvs, ams, rs
        o.rates = _comp_rates(o.rates, rs, self.ref_note)
        self._ks(o, ks)
        return self

    def _ks(self, o: dx7.Op, ks):
        """ks = dict(bp=MIDI note, l=(curve, depth), r=(curve, depth))"""
        if not ks:
            return
        o.bp = dx7.midi_to_bp(ks.get("bp", 60))
        if "l" in ks:
            o.lc, o.ld = CURVES[ks["l"][0]], ks["l"][1]
        if "r" in ks:
            o.rc, o.rd = CURVES[ks["r"][0]], ks["r"][1]

    def off(self, *ns):
        for n in ns:
            self.v.op(n).ol = 0
        return self

    # -- global ----------------------------------------------------------------------------------
    def fbk(self, fb: int):
        self.v.fb = fb
        return self

    def lfo(self, hz: float = 5.5, wave: str = "tri", vib_cents: float = 0.0, amd: int = 0,
            delay_s: float = 0.0, sync: bool = True, trem_db: float = 0.0, trem_ops=None):
        """trem_db: audible tremolo depth (dB peak-to-peak) applied to `trem_ops` (default: all carriers)."""
        v = self.v
        if trem_db > 0:
            ams, amd = am_params(trem_db)
            for n in (trem_ops or self.t.carriers):
                if v.op(n).ol:
                    v.op(n).ams = ams
        v.lfo_speed = U.lfo_speed(hz)
        v.lfo_wave = WAVES[wave]
        pmd, pms = U.vib(vib_cents)
        v.lfo_pmd, v.pms = pmd, pms
        v.lfo_amd = amd
        v.lfo_delay = U.lfo_delay_for(delay_s) if delay_s > 0 else 0
        v.lfo_sync = 1 if sync else 0
        return self

    def peg(self, start: float = 0.0, to: Sequence[Tuple[float, float]] = ()):
        """Pitch EG: begins at `start` semitones (key-on), then moves through up to three (semitones, secs)
        targets.  Key-up drifts slowly back to `start`."""
        v = self.v
        if len(to) > 3:
            raise ValueError("pitch EG has three stages")
        cur = start
        rates, levels = [], []
        for semi, secs in to:
            rates.append(dx7.peg_rate_for_time(semi - cur, secs))
            levels.append(dx7.peg_level_for_semitones(semi))
            cur = semi
        while len(rates) < 3:
            rates.append(99)
            levels.append(levels[-1] if levels else dx7.peg_level_for_semitones(start))
        rates.append(0)
        levels.append(dx7.peg_level_for_semitones(start))
        v.peg_rates, v.peg_levels = rates, levels
        return self

    def meta(self, **kw):
        self.v.meta.update(kw)
        return self

    def done(self) -> dx7.Voice:
        self.v.clamp()
        return self.v


# --------------------------------------------------------------------------------------------------
# little helpers shared by recipes
# --------------------------------------------------------------------------------------------------
def cents_to_det(c: float) -> int:
    """Rough conversion of a detune in cents to the DX7's -7..+7 detune steps (≈ 1.2-2 cents/step mid-range)."""
    return max(-7, min(7, round(c / 1.6)))
