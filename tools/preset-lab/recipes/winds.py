"""Winds and breath instruments: additive partials + FM breath noise (alg 31 / 5)."""
from studio import P, shape
import math

FLUTE_NOISE = (3900, 1800)   # (OP5 carrier Hz, OP6 feedback modulator Hz)


def additive_wind(name, partials, *, breath=-26, breath_t=0.15, chiff=0.0, att=60, rel=0.18, vib=14, vib_delay=0.35,
                  vib_hz=5.4, tr=24, key=72, vel=2, scoop=0.0, cat="wind", sus=0.0, spread=0, noise_hz=FLUTE_NOISE,
                  tremolo=0):
    """Algorithm 31: OP1-4 are sine partials, OP6(fb)>OP5 is a noisy breath channel with its own envelope.
    partials = [(ratio, db), ...] up to four."""
    p = P(name, 31, tr=tr, fb=7, key=key, cat=cat)
    ce = shape(att, [(sus, 0.5)] if sus else [], rel)
    for i, (r, db) in enumerate(partials[:4]):
        p.car(i + 1, r, db, ce, kvs=vel, det=(spread if i % 2 else -spread), ams=1 if tremolo else 0)
    if breath is not None:
        benv = shape(att * 0.6, [(-10, breath_t), (breath if chiff else -3, 0.3)], rel) if chiff else shape(att * 0.8, [], rel)
        p.mod(6, fx=noise_hz[1], beta=11.0, env=benv, kvs=0)
        p.car(5, fx=noise_hz[0], db=breath - (chiff or 0), env=benv, kvs=1)
    p.lfo(vib_hz, "sin", vib_cents=vib, delay_s=vib_delay, amd=tremolo)
    if scoop:
        p.peg(-scoop, [(0, 0.12)])
    return p.done()


def reed(name, *, ratio=2.0, b=1.6, b_floor=1.1, b2=1.0, hum=-6, att=35, rel=0.14, vib=10, vib_delay=0.4, fb=4, tr=24,
         key=60, vel=3, cat="wind", breath=-30, third=False, detune=2, tilt=1.0):
    """Clarinet/oboe/sax family via FM: 1:2 gives the odd-harmonic clarinet spectrum, 1:1 the fuller double-reed/sax one."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    sdb = 20 * math.log10(b_floor / b)
    p.car(1, 1.0, 0, ce, kvs=vel, det=-detune)
    p.mod(2, ratio, b, shape(att * 1.4, [(sdb, 0.3)], rel), kvs=vel + 1)
    p.car(3, 1.0, hum, ce, kvs=vel, det=detune)
    p.mod(4, ratio if not third else 3.0, b2 * tilt, shape(att * 1.4, [(sdb, 0.3)], rel), kvs=vel + 1)
    if breath is not None:
        benv = shape(att * 0.8, [], rel)
        p.mod(6, fx=1800, beta=11.0, env=benv, kvs=0)
        p.car(5, fx=3900, db=breath, env=benv, kvs=1)
    p.lfo(5.5, "sin", vib_cents=vib, delay_s=vib_delay)
    return p.done()
