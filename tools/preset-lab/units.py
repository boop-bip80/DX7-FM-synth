"""
units.py - convert musical quantities (milliseconds, dB/s, FM index, cents) into DX7 parameter values.

Every table here was *measured* on the repo's msfa engine with calibrate*.py:
  * a sine carrier at output level 99 peaks at 0.125 FS; level steps are 0.75 dB
  * modulation index (radians) at output level 99 is 4*pi and falls 0.75 dB per level unit
  * EG falls at  0.0638 * (4 + (q & 3)) * 2^(q >> 2)  dB/s with q = rate*41 >> 6
  * EG attack time (to -1 dB) is interpolated from a measured table
  * LFO vibrato depth in cents is linear in PMD with a per-PMS constant
"""
import math
import bisect
import dx7

# ---- envelope -------------------------------------------------------------------------------
def _fall_speed(rate: int) -> float:
    """dB per second of a falling EG segment."""
    q = (rate * 41) >> 6
    return 0.0638 * (4 + (q & 3)) * (2 ** (q >> 2))

_FALL = [_fall_speed(r) for r in range(100)]

def fall_rate(db_per_sec: float) -> int:
    best, err = 99, 1e9
    for r in range(100):
        e = abs(math.log(_FALL[r] / max(db_per_sec, 1e-3)))
        if e < err:
            best, err = r, e
    return best

def t60_rate(t60: float) -> int:
    """EG rate whose exponential-looking decay needs `t60` seconds to fall 60 dB."""
    return fall_rate(60.0 / max(t60, 1e-3))

def t60_of_rate(rate: int) -> float:
    return 60.0 / _FALL[max(0, min(99, rate))]

# measured: rate -> seconds to reach -1 dB of target from silence
_ATT = [(16, 7.09), (20, 5.32), (24, 3.05), (28, 2.14), (32, 1.33), (36, 0.764), (40, 0.53), (44, 0.332),
        (48, 0.222), (52, 0.134), (56, 0.094), (60, 0.054), (64, 0.032), (68, 0.022), (72, 0.014), (76, 0.010),
        (80, 0.006), (84, 0.003), (88, 0.002), (99, 0.001)]

def atk_rate(ms: float) -> int:
    t = max(ms, 0.5) / 1000.0
    best, err = 99, 1e9
    for r, tt in _ATT:
        e = abs(math.log(tt / t))
        if e < err:
            best, err = r, e
    return best

def atk_ms(rate: int) -> float:
    rs = [r for r, _ in _ATT]
    if rate <= rs[0]:
        return _ATT[0][1] * 1000
    if rate >= rs[-1]:
        return _ATT[-1][1] * 1000
    i = bisect.bisect_left(rs, rate)
    (r0, t0), (r1, t1) = _ATT[i - 1], _ATT[i]
    f = (rate - r0) / (r1 - r0)
    return math.exp(math.log(t0) * (1 - f) + math.log(t1) * f) * 1000

# ---- levels / FM index ------------------------------------------------------------------------
BETA_MAX = 4 * math.pi        # modulation index (rad) at 0 dB, measured 12.58

def beta_at_units(u: float) -> float:
    return BETA_MAX * 10 ** (-0.75 * (127 - u) / 20.0)

def beta_of(ol: int, eg_level: int = 99) -> float:
    """Index of a modulator with output level `ol` while its EG sits at `eg_level` (both 0..99)."""
    u = dx7.level_units(ol) + dx7.level_units(eg_level) - 127
    return beta_at_units(u) if u > 0 else 0.0

def ol_for_beta(beta: float) -> int:
    """Output level that yields index `beta` with the EG at 99."""
    if beta <= 0:
        return 0
    u = 127 + 20 * math.log10(min(beta, BETA_MAX) / BETA_MAX) / 0.75
    best, err = 0, 1e9
    for l in range(100):
        e = abs(dx7.level_units(l) - u)
        if e < err:
            best, err = l, e
    return best

