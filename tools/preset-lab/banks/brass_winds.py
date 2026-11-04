"""BRASS & WINDS: sections, stabs, solo brass, sax, flutes and reeds."""
from recipes.brass import section, stab, solo
from recipes.winds import additive_wind, reed

B09 = ("BRASS & WINDS", "Brass Sections & Stabs", [
    section("LA BRASS"), section("SUNSET BRS", peak=5.5, sus=3.6, swell=45), section("WIDE BRASS", algo=24, spread=6),
    section("SOFT BRASS", peak=2.8, sus=1.8, swell=90, spread=2), section("BRIGHT BRS", peak=6.5, sus=4.5, swell=35, mvel=6),
    section("MELLOW BRS", peak=3.6, sus=2.6, swell=140, spread=5, vib=6, ks_depth=40), section("SYN BRASS", peak=5.0, sus=4.0, swell=120, spread=5),
    section("ROCK BRASS", peak=6.0, sus=3.6, swell=28, vel=5), section("HORNS LOW", tr=12, peak=3.8, sus=2.8, swell=100, spread=4),
    section("TUTTI", algo=25, peak=5.0, sus=3.5, spread=5), section("SWELL BRS", swell=400, att=200, peak=5.0, sus=3.5, rel=0.5),
    section("FALL BRASS", swell=20, settle=1.2, peak=6.0, sus=2.0), section("JUMP BRASS", peak=5.5, sus=3.2, swell=30, spread=4),
    section("ARENA BRS", peak=6.0, sus=4.2, swell=60, spread=7, rel=0.5), section("VIBE BRASS", vib=8, vib_delay=0.3, peak=4.6, sus=3.2),
    section("FANFARE", algo=25, peak=6.5, sus=4.0, swell=20, spread=3),
    stab("BRASS STAB"), stab("STAB TWO", dec=((-8, 0.4), (-60, 0.7))), stab("SHORT STAB", dec=((-14, 0.12), (-60, 0.2))), stab("FAT STAB", spread=6, peak=6.0),
    stab("BRT STAB", peak=7.0, sus=1.5, mvel=7), stab("DTLA STAB", algo=24, spread=5), stab("SOFT STAB", peak=3.5, sus=0.8),
    stab("HIT BRASS", dec=((-6, 0.5), (-60, 1.2)), rel=0.3), stab("FALL STAB", settle=0.5, dec=((-10, 0.5), (-60, 0.6))), stab("SYN STAB", algo=25, peak=5.0, spread=4),
    stab("OCT STAB", cr=[1.0, 1.0, 2.0, 2.0]), stab("FIFTH STAB", cr=[1.0, 1.0, 1.5, 1.5]), stab("POWER STAB", cr=[1.0, 1.5, 2.0, 3.0], peak=5.0),
    stab("ECHO STAB", dec=((-10, 0.18), (-60, 1.6)), rel=0.4), stab("CLIP STAB", dec=((-20, 0.08), (-60, 0.15))), stab("ROCKT STAB", peak=7.5, settle=0.3, spread=4, dec=((-9, 0.3), (-60, 0.55))),
])

B10 = ("BRASS & WINDS", "Solo Brass & Sax", [
    solo("TRUMPET"), solo("TRUMPET 2", peak=5.5, sus=3.2, swell=20, blat=0.3), solo("MUTE TRPT", peak=3.5, sus=3.0, mr=[1.0, 3.0, 1.0], swell=15, vib=10),
    solo("FLUGEL", peak=2.6, sus=2.0, swell=60, vib=12), solo("CORNET", peak=3.8, sus=2.6, swell=45, vib=11), solo("TROMBONE", tr=12, peak=4.0, sus=2.8, swell=70),
    solo("FRENCH HRN", tr=12, peak=2.8, sus=2.0, swell=110, vib=10), solo("TUBA", tr=0, peak=3.0, sus=2.2, swell=90, vib=6), solo("PICCOLO TP", tr=36, peak=4.2, sus=3.0, swell=25, vib=9),
    solo("BRASS LEAD", peak=5.0, sus=3.6, swell=28, vib=16, vib_delay=0.25), solo("HARMON", peak=4.0, sus=3.4, mr=[1.0, 2.0, 5.0], swell=12, vib=8),
    solo("OPEN HORN", peak=3.4, sus=2.6, swell=80, vib=14, vib_delay=0.6), solo("BLAT HORN", peak=6.0, sus=3.0, swell=18, blat=0.5), solo("SOLO BRASS", peak=4.5, sus=3.0, spread=2), solo("MIAMI TRPT", peak=5.2, sus=3.4, swell=22, vib=12, vib_delay=0.3),
    reed("ALTO SAX", ratio=1.0, b=3.6, b_floor=2.4, fb=5, third=True), reed("TENOR SAX", tr=12, ratio=1.0, b=3.8, b_floor=2.6, fb=5, third=True),
    reed("SOPRANO", tr=36, ratio=1.0, b=3.2, b_floor=2.4, fb=4, third=True), reed("BARI SAX", tr=0, ratio=1.0, b=4.0, b_floor=2.8, fb=5, third=True),
    reed("SOFT SAX", ratio=1.0, b=2.2, b_floor=1.6, fb=3, third=True, vib=12), reed("BREATH SAX", ratio=1.0, b=2.8, b_floor=1.8, breath=-14, third=True, att=60),
    reed("GROWL SAX", ratio=1.0, b=4.6, b_floor=3.2, fb=7, third=True, vib=6), reed("SMOOTH SAX", ratio=1.0, b=2.8, b_floor=2.0, vib=15, att=50, third=True),
    reed("R&B SAX", tr=12, ratio=1.0, b=3.0, b_floor=2.0, vib=11, vib_delay=0.5, att=40, third=True), reed("LA SAX", ratio=1.0, b=3.4, b_floor=2.2, vib=12, third=True, key=60, detune=3),
    reed("SAX HONK", tr=12, ratio=1.0, b=4.2, b_floor=3.0, fb=6, third=True, att=12), reed("SAX SECT", ratio=1.0, b=3.2, b_floor=2.2, detune=5, third=True, vib=6),
    section("SYN BRS 2", algo=22, peak=4.8, sus=3.8, swell=90, spread=4, vib=6), section("LUSH BRASS", algo=24, peak=3.8, sus=2.8, swell=140, spread=6, rel=0.45),
    section("BRASS PAD", algo=25, peak=3.0, sus=2.4, swell=260, att=120, rel=0.8, spread=6), section("HORN PAD", tr=12, peak=2.6, sus=2.0, swell=300, att=160, rel=0.9, spread=5),
    section("WARM BRASS", peak=3.0, sus=2.4, swell=75, ks_depth=45, rel=0.3),
])

