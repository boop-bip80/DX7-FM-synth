"""STRINGS & PADS."""
from recipes.strings import ensemble, solo_string, pizzicato, tremolo_strings
from recipes.pads import pad, glass_pad, sweep_pad
from recipes.ens import ens
from recipes.fx import drone

B12 = ("STRINGS", "String Ensembles", [
    ensemble("LA STRINGS"), ensemble("SUNSET STR", att=380, peak=3.0, sus=2.6, swell=450, rel=1.3), ensemble("BIG STRNGS", algo=24, spread=7, peak=3.6),
    ensemble("SLOW STR", att=700, rel=1.6, swell=800), ensemble("FAST STR", att=60, rel=0.5, swell=120), ensemble("BRIGHT STR", peak=4.4, sus=3.8, mvel=6),
    ensemble("MELLOW STR", peak=2.4, sus=2.0, spread=4), ensemble("SYN STRNGS", algo=25, spread=6, peak=3.8), ensemble("STRNG MACH", algo=25, spread=7, peak=3.2, att=150, vib=7, lfo_hz=6.0),
    ensemble("CHAMBER", spread=2, att=190, peak=3.4, sus=3.0, vib=6), ensemble("VIOLINS", tr=36, att=230, vib=6, peak=3.6), ensemble("VIOLAS", algo=25, tr=24, att=300, peak=3.0, sus=2.6, spread=3),
    ensemble("CELLOS", tr=12, att=240, peak=3.4, sus=3.0, rel=0.8), ensemble("BASSES", tr=0, att=300, peak=3.0, sus=2.6), ensemble("OCT STRNGS", cr=[1.0, 1.0, 2.0, 2.0], att=260),
    ensemble("WARM STR", peak=2.8, sus=2.2, spread=5, att=330), ensemble("SILKY STR", peak=3.0, sus=2.6, spread=6, att=280, vib=7, lfo_hz=4.6), ensemble("SWEEP STR", att=450, swell=1400, peak=5.0, sus=2.6, settle=2.0),
    ensemble("ANALOG STR", algo=22, spread=7, peak=4.0, sus=3.4, vib=5, lfo_hz=5.4), ensemble("ENSEMBLE 2", algo=31, spread=5, peak=3.4),
    ensemble("PALISADES", att=500, peak=2.8, sus=2.4, rel=1.8, spread=6), ensemble("CANYON STR", att=340, peak=3.4, sus=3.0, spread=4, vib=8),
    ensemble("STACCATO", att=10, rel=0.12, swell=30, settle=0.2, peak=3.6, sus=2.4, sus_db=-12), ensemble("MARCATO", att=18, rel=0.25, swell=50, peak=4.0, sus=2.6, sus_db=-8),
    tremolo_strings("TREM STR"), tremolo_strings("TREM SLOW", rate=7.0, depth=7.0), tremolo_strings("TREM FAST", rate=14.0, depth=11.0), tremolo_strings("TREM SOFT", att=260, depth=6.0, peak=2.8),
    tremolo_strings("SHIM TREM", rate=18.0, depth=9.0, att=200, peak=3.4), tremolo_strings("SUSPENSE", rate=9.0, depth=12.0, att=420, rel=1.2, peak=2.8), tremolo_strings("TREM LOW", tr=12, rate=8.0, depth=8.0),
    tremolo_strings("TREM HIGH", tr=36, rate=12.0, depth=8.0, peak=3.0),
])

