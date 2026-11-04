import os
os.makedirs('build', exist_ok=True)
import numpy as np, dx7, lab
SR=lab.SR
def inst_freq(x):
    n=len(x); X=np.fft.fft(x); h=np.zeros(n); h[0]=1; h[1:n//2]=2; h[n//2]=1
    z=np.fft.ifft(X*h); ph=np.unwrap(np.angle(z)); f=np.diff(ph)*SR/(2*np.pi)
    return f
combos=[(pmd,pms) for pms in (1,2,3,4,5,6,7) for pmd in (5,10,15,25,40,60,99)]
vs=[]
for pmd,pms in combos:
    v=lab.simple_voice(32); o=v.op(1); o.ol=99
    v.lfo_speed=35; v.lfo_wave=4; v.lfo_pmd=pmd; v.pms=pms; v.lfo_delay=0
    vs.append(v)
a=lab.render_voices(vs,note=69,hold=2.0,tail=0.1)
res={}
for (pmd,pms),x in zip(combos,a):
    seg=x[int(0.3*SR):int(1.8*SR)].astype(np.float64)
    f=inst_freq(seg)[200:-200]
    # smooth over 2ms
    k=int(0.002*SR); f=np.convolve(f,np.ones(k)/k,mode='valid')
    c=1200*np.log2(np.percentile(f,98)/np.percentile(f,2))/2
    res[(pmd,pms)]=c
print("peak vibrato (cents, +/-) by PMD (cols) and PMS (rows)")
print("      "+"".join(f"{p:>8d}" for p in (5,10,15,25,40,60,99)))
for pms in range(1,8):
    print(f"pms{pms}: "+"".join(f"{res[(p,pms)]:8.1f}" for p in (5,10,15,25,40,60,99)))
import json; json.dump({f"{k[0]},{k[1]}":v for k,v in res.items()},open('build/vib.json','w'))
