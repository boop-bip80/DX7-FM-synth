"""Brass and reed section recipes built on the ensemble engine."""
from recipes.ens import ens


def section(name, *, algo=22, swell=55, peak=5.0, sus=3.2, settle=0.25, spread=3, att=14, rel=0.22, vib=0, vib_delay=0.4,
            vel=4, mvel=5, tr=24, key=60, fb=6, cr=None, mr=None, mscale=None, ks_depth=30, rs=1, sus_db=0.0,
            lfo_hz=5.4, cat="brass"):
    """Sustained brass section: slow index swell (the 'blat'), detuned carriers, optional delayed vibrato."""
    lfo = {"hz": lfo_hz, "wave": "tri", "vib": vib, "delay": vib_delay} if vib else None
    return ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, mscale=mscale, att=att, rel=rel,
               swell=swell, peak=peak, sus=sus, settle=settle, vel=vel, mvel=mvel, rs=rs, ks_depth=ks_depth,
               sus_db=sus_db, lfo=lfo)


def stab(name, *, algo=22, swell=6, peak=5.5, sus=1.0, settle=0.22, spread=3, att=3, dec=((-10, 0.28), (-60, 0.5)),
         rel=0.12, vel=4, mvel=6, tr=24, key=60, fb=6, cr=None, mr=None, mscale=None, ks_depth=30, rs=1, cat="brass"):
    """Staccato brass stab / hit: instant attack, index collapses fast, short tail."""
    return ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, mscale=mscale, att=att, rel=rel,
               swell=swell, peak=peak, sus=sus, settle=settle, vel=vel, mvel=mvel, rs=rs, ks_depth=ks_depth, dec=dec)


def solo(name, *, algo=5, swell=30, peak=4.5, sus=2.6, settle=0.2, spread=1, att=10, rel=0.18, vib=14, vib_delay=0.45,
         vel=4, mvel=6, tr=24, key=60, fb=5, cr=None, mr=None, mscale=None, ks_depth=25, rs=1, cat="brass", lfo_hz=5.6,
         blat=0.0):
    """Solo trumpet/trombone/horn style voice with delayed vibrato and an index overshoot ('blat') on the attack."""
    lfo = {"hz": lfo_hz, "wave": "sin", "vib": vib, "delay": vib_delay} if vib else None
    return ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, mscale=mscale, att=att, rel=rel,
               swell=swell, peak=peak * (1 + blat), sus=sus, settle=settle, vel=vel, mvel=mvel, rs=rs, ks_depth=ks_depth,
               lfo=lfo)
