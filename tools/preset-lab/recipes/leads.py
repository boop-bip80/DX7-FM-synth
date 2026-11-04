"""Lead synth recipes."""
from recipes.ens import ens, fan
from studio import P, shape
import math


def lead(name, *, algo=22, swell=25, peak=4.2, sus=3.2, settle=0.3, spread=1, att=6, rel=0.18, vib=12, vib_delay=0.35,
         lfo_hz=5.6, lfo_wave="sin", fb=6, vel=3, mvel=5, tr=24, key=72, cr=None, mr=None, mscale=None, ks_depth=25,
         rs=1, cat="lead", dec=None, sus_db=0.0):
    """Mono-style lead: a few near-unison carriers over a feedback-saw modulator, quick swell, delayed vibrato."""
    lfo = {"hz": lfo_hz, "wave": lfo_wave, "vib": vib, "delay": vib_delay} if vib else None
    return ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, mscale=mscale, att=att, rel=rel,
               swell=swell, peak=peak, sus=sus, settle=settle, vel=vel, mvel=mvel, rs=rs, ks_depth=ks_depth, lfo=lfo,
               dec=dec, sus_db=sus_db)


def square_lead(name, *, ratio=2.0, b=1.8, b_floor=1.2, swell=8, chorus=3, att=4, rel=0.15, vib=10, vib_delay=0.3, fb=3,
                tr=24, key=72, vel=3, mvel=4, cat="lead", detune_ratio=1.0, lvl2=-4, lfo_hz=5.8, sub=None):
    """1:2 FM gives only odd harmonics = square-like. Two detuned pairs thicken it; an optional third pair adds a sub or fifth."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    sdb = 20 * math.log10(b_floor / b)
    p.car(1, 1.0, 0, ce, kvs=vel, det=-chorus)
    p.mod(2, ratio, b, shape(swell, [(sdb, 0.4)], rel), kvs=mvel)
    p.car(3, detune_ratio, lvl2, ce, kvs=vel, det=chorus)
    p.mod(4, ratio, b * 0.95, shape(swell, [(sdb, 0.4)], rel), kvs=mvel)
    if sub is not None:
        p.car(5, 0.5 if sub < 0 else 1.5, -abs(sub), ce, kvs=vel)
        p.mod(6, ratio, b * 0.5, shape(swell, [(sdb, 0.4)], rel), kvs=mvel)
    p.lfo(lfo_hz, "sin", vib_cents=vib, delay_s=vib_delay)
    return p.done()


def pluck_lead(name, *, algo=22, peak=5.0, sus=1.2, settle=0.18, t60=1.1, spread=2, att=2, rel=0.12, fb=6, vel=4, mvel=6,
               tr=24, key=72, cr=None, mr=None, mscale=None, ks_depth=30, vib=0, cat="lead"):
    """Plucky hook lead: index collapses quickly, carriers ring out and fade."""
    return lead(name, algo=algo, swell=3, peak=peak, sus=sus, settle=settle, spread=spread, att=att, rel=rel, vib=vib,
                fb=fb, vel=vel, mvel=mvel, tr=tr, key=key, cr=cr, mr=mr, mscale=mscale, ks_depth=ks_depth, cat=cat,
                dec=[(-8, 0.2), (-60, t60)])


def bell_lead(name, *, ratio_a=3.5, ratio_b=7.0, ba=2.4, bb=1.4, ring=2.0, chorus=3, vib=7, vib_delay=0.5, tr=24, key=72,
              vel=4, fb=2, cat="lead", sustain=True, att=3):
    """Bright bell/glass lead - sustains (with vibrato) rather than decaying."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(-3, 0.6)], 0.35) if sustain else shape(att, [(-8, 0.4), (-60, ring)], 0.3)
    p.car(1, 1.0, 0, ce, kvs=vel, det=-chorus)
    p.mod(2, ratio_a, ba, shape(att, [(-18, 1.0)], 0.3), kvs=vel + 1)
    p.car(3, 1.0, -3, ce, kvs=vel, det=chorus)
    p.mod(4, ratio_b, bb, shape(att, [(-24, 0.8)], 0.3), kvs=vel + 1)
    p.car(5, 2.0, -12, ce, kvs=vel)
    p.mod(6, 1.0, 1.0, shape(att, [(-12, 0.8)], 0.3), kvs=vel)
    if vib:
        p.lfo(5.4, "sin", vib_cents=vib, delay_s=vib_delay)
    return p.done()


def sync_lead(name, *, mod_hz=700, b=6.0, sweep=-12, sweep_t=0.25, att=3, rel=0.2, vib=8, tr=24, key=72, vel=3,
              cat="lead", fb=5, car2=2.0):
    """'Sync-sweep' lead.  A fixed-frequency modulator (does not follow the pitch EG) against a ratio-mode carrier that the
    pitch EG sweeps gives the swirling, moving-formant timbre of a swept slave oscillator."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    p.car(1, 1.0, 0, ce, kvs=vel)
    p.mod(2, fx=mod_hz, beta=b, env=shape(att, [(-6, 0.6)], rel), kvs=vel)
    p.car(3, car2, -6, ce, kvs=vel, det=3)
    p.mod(4, fx=mod_hz * 1.5, beta=b * 0.7, env=shape(att, [(-8, 0.6)], rel), kvs=vel)
    p.peg(sweep, [(0, sweep_t)])
    p.lfo(5.8, "sin", vib_cents=vib, delay_s=0.4)
    return p.done()


def chip_lead(name, *, ratio=2.0, b=2.0, fb=4, att=1, rel=0.05, tr=24, key=72, vel=0, cat="lead", detune=0):
    """Hard-edged digital/arcade lead with no vibrato and almost no release."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    p.car(1, 1.0, 0, ce, kvs=vel, det=detune)
    p.mod(2, ratio, b, shape(att, [], rel), kvs=vel)
    p.car(3, 2.0, -9, ce, kvs=vel)
    p.mod(4, 1.0, b * 0.6, shape(att, [], rel), kvs=vel)
    return p.done()
