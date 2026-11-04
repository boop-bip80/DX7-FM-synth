"""SOUND FX and RHYTHMIC / MOTION patches."""
from recipes.fx import riser, impact, laser, siren, two_tone, gust, rotor, powerdown, bleeper, drone
from recipes.drums import cymbal
from recipes.seq import trill, gated, arp_pad, scanner

B30 = ("FX", "Risers, Impacts, Whooshes & Zaps", [
    riser("RISER"), riser("RISER SHRT", dur=1.5), riser("RISER LONG", dur=6.0, semis=36), riser("NOISE RISE", noise="air", tone_db=-20, dur=3.5), riser("TONE RISER", n_db=-20, tone_db=0, semis=36, b_end=8.0),
    riser("FALL RISER", fall=True), riser("SIREN RISE", semis=48, dur=2.5, tone_b=4.0), riser("DARK RISER", tr=12, noise="low", dur=4.0), riser("BRT RISER", noise="air", tr=36, dur=2.5), riser("REV SWELL", dur=2.0, semis=7),
    impact("IMPACT"), impact("BIG IMPACT", t60=5.0, n_t60=2.5), impact("DEEP HIT", tr=0, sweep=30, t60=4.0), impact("SHORT HIT", t60=1.0, n_t60=0.4), impact("DOOM HIT", tr=0, noise="low", n_t60=2.5, sweep=36, sweep_t=0.4),
    impact("METAL HIT", noise="high", n_db=0, crack=-2), impact("BOMB", sweep=40, sweep_t=0.6, t60=3.0), impact("SLAM", t60=0.8, n_t60=0.3, sweep=20),
    riser("WHOOSH", dur=1.2, semis=12, rel=0.5), riser("WHOOSH 2", dur=0.8, noise="high", semis=7), riser("JET PASS", dur=2.0, fall=True, noise="mid", semis=24), riser("SWISH", dur=0.5, semis=5, rel=0.2),
    riser("SWEEP UP", dur=2.0, semis=36, n_db=-20), riser("SWEEP DOWN", fall=True, semis=36, dur=2.0, n_db=-20), riser("AIR SWEEP", noise="air", dur=1.5, semis=12), cymbal("REV CYMBAL", swell=1500, wash=2.0, att=900),
    laser("LASER"), laser("ZAP UP", up=True, start=-12, end=36, t=0.18), laser("BLASTER", start=48, end=-24, t=0.4), laser("PHASER", t=0.15, ratio=1.0, mod_ratio=2.0),
    laser("RAY GUN", mod_ratio=1.5, b=8.0, t=0.3), laser("DROP ZAP", start=60, end=0, t=0.6, t60=0.8),
])

