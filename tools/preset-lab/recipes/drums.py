"""Drums and percussion.

Noise is made the way DX7 drum voices were: a chain of fixed-frequency operators at maximum index with
feedback (see search_noise.py - frequency sets were chosen by measuring spectral flatness on the engine).
Pitch-swept drums use ratio-mode operators so the pitch EG can drive them; fixed-frequency operators ignore
the pitch EG and key, which is what we want for snares, hats and metals.
"""
from studio import P, shape, WAVES
import units as U

NOISE = {  # fixed-frequency chains for OP6(fb)>OP5>OP4>OP3(carrier)
    "air":  (4458, 8491, 6383, 7925),
    "high": (4613, 1929, 2242, 3928),
    "mid":  (3481, 804, 4714, 626),
    "low":  (620, 410, 540, 470),
}


def _noise(p, kind, env, db=0.0, beta=12.5, mod_env=None, car=3):
    f6, f5, f4, f3 = NOISE[kind]
    me = mod_env or env
    p.mod(6, fx=f6, beta=beta, env=me, kvs=0)
    p.mod(5, fx=f5, beta=beta, env=me, kvs=0)
    p.mod(4, fx=f4, beta=beta, env=me, kvs=0)
    p.car(car, fx=f3, db=db, env=env, kvs=1)


# --------------------------------------------------------------------------------------------------
# Kick / tom
# --------------------------------------------------------------------------------------------------
def kick(name, *, sweep=24, sweep_t=0.05, body_t=0.45, punch=-6, punch_t=0.12, click=-12, click_hz=3800, click_t=0.012,
         body_b=1.0, tr=0, key=60, fb=0, vel=3, drive=0.0, ratio=1.0, rel=0.05, cat="drum", boom=None):
    """Kick: sine body with a fast pitch drop, a 2nd-harmonic 'knock', and a short noisy beater click."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    p.car(1, ratio, 0, shape(0.5, [(-6, body_t * 0.3), (-60, body_t)], rel), kvs=vel)
    p.mod(2, 1.0, body_b + drive, shape(0.5, [(-30, body_t * 0.6)], rel), kvs=vel)
    p.car(3, 2.0 * ratio, punch, shape(0.5, [(-60, punch_t)], rel), kvs=vel)
    p.mod(4, 1.0, 1.5, shape(0.5, [(-40, punch_t)], rel), kvs=vel)
    if click is not None:
        p.car(5, fx=click_hz, db=click, env=shape(0.5, [(-60, click_t)], rel), kvs=1)
        p.mod(6, fx=click_hz * 0.43, beta=12.0, env=shape(0.5, [(-60, click_t)], rel), kvs=0)
    if sweep:
        p.peg(sweep, [(0, sweep_t)])
    if boom:
        p.car(3, ratio * 0.5, boom, shape(0.5, [(-60, body_t * 1.6)], rel), kvs=vel)
    return p.done()


def tom(name, *, sweep=10, sweep_t=0.09, t60=0.55, stick=-9, stick_hz=2600, noise=-18, ring=0.0, tr=24, key=60,
        vel=4, body_b=1.2, cat="drum", rel=0.1, fb=0, boom=0.0):
    """Tom: pitched body (tracks the keyboard), small pitch sag, stick tick and a touch of shell noise."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    p.car(1, 1.0, 0, shape(0.5, [(-8, t60 * 0.3), (-60, t60)], rel), kvs=vel)
    p.mod(2, 1.0, body_b, shape(0.5, [(-26, t60 * 0.5)], rel), kvs=vel)
    p.car(3, 1.5 + ring, -10, shape(0.5, [(-60, t60 * 0.45)], rel), kvs=vel)
    p.mod(4, 1.0, 1.8, shape(0.5, [(-34, t60 * 0.3)], rel), kvs=vel)
    p.car(5, fx=stick_hz, db=stick, env=shape(0.5, [(-60, 0.02)], rel), kvs=1)
    p.mod(6, fx=stick_hz * 0.47, beta=12.0, env=shape(0.5, [(-60, 0.02)], rel), kvs=0)
    if sweep:
        p.peg(sweep, [(0, sweep_t)])
    return p.done()


