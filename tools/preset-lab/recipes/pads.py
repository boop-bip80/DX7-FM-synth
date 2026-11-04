"""Pad and atmosphere recipes built on the ensemble engine."""
from recipes.ens import ens
from studio import P, shape


def pad(name, *, algo=22, att=600, rel=1.6, spread=6, peak=2.6, sus=2.2, swell=900, settle=1.2, fb=4, vel=2, mvel=3,
        tr=24, key=60, cr=None, mr=None, mscale=None, vib=0, lfo_hz=0.6, lfo_wave="sin", trem_db=0.0, ams_mods=0,
        cat="pad", sus_db=0.0, ks_depth=20, rs=0, vib_delay=0.8):
    """Warm/lush pad: very slow bloom, wide detune, optional slow LFO drift (pitch and/or amp)."""
    lfo = {"hz": lfo_hz, "wave": lfo_wave, "vib": vib, "trem_db": trem_db, "delay": vib_delay} if (vib or trem_db) else None
    return ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, mscale=mscale, att=att, rel=rel,
               swell=swell, peak=peak, sus=sus, settle=settle, vel=vel, mvel=mvel, rs=rs, ks_depth=ks_depth,
               sus_db=sus_db, lfo=lfo, ams_mods=ams_mods)


def glass_pad(name, *, att=450, rel=2.0, ratio_a=3.5, ratio_b=5.04, ba=1.6, bb=1.2, ring=6.0, chorus=4, fb=2, vel=2,
              tr=24, key=60, cat="pad", shim=-9, lfo_hz=0.5, trem_db=2.0):
    """Glassy shimmering pad: inharmonic 3.5/5.04 modulators on detuned 1:1 carriers, bell-like top, slow tremolo."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(-3, 1.5)], rel)
    p.car(1, 1.0, 0, ce, det=-chorus, kvs=vel)
    p.mod(2, ratio_a, ba, shape(att * 1.3, [(-8, 2.0)], rel), kvs=vel + 1)
    p.car(3, 1.0, -1, ce, det=chorus, kvs=vel)
    p.mod(4, ratio_b, bb, shape(att * 1.6, [(-10, 2.0)], rel), kvs=vel + 1)
    p.car(5, 2.0, shim, shape(att * 1.5, [(-6, ring)], rel), kvs=vel)
    p.mod(6, 1.0, 1.0, shape(att, [(-6, 2.0)], rel), kvs=vel)
    p.lfo(lfo_hz, "sin", trem_db=trem_db)
    return p.done()


def sweep_pad(name, *, algo=22, rise=2500, fall=3500, peak=4.6, floor=0.7, att=700, rel=1.8, spread=5, fb=5, vel=2,
              tr=24, key=60, cat="pad", lfo_hz=0.35, vib=0, cr=None, mr=None):
    """Analog-style filter-sweep pad: modulator index rises over `rise` ms then relaxes over `fall` s."""
    from recipes.ens import fan
    import math
    p = P(name, algo, tr=tr, fb=fb, key=key, cat=cat)
    car = sorted(p.t.carriers)
    mods = [n for n in range(1, 7) if n not in car]
    dets = fan(len(car), spread)
    ce = shape(att, [(-2, 2.0)], rel)
    cr = cr or [1.0]
    for i, n in enumerate(car):
        p.car(n, cr[i % len(cr)], 0, ce, det=dets[i], kvs=vel)
    mr = mr or [1.0]
    for i, n in enumerate(mods):
        e = shape(rise, [(20 * math.log10(floor / peak), fall / 1000.0)], rel)
        p.mod(n, mr[i % len(mr)], peak * (1.0 - 0.12 * i), e, kvs=vel + 1)
    if vib:
        p.lfo(lfo_hz, "sin", vib_cents=vib, delay_s=1.0)
    return p.done()
