"""DRUMS & PERCUSSION: five cartridges."""
from recipes.drums import kick, tom, simmons, snare, clap, rimshot, hat, cymbal, shaker, tambourine, metal, conga, roll
from recipes.plucks import gong

B25 = ("DRUMS", "Kicks & Toms", [
    kick("LA KICK"), kick("MALIBU KIK", sweep=18, body_t=0.6, click=-16), kick("TIGHT KICK", sweep=20, sweep_t=0.03, body_t=0.22, punch_t=0.06), kick("BOOM KICK", sweep=26, sweep_t=0.07, body_t=0.9, click=-22, boom=-4),
    kick("PUNCH KICK", sweep=30, sweep_t=0.04, body_t=0.3, punch=-3, click=-8), kick("DRY KICK", body_t=0.16, click=-10, sweep=16, sweep_t=0.03), kick("808 KICK", sweep=14, sweep_t=0.06, body_t=1.2, click=None, punch=-14),
    kick("SUB KICK", sweep=10, sweep_t=0.05, body_t=1.0, punch=-18, click=None, ratio=0.5), kick("CLICK KICK", click=-3, click_t=0.02, click_hz=5200, body_t=0.3), kick("DISCO KICK", sweep=22, body_t=0.35, punch=-5, click=-9, drive=0.6),
    kick("ROCK KICK", sweep=24, body_t=0.4, punch=-4, click=-6, click_hz=2800, drive=1.0), kick("BIG GATED", body_t=0.28, boom=-5, sweep=22, click=-9), kick("ELEC KICK", sweep=34, sweep_t=0.09, body_t=0.45, drive=1.5, click=-12),
    kick("SOFT KICK", sweep=12, body_t=0.35, click=-24, punch=-10, vel=2), kick("HARD KICK", sweep=28, sweep_t=0.03, body_t=0.32, punch=-3, click=-5, drive=1.2), kick("FUNK KICK", sweep=18, body_t=0.2, punch=-6, click=-8, click_hz=3000),
    tom("TOM HI", tr=30), tom("TOM MID", tr=24), tom("TOM LOW", tr=18), tom("TOM FLOOR", tr=12, t60=0.8), tom("ROTO TOM", tr=24, sweep=14, t60=0.4, stick=-12), tom("DRY TOM", tr=24, t60=0.25, sweep=6),
    tom("LONG TOM", tr=18, t60=1.1, sweep=8, sweep_t=0.12), tom("ROOM TOM", tr=24, t60=0.85, noise=-12, ring=0.3), tom("POWER TOM", tr=18, t60=0.6, stick=-5, body_b=2.2, sweep=12), tom("FUSION TOM", tr=30, t60=0.4, sweep=5, stick=-8),
    simmons("SYNTOM"), simmons("SYNTOM HI", tr=30, sweep=15, sweep_t=0.12, t60=0.4), simmons("SYNTOM MID", tr=24, sweep=17), simmons("SYNTOM LOW", tr=18, sweep=19, t60=0.6), simmons("SPACE TOM", sweep=28, sweep_t=0.3, t60=0.7), simmons("PEW TOM", sweep=24, sweep_t=0.12, t60=0.35, bite=-12),
])

