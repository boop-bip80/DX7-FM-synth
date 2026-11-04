import numpy as np, dx7, lab
SR=lab.SR
f0=261.6256
vs=[]
for fb in range(8):
    v=lab.simple_voice(32); v.fb=fb; o=v.op(6); o.ol=99
    vs.append(v)
a=lab.render_voices(vs,hold=1.0,tail=0.1)
print("feedback sine-stack: harmonic amplitudes relative to h1 (dB) h1..h8 and centroid(Hz)")
n=32768
for fb,x in enumerate(a):
    seg=x[10000:10000+n]; S=np.abs(np.fft.rfft(seg*np.hanning(n)))
    hs=[S[int(round(h*f0*n/SR))-2:int(round(h*f0*n/SR))+3].max() for h in range(1,9)]
    hs=np.array(hs)/hs[0]
    print(fb," ".join(f"{20*np.log10(h+1e-9):6.1f}" for h in hs), f"  centroid={lab.centroid(x,0.3,0.5):7.0f}")
# vibrato depth: algo 32 single carrier A4 (ratio 1) with sine LFO, delay 0
print("vibrato: pmd,pms -> peak cents")
combos=[(pmd,pms) for pms in (1,2,3,4,5,6,7) for pmd in (10,25,50,99)]
vs=[]
for pmd,pms in combos:
    v=lab.simple_voice(32); o=v.op(1); o.ol=99
    v.lfo_speed=35; v.lfo_wave=4; v.lfo_pmd=pmd; v.pms=pms; v.lfo_delay=0; v.lfo_sync=1
    vs.append(v)
a=lab.render_voices(vs,note=69,hold=2.0,tail=0.1)
res={}
for (pmd,pms),x in zip(combos,a):
    # instantaneous frequency via zero crossing windows
    seg=x[int(0.3*SR):int(1.9*SR)].astype(np.float64)
    w=int(0.01*SR); fr=[]
    for i in range(0,len(seg)-w,w//2):
        s=seg[i:i+w]; z=np.where(np.diff(np.sign(s))>0)[0]
        if len(z)>=3: fr.append(SR*(len(z)-1)/(z[-1]-z[0]))
    fr=np.array(fr); cents=1200*np.log2(fr.max()/fr.min())/2
    res[(pmd,pms)]=cents
for pms in (1,2,3,4,5,6,7):
    print(f"pms {pms}: "+"  ".join(f"pmd{pmd}={res[(pmd,pms)]:5.1f}c" for pmd in (10,25,50,99)))