B31 = ("FX", "Sci-Fi, Nature & Machines", [
    siren("SIREN"), siren("AMBULANCE", hz=1.6, depth=500, wave="sq", ratio=1.0), siren("POLICE", hz=0.9, depth=700, wave="tri", ratio=2.0, b=2.4),
    siren("RED ALERT", hz=3.0, depth=300, wave="sq", ratio=1.0, b=2.8),
    two_tone("PHONE RING", pulse_hz=20.0), two_tone("DIAL TONE", f1=350, f2=440), two_tone("BUSY TONE", f1=480, f2=620, pulse_hz=1.9, amd=99), two_tone("ALARM", f1=880, f2=1100, pulse_hz=6.0),
    gust("WIND"), gust("STORM", kind="low", hi=True, gust_depth=90, gust_hz=0.6), gust("RAIN", kind="high", gust_hz=14.0, gust_depth=70, att=40), gust("SURF", kind="low", gust_hz=0.25, wave="tri", gust_depth=55, att=1200),
    gust("STATIC", kind="air", gust_hz=40.0, gust_depth=60, att=10, rel=0.2), gust("WATERFALL", kind="mid", gust_hz=20.0, gust_depth=60, att=300),
    rotor("HELI"), rotor("ENGINE", hz=25.0, depth=60, wave="tri", noise_db=-14), rotor("TURBINE", hz=44.0, depth=40, ratio=2.0, b=2.0, noise_db=-10, wave="tri", tr=24), rotor("TRAIN", hz=4.5, depth=70, kind="low", noise_db=-6),
    rotor("HEARTBEAT", hz=1.15, wave="saw-dn", depth=99, ratio=0.5, b=0.6, noise_db=-50, att=8, rel=0.15),
    powerdown("TAPE STOP"), powerdown("POWER DOWN", semis=-48, t=3.0, b=3.0), powerdown("POWER UP", semis=-36, t=1.2, up=True),
    bleeper("BLEEPS"), bleeper("COMPUTER", hz=20.0, depth=1500, b=2.4), bleeper("BUBBLES", hz=9.0, depth=900, t60=0.15, wave="sh"), bleeper("SPARKLE", hz=18.0, depth=1200, tr=48, t60=0.4),
    drone("HUM", ratios=(1.0, 1.0, 0.5, 2.0), b=0.8, lfo_hz=0.06, vib=4, att=500), drone("REACTOR", key=36, tr=12, ratios=(1.0, 1.01, 0.5, 1.5), b=3.4, vib=10, lfo_hz=0.4, amd=90), drone("XFORMER", key=36, ratios=(1.0, 2.0, 3.0, 4.0), b=1.4, vib=2, lfo_hz=0.1, att=400),
    bleeper("ROBOT", hz=11.0, depth=700, wave="sq", ratio=1.0, b=3.0, t60=0.0), drone("GHOST", ratios=(1.0, 1.0, 2.0, 3.0), b=2.2, inharm=0.03, vib=50, lfo_hz=0.25, amd=70, att=2200), impact("THUNDER", noise="low", n_t60=4.0, t60=2.5, sweep=10, sweep_t=0.8, tr=0),
])

B32 = ("SEQUENCE", "Arps, Trills & Motion", [
    trill("TRILL 5TH", semis=7, rate=7.0), trill("TRILL OCT", semis=12, rate=6.0), trill("TRILL 3RD", semis=4, rate=8.0), trill("TRILL MIN", semis=3, rate=8.0), trill("TRILL 4TH", semis=5, rate=7.0),
    trill("FAST TRILL", semis=7, rate=12.0), trill("SLOW ARP", semis=12, rate=3.5), trill("SUS TRILL", base="sus", semis=7, rate=6.5), trill("BELL TRILL", semis=12, rate=5.5, b=3.5, ratio=1.0, ring=0.9),
    trill("TRILL LOW", tr=12, semis=7, rate=6.0), trill("TRILL HIGH", tr=36, semis=12, rate=8.0), trill("PLUCK ARP", semis=7, rate=9.0, ring=0.3),
    gated("GATED PAD"), gated("CHOP PAD", rate=6.0, wave="saw-dn"), gated("PULSE GATE", rate=10.0), gated("TRANCEGATE", rate=12.0, depth=95), gated("SLOW GATE", rate=3.0),
    gated("TRIPLET GT", rate=9.3, algo=24), gated("BRASS GATE", algo=22, peak=4.5, sus=3.4, rate=8.0, swell=40), gated("STRNG GATE", algo=25, rate=5.0, spread=6),
    arp_pad("OCT HOP", semis=12, rate=6.0), arp_pad("5TH PULSE", semis=7, rate=7.0), arp_pad("OCT FAST", semis=12, rate=11.0), arp_pad("HOP PAD", semis=5, rate=4.0),
    scanner("SCANNER"), scanner("RADAR", hz=0.8, depth=1200), scanner("WARBLE", hz=7.0, depth=80), scanner("SIREN SCAN", hz=2.2, depth=1100),
    bleeper("RANDOM ARP", hz=7.0, depth=1200, wave="sh", cat="seq"), bleeper("SMPL HOLD", hz=10.0, depth=600, wave="sh", cat="seq"), bleeper("COMPTR ARP", hz=14.0, depth=1800, b=2.4, wave="sh", cat="seq"), bleeper("RICOCHET", hz=5.0, depth=2400, wave="sh", t60=0.2, cat="seq"),
])
BANKS = [B30, B31, B32]
