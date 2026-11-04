"""Bass recipes.  Basses sound one octave (tr=12) below the key; audit/test key is C2 (48)."""
from studio import P, shape, cents_to_det

BK = dict(tr=12, key=48)


def fm_bass(name, *, thump=5.5, thump_t=0.2, floor=-16, decay=(-8, 0.4, 1.4), sustain=None, click=2.5,
            click_ratio=5.0, click_t=0.05, sub=-13, mod_ratio=1.0, upper=2.0, upper_lvl=-10, fb=2, vel=4,
            rel=0.12, att=1.0, tr=12, key=48, rs=1, cat="bass", det=0):
    """The classic DX bass: 1:1 'thump' modulator whose index collapses in ~150 ms, a clicky high-ratio snap,
    a sub-octave and a 2nd-harmonic carrier.  sustain=None -> plucked decay; sustain=dB -> held note."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    d1, t1, t2 = decay
    ce = shape(att, [(d1, t1), (-60, t2)], rel) if sustain is None else shape(att, [(sustain, t1)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1, rs=rs, det=det)
    p.mod(2, mod_ratio, thump, shape(att, [(floor, thump_t)], rel), kvs=vel + 1 if vel < 7 else 7, rs=rs,
          ks={"bp": 48, "r": ("-lin", 25)})
    p.car(3, upper, upper_lvl, shape(att, [(-14, t1 * 0.6), (-60, t2 * 0.5)], rel), kvs=vel - 1, rs=rs)
    p.mod(4, click_ratio, click, shape(att, [(-44, click_t)], 0.05), kvs=min(7, vel + 2), rs=rs)
    if sub is not None:
        p.car(5, 0.5, sub, ce, kvs=vel - 1, rs=rs)
        p.mod(6, 0.5, thump * 0.35, shape(att, [(floor, thump_t * 1.5)], rel), kvs=vel, rs=rs)
    return p.done()


def slap(name, *, pop=6.0, pop_t=0.05, thumb=6.0, thumb_t=0.18, string_ratio=5.04, decay=0.9, vel=6, fb=3,
         tr=12, key=48, rel=0.1, cat="bass", mute=0, sub=-14, bright=1.0):
    """Slap/pop bass: a loud low 'thumb' transient plus a high inharmonic 'pop' that dies in ~50 ms."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(0.5, [(-12, 0.12), (-60, decay)], rel)
    p.car(1, 1.0, 0, ce, kvs=3, rs=1)
    p.mod(2, 1.0, thumb * bright, shape(0.5, [(-26, thumb_t)], rel), kvs=vel, rs=1)
    p.car(3, 2.0, -7, shape(0.5, [(-16, 0.1), (-60, decay * 0.6)], rel), kvs=3, rs=1)
    p.mod(4, string_ratio, pop * bright, shape(0.5, [(-48, pop_t)], 0.04), kvs=7, rs=1)
    p.car(5, 0.5, sub, ce, kvs=3, rs=1)
    p.mod(6, 3.0, 2.0 + mute * 0.5, shape(0.5, [(-36, 0.07)], 0.05), kvs=min(7, vel + 1), rs=1)
    return p.done()


def fretless(name, *, mwah=2.2, mwah_peak_ms=90, sustain=-3, vib=7.0, vib_delay=0.4, bright=1.0, fb=3, att=12,
             tr=12, key=48, rel=0.25, cat="bass", harm=-10, vel=4):
    """Fretless: mid-forward 'mwah' (index swells after the attack), singing sustain, delayed vibrato."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(sustain, 0.5)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1, rs=1)
    p.mod(2, 1.0, mwah * bright, shape(mwah_peak_ms, [(-10, 0.7)], rel), kvs=vel, rs=1)
    p.car(3, 2.0, harm, ce, kvs=vel - 1, rs=1)
    p.mod(4, 1.0, 1.0 * bright, shape(mwah_peak_ms * 1.4, [(-12, 0.6)], rel), kvs=vel, rs=1)
    p.car(5, 0.5, -15, ce, kvs=vel - 1, rs=1)
    p.mod(6, 1.0, 0.7, shape(att, [(-8, 0.5)], rel), kvs=vel, rs=1)
    p.lfo(5.6, "sin", vib_cents=vib, delay_s=vib_delay)
    return p.done()


def upright(name, *, thud=4.0, thud_t=0.1, decay=1.6, wood=4.5, wood_t=0.03, bow=0.0, fb=3, vel=4, tr=12, key=48,
            rel=0.15, cat="bass", body=-5):
    """Acoustic/upright: round fundamental, woody finger-board 'clack', moderate decay."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(3.0 + bow * 120, [(-10, 0.3), (-60, decay)], rel)
    p.car(1, 1.0, 0, ce, kvs=3, rs=2)
    p.mod(2, 1.0, thud, shape(3.0 + bow * 120, [(-30, thud_t * 3)], rel), kvs=vel, rs=2)
    p.car(3, 2.0, body, shape(3.0, [(-18, 0.18), (-60, decay * 0.5)], rel), kvs=3, rs=2)
    p.mod(4, 3.0, wood, shape(1.0, [(-46, wood_t)], 0.04), kvs=vel + 1 if vel < 7 else 7, rs=2)
    p.car(5, 0.5, -16, ce, kvs=3, rs=2)
    p.mod(6, 1.0, 0.8, shape(3.0, [(-14, 0.4)], rel), kvs=vel, rs=2)
    return p.done()