def simmons(name, *, sweep=19, sweep_t=0.16, t60=0.5, bite=-8, tr=24, key=60, vel=3, body_b=0.8, cat="drum", rel=0.08,
            dist=2.0):
    """Electronic (hex-pad style) tom: long dramatic pitch sweep on a pure tone with a noise bite on top."""
    p = P(name, 1, tr=tr, fb=0, key=key, cat=cat)
    p.car(1, 1.0, 0, shape(0.5, [(-6, t60 * 0.25), (-60, t60)], rel), kvs=vel)
    p.mod(2, 1.0, body_b + dist * 0.2, shape(0.5, [(-30, t60 * 0.5)], rel), kvs=vel)
    f6, f5, f4, f3 = NOISE["mid"]
    nz = shape(0.5, [(-60, 0.05)], rel)
    p.mod(6, fx=f6, beta=12.5, env=nz, kvs=0)
    p.mod(5, fx=f5, beta=12.5, env=nz, kvs=0)
    p.mod(4, fx=f4, beta=12.5, env=nz, kvs=0)
    p.car(3, fx=f3, db=bite, env=nz, kvs=1)
    p.peg(sweep, [(0, sweep_t)])
    return p.done()


# --------------------------------------------------------------------------------------------------
# Snare / clap / rim
# --------------------------------------------------------------------------------------------------
def snare(name, *, noise="mid", n_t60=0.22, n_db=-2, tone_hz=190, tone_t60=0.09, tone_db=-5, tone_b=2.0, snap=True,
          gate=None, att=0.6, kind_ring=0.0, cat="drum", rel=0.05, key=60, hi_db=None, beta=12.5, tone2=1.9):
    """Snare: FM-noise rattle on top of a fixed-pitch shell tone.  gate=(hold_s) makes the 80s gated-reverb cut."""
    p = P(name, 1, tr=24, fb=7, key=key, cat=cat)
    if gate:
        nenv = shape(att, [(-3, gate), (-60, 0.025)], rel)
    elif snap:
        nenv = shape(att, [(-10, n_t60 * 0.15), (-60, n_t60)], rel)
    else:
        nenv = shape(att, [(-60, n_t60)], rel)
    _noise(p, noise, nenv, n_db, beta=beta, mod_env=shape(att, [(-60, n_t60 * 1.6)], rel) if not gate else nenv)
    if tone_db is not None:
        if gate:
            tenv = shape(att, [(-60, tone_t60 * 1.4)], rel)
        else:
            tenv = shape(att, [(-60, tone_t60)], rel)
        p.car(1, fx=tone_hz, db=tone_db, env=tenv, kvs=1)
        p.mod(2, fx=tone_hz * tone2, beta=tone_b, env=shape(att, [(-50, tone_t60 * 0.5)], rel), kvs=2)
    return p.done()


def clap(name, *, bursts_hz=44, burst_t=0.075, tail_t60=0.22, tail_att=36, noise="mid", tail_db=-4, burst_db=0,
         amd=99, cat="drum", rel=0.05, key=60, wave="sq", tone_hz=0.0):
    """Hand clap: an LFO-gated noise chain gives the rapid 'flam' bursts, a second slower noise voice is the room tail.
    Uses algorithm 3: OP6(fb)>OP5>OP4 (carrier, gated) and OP3>OP2>OP1 (carrier, tail)."""
    p = P(name, 3, tr=24, fb=7, key=key, cat=cat)
    f6, f5, f4, f3 = NOISE[noise]
    # the square LFO starts in its low state, so the first burst arrives one half-cycle late: hold the burst
    # level for `burst_t` instead of decaying from the start, then cut.
    benv = shape(0.5, [(-3, burst_t), (-60, 0.035)], rel)
    p.mod(6, fx=f6, beta=12.5, env=benv, kvs=0)
    p.mod(5, fx=f5, beta=12.5, env=benv, kvs=0)
    p.car(4, fx=f4, db=burst_db, env=benv, kvs=1, ams=3)
    tenv = shape(tail_att, [(-60, tail_t60)], rel)
    tmod = shape(tail_att, [(-60, tail_t60 * 1.5)], rel)
    p.mod(3, fx=f3 * 0.62, beta=11.0, env=tmod, kvs=0)
    p.mod(2, fx=f6 * 0.71, beta=11.0, env=tmod, kvs=0)
    p.car(1, fx=f5 * 0.83, db=tail_db, env=tenv, kvs=1)
    p.lfo(bursts_hz, wave, amd=amd)
    return p.done()