B13 = ("STRINGS", "Solo Strings, Pizzicato & Orchestral Hits", [
    solo_string("VIOLIN"), solo_string("SOFT VLN", att=110, peak=2.6, sus=2.2, vib=14), solo_string("FIDDLE", peak=4.4, sus=3.6, att=30, vib=10, lfo_hz=6.2),
    solo_string("VIOLA", tr=24, att=90, peak=3.4, vib=15), solo_string("CELLO", tr=12, key=60, att=100, peak=3.4, vib=14), solo_string("CELLO SOLO", tr=0, key=72, att=120, peak=3.0, vib=16),
    solo_string("DBL BASS", tr=0, key=60, att=130, peak=2.8, vib=8), solo_string("E VIOLIN", att=50, peak=4.0, sus=3.4, vib=22, vib_delay=0.2), solo_string("SOLO STR 2", algo=22, att=80, peak=3.2, vib=16),
    solo_string("LA FIDDLE", algo=22, att=26, peak=4.2, sus=3.4, vib=9, lfo_hz=6.4, spread=2),
    pizzicato("PIZZICATO"), pizzicato("PIZZ SOFT", pluck=2.2, decay=(-18, 0.1, 0.25)), pizzicato("PIZZ HARD", pluck=5.0, decay=(-14, 0.08, 0.3)), pizzicato("PIZZ LOW", tr=12, decay=(-14, 0.14, 0.5)),
    pizzicato("PIZZ HIGH", tr=36, decay=(-16, 0.08, 0.25)), pizzicato("SPICCATO", pluck=2.8, pluck_t=0.2, decay=(-10, 0.14, 0.18)), pizzicato("COL LEGNO", pluck=6.5, ratio2=4.0, decay=(-18, 0.05, 0.14)),
    pizzicato("PLUCK ENS", spread=5, decay=(-12, 0.16, 0.5)),
    ens("ORCH HIT", 24, tr=24, cat="strings", fb=7, cr=[1.0, 1.5, 2.0, 3.0, 4.0], spread=3, att=2, rel=0.35, swell=3, peak=6.5, sus=1.2, settle=0.3, dec=[(-10, 0.4), (-60, 1.2)], mvel=6),
    ens("ORCH HIT 2", 24, tr=24, cat="strings", fb=7, cr=[1.0, 2.0, 3.0, 4.0, 6.0], spread=5, att=2, rel=0.4, swell=3, peak=7.0, sus=1.5, settle=0.4, dec=[(-8, 0.5), (-60, 1.6)], mvel=6),
    ens("ORCH LOW", 24, tr=12, cat="strings", fb=7, cr=[0.5, 1.0, 1.5, 2.0, 3.0], spread=4, att=3, rel=0.45, swell=4, peak=6.0, sus=1.0, settle=0.35, dec=[(-8, 0.5), (-60, 1.8)], mvel=6),
    ens("ORCH SHORT", 24, tr=24, cat="strings", fb=7, cr=[1.0, 1.5, 2.0, 3.0, 4.0], spread=3, att=2, rel=0.15, swell=2, peak=6.0, sus=0.8, settle=0.2, dec=[(-14, 0.12), (-60, 0.25)], mvel=6),
    ens("ORCH BRASS", 24, tr=24, cat="strings", fb=6, cr=[1.0, 1.0, 2.0, 2.0, 3.0], spread=4, att=4, rel=0.4, swell=15, peak=5.5, sus=2.0, settle=0.3, dec=[(-8, 0.6), (-60, 1.2)], mvel=6),
    ens("ORCH DOOM", 24, tr=0, cat="strings", fb=7, cr=[0.5, 1.0, 1.5, 2.0, 4.0], spread=6, att=4, rel=0.9, swell=6, peak=6.5, sus=1.5, settle=0.5, dec=[(-6, 0.9), (-60, 2.6)], mvel=6),
    ens("ORCH CRASH", 24, tr=24, cat="strings", fb=7, cr=[1.0, 1.41, 2.0, 2.83, 4.0], spread=5, att=2, rel=0.6, swell=2, peak=7.0, sus=2.0, settle=0.6, dec=[(-8, 0.7), (-60, 2.0)], mvel=7),
    ens("ORCH STAB", 24, tr=24, cat="strings", fb=6, cr=[1.0, 2.0, 3.0, 4.0, 5.0], spread=2, att=2, rel=0.12, swell=2, peak=5.5, sus=1.0, settle=0.2, dec=[(-12, 0.14), (-60, 0.3)], mvel=6),
    ensemble("LA SECTION", att=300, peak=3.4, sus=3.0, spread=5, vib=6, rel=0.7), ensemble("STUDIO STR", att=240, algo=22, peak=3.2, sus=2.8, spread=3, mvel=5), ensemble("HLLYWD STR", algo=24, att=420, rel=1.2, peak=3.4, sus=2.8, spread=7, vib=7),
    ensemble("STR + HIT", algo=24, att=40, swell=60, peak=4.8, sus=3.0, settle=0.9, spread=6, rel=0.8),
    ensemble("STACC LOW", tr=12, att=8, rel=0.1, swell=24, settle=0.2, peak=3.4, sus=2.4, sus_db=-14),
    ensemble("STACC HIGH", tr=36, att=8, rel=0.1, swell=24, settle=0.2, peak=3.4, sus=2.4, sus_db=-14),
])