B26 = ("DRUMS", "Snares, Claps & Rimshots", [
    snare("LA SNARE"), snare("TIGHT SNR", n_t60=0.14, tone_t60=0.06), snare("FAT SNARE", tone_hz=160, tone_db=-2, n_t60=0.28), snare("BIG SNARE", n_t60=0.34, tone_hz=150, tone_t60=0.14),
    snare("GATED SNR", gate=0.14), snare("GATE BIG", gate=0.22, tone_hz=150, tone_db=-2), snare("GATE SHORT", gate=0.09), snare("ROOM SNARE", n_t60=0.5, snap=False, tone_t60=0.12),
    snare("POP SNARE", noise="high", n_t60=0.2, tone_hz=210), snare("DRY SNARE", n_t60=0.1, tone_t60=0.05), snare("ROCK SNARE", tone_hz=175, tone_b=3.0, n_t60=0.24, n_db=0),
    snare("DISCO SNR", noise="high", n_t60=0.26, tone_hz=200), snare("ELEC SNARE", noise="air", n_t60=0.18, tone_hz=240, tone_b=4.0), snare("SYN SNARE", tone_hz=280, tone_t60=0.12, n_t60=0.22, tone_db=-2),
    snare("SOFT SNARE", n_db=-8, att=2.0), snare("BRUSH SNR", noise="high", att=14.0, n_t60=0.3, tone_db=None, snap=False), snare("PICCOLO SN", tone_hz=330, n_t60=0.12),
    snare("DEEP SNARE", tone_hz=130, n_t60=0.3, tone_t60=0.14), snare("TOM SNARE", tone_hz=150, tone_t60=0.18, n_db=-8, n_t60=0.12), snare("CLICK SNR", tone_b=5.0, tone_hz=400, n_t60=0.1),
    clap("CLAP"), clap("BIG CLAP", tail_t60=0.4), clap("TIGHT CLAP", tail_t60=0.12, burst_t=0.05), clap("GATED CLAP", tail_t60=0.15, bursts_hz=38), clap("DISCO CLAP", bursts_hz=48, tail_t60=0.3),
    clap("ROOM CLAP", tail_t60=0.6, tail_att=50), clap("ELEC CLAP", noise="high", bursts_hz=49), clap("SOFT CLAP", burst_db=-8, tail_db=-3, tail_att=70, tail_t60=0.3, bursts_hz=36, burst_t=0.1, noise="low"),
    rimshot("RIMSHOT"), rimshot("SIDE STICK", tone_hz=1100, body_hz=260, t60=0.06), rimshot("WOOD CRACK", tone_hz=2400, t60=0.035), rimshot("ELEC RIM", tone_hz=1900, t60=0.09, crack=1.4),
])

B27 = ("DRUMS", "Hats, Cymbals & Shakers", [
    hat("HAT CLOSED"), hat("HAT TIGHT", t60=0.03), hat("HAT SOFT", db=-8, t60=0.05), hat("HAT METAL", noise="high", tone_db=-3, tone_hz=8000, t60=0.06), hat("HAT SIZZLE", t60=0.09),
    hat("HAT DISCO", t60=0.07, tone_db=-6, tone_hz=7200), hat("HAT PEDAL", t60=0.1, tone_db=-12), hat("HAT OPEN", t60=0.35, open_swell=0.004), hat("HAT OPEN 2", t60=0.6, open_swell=0.006, tone_t=0.3),
    hat("HAT HALF", t60=0.18), hat("HAT ELEC", noise="high", t60=0.22, tone_db=-4), hat("HAT SWISH", t60=0.45, open_swell=0.02),
    cymbal("CRASH"), cymbal("CRASH BIG", wash=3.6), cymbal("CRASH SHRT", wash=1.2), cymbal("SPLASH", wash=0.7, ping_hz=7000), cymbal("CHINA", wash=1.6, ping_hz=4300, noise="high"),
    cymbal("RIDE", wash=0.9, ride=True), cymbal("RIDE BELL", ping_db=-2, wash=0.5, ride=True, bell_db=-4), cymbal("CYMBL SWL", swell=900, wash=3.0),
    shaker("SHAKER"), shaker("SHAKR LONG", t60=0.22, att_ms=20), shaker("CABASA", t60=0.09, att_ms=8, noise="air", tone=True), shaker("MARACAS", t60=0.07, att_ms=10, noise="high"),
    shaker("EGG SHAKER", t60=0.12, att_ms=14, noise="mid"), shaker("SAND BLOCK", t60=0.16, att_ms=25, noise="low"),
    tambourine("TAMBOURINE"), tambourine("TAMB SOFT", t60=0.2, db=-8), tambourine("TAMB SHAKE", t60=0.5, jingle_t=1.0, att_ms=12),
    rimshot("HAND SNAP", tone_hz=1300, body_hz=500, t60=0.04, crack=0.7), rimshot("CASTANETS", tone_hz=2100, body_hz=700, t60=0.05, crack=1.2), tambourine("SLEIGH SHK", att_ms=40, t60=0.3, jingle_hz=7400, jingle_t=0.8),
])

