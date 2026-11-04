"""PLUCKS & GUITARS, WORLD STRINGS, VOICES."""
from recipes.plucks import (nylon, steel_gtr, clean_gtr, muted_gtr, banjo, mandolin, harp, koto, sitar, dulcimer, pairs, pr)
from recipes.voices import choir, choir_breath, vox_fm

B22 = ("PLUCKS & GUITARS", "Guitars", [
    nylon("NYLON GTR"), nylon("SOFT NYLON", bright=0.7, pick=2.0), nylon("BRT NYLON", bright=1.4, pick=4.0), nylon("CLASSICAL", ring=2.2, body=1.0, bright=0.9), nylon("FLAMENCO", ring=1.2, bright=1.5, pick=5.0, vel=7),
    nylon("LOW NYLON", tr=12, ring=2.0), nylon("BOSSA GTR", ring=1.4, bright=0.8, pick=2.5),
    steel_gtr("STEEL GTR"), steel_gtr("BRT STEEL", bright=1.4, pick=5.0), steel_gtr("SOFT STEEL", bright=0.7, pick=2.5), steel_gtr("FOLK GTR", ring=2.6, shimmer=-7), steel_gtr("12 STRING", shimmer=-4, ring=2.8, bright=1.2),
    steel_gtr("STRUM GTR", ring=1.6, pick=5.5, bright=1.1), steel_gtr("LOW STEEL", tr=12, ring=2.6),
    clean_gtr("CLEAN GTR"), clean_gtr("CHORUS GTR", chorus=7, ring=3.0), clean_gtr("JAZZ GTR", bright=0.6, ring=2.4, chorus=2), clean_gtr("SURF GTR", bright=1.3, trem=8.0, ring=3.2), clean_gtr("FUNK GTR", bright=1.2, ring=1.0),
    clean_gtr("SUNSET GTR", chorus=5, vib=6, ring=3.5), clean_gtr("LA CLEAN", bright=1.0, chorus=6, ring=2.8), clean_gtr("BRT CLEAN", bright=1.5, chorus=3),
    muted_gtr("MUTED GTR"), muted_gtr("PALM MUTE", ring=0.16, chunk=3.0), muted_gtr("FUNK CHOP", ring=0.3, bright=1.3, chunk=5.0), muted_gtr("DEAD NOTE", ring=0.1, bright=0.8, chunk=2.5),
    muted_gtr("STACC GTR", ring=0.35, bright=1.1), muted_gtr("SCRTCH GTR", ring=0.14, bright=1.5, chunk=6.0, vel=7),
    clean_gtr("CRYSTL GTR", bright=1.2, chorus=8, ring=3.8, vib=4), steel_gtr("CNTRY GTR", bright=1.1, ring=2.0, pick=4.5, shimmer=-11, fb=4),
    pairs("GTR HARMNC", [pr(2.0, 0, 2.5, 1.0, 0.6, 0.2), pr(3.0, -10, 1.8, 1.0, 0.5, 0.1), pr(4.0, -16, 1.4, 1.0, 0.4, 0.08)], cat="pluck", rel=0.25, vel=3),
    pairs("GTR BELLS", [pr(1.0, 0, 3.0, 3.5, 1.4, 0.3, det=3), pr(2.0, -6, 2.5, 1.0, 1.0, 0.2, det=-3), pr(3.0, -12, 2.0, 1.0, 0.8, 0.1)], cat="pluck", rel=0.3, vel=4),
])

