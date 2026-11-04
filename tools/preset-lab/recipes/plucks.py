"""Mallets, bells, plucked strings and struck metals.

Every voice is up to three (carrier, modulator) pairs on algorithm 5.  A pair is described by `pr(...)`:
    pr(car_ratio, db, T60, mod_ratio, beta, mod_T40, det=0, fall=None)
 car_ratio : partial ratio of the carrier        db     : level of the partial in dB
 T60       : how long the partial rings (s)      fall   : optional (db, secs) initial fast drop (the 'prompt' decay)
 mod_ratio/beta : the FM transient on this partial   mod_T40 : seconds for the transient to fall 40 dB
"""
from studio import P, shape


def pr(cr, db, t60, mr, beta, mt=0.1, det=0, fall=None):
    return dict(cr=cr, db=db, t60=t60, mr=mr, beta=beta, mt=mt, det=det, fall=fall)


def pairs(name, specs, *, tr=24, key=60, fb=0, rel=0.25, att=0.7, vel=5, rs=3, ks_depth=40, cat="mallet",
          trem=None, vib=None, peg=None, mod_floor=-60, kv_car=2):
    """Build a pair-based struck/plucked voice."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ops = [(1, 2), (3, 4), (5, 6)]
    for (cn, mn), s in zip(ops, specs):
        if s["fall"]:
            ce = shape(att, [s["fall"], (-60, s["t60"])], rel)
        else:
            ce = shape(att, [(-60, s["t60"])], rel)
        p.car(cn, s["cr"], s["db"], ce, kvs=kv_car, rs=rs, det=s["det"])
        if s["beta"] > 0:
            p.mod(mn, s["mr"], s["beta"], shape(att, [(-40 if mod_floor <= -40 else mod_floor, s["mt"])], 0.05),
                  kvs=vel, rs=rs, ks={"bp": 60, "r": ("-lin", ks_depth)} if ks_depth else None)
    if trem:
        p.lfo(trem.get("hz", 5.0), trem.get("wave", "sin"), trem_db=trem.get("db", 6.0))
    if vib:
        p.lfo(vib.get("hz", 5.5), "sin", vib_cents=vib.get("cents", 8), delay_s=vib.get("delay", 0.3))
    if peg:
        p.peg(*peg)
    return p.done()


# --- mallets -----------------------------------------------------------------------------------------
def marimba(name, *, ring=1.0, hard=1.0, tone=1.0, tr=24, key=60, p3=True, wood=4.0, vel=6, fb=0, tune=4.0):
    """Marimba: fundamental + 4x partial (bar tuning) with a woody FM 'thock'."""
    specs = [pr(1.0, 0, ring, 1.0, 1.4 * hard * tone, 0.25),
             pr(tune, -12, ring * 0.35, tune * 0.5, 2.2 * hard, 0.08),
             pr(10.0, -22, ring * 0.12, 1.0, wood * hard, 0.05)] if p3 else \
            [pr(1.0, 0, ring, 1.0, 1.4 * hard, 0.25), pr(tune, -12, ring * 0.35, 1.0, 2.0 * hard, 0.08)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="mallet", rs=4)


def xylophone(name, *, ring=0.45, hard=1.2, tr=36, key=60, vel=6, tune=3.0):
    specs = [pr(1.0, 0, ring, 1.0, 1.6 * hard, 0.12), pr(tune, -8, ring * 0.5, 1.0, 2.6 * hard, 0.06),
             pr(6.0, -16, ring * 0.25, 1.0, 3.0 * hard, 0.04)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="mallet", rs=4, rel=0.1)


def vibes(name, *, ring=3.5, trem_hz=4.6, trem_db=6.0, hard=1.0, tr=24, key=60, vel=5, tune=4.0, motor=True, soft=False):
    """Vibraphone: long-ringing bars with the motor-driven tremolo."""
    specs = [pr(1.0, 0, ring, 1.0, (0.9 if soft else 1.6) * hard, 0.3),
             pr(tune, -14, ring * 0.3, 1.0, 1.6 * hard, 0.1),
             pr(10.0, -26, ring * 0.1, 1.0, 2.0 * hard, 0.06)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="mallet", rs=3,
                 trem={"hz": trem_hz, "db": trem_db} if motor else None, rel=0.6)


def glock(name, *, ring=2.5, hard=1.0, tr=36, key=60, vel=5, partials=(2.76, 5.4), mod=3.5, rel=0.5):
    """Glockenspiel / crotale / celesta-top: bright, inharmonic metal bar partials."""
    specs = [pr(1.0, 0, ring, mod, 1.6 * hard, 0.15),
             pr(partials[0], -10, ring * 0.5, 1.0, 1.2 * hard, 0.1),
             pr(partials[1], -18, ring * 0.25, 1.0, 1.2 * hard, 0.06)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="bell", rs=2, rel=rel)


def celesta(name, *, ring=2.2, soft=1.0, tr=36, key=60, vel=4):
    specs = [pr(1.0, 0, ring, 1.0, 0.7 * soft, 0.4), pr(4.0, -10, ring * 0.4, 1.0, 0.9 * soft, 0.12),
             pr(6.0, -20, ring * 0.2, 1.0, 1.0 * soft, 0.08)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="mallet", rs=2, rel=0.4)


def music_box(name, *, ring=1.6, hard=1.0, tr=48, key=60, vel=4, ratio=6.0):
    specs = [pr(1.0, 0, ring, ratio, 2.0 * hard, 0.08), pr(3.0, -12, ring * 0.4, 1.0, 1.2 * hard, 0.05),
             pr(7.0, -22, ring * 0.15, 1.0, 1.0, 0.04)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="mallet", rs=3, rel=0.25)


def tubular(name, *, ring=7.0, hard=1.0, tr=24, key=60, vel=5, mod=3.5, strike=3.5, rel=1.5, part2=2.4):
    """Tubular bells / church bell: slow-ringing inharmonic FM (1:3.5 with 1:1.4 strike partial)."""
    specs = [pr(1.0, 0, ring, mod, 3.0 * hard, 1.2),
             pr(part2, -9, ring * 0.7, 1.4, 2.0 * hard, 0.7),
             pr(strike, -14, ring * 0.35, 1.0, strike * 0.9 * hard, 0.12)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="bell", rs=2, rel=rel)


def kalimba(name, *, ring=1.0, hard=1.0, tr=36, key=60, vel=5, mod=5.4, tine=1.0):
    specs = [pr(1.0, 0, ring, 1.0, 1.0 * hard * tine, 0.2), pr(mod, -14, ring * 0.15, 1.0, 2.2 * hard, 0.04),
             pr(2.0, -12, ring * 0.45, 1.0, 0.8 * hard, 0.12)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="mallet", rs=3, rel=0.15)


def steel_drum(name, *, ring=1.1, hard=1.0, tr=24, key=60, vel=5, beat=3, trem=False):
    """Steel pan: octave + twelfth partials with a hollow FM strike and slight beating."""
    specs = [pr(1.0, 0, ring, 2.0, 2.4 * hard, 0.18, det=beat),
             pr(2.0, -6, ring * 0.8, 3.0, 1.6 * hard, 0.1, det=-beat),
             pr(3.0, -12, ring * 0.5, 1.0, 1.0 * hard, 0.06)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="mallet", rs=2, rel=0.3,
                 trem={"hz": 6.0, "db": 3.0} if trem else None)


def gong(name, *, ring=9.0, strike=3.0, tr=12, key=60, vel=5, ratios=(1.0, 1.59, 2.14), bloom=0):
    """Gong/tam-tam/metal sheet: dense inharmonic stack."""
    specs = [pr(ratios[0], 0, ring, 1.41, 3.5 * strike / 3, 1.5, det=2),
             pr(ratios[1], -4, ring * 0.9, 2.76, 3.0 * strike / 3, 1.2, det=-3),
             pr(ratios[2], -8, ring * 0.7, 3.2, 2.5 * strike / 3, 1.0, det=4)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="bell", rs=1, rel=2.0, att=0.7 + bloom, fb=2)


# --- plucked strings ---------------------------------------------------------------------------------
def nylon(name, *, ring=1.6, bright=1.0, pick=3.0, body=1.0, tr=24, key=60, vel=5, fb=2, rel=0.15):
    specs = [pr(1.0, 0, ring, 1.0, 2.0 * bright, 0.45),
             pr(2.0, -8, ring * 0.7, 3.0, 1.4 * bright, 0.2),
             pr(1.0, -10, ring * 0.9, 9.0, pick * bright, 0.03, det=2)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="pluck", rs=3, rel=rel, ks_depth=45)


def steel_gtr(name, *, ring=2.2, bright=1.0, pick=4.0, tr=24, key=60, vel=5, fb=3, rel=0.2, shimmer=-9):
    specs = [pr(1.0, 0, ring, 1.0, 2.6 * bright, 0.55),
             pr(2.0, shimmer, ring * 0.8, 3.0, 2.2 * bright, 0.3),
             pr(1.0, -9, ring, 7.0, pick * bright, 0.04, det=3)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="pluck", rs=3, rel=rel, ks_depth=40)


def clean_gtr(name, *, ring=2.5, bright=1.0, chorus=4, tr=24, key=60, vel=5, fb=3, rel=0.25, vib=0, trem=0):
    """Clean electric guitar with chorus-style detuned double."""
    specs = [pr(1.0, 0, ring, 1.0, 2.2 * bright, 0.7, det=-chorus),
             pr(1.0, -1, ring, 1.0, 2.0 * bright, 0.7, det=chorus),
             pr(2.0, -10, ring * 0.6, 3.0, 2.4 * bright, 0.1)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="pluck", rs=2, rel=rel,
                 vib={"hz": 5.2, "cents": vib, "delay": 0.4} if vib else None,
                 trem={"hz": 5.0, "db": trem} if trem else None)


def muted_gtr(name, *, ring=0.22, bright=1.0, chunk=4.0, tr=24, key=60, vel=5, fb=3, rel=0.04):
    """Palm-muted/funk chop: very short decay, bright pick transient."""
    specs = [pr(1.0, 0, ring, 1.0, 3.2 * bright, 0.09),
             pr(2.0, -6, ring * 0.7, 2.0, 2.2 * bright, 0.05),
             pr(1.0, -8, ring * 0.5, 5.0, chunk * bright, 0.03)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="pluck", rs=3, rel=rel)


def banjo(name, *, ring=0.8, twang=4.0, tr=36, key=60, vel=6, fb=3):
    specs = [pr(1.0, 0, ring, 2.0, twang, 0.22), pr(3.0, -9, ring * 0.5, 1.0, 2.0, 0.1), pr(2.0, -8, ring * 0.6, 5.0, 3.0, 0.04)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="pluck", rs=3, rel=0.05)


def mandolin(name, *, ring=0.9, tremolo_hz=0.0, tr=36, key=60, vel=5, bright=1.0, spread=5):
    specs = [pr(1.0, 0, ring, 3.0, 2.4 * bright, 0.15, det=-spread), pr(1.0, 0, ring, 3.0, 2.4 * bright, 0.15, det=spread),
             pr(2.0, -10, ring * 0.5, 1.0, 1.4 * bright, 0.06)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="pluck", rs=3, rel=0.06,
                 trem={"hz": tremolo_hz, "db": 8.0} if tremolo_hz else None)


def harp(name, *, ring=3.5, soft=1.0, tr=24, key=60, vel=4, fb=0, rel=0.35):
    specs = [pr(1.0, 0, ring, 1.0, 1.1 * soft, 0.35), pr(2.0, -9, ring * 0.7, 2.0, 1.0 * soft, 0.2),
             pr(4.0, -18, ring * 0.35, 1.0, 1.2 * soft, 0.08)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="pluck", rs=2, rel=rel, ks_depth=30)


def koto(name, *, ring=1.8, bend=1.0, pluck=3.0, tr=24, key=60, vel=5, fb=2):
    """Koto/shamisen-style: bright pluck with a small upward pitch scoop."""
    specs = [pr(1.0, 0, ring, 1.0, 2.4, 0.3), pr(2.0, -8, ring * 0.6, 3.0, pluck, 0.1), pr(3.0, -14, ring * 0.4, 1.0, 1.4, 0.06)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="pluck", rs=3, rel=0.2,
                 peg=(-1.5 * bend, [(0, 0.09)]))


def sitar(name, *, ring=3.0, buzz=4.5, tr=24, key=60, vel=5, fb=5, drone=False):
    """Sitar: bridge 'buzz' from a feedback-driven high-ratio modulator and a sympathetic drone partial."""
    specs = [pr(1.0, 0, ring, 1.0, buzz, 1.2), pr(2.0, -8, ring * 0.7, 3.5, buzz * 0.6, 0.8),
             pr(1.5 if drone else 3.0, -12, ring * 1.2, 1.0, 1.0, 0.3, det=2)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, fb=fb, cat="pluck", rs=2, rel=0.3)


def dulcimer(name, *, ring=2.0, hard=1.0, tr=36, key=60, vel=5, spread=4):
    specs = [pr(1.0, 0, ring, 1.0, 1.4 * hard, 0.12, det=-spread), pr(1.0, -1, ring, 2.0, 1.2 * hard, 0.12, det=spread),
             pr(3.0, -12, ring * 0.4, 1.0, 1.4 * hard, 0.06)]
    return pairs(name, specs, tr=tr, key=key, vel=vel, cat="pluck", rs=3, rel=0.25)
