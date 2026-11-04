import os
os.makedirs('build', exist_ok=True)
import numpy as np, dx7, lab
rates=sorted(set(list(range(0,100,4))+[99,98,97,96,95,94,93,92,91,90]))
vs=[]
for r in rates:
    v=lab.simple_voice(32); o=v.op(1); o.ol=99
    o.rates=[r,99,99,99]; o.levels=[99,99,99,0]
    vs.append(v)
a=lab.render_voices(vs,hold=12.0,tail=0.1)
print("attack rate: t(-20dB) t(-6dB) t(-1dB)  [seconds from note on]")
for r,x in zip(rates,a):
    env=lab.rms_env(x,0.002); d=lab.db(env); pk=d.max()
    def first(thr):
        idx=np.where(d>pk-thr)[0]; return idx[0]*0.002 if len(idx) else None
    print(f"{r:2d}: {first(20)} {first(6)} {first(1)}")
# fit the decay model speed = k*(4+(q&3))*2^(q>>2)
import json
dec=json.load(open('build/decay.json'))
print("fit check (dB/s) rate measured model")
k=0.0638
for r in (30,39,48,57,66,75):
    q=(r*41)>>6; model=k*(4+(q&3))*2**(q>>2)
    t=dec[str(r)]['40']; print(r, round(40/t,1), round(model,1))