B23 = ("PLUCKS & GUITARS", "Harps, Koto, Banjo & World Strings", [
    harp("HARP"), harp("SOFT HARP", soft=0.7, ring=4.2), harp("BRT HARP", soft=1.4, ring=3.0), harp("LOW HARP", tr=12, ring=4.5), harp("GLISS HARP", tr=36, ring=3.2, soft=1.1), harp("CELTIC HRP", ring=5.5, soft=0.6, tr=24, fb=2, rel=0.6),
    koto("KOTO"), koto("BRT KOTO", pluck=4.5, ring=1.4), koto("SHAMISEN", pluck=5.0, ring=1.0, bend=1.4, fb=3), koto("BIWA", pluck=3.0, ring=2.2, bend=0.6, tr=12),
    banjo("BANJO"), banjo("BRT BANJO", twang=5.5, ring=0.6), banjo("SOFT BANJO", twang=3.0, ring=1.0), banjo("TENOR BNJO", tr=48, twang=4.0, ring=0.7),
    mandolin("MANDOLIN"), mandolin("MANDO TREM", tremolo_hz=9.0, ring=1.4), mandolin("BOUZOUKI", tr=24, ring=1.2, bright=1.3, spread=4), mandolin("UKULELE", tr=48, ring=0.7, bright=0.8, spread=3),
    sitar("SITAR"), sitar("SITAR DRN", drone=True, ring=4.0), sitar("TANPURA", buzz=3.0, ring=5.0, drone=True, tr=12), sitar("SARANGI", buzz=3.5, ring=2.4, fb=6),
    dulcimer("DULCIMER"), dulcimer("HMMR DULC", hard=1.4, ring=1.6), dulcimer("ZITHER", ring=2.6, hard=0.8, spread=5), dulcimer("PSALTERY", tr=48, ring=2.0, hard=0.9),
    nylon("LUTE", bright=0.9, ring=1.8, tr=36, pick=3.5), nylon("OUD", tr=12, bright=1.1, ring=1.6, pick=4.0), steel_gtr("DOBRO", bright=1.2, ring=1.8, shimmer=-12, fb=4),
    steel_gtr("BALALAIKA", tr=48, bright=1.3, ring=1.1), nylon("CHARANGO", tr=48, bright=1.2, ring=1.0), clean_gtr("STICK GTR", bright=1.1, chorus=3, ring=2.8, tr=12),
])

B24 = ("VOICES", "Choirs & Human Voices", [
    choir("CHOIR AH"), choir("CHOIR OO", "oo"), choir("CHOIR OH", "oh"), choir("CHOIR EH", "eh"), choir("CHOIR EE", "ee"), choir("CHOIR UH", "uh"),
    choir("HUM CHOIR", "mm", tr=12, att=600, rel=1.2, vib=14), choir("SLOW AH", "ah", att=900, rel=2.0), choir("BOYS CHOIR", "ah", tr=36, vib=8, att=300), choir("MALE CHOIR", "oh", tr=12, vib=8, att=350),
    choir("WIDE CHOIR", "ah", spread=7, vib=16, att=500, ratios=(1, 2, 3, 4, 6, 8)), choir("TIGHT CHR", "ah", spread=0, vib=4, att=300),
    choir_breath("AIR CHOIR"), choir_breath("BREATHY AH", "ah", breath=-14), choir_breath("BREATHY OO", "oo", breath=-14), choir_breath("WHISPER", "oo", breath=-8, att=500),
    choir_breath("ANGELS", "ah", tr=36, breath=-22, att=600, rel=1.6, vib=8), choir_breath("MALE AIR", "oh", tr=12, breath=-20, att=420), choir_breath("EH BREATH", "eh", breath=-18),
    choir_breath("GHOST VOX", "mm", breath=-12, att=900, rel=2.4, vib=6),
    vox_fm("VOX FM"), vox_fm("OOH VOX", b=0.8, ratio=2.0, att=500, rel=1.0, lvl=-9), vox_fm("AAH VOX", b=1.8, ratio=5.0, r2=3.0, att=350), vox_fm("SYN VOICE", b=2.4, ratio=2.0, att=280, vib=7),
    vox_fm("TALK VOX", b=2.8, ratio=3.0, r2=7.0, att=140, rel=0.5, vib=3), vox_fm("DREAM VOX", b=1.2, ratio=3.0, att=800, rel=2.0, vib=6), vox_fm("LOW VOX", tr=12, b=1.6, ratio=3.0, att=360),
    vox_fm("SHE VOX", tr=36, b=1.4, ratio=5.0, att=300, vib=11), vox_fm("SIGH", b=1.6, ratio=3.0, att=120, rel=1.4, vib=4), vox_fm("LA LA", b=1.8, ratio=3.0, att=40, rel=0.25, vib=5),
    choir("SUNSET CHR", "oh", att=700, rel=1.8, vib=9, spread=5), choir("GOSPEL AH", "aw", att=220, rel=0.8, vib=14, spread=5),
])
BANKS = [B22, B23, B24]