def rimshot(name, *, tone_hz=1700, t60=0.05, body_hz=330, body_t=0.07, click_db=-3, crack=1.0, cat="drum", rel=0.03,
            key=60):
    """Rimshot / side stick / wood crack: very short fixed-pitch tones plus a noise crack."""
    p = P(name, 3, tr=24, fb=7, key=key, cat=cat)
    f6, f5, f4, f3 = NOISE["high"]
    nenv = shape(0.3, [(-60, 0.03)], rel)
    p.mod(6, fx=f6, beta=12, env=nenv, kvs=0)
    p.mod(5, fx=f5, beta=12, env=nenv, kvs=0)
    p.car(4, fx=f4, db=-8 + click_db, env=nenv, kvs=1)
    p.mod(3, fx=body_hz * 1.6, beta=1.5 * crack, env=shape(0.3, [(-40, body_t)], rel), kvs=2)
    p.mod(2, fx=tone_hz * 0.6, beta=2.0 * crack, env=shape(0.3, [(-40, t60)], rel), kvs=2)
    p.car(1, fx=tone_hz, db=-2, env=shape(0.3, [(-60, t60)], rel), kvs=1)
    return p.done()


# --------------------------------------------------------------------------------------------------
# Hats / cymbals / shakers
# --------------------------------------------------------------------------------------------------
def hat(name, *, noise="air", t60=0.045, open_swell=0.0, tone_db=-8, tone_hz=6400, tone_b=3.0, tone_t=0.04, att=0.5,
        cat="drum", rel=0.02, key=60, db=-2, tone2=1.5, snap=0.0):
    """Hi-hat: dense high noise plus a tonal metallic pair; closed/pedal/open set by t60."""
    p = P(name, 1, tr=24, fb=7, key=key, cat=cat)
    env = shape(att + open_swell * 1000, [(-60, t60)], rel)
    menv = shape(att, [(-60, t60 * 1.8)], rel)
    _noise(p, noise, env, db, mod_env=menv)
    if tone_db is not None:
        p.car(1, fx=tone_hz, db=tone_db, env=shape(att, [(-60, tone_t)], rel), kvs=1)
        p.mod(2, fx=tone_hz * tone2, beta=tone_b, env=shape(att, [(-50, tone_t)], rel), kvs=2)
    return p.done()


def cymbal(name, *, noise="air", wash=2.2, ping_hz=5200, ping_db=-6, ping_t=1.0, bell_hz=3150, bell_db=-10, bell_t=1.4,
           att=0.8, swell=0, cat="drum", rel=0.5, key=60, db=-4, ride=False):
    """Crash/ride/splash/china: long noise wash + inharmonic metallic partials (alg 24: OP6 drives three carriers)."""
    p = P(name, 24, tr=24, fb=7, key=key, cat=cat)
    f6, f5, f4, f3 = NOISE[noise]
    wenv = shape(att + swell, [(-8, wash * 0.1), (-60, wash)], rel)
    p.mod(6, fx=f6, beta=12.5, env=shape(att, [(-60, wash * 1.6)], rel), kvs=0)
    p.car(5, fx=f5, db=db, env=wenv, kvs=1)
    p.car(4, fx=ping_hz, db=ping_db, env=shape(att, [(-60, ping_t)], rel), kvs=1)
    p.car(3, fx=bell_hz, db=bell_db, env=shape(att, [(-60, bell_t)], rel), kvs=1)
    p.car(2, fx=f4 * 0.7, db=db - 3, env=wenv, kvs=1)
    p.car(1, fx=ping_hz * 1.41, db=ping_db - 3 if ride else ping_db - 6, env=shape(att, [(-60, ping_t * 0.8)], rel), kvs=1)
    return p.done()


def shaker(name, *, rate_hz=0, t60=0.12, att_ms=14, noise="high", tone=False, db=-2, cat="drum", rel=0.03, key=60,
           color=1.0, tail=0.0):
    """Shaker/maracas/cabasa/tambourine noise.  A soft attack (att_ms) is the 'sh' of the shake."""
    p = P(name, 1, tr=24, fb=7, key=key, cat=cat)
    env = shape(att_ms, [(-60, t60)], rel)
    _noise(p, noise, env, db, mod_env=shape(att_ms, [(-60, t60 * 1.4)], rel))
    if tone:
        p.car(1, fx=7300, db=-10, env=shape(att_ms, [(-60, t60 * 1.3 + tail)], rel), kvs=1)
        p.mod(2, fx=9100, beta=4.0 * color, env=shape(att_ms, [(-50, t60)], rel), kvs=1)
    return p.done()


