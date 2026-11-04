import os
os.makedirs('build', exist_ok=True)
import json, numpy as np, dx7, lab
SR=lab.SR
rates=list(range(0,100,3))+[99]
rates=sorted(set(rates))
# decay: 99 -> 0 at rate r, measure dB/sec
vs=[]
for r in rates:
    v=lab.simple_voice(32); o=v.op(1); o.ol=99
    o.rates=[99,r,99,99]; o.levels=[99,0,0,0]   # R2 falls to 0 and stays; keydown holds
    vs.append(v)
a=lab.render_voices(vs,hold=6.0,tail=0.1)
dec={}
for r,x in zip(rates,a):
    env=lab.rms_env(x,0.005)
    d=lab.db(env)
    pk=d.max(); ipk=int(d.argmax())
    t={}
    for thr in (10,20,40,60):
        idx=np.where(d[ipk:]<pk-thr)[0]
        t[thr]=(idx[0]*0.005) if len(idx) else None
    dec[r]=t
    print(f"decay rate {r:2d}: -10dB {t[10]}, -20dB {t[20]}, -40dB {t[40]}, -60dB {t[60]}")
json.dump({str(k):v for k,v in dec.items()},open('build/decay.json','w'))
