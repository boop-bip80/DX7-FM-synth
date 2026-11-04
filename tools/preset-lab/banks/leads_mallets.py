"""LEADS and MALLETS & BELLS."""
from recipes.leads import lead, square_lead, pluck_lead, bell_lead, sync_lead, chip_lead
from recipes.winds import additive_wind, reed
from recipes.voices import choir, choir_breath, vox_fm
from recipes.strings import solo_string
from recipes.brass import solo
from recipes.plucks import (marimba, xylophone, vibes, glock, celesta, music_box, tubular, kalimba, steel_drum, gong)

B17 = ("LEADS", "Synth Leads", [
    lead("SUNSET LD"), lead("BLVD LEAD", algo=2, fb=6), lead("MONO LEAD", spread=0), lead("FAT LEAD", spread=3, peak=5.0), lead("SAW LEAD", peak=5.5, sus=4.5),
    lead("SOFT LEAD", peak=2.8, sus=2.2, vib=8), lead("BRIGHT LD", peak=6.0, sus=5.0, mvel=6), lead("SYN LEAD", algo=24, spread=2), lead("ANALOG LD", algo=25, spread=3, vib=10),
    lead("WIDE LEAD", spread=6, vib=6), lead("DIGI LEAD", algo=5, fb=3), lead("SCREAMER", peak=7.0, sus=6.0, fb=7), lead("FILTER LD", peak=6.0, sus=2.0, settle=0.5),
    lead("OCT LEAD", cr=[1.0, 1.0, 2.0, 2.0]), lead("FIFTH LEAD", cr=[1.0, 1.0, 1.5, 1.5]), lead("BUZZ LEAD", fb=7, peak=4.0, algo=5),
    square_lead("SQUARE LD"), square_lead("PULSE LEAD", ratio=2.0, b=2.6), square_lead("SOFT SQR", b=1.0, b_floor=0.7), square_lead("HOLLOW LD", b=1.2, chorus=5), square_lead("CHIME SQR", ratio=4.0, b=1.4),
    square_lead("SQR+SUB", sub=-6), square_lead("SQR+FIFTH", sub=6), square_lead("SQR BRIGHT", b=3.0, b_floor=2.0, swell=4),
    sync_lead("SYNC LEAD"), sync_lead("SYNC SWEEP", sweep=-24, sweep_t=0.5, mod_hz=500), sync_lead("SYNC BITE", b=8.0, mod_hz=900, sweep=-7, sweep_t=0.15), sync_lead("SYNC DEEP", mod_hz=300, sweep=-12, b=7.0, car2=1.0),
    chip_lead("CHIP LEAD"), chip_lead("ARCADE", ratio=3.0, b=1.4), chip_lead("8BIT SQR", ratio=2.0, b=1.0, fb=2), chip_lead("DIGI BEEP", ratio=1.0, b=2.4, fb=5),
])