B14 = ("PADS", "Warm & Lush Pads", [
    pad("SUNSET PAD"), pad("MALIBU PAD", att=900, rel=2.2, swell=1400, peak=2.2, sus=1.8, spread=7), pad("WARM PAD", peak=2.0, sus=1.6, att=500), pad("LUSH PAD", algo=24, spread=7, peak=2.8, sus=2.4),
    pad("SLOW PAD", att=1800, swell=2400, rel=3.0), pad("FAST PAD", att=180, swell=300, rel=0.8), pad("BIG PAD", algo=25, spread=7, peak=3.0, att=700),
    pad("SOFT PAD", peak=1.6, sus=1.2, spread=4, att=800), pad("ANALOG PAD", algo=22, spread=7, peak=3.4, sus=2.8, vib=4, lfo_hz=0.5), pad("VINTG PAD", algo=22, spread=6, peak=2.8, sus=2.2, vib=5, lfo_hz=5.2, vib_delay=0.4, att=380, mr=[1.0, 2.0]),
    pad("POLY PAD", algo=22, spread=5, peak=3.0, sus=2.0, swell=300, settle=1.6), pad("CHORUS PAD", spread=7, algo=22, mr=[1.0, 2.0], peak=2.4, sus=1.8, vib=5, lfo_hz=1.6), pad("BRS PAD", algo=22, peak=3.8, sus=3.0, att=420),
    pad("VELVET", peak=1.4, sus=1.2, att=1000, spread=5, rel=2.5), pad("CANYON PAD", algo=7, fb=5, peak=2.4, sus=2.0, att=650, trem_db=2.5, lfo_hz=0.3, spread=5), pad("PACIFIC", att=1300, rel=2.8, peak=2.0, sus=1.7, spread=6, vib=3, lfo_hz=0.25),
    pad("SUNRISE", att=2200, swell=3000, rel=2.5, peak=3.0, sus=2.0), pad("SUNSET", att=900, rel=3.6, swell=1200, peak=2.6, sus=1.4, settle=3.0), pad("EVENING", peak=1.8, sus=1.3, att=1400, rel=3.2, spread=7),
    pad("STRING PAD", algo=25, peak=3.0, sus=2.6, spread=6, att=600), pad("CHOIR PAD", algo=22, peak=1.6, sus=1.4, mr=[3.0, 1.0], att=800, vib=4, lfo_hz=5.0, vib_delay=1.0),
    pad("OCT PAD", cr=[1.0, 1.0, 2.0, 2.0], att=700, peak=2.2), pad("FIFTH PAD", cr=[1.0, 1.0, 1.5, 1.5], att=700, peak=2.2), pad("MINOR PAD", cr=[1.0, 1.0, 1.19, 1.5], att=800, peak=2.0),
    pad("OPEN PAD", cr=[0.5, 1.0, 1.5, 2.0], att=800, peak=2.0), pad("SUB PAD", tr=12, att=900, peak=1.8, sus=1.4), pad("HIGH PAD", tr=36, att=500, peak=2.0, sus=1.6),
    pad("PRESSURE", peak=4.2, sus=2.4, swell=2500, settle=3.0, att=1200), pad("AIR PAD", peak=1.2, sus=1.0, att=1100, spread=7, rel=2.8), pad("ROOM PAD", att=240, rel=1.2, peak=2.6, sus=2.0, spread=5),
    pad("SILK PAD", peak=1.5, sus=1.3, att=1600, rel=3.4, spread=6, vib=2, lfo_hz=0.2), pad("AMBER PAD", algo=24, peak=2.0, sus=1.7, att=1000, rel=2.6, spread=5, tr=12, key=72),
])

