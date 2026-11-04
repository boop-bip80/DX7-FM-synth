import os
os.makedirs('build', exist_ok=True)
"""Measure the engine so recipes can be written in physical units."""
import sys, json, math
import numpy as np
import dx7, lab

def carrier_voice(ol=99, rates=(99,99,99,99), levels=(99,99,99,0), ratio=1.0):
    v = lab.simple_voice(32)
    o = v.op(1); o.ol = ol; o.rates = list(rates); o.levels = list(levels)
    o.coarse, o.fine = dx7.ratio_to_coarse_fine(ratio)
    return v

# 1. level calibration: peak amplitude of a single sine carrier vs output level
print("== carrier peak vs output level (note 60, vel 100)")
vs = [carrier_voice(ol) for ol in (99, 90, 80, 70, 60, 50, 40, 30)]
a = lab.render_voices(vs, hold=0.6, tail=0.2)
for ol, x in zip((99, 90, 80, 70, 60, 50, 40, 30), a):
    pk = np.abs(x[2000:20000]).max()
    print(f"ol={ol:2d} peak={pk:.4f} ({lab.db(pk):6.1f} dBFS)")

# 2. modulation index: op2 (ratio r2) -> op1 (ratio 1), algo 5
def pm_spectrum(beta, r_mod, f0, n):
    t = np.arange(n) / lab.SR
    return np.sin(2*np.pi*f0*t + beta*np.sin(2*np.pi*f0*r_mod*t))

print("== modulation index vs modulator level (algo 2->1, ratio 1:1)")
levels = [99, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20]
vs = []
for L in levels:
    v = lab.simple_voice(5)
    c = v.op(1); c.ol = 99
    m = v.op(2); m.ol = L
    vs.append(v)
a = lab.render_voices(vs, note=60, hold=1.0, tail=0.1)
f0 = 261.6256
n = 16384
betas = np.linspace(0, 20, 801)
res = {}
for L, x in zip(levels, a):
    seg = x[20000:20000+n]
    S = np.abs(np.fft.rfft(seg*np.hanning(n)))
    S /= S.max()
    best, be = 0, 1e9
    # amplitude-normalised spectral match over first 40 harmonics
    kk = np.array([round(h*f0*n/lab.SR) for h in range(1, 41)])
    meas = np.array([S[max(0,k-1):k+2].max() for k in kk]); meas/=meas.max()
    for b in betas:
        ref = pm_spectrum(b, 1.0, f0, n)
        R = np.abs(np.fft.rfft(ref*np.hanning(n))); 
        r = np.array([R[max(0,k-1):k+2].max() for k in kk]); r/=max(r.max(),1e-9)
        e = ((meas-r)**2).sum()
        if e < be: best, be = b, e
    res[L] = best
    print(f"mod level {L:2d} -> beta ~ {best:5.2f} rad  (fit err {be:.3f})")
json.dump(res, open("build/beta_levels.json","w"))
