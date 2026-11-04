"""BASS: FM classics, plucked/slap/fretless/upright, synth and sequenced bass."""
from recipes.bass import *

B06 = ("BASS", "FM Bass Classics", [
    fm_bass("LA BASS 1"), fm_bass("DX BASS", thump=6.5, floor=-14, decay=(-6, 0.5, 1.8)), fm_bass("PCH BASS", sustain=-3, thump=4.5),
    fm_bass("THUMP", thump=7.5, thump_t=0.12, click=3.5), fm_bass("ROUND", thump=3.5, sub=-10, click=1.5), fm_bass("FAT FM", sub=-6, upper_lvl=-4, thump=6.5, det=3),
    fm_bass("TIGHT", decay=(-10, 0.15, 0.5), thump_t=0.1), fm_bass("MELLOW FM", thump=3.0, floor=-22, click=0.8, vel=3),
    fm_bass("CLICK BASS", click=5.0, click_ratio=7.0, click_t=0.07), fm_bass("PUNCH", thump=6.5, thump_t=0.08, decay=(-6, 0.2, 0.9)),
    fm_bass("WARM FM", thump=4.0, floor=-20, upper_lvl=-14), fm_bass("BRIGHT FM", thump=7.0, floor=-10, upper_lvl=-6, click=3.5),
    fm_bass("SQUARE FM", mod_ratio=2.0, thump=2.5), fm_bass("METAL BASS", mod_ratio=3.0, thump=2.0, click_ratio=7.0, click=3.0),
    fm_bass("GLASS BASS", mod_ratio=3.5, thump=1.5, click=3.5, sub=-10), fm_bass("OCT FM", upper=1.0, upper_lvl=-4, det=2), fm_bass("BARITONE", tr=24, thump=4.5),
    rubber_bass("RUBBER"), rubber_bass("ELASTIC", wow=9.0, wow_t=0.3), rubber_bass("WAH BASS", wow=7.5, wow_t=0.35, sustain=-3),
    rubber_bass("SQUELCH", wow=10.0, wow_t=0.16, decay=0.35), rubber_bass("BOING", wow=8.0, wow_t=0.5, ratio=2.0), rubber_bass("STRETCH", wow=6.0, wow_t=0.4, sustain=-2, fb=5),
    sub_bass("SUB 808", drop=14, decay=(-3, 1.2, 2.0)), sub_bass("SUB LONG", decay=(-2, 2.0, 3.0)), sub_bass("SUB THUD", drop=18, drop_t=0.04, decay=(-8, 0.5, 1.0)),
    sub_bass("SUB SINE", sustain=-1, harm=-30), sub_bass("SUB BOOM", drop=24, drop_t=0.08, decay=(-3, 1.5, 3.0)), sub_bass("SUB PUNCH", drop=12, punch=3.0, harm=-10, decay=(-6, 0.6, 1.2)),
    fm_bass("STUDIO BS", thump=5.0, floor=-18, decay=(-7, 0.45, 1.6), click=2.0, sub=-15), fm_bass("VENICE BS", thump=4.0, sustain=-5, click=1.8, det=1),
    fm_bass("MARINA BS", thump=4.8, thump_t=0.3, floor=-12, sustain=-4, click=3.0, click_ratio=3.0, upper_lvl=-7),
])