B18 = ("LEADS", "Expressive & Vocal Leads", [
    additive_wind("FLUTE LEAD", [(1, 0), (2, -12), (3, -24)], breath=-26, chiff=3, vib=18, key=72), additive_wind("SOLO FLUTE", [(1, 0), (2, -16)], breath=-22, chiff=3, vib=20, att=70),
    additive_wind("WHISTLE LD", [(1, 0), (3, -26)], breath=-30, scoop=5, vib=22, vib_delay=0.3, att=30), additive_wind("OCARINA LD", [(1, 0), (2, -24)], breath=-30, vib=14, att=45),
    additive_wind("SHAKU LEAD", [(1, 0), (2, -10), (3, -16)], breath=-10, chiff=3, vib=22, scoop=3), additive_wind("BREATH LD", [(1, 0), (2, -14), (3, -26)], breath=-12, att=90, vib=16),
    additive_wind("PAN LEAD", [(1, 0), (2, -18)], breath=-14, att=60, vib=8), additive_wind("BIRD LEAD", tr=48, partials=[(1, 0), (3, -22)], breath=-34, scoop=8, vib=28, vib_hz=8.0, att=20),
    reed("SAX LEAD", ratio=1.0, b=4.2, b_floor=3.0, fb=6, third=True, vib=14, att=25, detune=3, breath=-22), reed("SMOOTH LD", ratio=1.0, b=2.6, b_floor=1.8, third=True, vib=15, att=50), reed("SOPRANO LD", tr=36, ratio=1.0, b=3.4, b_floor=2.4, third=True, vib=12),
    reed("OBOE LEAD", ratio=1.0, b=3.2, b_floor=2.2, vib=13), reed("CLRNT LEAD", ratio=2.0, b=2.6, b_floor=1.9, third=True, vib=13, att=20), reed("HARMNCA LD", ratio=1.0, b=2.6, b_floor=2.0, detune=5, vib=7, breath=-20),
    reed("DUDUK LEAD", tr=24, ratio=1.0, b=2.2, b_floor=1.6, vib=14, att=50, breath=-20), reed("VOX REED", ratio=3.0, b=1.4, b_floor=1.0, third=True, vib=11),
    choir("OOH LEAD", "oo", att=90, rel=0.35, vib=16, vib_delay=0.4, key=72), choir("AAH LEAD", "ah", att=80, rel=0.35, vib=15, vib_delay=0.4, key=72), choir("OH LEAD", "oh", att=90, rel=0.35, vib=14, key=72),
    choir_breath("AIR LEAD", "ah", att=100, rel=0.4, breath=-20, vib=14, key=72), vox_fm("VOX LEAD", att=60, rel=0.3, vib=14, key=72), vox_fm("FM VOICE", b=2.2, ratio=2.0, att=50, rel=0.3, vib=12, key=72),
    vox_fm("SINGER", b=1.8, ratio=5.0, r2=3.0, att=70, rel=0.3, vib=16, key=72), choir_breath("WHISPER LD", "oo", att=120, rel=0.4, breath=-10, vib=10, key=72),
    solo_string("EVIOLIN LD", att=45, peak=4.0, sus=3.4, vib=22, vib_delay=0.2, key=72), solo_string("SING STR", att=120, peak=2.8, vib=18, key=72), solo_string("CELLO LEAD", tr=12, key=72, att=140, peak=2.8, sus=2.4, vib=19, vib_delay=0.5, fb=5),
    solo("TRUMPET LD", peak=5.0, sus=3.4, swell=22, vib=14, key=72), solo("FLUGEL LD", peak=2.6, sus=2.0, swell=60, vib=12, key=72), solo("HORN LEAD", peak=3.4, sus=2.6, swell=80, vib=13, key=72),
    vox_fm("HLLYWD VOX", b=1.2, ratio=3.0, att=100, rel=0.4, vib=13, key=72),
    solo("SYN BRS LD", algo=22, peak=5.0, sus=3.8, swell=30, vib=12, spread=2, key=72),
])

