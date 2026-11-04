"""Keyboard recipes: electric pianos, acoustic pianos, clavs, harpsichords, organs.

Design notes (all FM, all derived from how the real instruments behave):
  * Tine/reed/hammer 'attack' = a high-ratio modulator that dies in tens/hundreds of ms.
  * 'Body' = a 1:1 modulator whose index fades slowly, so the tone mellows as it rings.
  * Chorus/beating = a second carrier pair detuned by a few cents.
  * Velocity drives *modulator* level (brightness) harder than carrier level (loudness).
  * Key scaling pulls modulator level down at the top of the keyboard and rate scaling shortens decays there.
"""
from studio import P, shape, cents_to_det


# --------------------------------------------------------------------------------------------------
# Electric pianos
# --------------------------------------------------------------------------------------------------
def rhodes(name, *, tine=14.0, tine_b=4.0, tine_t=0.35, tine_floor=-34, body_b=2.0, body_floor=-16, body_t=0.9,
           decay=(-6, 0.5, 2.5), tine_lvl=-10, chorus=3, chorus_lvl=-5, fb=5, vel=5, tr=24, key=60, rel=0.35,
           att=1.0, trem=0, trem_hz=5.2, rs=3, ks_depth=45, body_ratio=1.0, bark_ratio=1.0, cat="keys"):
    """Algorithm 5 three-pair EP: body pair + bell-like tine pair + detuned chorus pair."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    d1, t1, t2 = decay
    car_env = shape(att, [(d1, t1), (-60, t2)], rel)
    p.car(1, 1.0, 0, car_env, kvs=2, rs=rs)
    p.mod(2, body_ratio, body_b, shape(att, [(body_floor, body_t)], rel * 0.6), kvs=vel, rs=rs,
          ks={"bp": 60, "r": ("-lin", ks_depth)})
    p.car(3, 1.0, tine_lvl, shape(att, [(-14, t1 * 0.8), (-60, t2 * 0.8)], rel * 0.7), kvs=2, rs=rs)
    p.mod(4, tine, tine_b, shape(att, [(tine_floor, tine_t)], 0.08), kvs=min(7, vel + 1), rs=rs,
          ks={"bp": 55, "r": ("-lin", ks_depth + 15)})
    p.car(5, 1.0, chorus_lvl, car_env, det=chorus, kvs=2, rs=rs)
    p.mod(6, bark_ratio, body_b * 0.8, shape(att, [(body_floor - 4, body_t * 1.2)], rel * 0.6), kvs=vel, rs=rs,
          ks={"bp": 60, "r": ("-lin", ks_depth)})
    if trem:
        p.lfo(trem_hz, "tri", trem_db=trem)
    return p.done()


def wurly(name, *, bark=3.0, reed2=1.6, decay=1.8, sus=-14, trem=4.0, trem_hz=5.6, chorus=2, fb=4, vel=5, tr=24,
          key=60, rel=0.25, cat="keys", bite=2.0):
    """Reed-bar EP: strong 2nd harmonic 'bark' at the attack, reedy odd content, optional tremolo."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(1.0, [(-8, 0.25), (-60, decay)], rel)
    p.car(1, 1.0, 0, ce, kvs=2, rs=2)
    p.mod(2, 1.0, bark, shape(1.0, [(sus, decay * 0.5)], rel * 0.7), kvs=vel, rs=2, ks={"bp": 62, "r": ("-lin", 40)})
    p.car(3, 2.0, -9, shape(1.0, [(-12, 0.2), (-60, decay * 0.8)], rel), kvs=2, rs=2)
    p.mod(4, 1.0, reed2, shape(1.0, [(sus - 4, decay * 0.4)], rel * 0.6), kvs=vel, rs=2)
    p.car(5, 1.0, -5, ce, det=chorus, kvs=2, rs=2)
    p.mod(6, 2.0, bite, shape(1.0, [(-30, 0.4)], 0.1), kvs=min(7, vel + 1), rs=2)
    if trem:
        p.lfo(trem_hz, "tri", trem_db=trem)
    return p.done()