B07 = ("BASS", "Pluck, Slap & Acoustic Bass", [
    slap("SLAP"), slap("SLAP POP", pop=8.0, pop_t=0.06), slap("SLAP HARD", thumb=7.5, vel=7, bright=1.2), slap("THUMB", pop=3.0, thumb=7.5, thumb_t=0.22),
    slap("FUNK SLAP", decay=0.5, mute=1), slap("DISCO SLAP", decay=1.2, pop=5.0), slap("SOFT SLAP", bright=0.7, vel=4), slap("SLAP+SUB", sub=-6, thumb=6.5),
    slap("TIGHT SLAP", decay=0.35, thumb_t=0.08), slap("RUBBER SLP", string_ratio=3.0, pop=4.0),
    fretless("FRETLESS"), fretless("MWAH BASS", mwah=3.0, mwah_peak_ms=120), fretless("SINGING", vib=10, mwah=2.5, sustain=-2), fretless("LYRICAL BS", vib=9, vib_delay=0.6, mwah=2.0),
    fretless("MELLOW FRT", bright=0.7), fretless("BRIGHT FRT", bright=1.4, harm=-6), fretless("WARM FRTLS", att=25, mwah=1.4), fretless("GROWL FRT", mwah=3.6, sustain=-1, vel=6, fb=5),
    upright("UPRIGHT"), upright("WALKING", thud=3.0, decay=1.2, wood=5.0), upright("JAZZ BASS", decay=2.0, thud=3.0), upright("WOODY", wood=6.5, wood_t=0.05, decay=1.0, thud=3.5),
    upright("ACOUSTIC", thud=5.0, decay=2.2), upright("DEEP STRNG", thud=4.5, decay=2.8, body=-9),
    fm_bass("PICK BASS", click=4.0, click_t=0.03, decay=(-8, 0.3, 1.0)), fm_bass("FINGER BS", thump=3.5, click=1.5, decay=(-6, 0.5, 1.6)),
    fm_bass("MUTED BASS", decay=(-12, 0.1, 0.4), thump_t=0.08), fm_bass("BOUNCE", decay=(-9, 0.2, 0.7), thump=5.0, floor=-20),
    fm_bass("PLUCK FM", decay=(-10, 0.15, 0.9), upper_lvl=-6), fm_bass("POP BASS", click=4.5, click_ratio=6.0, decay=(-8, 0.25, 0.8)),
    fm_bass("DRY BASS", decay=(-8, 0.25, 0.6)), fm_bass("WET BASS", decay=(-4, 0.6, 2.4), sub=-10),
])

B08 = ("BASS", "Synth & Sequenced Bass", [
    synth_bass("ANALOG BS"), synth_bass("SAW BASS", saw=5, sweep=6.0, floor=-12), synth_bass("PWM BASS", pwm=1.5, det=5), synth_bass("RES BASS", sweep=7.0, sweep_t=0.35, floor=-18),
    synth_bass("PLUCK SYN", sustain=-10, sweep_t=0.15), synth_bass("SOFT SYN", sweep=3.0, floor=-8), synth_bass("BIG SYN", det=6, sub=-2), synth_bass("TIGHT SYN", sweep_t=0.12),
    synth_bass("FAT SAW", saw=6, det=7), synth_bass("BUZZ BASS", fb=7, sweep=7.0), synth_bass("VIB SYN", vib=45, sweep=4.0, floor=-6, sub=-9, saw=3), synth_bass("DEEP SYN", sub=-3, sweep=4.0, floor=-14, tr=0, key=60),
    pulse_bass("PULSE 16TH", rate_hz=8.0), pulse_bass("PULSE 8TH", rate_hz=4.0), pulse_bass("PULSE FAST", rate_hz=12.0), pulse_bass("GATE BASS", depth=90, rate_hz=6.0),
    pulse_bass("STACC BASS", rate_hz=10.0, shape_w="saw-dn"), pulse_bass("CHOP BASS", rate_hz=5.5, saw=5.0, shape_w="tri"),
    oct_bass("OCTAVE BS", rate_hz=7.5), oct_bass("DISCO OCT", rate_hz=8.5, saw=4.5), oct_bass("FIFTH BASS", rate_hz=6.0, semis=7), oct_bass("OCT PULSE", rate_hz=5.0),
    growl_bass("GROWL"), growl_bass("WOBBLE", lfo_hz=3.0, lfo_depth=60), growl_bass("MUTANT", growl=8.0, lfo_hz=4.5), growl_bass("SNARL", lfo_hz=9.0, lfo_depth=70, growl=7.0),
    rubber_bass("SQUELCH 80", wow=9.5, wow_t=0.2, decay=0.4, sub=-8), rubber_bass("ZAP BASS", wow=10.0, wow_t=0.12, decay=0.3), rubber_bass("SEQ BASS", wow=8.0, wow_t=0.14, decay=0.25, ratio=1.0),
    sub_bass("SYN SUB", decay=(-4, 1.0, 2.0), drop=7, punch=2.0), sub_bass("DEEP SEQ", sustain=-2, harm=-14, punch=2.5), sub_bass("SUB PULSE", drop=5, decay=(-5, 0.35, 0.8)),
])

BANKS = [B06, B07, B08]