def eg_level_for_ratio(ratio: float) -> int:
    """EG level giving `ratio` (<=1) of the full-scale amplitude."""
    if ratio >= 1:
        return 99
    if ratio <= 0.0005:
        return 0
    u = 127 + 20 * math.log10(ratio) / 0.75
    best, err = 0, 1e9
    for l in range(100):
        e = abs(dx7.level_units(l) - u)
        if e < err:
            best, err = l, e
    return best

def db_to_level(db: float) -> int:
    return dx7.level_for_db(db)

# ---- LFO ---------------------------------------------------------------------------------------
_LFO_HZ = [
    0.062541, 0.125031, 0.312393, 0.437120, 0.624610, 0.750694, 0.936330, 1.125302, 1.249609, 1.436782,
    1.560915, 1.752081, 1.875117, 2.062494, 2.247191, 2.374451, 2.560492, 2.686728, 2.873976, 2.998950,
    3.188013, 3.369840, 3.500175, 3.682224, 3.812065, 4.000800, 4.186202, 4.310716, 4.501260, 4.623209,
    4.814636, 4.930480, 5.121901, 5.315191, 5.434783, 5.617346, 5.750431, 5.946717, 6.062811, 6.248438,
    6.431695, 6.564264, 6.749460, 6.868132, 7.052186, 7.250580, 7.375719, 7.556294, 7.687577, 7.877738,
    7.993605, 8.181967, 8.372405, 8.504848, 8.685079, 8.810573, 8.986341, 9.122423, 9.300595, 9.500285,
    9.607994, 9.798158, 9.950249, 10.117361, 11.251125, 11.384335, 12.562814, 13.676149, 13.904338, 15.092062,
    16.366612, 16.638935, 17.869907, 19.193858, 19.425019, 20.833333, 21.034918, 22.502250, 24.003841, 24.260068,
    25.746653, 27.173913, 27.578599, 29.052876, 30.693677, 31.191516, 32.658393, 34.317090, 34.674064, 36.416606,
    38.197097, 38.550501, 40.387722, 40.749796, 42.625746, 44.326241, 44.883303, 46.772685, 48.590865, 49.261084,
]
assert len(_LFO_HZ) == 100

def lfo_speed(hz: float) -> int:
    best, err = 0, 1e9
    for i, h in enumerate(_LFO_HZ):
        e = abs(math.log(h / hz))
        if e < err:
            best, err = i, e
    return best

# peak vibrato in cents per unit PMD, by PMS (measured)
_VIB_PER_PMD = [0.0, 0.4709, 0.9414, 1.5525, 2.588, 4.329, 7.198, 11.998]

def vib(cents: float):
    """(pmd, pms) giving about +/- `cents` of vibrato."""
    if cents <= 0:
        return 0, 0
    best = (0, 0)
    best_err = 1e9
    for pms in range(1, 8):
        pmd = cents / _VIB_PER_PMD[pms]
        if pmd > 99.4:
            continue
        pmd_i = max(1, round(pmd))
        err = abs(pmd_i * _VIB_PER_PMD[pms] - cents) / cents + (0.02 if pmd_i < 10 else 0)
        if err < best_err:
            best, best_err = (pmd_i, pms), err
    if best == (0, 0):
        best = (99, 7)
    return best

def lfo_delay_for(seconds: float) -> int:
    """LFO delay parameter for a vibrato/tremolo that reaches ~full depth after `seconds`.
    Measured on the engine (calibrate_lfodelay.py): param 40 -> 0.2 s, 50 -> 0.4 s, 60 -> 0.8 s, 70 -> 1.1 s,
    80 -> 1.5 s, 90 -> 2.1 s, 95 -> 2.5 s (below ~30 the LFO is effectively immediate)."""
    pts = [(0, 0.0), (30, 0.05), (40, 0.2), (50, 0.4), (60, 0.8), (70, 1.1), (80, 1.5), (90, 2.1), (95, 2.5), (99, 3.4)]
    if seconds <= 0:
        return 0
    for (p0, t0), (p1, t1) in zip(pts, pts[1:]):
        if seconds <= t1:
            f = (seconds - t0) / (t1 - t0)
            return int(round(p0 + f * (p1 - p0)))
    return 99
