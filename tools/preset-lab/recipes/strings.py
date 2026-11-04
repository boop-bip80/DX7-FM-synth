"""String and orchestral recipes built on the ensemble engine."""
from recipes.ens import ens
from studio import P, shape


def ensemble(name, *, algo=22, att=260, rel=0.9, spread=5, peak=3.4, sus=3.0, swell=300, settle=0.8, fb=5, vel=3,
             mvel=4, tr=24, key=60, cr=None, mr=None, mscale=None, vib=4, lfo_hz=5.0, vib_delay=0.5, ks_depth=25,
             rs=0, cat="strings", sus_db=0.0, wave="tri", trem_db=0.0):
    """Lush string section: feedback-saw modulators over 3-4 detuned carriers, slow bloom, gentle ensemble vibrato."""
    lfo = {"hz": lfo_hz, "wave": wave, "vib": vib, "delay": vib_delay, "trem_db": trem_db} if (vib or trem_db) else None
    return ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, mscale=mscale, att=att, rel=rel,
               swell=swell, peak=peak, sus=sus, settle=settle, vel=vel, mvel=mvel, rs=rs, ks_depth=ks_depth,
               sus_db=sus_db, lfo=lfo)


def solo_string(name, *, algo=1, att=70, rel=0.35, peak=3.6, sus=3.0, swell=90, settle=0.4, fb=6, vel=4, mvel=5,
                tr=24, key=60, vib=18, vib_delay=0.35, lfo_hz=5.8, cr=None, mr=None, mscale=None, spread=1, rs=1,
                cat="strings", ks_depth=20):
    """Solo violin/viola/cello: a single bowed stack, expressive delayed vibrato."""
    lfo = {"hz": lfo_hz, "wave": "sin", "vib": vib, "delay": vib_delay}
    return ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, mscale=mscale, att=att, rel=rel,
               swell=swell, peak=peak, sus=sus, settle=settle, vel=vel, mvel=mvel, rs=rs, ks_depth=ks_depth, lfo=lfo)


def pizzicato(name, *, algo=5, pluck=3.5, pluck_t=0.12, decay=(-16, 0.1, 0.35), spread=2, vel=4, mvel=6, tr=24,
              key=60, fb=3, ratio2=2.0, cat="strings", rel=0.05):
    """Plucked/pizzicato strings: instant attack, bright snap fading to a soft body, short ring."""
    p = P(name, algo, tr=tr, fb=fb, key=key, cat=cat)
    d1, t1, t2 = decay
    ce = shape(0.5, [(d1, t1), (-60, t2)], rel)
    car = sorted(p.t.carriers)
    for i, n in enumerate(car):
        p.car(n, 1.0 if i != 1 else ratio2, 0 if i != 1 else -8, ce, det=(-spread, 0, spread)[i % 3], kvs=vel, rs=2)
    for i, n in enumerate([m for m in range(1, 7) if m not in car]):
        p.mod(n, 1.0 if i != 1 else 3.0, pluck * (1.0 - 0.15 * i), shape(0.5, [(-34, pluck_t)], rel), kvs=mvel, rs=2)
    return p.done()


def tremolo_strings(name, *, rate=10.5, depth=9.0, att=120, rel=0.5, spread=5, peak=3.2, sus=3.0, fb=5, tr=24, key=60,
                    cat="strings", wave="tri", vel=3):
    """Tremolo strings: fast amplitude LFO on all carriers."""
    return ensemble(name, algo=22, att=att, rel=rel, spread=spread, peak=peak, sus=sus, swell=att * 1.2, settle=0.6,
                    fb=fb, vel=vel, tr=tr, key=key, vib=0, lfo_hz=rate, wave=wave, trem_db=depth, cat=cat)