B28 = ("DRUMS", "Latin & World Percussion", [
    conga("CONGA HI", tr=30), conga("CONGA LO", tr=18, t60=0.5), conga("CONGA MUTE", open_=False, t60=0.2), conga("CONGA SLAP", slap=-2, slap_t=0.05, open_=False), conga("CONGA OPEN", sweep=3.0, t60=0.45), conga("TUMBA", tr=12, t60=0.6),
    conga("BONGO HI", tr=42, t60=0.18), conga("BONGO LO", tr=36, t60=0.24), conga("BONGO MUTE", tr=39, open_=False, t60=0.1), conga("BONGO SLAP", tr=42, slap=-2, slap_t=0.05, t60=0.12),
    metal("TIMBALE HI", [(620, 0, 0.5, 940, 2.5, 0.15), (1250, -4, 0.3, 1900, 2.0, 0.08), (2900, -14, 0.12, 4400, 3.0, 0.04)]),
    metal("TIMBALE LO", [(440, 0, 0.6, 660, 2.5, 0.15), (900, -4, 0.35, 1350, 2.0, 0.08), (2100, -14, 0.14, 3200, 3.0, 0.04)]),
    metal("TIMBL SIDE", [(1800, 0, 0.06, 2700, 3.0, 0.03), (900, -6, 0.08, 1300, 2.0, 0.04)]), metal("PAILAS", [(780, 0, 0.35, 1170, 3.0, 0.1), (1500, -6, 0.2, 2300, 2.5, 0.06), (3300, -12, 0.1, 5000, 3.0, 0.04)]),
    metal("COWBELL", [(540, 0, 0.35, 800, 2.5, 0.1), (800, -3, 0.28, 1100, 2.0, 0.1)]), metal("COWBELL LO", [(420, 0, 0.4, 630, 2.5, 0.1), (630, -3, 0.32, 880, 2.0, 0.1)]),
    metal("COWBELL HI", [(700, 0, 0.3, 1050, 2.5, 0.1), (1050, -3, 0.24, 1480, 2.0, 0.1)]), metal("COWBL SOFT", [(540, -4, 0.2, 800, 1.6, 0.06), (800, -7, 0.16, 1100, 1.4, 0.06)]),
    metal("AGOGO HI", [(880, 0, 0.6, 1320, 3.0, 0.2), (1320, -4, 0.45, 1980, 2.0, 0.12), (2640, -12, 0.2, 3960, 2.5, 0.08)]), metal("AGOGO LO", [(660, 0, 0.7, 990, 3.0, 0.2), (990, -4, 0.5, 1485, 2.0, 0.12), (1980, -12, 0.22, 2970, 2.5, 0.08)]),
    metal("CLAVES", [(2500, 0, 0.06, 3750, 1.5, 0.03)]), metal("CLAVES LO", [(1800, 0, 0.08, 2700, 1.5, 0.04), (3600, -10, 0.04, 5400, 1.0, 0.02)]),
    metal("WOODBLOCK", [(1400, 0, 0.05, 2100, 2.0, 0.03), (2800, -9, 0.03, 4200, 1.5, 0.02)]), metal("TEMPLE BLK", [(900, 0, 0.12, 1350, 2.0, 0.06), (1500, -6, 0.08, 2250, 1.5, 0.04)]),
    metal("TRIANGLE", [(4800, 0, 1.6, 7200, 1.5, 0.6), (7500, -6, 1.2, 11200, 1.2, 0.4), (11000, -12, 0.9, 5500, 1.0, 0.3)]), metal("TRIANGL MT", [(4800, 0, 0.18, 7200, 1.5, 0.08), (7500, -6, 0.14, 11200, 1.2, 0.06)]),
    clap("GUIRO", bursts_hz=28, burst_t=0.28, tail_db=-40, tail_t60=0.05, burst_db=0, noise="high", wave="saw-dn"), clap("GUIRO LONG", bursts_hz=22, burst_t=0.5, tail_db=-40, tail_t60=0.05, noise="high", wave="saw-dn"),
    conga("DJEMBE", tr=24, slap=-1, t60=0.3, sweep=4.0), conga("TABLA", tr=30, sweep=5.0, t60=0.3, ring=2.0, body_b=2.0), conga("DHOLAK", tr=18, t60=0.4, sweep=3.0, body_b=1.6), conga("TALK DRUM", tr=24, sweep=-7.0, t60=0.45, body_b=1.0),
])