def synth_bass(name, *, saw=5, sweep=5.0, sweep_t=0.25, floor=-16, sustain=-1, det=3, sub=-5, fb=5, rel=0.1, att=1.0,
               tr=12, key=48, cat="bass", pwm=0.0, vel=3, ratio2=1.0, vib=0.0):
    """Analog-style bass: feedback sawtooth (alg 2: OP2 fb -> OP1) detuned against a second saw stack
    (OP6>5>4>3) with a filter-like 'pluck' made from a collapsing modulator index."""
    p = P(name, 2, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(sustain, 0.3)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1, rs=0)
    p.mod(2, 1.0, sweep, shape(att, [(floor, sweep_t)], rel), kvs=vel, rs=0, ks={"bp": 48, "r": ("-lin", 20)})
    p.mod(6, ratio2, 4.5 * saw / 5.0, shape(att, [(floor - 4, sweep_t)], rel), kvs=vel, rs=0)
    p.mod(5, 1.0, 2.5 + pwm, shape(att, [(floor - 2, sweep_t * 1.3)], rel), kvs=vel, rs=0)
    p.mod(4, 1.0, 1.5, shape(att, [(floor, sweep_t * 1.5)], rel), kvs=vel, rs=0)
    p.car(3, 1.0, sub if sub is not None else -60, ce, det=det, kvs=vel - 1, rs=0)
    if vib:
        p.lfo(5.5, "tri", vib_cents=vib, delay_s=0.2)
    return p.done()


