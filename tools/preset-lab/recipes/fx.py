"""Sound effects: risers, impacts, lasers, sirens, weather and machines."""
import math
from studio import P, shape, WAVES
from recipes.drums import NOISE


def _chain(p, kind, env, db=0.0, beta=12.5, mod_env=None):
    f6, f5, f4, f3 = NOISE[kind]
    me = mod_env or env
    p.mod(6, fx=f6, beta=beta, env=me, kvs=0)
    p.mod(5, fx=f5, beta=beta, env=me, kvs=0)
    p.mod(4, fx=f4, beta=beta, env=me, kvs=0)
    p.car(3, fx=f3, db=db, env=env, kvs=0)


def riser(name, *, dur=3.0, semis=24, noise="high", n_db=-4, tone_ratio=1.0, tone_b=2.0, b_end=6.0, tone_db=-3, tr=24, key=60,
          rel=0.35, cat="fx", fall=False, hold_end=True):
    """Build-up: noise swells while a modulated tone sweeps up (or down with fall=True)."""
    p = P(name, 1, tr=tr, fb=7, key=key, cat=cat)
    env = shape(dur * 1000, [(-1, 0.2)], rel)
    _chain(p, noise, env, n_db, mod_env=shape(dur * 1000, [], rel))
    p.car(1, tone_ratio, tone_db, shape(dur * 800, [(-1, 0.2)], rel), kvs=0)
    p.mod(2, 1.0, b_end, shape(dur * 1000, [], rel), kvs=0)
    p.peg(semis if fall else 0.0, [(0.0 if fall else float(semis), dur)])
    return p.done()


def impact(name, *, boom_hz_ratio=0.5, sweep=24, sweep_t=0.25, t60=3.0, noise="low", n_t60=1.2, n_db=-3, crack=-8, tr=0,
           key=60, rel=0.5, cat="fx", metal=None):
    """Cinematic hit: sub boom with a deep pitch drop, noise crash and a metallic transient."""
    p = P(name, 5, tr=tr, fb=0, key=key, cat=cat)
    p.car(1, boom_hz_ratio * 2, 0, shape(0.5, [(-6, t60 * 0.2), (-60, t60)], rel), kvs=2)
    p.mod(2, 1.0, 1.2, shape(0.5, [(-30, t60 * 0.3)], rel), kvs=3)
    p.car(3, 1.0, -9, shape(0.5, [(-60, t60 * 0.6)], rel), kvs=2)
    p.mod(4, 1.0, 2.2, shape(0.5, [(-30, t60 * 0.4)], rel), kvs=3)
    f6, f5, f4, f3 = NOISE[noise]
    p.mod(6, fx=f6, beta=12.5, env=shape(0.5, [(-60, n_t60 * 1.4)], rel), kvs=0)
    p.car(5, fx=f5, db=n_db + crack, env=shape(0.5, [(-60, n_t60)], rel), kvs=0)
    p.peg(sweep, [(0, sweep_t)])
    return p.done()


