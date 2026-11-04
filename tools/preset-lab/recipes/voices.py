"""Human-voice style patches: formant-weighted additive partials + breath, and FM 'ooh' stacks."""
from studio import P, shape
import math

# partial level pattern (dB) for harmonics 1..6, tuned to put F1/F2 near the right harmonics for a ~C4 fundamental
VOWELS = {
    "ah": (0, -9, -2, -6, -12, -20),
    "oh": (-2, 0, -5, -16, -26, -34),
    "oo": (0, -12, -8, -22, -30, -40),
    "eh": (-12, -6, -14, 0, -4, -16),
    "ee": (0, -16, -22, -14, -10, -24),
    "aw": (-8, -1, 0, -9, -20, -30),
    "mm": (0, -8, -16, -24, -32, -40),
    "uh": (-6, -2, 0, -3, -10, -22),
}


def choir(name, vowel="ah", *, att=420, rel=1.0, spread=3, vib=10, vib_hz=5.2, vib_delay=0.6, breath=None, tr=24,
          key=60, vel=2, cat="vox", amd=0, sus=0.0, morph=None, ratios=(1, 2, 3, 4, 5, 6), shimmer=2):
    """Six sine partials weighted like a vowel.  Per-partial detune makes the 'many singers' beating.
    morph=(vowel2, secs): the upper partials glide to a second vowel."""
    p = P(name, 32, tr=tr, fb=0, key=key, cat=cat)
    pat = VOWELS[vowel]
    pat2 = VOWELS[morph[0]] if morph else None
    for i in range(6):
        db = pat[i]
        if db < -44:
            continue
        if pat2:
            e = shape(att, [(pat2[i] - pat[i] + 0, morph[1])], rel, peak_db=0.0)
            # morph envelope: partial moves from its level to the 2nd vowel's level relative to its own peak
        else:
            e = shape(att, [(sus, 0.8)] if sus else [], rel)
        d = (-spread if i % 2 else spread) * (1 + (i % 3)) // 2 if spread else 0
        p.car(i + 1, ratios[i], db, e, kvs=vel, det=max(-7, min(7, d)), ams=1 if amd else 0)
    p.lfo(vib_hz, "sin", vib_cents=vib, delay_s=vib_delay, amd=amd)
    return p.done()


def choir_breath(name, vowel="ah", *, breath=-24, att=380, rel=0.9, spread=3, vib=9, tr=24, key=60, vel=2, cat="vox",
                 vib_delay=0.6, amd=0):
    """Five formant partials + a noise 'air' channel (algorithm 31)."""
    p = P(name, 31, tr=tr, fb=7, key=key, cat=cat)
    pat = VOWELS[vowel]
    e = shape(att, [], rel)
    for i in range(5):
        d = (-spread if i % 2 else spread)
        p.car(i + 1, i + 1.0, pat[i], e, kvs=vel, det=d)
    benv = shape(att * 0.7, [], rel)
    p.mod(6, fx=2200, beta=11.0, env=benv, kvs=0)
    p.car(5 if False else 5, fx=3600, db=breath, env=benv, kvs=1)
    p.lfo(5.1, "sin", vib_cents=vib, delay_s=vib_delay, amd=amd)
    return p.done()


def vox_fm(name, *, b=1.4, ratio=3.0, att=300, rel=0.8, chorus=4, vib=9, tr=24, key=60, vel=2, fb=3, cat="vox", b2=1.0,
           r2=5.0, lvl=-6):
    """Compact FM 'ooh/aah': 1:1 carriers modulated by formant-region partials, stereo-style detune."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    p.car(1, 1.0, 0, ce, kvs=vel, det=-chorus)
    p.mod(2, ratio, b, shape(att, [(-6, 1.0)], rel), kvs=vel)
    p.car(3, 1.0, lvl, ce, kvs=vel, det=chorus)
    p.mod(4, r2, b2, shape(att, [(-8, 1.0)], rel), kvs=vel)
    p.car(5, 2.0, lvl - 3, ce, kvs=vel)
    p.mod(6, 1.0, 0.8, shape(att, [], rel), kvs=vel)
    p.lfo(5.0, "sin", vib_cents=vib, delay_s=0.6)
    return p.done()
