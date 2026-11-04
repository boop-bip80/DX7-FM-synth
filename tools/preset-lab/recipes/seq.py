"""Rhythmic / motion patches: trills, gated pads, arps built from LFO pitch steps, scanners."""
import math
from studio import P, shape
from recipes.ens import ens, fan


def trill(name, *, semis=7, rate=7.0, base="pluck", b=2.4, ring=0.6, att=2, rel=0.12, wave="sq", tr=24, key=72, vel=3,
          cat="seq", fb=4, chorus=3, ratio=1.0, up_down=False):
    """Two-note arpeggio: a square-wave pitch LFO flips between the key and `semis` above it.
    The LFO swings +/- semis/2 around a pitch-EG offset of -semis/2, i.e. note and note+semis."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    if base == "pluck":
        ce = shape(att, [(-6, 0.15), (-60, ring)], rel)
    else:
        ce = shape(att, [(-2, 0.4)], rel)
    p.car(1, ratio, 0, ce, kvs=vel, det=-chorus)
    p.mod(2, 1.0, b, shape(att, [(-24, 0.4 if base == "pluck" else 2.0)], rel), kvs=vel + 1)
    p.car(3, 2.0 * ratio, -8, ce, kvs=vel, det=chorus)
    p.mod(4, 1.0, b * 0.7, shape(att, [(-24, 0.4 if base == "pluck" else 2.0)], rel), kvs=vel + 1)
    p.car(5, 1.0 * ratio, -3, ce, kvs=vel, det=chorus // 2)
    p.mod(6, 1.0, b * 0.9, shape(att, [(-20, 0.5)], rel), kvs=vel + 1)
    cents = abs(semis) * 50.0
    p.lfo(rate, wave, vib_cents=cents)
    p.peg(-semis / 2.0, [(-semis / 2.0, 0.003)])
    return p.done()


def gated(name, *, rate=8.0, depth=99, algo=22, att=20, rel=0.12, peak=3.0, sus=2.6, spread=5, wave="sq", fb=5, vel=3,
          tr=24, key=60, cat="seq", cr=None, mr=None, mscale=None, swell=60):
    """'Chopped' pad: a square LFO on the carriers' amp-mod makes a rhythmic gate (trance-gate ancestor)."""
    return ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, mscale=mscale, att=att, rel=rel,
               swell=swell, peak=peak, sus=sus, settle=0.4, vel=vel, mvel=4, lfo={"hz": rate, "wave": wave, "amd": depth},
               ams_cars=3)


def scanner(name, *, hz=1.4, depth=600, wave="tri", att=6, rel=0.2, b=3.0, ratio=1.0, tr=24, key=60, cat="seq", fb=5, amd=0,
            lvl2=-5):
    """Scanning/sweeping line: triangle pitch LFO on a bright FM tone (with optional amp-mod ripple)."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [], rel)
    p.car(1, ratio, 0, ce, kvs=2, ams=1 if amd else 0)
    p.mod(2, 1.0, b, ce, kvs=3)
    p.car(3, 1.0, lvl2, ce, kvs=2, det=4, ams=1 if amd else 0)
    p.mod(4, 2.0, b * 0.6, ce, kvs=3)
    p.lfo(hz, wave, vib_cents=depth, amd=amd)
    return p.done()


def arp_pad(name, *, semis=12, rate=6.0, algo=22, att=8, rel=0.2, peak=3.0, sus=2.5, spread=3, wave="sq", fb=5, vel=3, tr=24,
            key=60, cat="seq", cr=None, mr=None, swell=30, cents=None):
    """Pad that hops between two pitches (square pitch LFO about a pitch-EG offset) - classic 80s 'octave pulse'."""
    v = ens(name, algo, tr=tr, key=key, cat=cat, fb=fb, cr=cr, spread=spread, mr=mr, att=att, rel=rel, swell=swell,
            peak=peak, sus=sus, settle=0.4, vel=vel, mvel=4, lfo={"hz": rate, "wave": wave, "vib": (cents or abs(semis) * 50.0)})
    from studio import P as _P  # apply pitch-EG offset on the finished voice
    import dx7
    lv = dx7.peg_level_for_semitones(-semis / 2.0)
    r = dx7.peg_rate_for_time(1.0, 0.003)
    v.peg_rates = [r, 99, 99, 0]
    v.peg_levels = [lv, lv, lv, lv]
    return v