B19 = ("LEADS", "Hook, Pluck & Bell Leads", [
    pluck_lead("HOOK LEAD"), pluck_lead("PLUCK LEAD", peak=6.0, sus=1.0, t60=0.8), pluck_lead("SHORT HOOK", t60=0.4, settle=0.1), pluck_lead("LONG PLUCK", t60=2.4, sus=1.6), pluck_lead("BRIGHT PLK", peak=7.0, sus=1.5, mvel=7),
    pluck_lead("SOFT PLUCK", peak=3.0, sus=0.8, t60=1.4), pluck_lead("FAT PLUCK", spread=5, peak=5.0), pluck_lead("SYN PLUCK", algo=24, spread=3), pluck_lead("ANALOG PLK", algo=25, spread=4, peak=5.5),
    pluck_lead("DIGI PLUCK", algo=5, fb=3, peak=4.0), pluck_lead("OCT PLUCK", cr=[1.0, 1.0, 2.0, 2.0]), pluck_lead("5TH PLUCK", cr=[1.0, 1.0, 1.5, 1.5]),
    pluck_lead("ZAPPY", peak=7.5, settle=0.08, t60=0.5, fb=7), pluck_lead("RUBBER LD", peak=6.5, sus=0.6, settle=0.25, t60=0.9, algo=2), pluck_lead("GLASS PLK", cr=[1.0, 2.0, 1.0, 3.0], peak=4.0, sus=1.0, mr=[3.5, 1.0]),
    bell_lead("BELL LEAD"), bell_lead("GLASS LEAD", ratio_a=5.0, ratio_b=9.0, ba=1.8, bb=1.2), bell_lead("CRYSTAL LD", ratio_a=7.0, ratio_b=11.0, ba=1.5, bb=1.0), bell_lead("DX LEAD", ratio_a=3.5, ratio_b=3.5, ba=2.0, bb=2.0),
    bell_lead("FM BELL LD", ratio_a=1.0, ratio_b=3.5, ba=2.8, bb=2.0, sustain=False, ring=3.0), bell_lead("TINKLE LD", tr=36, ratio_a=3.5, ratio_b=7.0, sustain=False, ring=1.8),
    bell_lead("CELESTE LD", tr=36, ratio_a=4.0, ratio_b=6.0, ba=1.2, bb=0.8, vib=10), bell_lead("STAR LEAD", ratio_a=2.76, ratio_b=5.4, ba=2.0, bb=1.4, chorus=5), bell_lead("MARBLE LD", ratio_a=3.0, ratio_b=5.04, ba=2.6, bb=1.8, vib=5),
    bell_lead("SHIMMER LD", ratio_a=3.5, ratio_b=7.0, chorus=7, vib=9), bell_lead("MBOX LEAD", tr=48, ratio_a=6.0, ratio_b=7.0, sustain=False, ring=1.4, vib=0),
    bell_lead("CHIME LEAD", ratio_a=4.0, ratio_b=10.0, ba=1.6, bb=1.0, sustain=False, ring=2.6, tr=36),
    sync_lead("SYNC HOOK", att=2, sweep=-14, sweep_t=0.08, vib=0), square_lead("ARP SQUARE", att=1, rel=0.08, vib=0, swell=3), chip_lead("PING LEAD", ratio=3.0, b=1.2, att=1, rel=0.4, fb=3),
    lead("SPIKE LEAD", dec=[(-6, 0.15), (-60, 0.8)], peak=6.0, sus=1.5, settle=0.12), lead("SNAP LEAD", dec=[(-10, 0.1), (-60, 0.4)], peak=6.5, sus=1.0, settle=0.08),
])