def tambourine(name, *, t60=0.35, jingle_hz=6200, jingle_t=0.6, thump_hz=170, thump_db=-6, att_ms=2, db=-3, cat="drum",
               rel=0.05, key=60):
    """Tambourine: noise burst + ringing inharmonic jingle pair + head thump."""
    p = P(name, 1, tr=24, fb=7, key=key, cat=cat)
    env = shape(att_ms, [(-60, t60)], rel)
    _noise(p, "air", env, db, mod_env=shape(att_ms, [(-60, t60 * 1.7)], rel))
    p.car(1, fx=jingle_hz, db=-6, env=shape(att_ms, [(-60, jingle_t)], rel), kvs=1)
    p.mod(2, fx=jingle_hz * 1.38, beta=5.0, env=shape(att_ms, [(-50, jingle_t * 0.6)], rel), kvs=1)
    return p.done()


# --------------------------------------------------------------------------------------------------
# Tuned/untuned percussion (fixed-pitch)
# --------------------------------------------------------------------------------------------------
def metal(name, parts, *, att=0.4, cat="drum", rel=0.05, key=60, fb=0, noise_db=None, noise_t=0.03, peg=None):
    """Metallic hit from up to 3 fixed-pitch (carrier_hz, db, t60, mod_hz, beta, mod_t40) pairs on algorithm 5."""
    p = P(name, 5, tr=24, fb=fb, key=key, cat=cat)
    for (cn, mn), (hz, db, t60, mhz, b, mt) in zip(((1, 2), (3, 4), (5, 6)), parts):
        p.car(cn, fx=hz, db=db, env=shape(att, [(-60, t60)], rel), kvs=1)
        if b > 0:
            p.mod(mn, fx=mhz, beta=b, env=shape(att, [(-50, mt)], rel), kvs=2)
    if peg:
        p.peg(*peg)
    return p.done()


def conga(name, *, hz=0, tr=24, t60=0.35, slap=-6, slap_t=0.03, open_=True, sweep=2.0, key=60, body_b=1.2, cat="drum",
          rel=0.06, thump=-8, ring=1.0):
    """Hand drums (conga/bongo/djembe/tabla-ish): pitched skin with a small drop plus a slap transient (ratio mode, key-tracked)."""
    p = P(name, 5, tr=tr, fb=0, key=key, cat=cat)
    p.car(1, 1.0, 0, shape(0.4, [(-10, t60 * 0.3), (-60, t60 if open_ else t60 * 0.5)], rel), kvs=3)
    p.mod(2, 1.0, body_b, shape(0.4, [(-30, t60 * 0.4)], rel), kvs=4)
    p.car(3, 1.5 * ring, thump, shape(0.4, [(-60, t60 * 0.4)], rel), kvs=3)
    p.mod(4, 2.0, 1.4, shape(0.4, [(-40, t60 * 0.3)], rel), kvs=4)
    p.car(5, fx=3200, db=slap, env=shape(0.4, [(-60, slap_t)], rel), kvs=1)
    p.mod(6, fx=1400, beta=12.0, env=shape(0.4, [(-60, slap_t)], rel), kvs=0)
    if sweep:
        p.peg(sweep, [(0, 0.05)])
    return p.done()


def roll(name, *, hz=44.0, depth_db=14.0, noise="mid", att=180, rel=0.35, db=-2, wave="sq", tone_hz=0.0, cat="drum", key=60):
    """Snare/cymbal roll: a noise chain whose amplitude is chopped by a fast LFO (hand-roll flutter)."""
    p = P(name, 1, tr=24, fb=7, key=key, cat=cat)
    env = shape(att, [], rel)
    _noise(p, noise, env, db, mod_env=env)
    if tone_hz:
        p.car(1, fx=tone_hz, db=-6, env=env, kvs=0)
        p.mod(2, fx=tone_hz * 1.9, beta=2.0, env=env, kvs=0)
    p.lfo(hz, wave, trem_db=depth_db, trem_ops=[3] + ([1] if tone_hz else []))
    return p.done()