B15 = ("PADS", "Glass & Digital Pads", [
    glass_pad("GLASS PAD"), glass_pad("CRYSTL PAD", ratio_a=5.0, ratio_b=7.0, ba=1.4, bb=1.0), glass_pad("ICE PAD", ratio_a=7.0, ratio_b=11.0, ba=1.2, bb=0.9, ring=8.0),
    glass_pad("BELL PAD", ratio_a=3.5, ratio_b=3.5, ba=2.2, bb=1.8, att=200), glass_pad("SHIMMER", att=700, rel=3.0, trem_db=2.7, lfo_hz=0.9, shim=-6), glass_pad("FROST", ratio_a=4.0, ratio_b=9.0, ba=1.6, bb=1.2, att=900),
    glass_pad("NEON PAD", ratio_a=2.0, ratio_b=3.5, ba=2.0, bb=1.6, att=300, fb=4), glass_pad("PRISM PAD", ratio_a=2.76, ratio_b=5.4, att=550, trem_db=2.0), glass_pad("AURORA PAD", att=1100, rel=3.5, ratio_a=3.5, ratio_b=5.04, ba=1.0, bb=0.8),
    glass_pad("HALO", ratio_a=3.0, ratio_b=6.0, att=800, rel=3.0, shim=-4), glass_pad("DREAMGLASS", att=1500, rel=4.0, ba=0.9, bb=0.7, chorus=6), glass_pad("STARDUST", ratio_a=5.0, ratio_b=8.0, ba=1.8, bb=1.4, chorus=7, att=400),
    glass_pad("PIANO PAD", ratio_a=1.0, ratio_b=2.0, ba=1.2, bb=0.9, att=100, ring=3.0), glass_pad("DX PAD", ratio_a=3.5, ratio_b=7.0, ba=1.8, att=260, chorus=5), glass_pad("DIGI PAD", ratio_a=7.0, ratio_b=14.0, ba=1.0, bb=0.8, att=350),
    glass_pad("MARMB PAD", ratio_a=4.0, ratio_b=10.0, ba=1.2, bb=1.0, att=180, ring=2.0), glass_pad("CELESTE", tr=36, att=500, rel=3.0, ba=1.0, bb=0.7), glass_pad("TWINKLE", tr=48, att=300, ba=1.4, bb=1.2, rel=2.0),
    pad("SPRKL PAD", algo=5, fb=3, cr=[1.0, 2.0, 1.0], mr=[3.5, 1.0, 7.0], att=500, peak=1.6, sus=1.2, spread=5, vib=3, lfo_hz=0.4), pad("MIRROR PAD", algo=5, fb=2, cr=[1.0, 1.0, 2.0], mr=[5.04, 3.5, 1.0], att=700, peak=1.4, sus=1.0, spread=6),
    pad("FM PAD 1", algo=5, fb=4, att=450, peak=2.0, sus=1.5, spread=4), pad("FM PAD 2", algo=7, fb=5, att=600, peak=2.4, sus=1.8, spread=5), pad("FM PAD 3", algo=14, fb=5, att=500, peak=2.2, sus=1.7, spread=5),
    pad("VOX PAD", algo=5, fb=3, cr=[1.0, 1.0, 1.0], mr=[3.0, 5.0, 1.0], att=550, peak=1.4, sus=1.1, spread=4, vib=5, lfo_hz=5.0, vib_delay=1.0), pad("BRIGHT PAD", algo=22, peak=4.2, sus=3.4, att=350, spread=5),
    pad("PULSE PAD", algo=22, att=300, peak=2.4, sus=2.0, spread=6, trem_db=4.0, lfo_hz=0.8, lfo_wave="tri"), pad("MOTION PAD", algo=24, att=800, peak=2.6, sus=2.0, spread=7, trem_db=2.7, lfo_hz=0.45, vib=6),
    pad("HOLLYWD PD", algo=23, att=1000, rel=2.8, peak=2.6, sus=2.0, spread=6, vib=3, lfo_hz=0.35, mr=[1.0, 2.0], cr=[1.0, 1.0, 2.0, 2.0]), pad("BOULEVARD", algo=25, att=700, rel=2.0, peak=2.8, sus=2.2, spread=6),
    pad("SPACE PAD", algo=5, fb=5, cr=[1.0, 1.5, 2.0], mr=[1.0, 7.0, 3.5], att=900, peak=1.8, sus=1.4, spread=6, vib=4, lfo_hz=0.3), pad("DIGI CHOIR", algo=22, mr=[2.0, 3.0], att=700, peak=1.6, sus=1.4, vib=4, lfo_hz=5.2, vib_delay=1.0), pad("DREAM PAD", att=1800, rel=4.0, peak=1.6, sus=1.3, spread=7, algo=25),
])