def bell_ep(name, *, mod_ratio=3.5, b=2.4, b_t=1.2, ring=3.5, shimmer=0.3, shim_ratio=7.0, chorus=3, vel=5,
            tr=24, key=60, rel=0.5, fb=3, cat="keys", sub=-8):
    """FM 'bell piano': inharmonic 1:3.5-style modulator that rings, plus 1:1 body and a glassy top."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(1.0, [(-8, 0.4), (-60, ring)], rel)
    p.car(1, 1.0, 0, ce, kvs=2, rs=2)
    p.mod(2, mod_ratio, b, shape(1.0, [(-24, b_t)], rel * 0.7), kvs=vel, rs=2, ks={"bp": 64, "r": ("-lin", 40)})
    p.car(3, 1.0, sub, ce, det=chorus, kvs=2, rs=2)
    p.mod(4, 1.0, 1.6, shape(1.0, [(-14, 0.8)], rel * 0.6), kvs=vel, rs=2)
    p.car(5, 2.0, -12, shape(1.0, [(-16, 0.3), (-60, ring * 0.6)], rel), kvs=2, rs=2)
    p.mod(6, shim_ratio, shimmer * 10, shape(1.0, [(-36, 0.5)], 0.1), kvs=vel, rs=2)
    return p.done()


def stage_ep(name, *, hammer=7.0, hammer_b=3.0, hammer_t=0.06, body_b=3.0, body_ratio=1.0, decay=4.0,
             upper=2.0, upper_lvl=-7, detune=4, vel=5, tr=24, key=60, rel=0.4, cat="keys", fb=4, trem=0):
    """Electric-grand/stage piano: strings of two registers, hard metallic hammer ping, long even decay."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(1.0, [(-10, 0.3), (-60, decay)], rel)
    p.car(1, 1.0, 0, ce, kvs=3, rs=3)
    p.mod(2, body_ratio, body_b, shape(1.0, [(-20, 1.0)], rel * 0.6), kvs=vel, rs=3, ks={"bp": 60, "r": ("-lin", 55)})
    p.car(3, upper, upper_lvl, shape(1.0, [(-14, 0.2), (-60, decay * 0.7)], rel), det=detune, kvs=3, rs=3)
    p.mod(4, 1.0, body_b * 0.5, shape(1.0, [(-24, 0.6)], rel * 0.6), kvs=vel, rs=3)
    p.car(5, 1.0, -9, ce, det=-detune, kvs=3, rs=3)
    p.mod(6, hammer, hammer_b, shape(1.0, [(-50, hammer_t)], 0.05), kvs=min(7, vel + 1), rs=3)
    if trem:
        p.lfo(4.8, "tri", trem_db=trem)
    return p.done()