def sub_bass(name, *, drop=0, drop_t=0.06, decay=(-4, 1.2, 2.0), sustain=None, harm=-18, harm_ratio=2.0, punch=1.5,
             att=2.0, tr=12, key=48, rel=0.15, cat="bass", fb=0, vel=3, top=-30):
    """Sub bass: nearly pure sine (two stacked octaves) with an optional kick-like pitch drop and a hint of punch."""
    p = P(name, 5, tr=tr, fb=fb, key=key, cat=cat)
    d1, t1, t2 = decay
    ce = shape(att, [(d1, t1), (-60, t2)], rel) if sustain is None else shape(att, [(sustain, 0.4)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1, rs=0)
    p.mod(2, 1.0, punch, shape(att, [(-30, 0.1)], rel), kvs=vel, rs=0)
    p.car(3, harm_ratio, harm, ce, kvs=vel - 1, rs=0)
    p.mod(4, 1.0, punch * 0.5, shape(att, [(-30, 0.08)], rel), kvs=vel, rs=0)
    p.car(5, 0.5, -8, ce, kvs=vel - 1, rs=0)
    p.mod(6, 1.0, 0.3, shape(att, [(-30, 0.1)], rel), kvs=vel, rs=0)
    if drop:
        p.peg(drop, [(0, drop_t)])
    return p.done()


def pulse_bass(name, *, rate_hz=8.0, depth=99, shape_w="sq", saw=4.0, floor=-8, sweep_t=0.3, fb=4, rel=0.08, att=1.0,
               tr=12, key=48, cat="bass", det=3, vel=3, sub=-4):
    """Gated/pulsing synth bass: square-wave amp-mod on the carriers chops the tone into 16th-note style pulses."""
    p = P(name, 2, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(-2, 0.3)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1, ams=3)
    p.mod(2, 1.0, saw, shape(att, [(floor, sweep_t)], rel), kvs=vel)
    p.mod(6, 1.0, saw * 0.8, shape(att, [(floor, sweep_t)], rel), kvs=vel)
    p.mod(5, 1.0, 2.0, shape(att, [(floor, sweep_t)], rel), kvs=vel)
    p.mod(4, 1.0, 1.2, shape(att, [(floor, sweep_t)], rel), kvs=vel)
    p.car(3, 1.0, sub, ce, det=det, kvs=vel - 1, ams=3)
    p.lfo(rate_hz, shape_w, amd=depth, sync=True)
    return p.done()


def oct_bass(name, *, rate_hz=7.5, semis=12, saw=3.5, floor=-10, sweep_t=0.4, fb=4, rel=0.08, att=1.0, tr=12,
             key=48, cat="bass", det=2, vel=3):
    """Octave-jumping disco/Eurodisco bass: a square-wave pitch LFO around a static pitch-EG offset flips between
    the note and the octave below it at `rate_hz`."""
    from units import vib
    p = P(name, 2, tr=tr + semis // 2 if False else tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(-2, 0.3)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1)
    p.mod(2, 1.0, saw, shape(att, [(floor, sweep_t)], rel), kvs=vel)
    p.mod(6, 1.0, saw * 0.7, shape(att, [(floor, sweep_t)], rel), kvs=vel)
    p.mod(5, 1.0, 2.0, shape(att, [(floor, sweep_t)], rel), kvs=vel)
    p.mod(4, 1.0, 1.0, shape(att, [(floor, sweep_t)], rel), kvs=vel)
    p.car(3, 1.0, -5, ce, det=det, kvs=vel - 1)
    p.lfo(rate_hz, "sq", vib_cents=semis * 50)     # +/- semis/2 semitones around the pitch-EG offset
    p.peg(-semis / 2.0 + 0.0, [(-semis / 2.0, 0.005)])
    return p.done()


def rubber_bass(name, *, wow=7.0, wow_t=0.22, floor=-20, decay=0.5, sustain=None, fb=4, vel=4, tr=12, key=48,
                rel=0.08, cat="bass", ratio=1.0, sub=-6, att=1.0):
    """'Rubber'/filter-pluck bass: a very high initial FM index that sweeps closed - the DX answer to a resonant filter."""
    p = P(name, 1, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(-6, 0.2), (-60, decay)], rel) if sustain is None else shape(att, [(sustain, 0.3)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1)
    p.mod(2, ratio, 2.0, shape(att, [(-20, 0.3)], rel), kvs=vel)
    p.car(3, 1.0, sub, ce, det=2, kvs=vel - 1)
    p.mod(4, ratio, wow * 0.55, shape(att, [(floor, wow_t * 1.2)], rel), kvs=vel)
    p.mod(5, 1.0, wow * 0.8, shape(att, [(floor, wow_t)], rel), kvs=vel)
    p.mod(6, 1.0, wow, shape(att, [(floor - 4, wow_t * 0.9)], rel), kvs=vel)
    return p.done()


def growl_bass(name, *, growl=6.0, lfo_hz=6.0, lfo_depth=45, floor=-12, fb=6, vel=3, tr=12, key=48, rel=0.12,
               cat="bass", sustain=-1, att=3.0, sub=-5):
    """Moving, vocal 'growl' bass: high feedback with LFO-driven index (amp-mod on the modulators ~ wobble)."""
    p = P(name, 2, tr=tr, fb=fb, key=key, cat=cat)
    ce = shape(att, [(sustain, 0.3)], rel)
    p.car(1, 1.0, 0, ce, kvs=vel - 1)
    p.mod(2, 1.0, growl, shape(att, [(floor, 0.5)], rel), kvs=vel, ams=3)
    p.mod(6, 1.0, growl * 0.7, shape(att, [(floor, 0.5)], rel), kvs=vel, ams=2)
    p.mod(5, 1.0, 2.0, shape(att, [(floor, 0.6)], rel), kvs=vel, ams=1)
    p.mod(4, 2.0, 1.0, shape(att, [(floor, 0.6)], rel), kvs=vel)
    p.car(3, 1.0, sub, ce, det=2, kvs=vel - 1)
    p.lfo(lfo_hz, "tri", amd=lfo_depth)
    return p.done()