B20 = ("MALLETS & BELLS", "Marimba, Xylophone, Vibes & Glock", [
    marimba("MARIMBA"), marimba("SOFT MRMBA", hard=0.6, ring=1.2), marimba("HARD MRMBA", hard=1.5, ring=0.8, wood=5.0), marimba("LOW MRMBA", tr=12, ring=1.6, hard=1.0), marimba("HIGH MRMBA", tr=36, ring=0.7),
    marimba("ROSEWOOD", tune=3.9, ring=1.3, hard=0.9), marimba("BASS MRMBA", tr=0, ring=2.0, hard=1.0), marimba("MARIMBA 2", p3=False, hard=1.1),
    xylophone("XYLOPHONE"), xylophone("SOFT XYLO", hard=0.7, ring=0.55), xylophone("DRY XYLO", ring=0.28, hard=1.4), xylophone("BRT XYLO", tr=48, hard=1.5), xylophone("WOODBLK XY", tune=4.0, ring=0.2, hard=1.6),
    xylophone("TUBE XYLO", tune=2.76, ring=0.7, hard=1.0),
    vibes("VIBES"), vibes("SOFT VIBES", soft=True, hard=0.7), vibes("MOTOR OFF", motor=False), vibes("SLOW VIBES", trem_hz=3.2, trem_db=7.0), vibes("FAST VIBES", trem_hz=7.0, trem_db=6.0),
    vibes("LONG VIBES", ring=6.0), vibes("HARD VIBES", hard=1.4, ring=3.0), vibes("LOW VIBES", tr=12, ring=4.0),
    glock("GLOCKENSPL"), glock("SOFT GLOCK", hard=0.7, ring=2.0), glock("BRT GLOCK", hard=1.4, tr=48), glock("CROTALE", ring=4.5, tr=48, partials=(2.76, 5.4)), glock("BELL LYRA", ring=1.4, hard=1.0, mod=5.0), glock("TUBE CHIME", ring=3.5, tr=24, partials=(2.0, 4.0), rel=0.8),
    celesta("CELESTA"), celesta("SOFT CELST", soft=0.6, ring=2.8), celesta("BRIGHT CEL", soft=1.5, tr=48), celesta("DREAM CEL", ring=3.6, tr=36, soft=0.8),
])

B21 = ("MALLETS & BELLS", "Bells, Chimes, Music Boxes & Metals", [
    tubular("CATH BELL", ring=11.0, tr=12, mod=3.0, strike=3.5, rel=2.5),
    tubular("TUBULAR"), tubular("CHURCHBELL", ring=9.0, tr=12, strike=4.0), tubular("SOFT TUBES", hard=0.6, ring=5.0), tubular("BRT BELL", hard=1.4, tr=36), tubular("LOW BELL", tr=0, ring=10.0, rel=2.5),
    tubular("BIG BEN", tr=0, ring=12.0, mod=2.0, strike=3.0, rel=3.0), tubular("HAND BELL", tr=36, ring=4.5, hard=0.9), tubular("SLEIGHBELL", tr=48, ring=2.0, hard=1.0, mod=5.0),
    glock("CRYSTAL", tr=48, ring=3.2, partials=(2.76, 6.0)), glock("ICE BELLS", tr=48, ring=3.0, partials=(3.0, 7.0), mod=5.0), glock("WIND CHIME", tr=48, ring=5.0, partials=(2.0, 3.0), hard=0.7, rel=1.0),
    glock("DX CHIME", tr=36, ring=4.0, partials=(3.5, 7.0), mod=3.5), glock("FM BELL", tr=24, ring=3.5, partials=(1.4, 2.8), mod=1.4), glock("SANTA BELL", tr=36, ring=2.2, partials=(2.4, 4.8), mod=3.0),
    music_box("MUSIC BOX"), music_box("LULLABY", hard=0.7, ring=2.2), music_box("TOY PIANO", ring=0.8, hard=1.2, tr=36, ratio=3.0), music_box("CARILLON", tr=36, ring=2.0, ratio=5.0), music_box("DREAM BOX", ring=3.0, tr=48, hard=0.6, ratio=9.0),
    kalimba("KALIMBA"), kalimba("THUMB PNO", ring=1.4, hard=1.2), kalimba("MBIRA", ring=1.2, mod=4.0, tine=1.4), kalimba("SOFT KALIM", ring=0.9, hard=0.6),
    gong("GONG"), gong("TAM TAM", ring=12.0, strike=3.5, bloom=0.3, tr=0), gong("TMPLE GONG", ring=7.0, ratios=(1.0, 1.5, 2.0)), gong("METALSHEET", ring=3.0, strike=4.0, ratios=(1.0, 1.7, 2.9), tr=24),
    steel_drum("STEEL DRUM"), steel_drum("PAN HI", tr=36, ring=0.9), steel_drum("PAN LO", tr=12, ring=1.4), steel_drum("CALYPSO", trem=True, ring=1.2),
])
BANKS = [B17, B18, B19, B20, B21]
