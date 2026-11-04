"""Ensemble engine used by brass, strings, pads and leads.

A single generic builder (`ens`) distributes carriers/modulators over *any* of the 32 algorithms using the
simulated topology, so families can use different architectures:
   alg 22: OP6(fb) drives three carriers (3,4,5) + OP2->OP1     (fat saw-like sections)
   alg 24/25/31: OP6 drives 3/2/1 carriers, the rest are free   (stacked detuned layers)
   alg 5/7/14/...: pairs and stacks                              (more focused / vocal timbres)
Modulator index follows a swell->settle contour, carriers follow an attack/sustain/release contour.
"""
import math
from studio import P, shape, WAVES


def fan(k, spread):
    """k detune offsets symmetric around 0, `spread` steps at the edges."""
    if k == 1:
        return [0]
    return [round(-spread + 2 * spread * i / (k - 1)) for i in range(k)]


def ens(name, algo=22, *, tr=24, key=60, cat="", fb=5, cr=None, spread=3, cdb=None, mr=None, mscale=None,
        att=20, dec=None, rel=0.3, sus_db=0.0, swell=40, peak=4.0, sus=3.0, settle=0.3, mod_rel=None, vel=3,
        mvel=4, rs=0, ks_depth=0, ks_bp=60, lfo=None, ams_mods=0, ams_cars=0, ratio_fine=None, mod_att=None, taper=0.5):
    t = P(name, algo, tr=tr, fb=fb, key=key, cat=cat)
    carriers = sorted(t.t.carriers)
    mods = [n for n in range(1, 7) if n not in carriers]
    k = len(carriers)
    cr = cr or [1.0] * k
    cdb = cdb or [0.0] * k
    dets = fan(k, spread)
    if dec:
        car_env = shape(att, list(dec), rel)
    elif sus_db:
        car_env = shape(att, [(sus_db, 0.6)], rel)
    else:
        car_env = shape(att, [], rel)
    for i, n in enumerate(carriers):
        t.car(n, cr[i % len(cr)], cdb[i % len(cdb)], car_env, det=dets[i], kvs=vel, rs=rs, ams=ams_cars)
    sus_ratio = max(1e-3, min(1.0, sus / peak)) if peak > 0 else 1.0
    sdb = 20 * math.log10(sus_ratio)
    mr = mr or [1.0] * max(1, len(mods))
    mscale = mscale or [1.0] * max(1, len(mods))
    ks = {"bp": ks_bp, "r": ("-lin", ks_depth)} if ks_depth else None
    for i, n in enumerate(mods):
        # a modulator that is itself modulated sits deeper in a chain: taper its index so long chains stay
        # tonal instead of collapsing into noise
        b = peak * mscale[i % len(mscale)] * (taper ** t.t.depth(n) if n not in t.t.feedback_ops else 1.0)
        if dec or sdb > -0.5:
            e = shape(mod_att if mod_att is not None else swell, [(sdb, settle)] if sdb < -0.5 else [], mod_rel or rel)
        else:
            e = shape(mod_att if mod_att is not None else swell, [(sdb, settle)], mod_rel or rel)
        t.mod(n, mr[i % len(mr)], b, e, kvs=mvel, rs=rs, ks=ks, ams=ams_mods)
    if lfo:
        t.lfo(lfo.get("hz", 5.5), lfo.get("wave", "tri"), vib_cents=lfo.get("vib", 0), amd=lfo.get("amd", 0),
              delay_s=lfo.get("delay", 0.0), trem_db=lfo.get("trem_db", 0.0))
    return t.done()