def laser(name, *, start=36, end=-12, t=0.25, ratio=1.0, mod_ratio=1.0, b=5.0, b_end=-24, t60=0.4, tr=36, key=60, fb=3,
          echo=0.0, rel=0.08, cat="fx", up=False):
    """Zap/laser/blaster: a fast exponential pitch sweep through an FM pair."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    p.car(1, ratio, 0, shape(0.5, [(-60, t60)], rel), kvs=2)
    p.mod(2, mod_ratio, b, shape(0.5, [(b_end, t * 1.2)], rel), kvs=3)
    p.car(3, ratio * 2.0, -9, shape(0.5, [(-60, t60 * 0.7)], rel), kvs=2)
    p.mod(4, mod_ratio * 1.01, b * 0.7, shape(0.5, [(b_end, t)], rel), kvs=3)
    if echo:
        p.car(5, ratio, -6, shape(0.5, [(-60, t60 * (1 + echo))], rel), kvs=2, det=3)
        p.mod(6, mod_ratio, b * 0.6, shape(0.5, [(b_end, t * 1.5)], rel), kvs=3)
    s, e = (end, start) if up else (start, end)
    p.peg(s, [(e, t)])
    return p.done()


def siren(name, *, hz=0.9, depth=900, wave="tri", ratio=1.0, b=1.8, att=40, rel=0.25, tr=24, key=60, fb=4, cat="fx",
          tone2=1.5, lvl2=-6, delay=0.0):
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    p.car(1, ratio, 0, ce, kvs=0)
    p.mod(2, 1.0, b, shape(att, [], rel), kvs=0)
    p.car(3, tone2, lvl2, ce, kvs=0, det=3)
    p.mod(4, 1.0, b * 0.8, shape(att, [], rel), kvs=0)
    p.lfo(hz, wave, vib_cents=depth, delay_s=delay)
    return p.done()


def two_tone(name, *, f1=440, f2=480, rate=0.0, pulse_hz=0.0, amd=99, wave="sq", b=0.6, att=6, rel=0.05, cat="fx", tr=24, key=60):
    """Phone ring / dial tone / alarm: two fixed tones, optionally gated by an LFO."""
    p = P(name, 5, tr=tr, fb=0, key=key, cat=cat)
    ce = shape(att, [], rel)
    ams = 3 if pulse_hz else 0
    p.car(1, fx=f1, db=0, env=ce, kvs=0, ams=ams)
    p.mod(2, fx=f1, beta=b, env=ce, kvs=0)
    p.car(3, fx=f2, db=0, env=ce, kvs=0, ams=ams)
    p.mod(4, fx=f2, beta=b, env=ce, kvs=0)
    if pulse_hz:
        p.lfo(pulse_hz, wave, amd=amd)
    return p.done()


def gust(name, *, kind="mid", gust_hz=0.35, gust_depth=70, att=900, rel=1.2, wave="sh", db=-2, tr=24, key=60, cat="fx",
         beta=12.5, speed=None, hi=False):
    """Wind / surf / rain: a noise chain whose amplitude is randomised by a sample-and-hold LFO."""
    p = P(name, 1, tr=tr, fb=7, key=key, cat=cat)
    env = shape(att, [], rel)
    f6, f5, f4, f3 = NOISE[kind]
    p.mod(6, fx=f6, beta=beta, env=env, kvs=0)
    p.mod(5, fx=f5, beta=beta, env=env, kvs=0)
    p.mod(4, fx=f4, beta=beta, env=env, kvs=0)
    p.car(3, fx=f3, db=db, env=env, kvs=0, ams=3)
    if hi:
        p.car(1, fx=f5 * 0.5, db=db - 6, env=env, kvs=0, ams=2)
        p.mod(2, fx=f6 * 0.6, beta=11.0, env=env, kvs=0)
    p.lfo(gust_hz, wave, amd=gust_depth)
    return p.done()


def rotor(name, *, hz=11.0, depth=90, ratio=0.5, b=3.0, noise_db=-8, att=250, rel=0.6, tr=12, key=60, cat="fx", wave="saw-dn",
          kind="low", fb=5):
    """Helicopter / engine / machinery: pulsing low tone and noise, amplitude-modulated at the blade rate."""
    p = P(name, 2, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    p.car(1, ratio, 0, ce, kvs=0, ams=3)
    p.mod(2, 1.0, b, ce, kvs=0)
    f6, f5, f4, f3 = NOISE[kind]
    p.mod(6, fx=f6, beta=12.0, env=ce, kvs=0)
    p.mod(5, fx=f5, beta=12.0, env=ce, kvs=0)
    p.mod(4, fx=f4, beta=12.0, env=ce, kvs=0)
    p.car(3, fx=f3, db=noise_db, env=ce, kvs=0, ams=3)
    p.lfo(hz, wave, amd=depth)
    return p.done()


def powerdown(name, *, semis=-36, t=1.8, ratio=1.0, b=2.5, tr=24, key=60, cat="fx", up=False, fb=4, lvl=-3):
    """Tape stop / power down / power up: the whole voice glides in pitch."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(3, [], 0.2)
    p.car(1, ratio, 0, ce, kvs=0)
    p.mod(2, 1.0, b, ce, kvs=0)
    p.car(3, 2.0, lvl - 3, ce, kvs=0, det=3)
    p.mod(4, 1.0, b * 0.8, ce, kvs=0)
    if up:
        p.peg(semis, [(0, t)])
    else:
        p.peg(0, [(semis, t)])
        p.v.peg_levels[3] = 50    # key-up returns to concert pitch
    return p.done()


def bleeper(name, *, hz=14.0, depth=1100, wave="sh", ratio=1.0, b=1.4, att=1, rel=0.1, tr=24, key=60, cat="fx", fb=3, t60=0.0):
    """Computer/sparkle/bubble bleeps: sample-and-hold or stepped pitch LFO on a sine/FM tone."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(-60, t60)] if t60 else [], rel)
    p.car(1, ratio, 0, ce, kvs=0)
    p.mod(2, 1.0, b, shape(att, [(-20, 0.3)], rel), kvs=0)
    p.car(3, 2.0, -9, ce, kvs=0)
    p.mod(4, 1.0, b, ce, kvs=0)
    p.lfo(hz, wave, vib_cents=depth)
    return p.done()


def drone(name, *, ratios=(1.0, 1.01, 0.5, 2.003), b=2.6, lfo_hz=0.12, vib=25, amd=40, att=1800, rel=2.5, tr=24, key=48, cat="fx",
          fb=5, inharm=0.0):
    """Space drone / dark atmosphere: detuned low partials and a slowly wandering FM index."""
    p = P(name, 22, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    p.car(1, ratios[0], 0, ce, kvs=0, det=-2, ams=1)
    p.mod(2, 1.0 + inharm, b * 0.6, ce, kvs=0, ams=1)
    p.car(3, ratios[1], -3, ce, kvs=0, det=2, ams=1)
    p.car(4, ratios[2], -5, ce, kvs=0, ams=1)
    p.car(5, ratios[3], -8, ce, kvs=0, det=-3, ams=1)
    p.mod(6, 1.0 + inharm * 2, b, shape(att * 1.4, [(-4, 3.0)], rel), kvs=0, ams=2)
    p.lfo(lfo_hz, "sin", vib_cents=vib, amd=amd)
    return p.done()