B11 = ("BRASS & WINDS", "Flutes, Reeds & Ethnic Winds", [
    additive_wind("FLUTE", [(1, 0), (2, -14), (3, -26)], breath=-24, chiff=4), additive_wind("BREATH FLT", [(1, 0), (2, -18), (3, -30)], breath=-14, chiff=2, att=80),
    additive_wind("CONCERT FL", [(1, 0), (2, -10), (3, -20), (4, -30)], breath=-26, chiff=3, vib=16), additive_wind("ALTO FLUTE", tr=12, partials=[(1, 0), (2, -16), (3, -28)], breath=-20, chiff=3),
    additive_wind("PICCOLO", tr=48, partials=[(1, 0), (2, -12), (3, -24)], breath=-24, chiff=3, vib=8), additive_wind("SOFT FLUTE", partials=[(1, 0), (2, -22)], breath=-30, att=110, vib=10),
    additive_wind("PAN FLUTE", partials=[(1, 0), (2, -20)], breath=-14, att=90), additive_wind("ZAMPONA", tr=12, partials=[(1, 0), (2, -24), (3, -34)], breath=-10, att=100, vib=6),
    additive_wind("NAY FLUTE", partials=[(1, 0), (2, -16), (3, -22)], breath=-12, att=70, vib=22), additive_wind("RECORDER", partials=[(1, 0), (2, -18), (3, -22)], breath=-28, att=25, vib=6, vib_delay=0.8),
    additive_wind("WHISTLE", partials=[(1, 0)], breath=-34, scoop=3), additive_wind("BIRD WHIST", tr=48, partials=[(1, 0), (2, -30)], breath=-34, scoop=5, vib=22, vib_hz=7.0),
    additive_wind("OCARINA", partials=[(1, 0), (2, -26)], breath=-32, att=50, vib=12), additive_wind("BOTTLE", partials=[(1, 0), (2, -22)], breath=-16, att=70, vib=0),
    additive_wind("SHAKUHACHI", partials=[(1, 0), (2, -12), (3, -18)], breath=-10, chiff=2, att=90, vib=18, scoop=2), additive_wind("DIZI", tr=24, partials=[(1, 0), (2, -8), (3, -16)], breath=-18, chiff=3, vib=10),
    reed("CLARINET"), reed("BASS CLRNT", tr=12, ratio=2.0, b=1.8, b_floor=1.3), reed("SOFT CLRNT", ratio=2.0, b=1.0, b_floor=0.8, vib=4, att=60, breath=-40), reed("JAZZ CLRNT", ratio=2.0, b=2.0, b_floor=1.4, vib=14, att=25),
    reed("OBOE", ratio=1.0, b=3.0, b_floor=2.0), reed("ENGL HORN", tr=12, ratio=1.0, b=2.6, b_floor=1.8, vib=11), reed("SOLO OBOE", ratio=1.0, b=3.4, b_floor=2.2, vib=13, vib_delay=0.5),
    reed("BASSOON", tr=0, ratio=1.0, b=3.0, b_floor=2.2, vib=6), reed("CONTRABSN", tr=0, ratio=1.0, b=2.2, b_floor=1.6, att=60, vib=3),
    reed("HARMONICA", ratio=1.0, b=2.4, b_floor=1.8, detune=5, vib=5, att=18, breath=-20), reed("MELODICA", ratio=2.0, b=1.4, b_floor=1.1, detune=4, vib=4, att=14, breath=-24),
    reed("BAGPIPE", ratio=1.0, b=3.8, b_floor=3.2, detune=3, vib=0, att=10, breath=None), reed("SHENAI", ratio=1.0, b=3.2, b_floor=2.6, detune=2, vib=16, att=20),
    reed("DUDUK", tr=12, ratio=1.0, b=2.0, b_floor=1.5, vib=12, att=45, breath=-22), reed("MUSETTE W", ratio=1.0, b=3.0, b_floor=2.4, detune=6, vib=8), additive_wind("WIND GHOST", partials=[(1, 0), (2, -20), (3, -30)], breath=-8, att=160, vib=20, vib_delay=0.5, rel=0.4),
])
BANKS = [B09, B10, B11]