B29 = ("DRUMS", "Electro & Orchestral Percussion", [
    tom("TIMPANI", tr=12, sweep=3.0, sweep_t=0.25, t60=1.8, stick=-14, stick_hz=900, vel=3), tom("TIMPANI LO", tr=6, sweep=3.0, sweep_t=0.3, t60=2.2, stick=-14, stick_hz=700), tom("TIMPANI HI", tr=18, sweep=3.0, sweep_t=0.22, t60=1.5, stick=-14, stick_hz=1100),
    tom("TIMP SOFT", tr=12, sweep=2.0, t60=1.4, stick=-22, stick_hz=700, vel=2), tom("TIMP BOOM", tr=6, sweep=5.0, sweep_t=0.4, t60=2.8, stick=-12),
    kick("CONCERT BD", sweep=6, sweep_t=0.2, body_t=1.6, click=-26, punch=-12), kick("BASSDRUM 2", sweep=8, sweep_t=0.15, body_t=1.2, click=-20, punch=-10, ratio=1.5), kick("ORCH BOOM", sweep=4, sweep_t=0.3, body_t=2.4, click=None, punch=-16),
    tom("TAIKO", tr=6, sweep=7.0, sweep_t=0.1, t60=1.2, stick=-6, stick_hz=1400), tom("TAIKO HIT", tr=12, sweep=9.0, sweep_t=0.07, t60=0.8, stick=-4), tom("ODAIKO", tr=0, sweep=10.0, sweep_t=0.12, t60=1.6, stick=-8),
    gong("ORCH GONG", ring=6.0, tr=12, strike=3.5, ratios=(1.0, 1.5, 2.5)), gong("CHINA GONG", ring=5.0, ratios=(1.0, 1.7, 2.9), strike=4.0, tr=12), gong("GONG SHORT", ring=3.0, tr=12, strike=3.0),
    clap("CLAP 808", noise="mid", bursts_hz=45, tail_t60=0.3, wave="saw-dn"), rimshot("FINGERSNAP", tone_hz=1200, body_hz=420, t60=0.045, crack=0.8), clap("CLAP SNAP", bursts_hz=49, burst_t=0.04, tail_t60=0.07, tail_db=-10), clap("CLAP ROOM2", tail_t60=0.8, tail_att=60, bursts_hz=40),
    simmons("ZAP TOM", sweep=36, sweep_t=0.2, t60=0.4, bite=-14), simmons("LASER TOM", sweep=30, sweep_t=0.1, t60=0.3, bite=-10, tr=30), kick("SPACE KICK", sweep=40, sweep_t=0.12, body_t=0.6, drive=1.0, click=-10),
    simmons("HEX TOM", sweep=12, sweep_t=0.2, t60=0.9, bite=-12), simmons("DRY HEX", sweep=8, sweep_t=0.1, t60=0.3, bite=-9),
    metal("COWBELL808", [(540, 0, 0.5, 800, 3.5, 0.2), (800, -2, 0.4, 1200, 3.0, 0.15)]), metal("CLAVE 808", [(2500, 0, 0.04, 3750, 2.0, 0.02)]), shaker("MARACA 808", t60=0.05, att_ms=6, noise="air"), rimshot("RIM 808", tone_hz=1700, t60=0.03, body_hz=500),
    kick("TECHNO KIK", sweep=32, sweep_t=0.05, body_t=0.3, punch=-3, click=-4, drive=1.4, click_hz=4400), shaker("SHAKER 808", t60=0.08, att_ms=8, noise="air"),
    roll("SNARE ROLL"), roll("SNR ROLL 2", hz=36.0, depth_db=16.0, noise="high", att=260, tone_hz=190), roll("CYMBL ROLL", hz=12.0, depth_db=6.0, noise="air", att=900, rel=1.2, wave="tri"),
])
BANKS = [B25, B26, B27, B28, B29]