B16 = ("PADS", "Atmospheres & Sweeps", [
    sweep_pad("SWEEP PAD"), sweep_pad("SLOW SWEEP", rise=4500, fall=6000, att=1500), sweep_pad("FAST SWEEP", rise=1200, fall=1800, att=300), sweep_pad("DOWN SWEEP", rise=300, fall=4500, peak=5.5, floor=0.6, att=400),
    sweep_pad("BIG SWEEP", algo=24, rise=2800, fall=4500, peak=5.0, spread=7), sweep_pad("PWM SWEEP", algo=25, rise=3000, fall=3500, peak=4.5, spread=7), sweep_pad("RESO SWEEP", peak=6.0, floor=1.0, rise=2200, fall=3000),
    sweep_pad("OCEAN SWP", rise=3500, fall=4500, peak=3.4, floor=0.9, att=1200, lfo_hz=0.2, vib=4), sweep_pad("DARK SWEEP", tr=12, rise=3200, fall=5000, peak=4.0, floor=0.7),
    sweep_pad("BRT SWEEP", tr=36, rise=2000, fall=3000, peak=5.0, floor=1.0), sweep_pad("HORN SWEEP", algo=24, rise=1800, fall=3500, peak=5.2, floor=1.5, spread=3), sweep_pad("LASER SWP", rise=1500, fall=2500, peak=7.0, floor=1.2, mr=[1.0, 2.0]), sweep_pad("VENICE SWP", algo=22, rise=2600, fall=4000, peak=4.2, floor=0.8, att=900, spread=6),
    drone("SPC DRONE"), drone("DARK DRONE", key=36, ratios=(1.0, 1.005, 0.5, 1.5), b=3.0, vib=15), drone("GHOST DRN", inharm=0.02, lfo_hz=0.2, vib=40, amd=60), drone("DEEP DRIFT", key=36, tr=12, lfo_hz=0.08, vib=30),
    drone("ALIEN", ratios=(1.0, 1.414, 0.5, 2.76), b=3.2, vib=35, lfo_hz=0.15, inharm=0.04), drone("TEMPLE", ratios=(1.0, 1.5, 0.5, 2.0), b=1.8, lfo_hz=0.1, vib=10, att=2500), drone("NEBULA", ratios=(1.0, 1.003, 2.0, 3.0), b=2.0, att=3000, rel=4.0, vib=20, amd=80),
    drone("COLD WIND", ratios=(1.0, 1.01, 4.0, 5.0), b=2.8, vib=22, lfo_hz=0.3, amd=70), drone("HORIZON", ratios=(1.0, 1.002, 0.5, 2.0), b=1.6, lfo_hz=0.1, vib=12, att=2800), drone("LIGHTYEARS", key=60, tr=24, b=1.8, vib=18, lfo_hz=0.12, att=3200, rel=5.0),
    pad("EVOLVER", algo=22, att=1500, swell=4000, peak=4.4, sus=1.5, settle=5.0, vib=5, lfo_hz=0.2, mr=[1.0, 2.0]), pad("BREATHING", algo=25, att=900, peak=2.6, sus=2.0, trem_db=5.0, lfo_hz=0.28, lfo_wave="sin", spread=5), pad("TIDE", att=1400, rel=3.5, trem_db=6.0, lfo_hz=0.15),
    pad("CASCADE", algo=24, att=700, peak=3.4, sus=2.2, settle=3.0, spread=7), pad("ZENITH", att=2000, swell=2500, rel=3.0, peak=3.8, sus=2.8, algo=25, spread=7), pad("NADIR", tr=0, key=60, att=1600, peak=2.2, sus=1.8, rel=3.0),
    pad("DISTANT", algo=24, att=2400, rel=4.0, peak=1.0, sus=0.9, vib=2, lfo_hz=0.1, spread=4, tr=36), pad("PASSAGE", algo=24, att=1200, swell=3500, settle=4.0, peak=4.0, sus=1.6, rel=2.2, spread=6), pad("DAWN CHRS", algo=5, fb=4, mr=[1.0, 2.0, 3.0], att=1000, peak=2.0, sus=1.5, trem_db=3.0, lfo_hz=0.6, vib=8, lfo_wave="tri"),
])
BANKS = [B12, B13, B14, B15, B16]