def dyno_ep(name, *, b1=2.8, b2=1.4, ring=2.8, mix=-6, tine=9.0, tine_b=2.5, vel=6, tr=24, key=60, fb=6, rel=0.3,
            cat="keys", chorus=5):
    """Algorithm 7 EP: one carrier fed by a 3-op cluster (two modulators + feedback tine) plus a clean 1:1 pair."""
    p = P(name, 7, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(1.0, [(-7, 0.4), (-60, ring)], rel)
    p.car(1, 1.0, 0, ce, kvs=2, rs=3)
    p.mod(2, 1.0, b1, shape(1.0, [(-18, 0.9)], rel * 0.6), kvs=vel, rs=3, ks={"bp": 60, "r": ("-lin", 45)})
    p.car(3, 1.0, mix, ce, det=chorus, kvs=2, rs=3)
    p.mod(4, tine, tine_b, shape(1.0, [(-36, 0.3)], 0.08), kvs=min(7, vel + 1), rs=3)
    p.mod(5, 1.0, b2, shape(1.0, [(-16, 1.2)], rel * 0.6), kvs=vel, rs=3)
    p.mod(6, 1.0, b2 * 1.2, shape(1.0, [(-20, 0.9)], rel * 0.6), kvs=vel, rs=3)
    return p.done()


def pad_ep(name, *, att=120, b=1.4, ring=3.0, chorus=4, fb=4, vel=3, tr=24, key=60, rel=1.2, cat="keys",
           tine=14.0, tine_b=1.5, trem=0, trem_hz=4.4):
    """Soft, slow-bloom electric piano (the 'EP pad' used under ballads)."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(-4, 0.6), (-60, ring)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1, rs=1)
    p.mod(2, 1.0, b, shape(att * 1.2, [(-14, 1.5)], rel * 0.7), kvs=vel, rs=1)
    p.car(3, 1.0, -4, ce, det=chorus, kvs=vel - 1, rs=1)
    p.mod(4, tine, tine_b, shape(att * 0.8, [(-30, 0.6)], 0.2), kvs=vel, rs=1)
    p.car(5, 1.0, -4, ce, det=-chorus, kvs=vel - 1, rs=1)
    p.mod(6, 1.0, b * 0.9, shape(att * 1.4, [(-14, 1.8)], rel * 0.7), kvs=vel, rs=1)
    if trem:
        p.lfo(trem_hz, "sin", trem_db=trem)
    return p.done()


# --------------------------------------------------------------------------------------------------
# Acoustic pianos, honky-tonk, clavs, harpsichords
# --------------------------------------------------------------------------------------------------
def grand(name, *, hammer=4.0, hammer_b=5.0, body_b=2.6, body_floor=-22, body_t=1.6, tail=7.0, detune=2,
          bright=1.0, vel=6, tr=24, key=60, rel=0.25, cat="keys", string2=2.0, string2_lvl=-9, fb=3, rs=4):
    """Digital grand: hammer thunk + string body with prompt/after-sound decay and velocity-bright tone."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(1.0, [(-14, 0.7), (-60, tail)], rel)
    p.car(1, 1.0, 0, ce, kvs=3, rs=rs)
    p.mod(2, 1.0, body_b * bright, shape(1.0, [(body_floor, body_t)], rel * 0.6), kvs=vel, rs=rs,
          ks={"bp": 60, "r": ("-lin", 60)})
    p.car(3, 1.0, -3, ce, det=detune, kvs=3, rs=rs)
    p.mod(4, hammer, hammer_b * bright, shape(1.0, [(-48, 0.10)], 0.05), kvs=min(7, vel + 1), rs=rs,
          ks={"bp": 60, "r": ("-lin", 40)})
    p.car(5, string2, string2_lvl, shape(1.0, [(-16, 0.5), (-60, tail * 0.5)], rel), kvs=3, rs=rs)
    p.mod(6, 1.0, body_b * 0.5 * bright, shape(1.0, [(-24, 0.5)], rel * 0.6), kvs=vel, rs=rs)
    return p.done()


def honky(name, *, detune=6, body_b=3.0, tail=3.0, vel=5, tr=24, key=60, cat="keys", rel=0.2, hammer=3.0,
          hammer_b=4.0):
    """Honky-tonk / upright: widely detuned string pairs (beating) and a clangy hammer."""
    p = P(name, 5, tr=tr, fb=4, key=key, cat=cat)
    ce = shape(1.0, [(-14, 0.4), (-60, tail)], rel)
    p.car(1, 1.0, 0, ce, det=detune, kvs=3, rs=4)
    p.mod(2, 1.0, body_b, shape(1.0, [(-20, 0.8)], rel * 0.6), kvs=vel, rs=4, ks={"bp": 60, "r": ("-lin", 50)})
    p.car(3, 1.0, -1, ce, det=-detune, kvs=3, rs=4)
    p.mod(4, hammer, hammer_b, shape(1.0, [(-44, 0.07)], 0.05), kvs=min(7, vel + 1), rs=4)
    p.car(5, 1.0, -6, shape(1.0, [(-16, 0.3), (-60, tail * 0.7)], rel), det=0, kvs=3, rs=4)
    p.mod(6, 1.0, body_b * 0.7, shape(1.0, [(-22, 0.6)], rel * 0.6), kvs=vel, rs=4)
    return p.done()


def clav(name, *, snap=5.0, snap_t=0.25, body_b=3.5, decay=0.8, harm=3.0, harm_b=2.5, vel=6, tr=24, key=60,
         rel=0.05, cat="keys", pickup=1.0, fb=5, rs=4):
    """Clavinet: bright nasal pluck with a hard 'snap'; very short release (damper)."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(0.5, [(-14, 0.12), (-60, decay)], rel)
    p.car(1, 1.0, 0, ce, kvs=3, rs=rs)
    p.mod(2, 1.0, body_b, shape(0.5, [(-30, snap_t * 1.5)], rel), kvs=vel, rs=rs, ks={"bp": 60, "r": ("-lin", 35)})
    p.car(3, harm, -6, shape(0.5, [(-18, 0.1), (-60, decay * 0.6)], rel), kvs=3, rs=rs)
    p.mod(4, 1.0, harm_b, shape(0.5, [(-36, snap_t)], rel), kvs=vel, rs=rs)
    p.car(5, 1.0, -10 * (1 - pickup) - 4, shape(0.5, [(-18, 0.08), (-60, decay * 0.5)], rel), det=2, kvs=3, rs=rs)
    p.mod(6, 3.0, snap, shape(0.5, [(-40, snap_t * 0.6)], rel), kvs=min(7, vel + 1), rs=rs)
    return p.done()


def harpsichord(name, *, reg8=0, reg4=-3, reg16=None, pluck=3.2, pluck_t=0.5, decay=1.1, click=3.0, tr=24, key=60,
                rel=0.04, cat="keys", buff=0, detune=2, vel=1):
    """Harpsichord: 8' and 4' registers, jack-pluck FM spectrum, instant damper release, almost no velocity."""
    p = P(name, 5, tr=tr, fb=2, key=key, cat=cat)
    ce8 = shape(0.5, [(-18, 0.15), (-60, decay)], rel)
    p.car(1, 1.0, reg8, ce8, det=detune, kvs=vel, rs=2)
    p.mod(2, 1.0, pluck, shape(0.5, [(-34, pluck_t)], rel), kvs=vel + 1, rs=2, ks={"bp": 60, "r": ("-lin", 30)})
    p.car(3, 2.0, reg4, shape(0.5, [(-18, 0.12), (-60, decay * 0.8)], rel), kvs=vel, rs=2)
    p.mod(4, 3.0, pluck * 0.8, shape(0.5, [(-34, pluck_t * 0.7)], rel), kvs=vel + 1, rs=2)
    if reg16 is not None:
        p.car(5, 0.5, reg16, shape(0.5, [(-18, 0.2), (-60, decay * 1.2)], rel), kvs=vel, rs=2)
        p.mod(6, 1.0, pluck * 0.6, shape(0.5, [(-30, pluck_t)], rel), kvs=vel + 1, rs=2)
    else:
        p.car(5, 1.0, -9 + buff, shape(0.5, [(-24, 0.06)], rel), kvs=vel, rs=2)
        p.mod(6, 7.0, click, shape(0.5, [(-50, 0.04)], rel), kvs=vel, rs=2)
    return p.done()


# --------------------------------------------------------------------------------------------------
# Organs
# --------------------------------------------------------------------------------------------------
_DRAW_RATIOS = [0.5, 1.5, 1.0, 2.0, 3.0, 4.0]    # 16', 5-1/3', 8', 4', 2-2/3', 2'


def drawbar(name, *, bars=(8, 0, 8, 8, 0, 0), perc=None, perc_t=0.35, click=0.0, leslie=0.0, leslie_hz=6.8,
            tr=24, key=60, att=3.0, rel=0.05, cat="keys", detune=0):
    """Tonewheel organ: algorithm 32 additive.  bars = drawbar settings 0..8 for
    16', 5-1/3', 8', 4', 2-2/3', 2'.  perc=(op index 0..5, ratio) adds a decaying percussion partial."""
    p = P(name, 32, tr=tr, key=key, cat=cat)
    ops = [1, 2, 3, 4, 5, 6]
    for i, (n, r, b) in enumerate(zip(ops, _DRAW_RATIOS, bars)):
        db = -3.0 * (8 - b) if b > 0 else None
        if db is None:
            continue
        p.car(n, r, db, shape(att, [], rel), kvs=0, det=detune if i == 2 else 0)
    if perc is not None:
        n, r, db = perc
        p.car(n, r, db, shape(att, [(-60, perc_t)], rel), kvs=1)
    if leslie:
        p.lfo(leslie_hz, "sin", vib_cents=leslie * 9, trem_db=leslie * 2.2)
    return p.done()


def pipe_organ(name, *, ranks=(0.5, 1.0, 2.0, 3.0), levels=(-3, 0, -5, -9), chiff=2.0, chiff_ratio=4.0,
               att=35, rel=0.5, brightness=1.6, tr=24, key=60, cat="keys", wind=0, fb=3):
    """Church/pipe organ: a few stacked ranks, FM-reed brightness, slow-ish attack with a 'chiff', long release.
    Algorithm 19: OP1 carries (fed by OP2<-OP3), OP4/OP5 are additional ranks driven by OP6."""
    p = P(name, 19, tr=tr, fb=fb, key=key, cat=cat)
    p.car(1, ranks[1], levels[1], shape(att, [], rel), kvs=0)
    p.mod(2, 1.0, 0.5 * brightness, shape(att, [], rel), kvs=0)
    p.mod(3, ranks[1], 0.8 * brightness, shape(att, [], rel), kvs=0)
    p.car(4, ranks[0], levels[0], shape(att * 1.2, [], rel), kvs=0)
    p.car(5, ranks[2], levels[2], shape(att * 1.5, [], rel), kvs=0)
    p.mod(6, chiff_ratio if chiff else 1.0, brightness * 0.9 + chiff, shape(att * 0.5, [(-28, 0.12)], rel), kvs=0)
    return p.done()


def reed_organ(name, *, reeds=3, spread=4, b=2.2, att=18, rel=0.12, tr=24, key=60, cat="keys", fb=5,
               vib=0.0, octave=0, bright_decay=None):
    """Harmonium/musette/accordion: stacked detuned FM-sawtooth reeds (feedback-driven)."""
    p = P(name, 22, tr=tr, fb=fb, key=key, cat=cat)
    env = shape(att, [], rel)
    p.car(1, 1.0, 0, env, kvs=0, det=0)
    p.mod(2, 1.0, b * 0.6, env, kvs=0)
    reeds_cfg = [(3, 1.0, spread), (4, 1.0, -spread), (5, 2.0 if octave else 1.0, 0)]
    for k, (n, r, d) in enumerate(reeds_cfg):
        if k < reeds - 1:
            p.car(n, r, -4 - k, env, det=d, kvs=0)
    p.mod(6, 1.0, b, shape(att, [(bright_decay, 0.5)] if bright_decay else [], rel), kvs=0)
    if vib:
        p.lfo(5.8, "tri", vib_cents=vib, delay_s=0.3)
    return p.done()


def combo_organ(name, *, b=2.4, partial=2.0, trem=0, vib=6.0, chorus=2, att=4, rel=0.06, tr=24, key=60, cat="keys",
                perc=False, fb=4):
    """Transistor combo organ (square-ish via feedback) with chorus/vibrato."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    env = shape(att, [], rel)
    p.car(1, 1.0, 0, env, kvs=0)
    p.mod(2, 1.0, b, env, kvs=0)
    p.car(3, partial, -6, shape(att, [(-60, 0.5)] if perc else [], rel), kvs=0, det=chorus)
    p.mod(4, 1.0, b * 0.5, env, kvs=0)
    p.car(5, 1.0, -5, env, det=-chorus, kvs=0)
    p.mod(6, 2.0, b * 0.7, env, kvs=0)
    p.lfo(6.0, "tri", vib_cents=vib, trem_db=trem)
    return p.done()


def digital_keys(name, *, ratio_a=3.5, ratio_b=5.04, ba=2.2, bb=1.5, ring=2.4, att=1.0, chorus=3, tr=24, key=60,
                 cat="keys", vel=5, rel=0.5, fb=3, lvl_b=-5):
    """Crystalline digital keys built on inharmonic modulator ratios."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(-8, 0.5), (-60, ring)], rel)
    p.car(1, 1.0, 0, ce, kvs=2, rs=2)
    p.mod(2, ratio_a, ba, shape(att, [(-26, 0.9)], rel * 0.6), kvs=vel, rs=2, ks={"bp": 64, "r": ("-lin", 45)})
    p.car(3, 1.0, lvl_b, ce, det=chorus, kvs=2, rs=2)
    p.mod(4, ratio_b, bb, shape(att, [(-30, 0.6)], rel * 0.6), kvs=vel, rs=2)
    p.car(5, 2.0, -12, shape(att, [(-14, 0.3), (-60, ring * 0.7)], rel), kvs=2, rs=2)
    p.mod(6, 1.0, 1.2, shape(att, [(-20, 0.7)], rel * 0.6), kvs=vel, rs=2)
    return p.done()
