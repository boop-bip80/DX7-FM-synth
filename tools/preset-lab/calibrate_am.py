import os
os.makedirs('build', exist_ok=True)
import numpy as np, dx7, lab, json
combos=[(ams,amd) for ams in (1,2,3) for amd in (10,25,40,60,80,99)]
vs=[]
for ams,amd in combos:
    v=lab.simple_voice(32); o=v.op(1); o.ol=99; o.ams=ams
    v.lfo_speed=40; v.lfo_wave=4; v.lfo_amd=amd; v.lfo_pmd=0; v.lfo_delay=0
    vs.append(v)
a=lab.render_voices(vs,hold=2.0,tail=0.1)
res={}
for (ams,amd),x in zip(combos,a):
    env=lab.rms_env(x[int(0.3*44100):int(1.8*44100)],0.004)
    d=lab.db(env); res[(ams,amd)]=(d.max()-d.min())
print("peak-to-peak tremolo depth in dB (rows AMS, cols AMD 10,25,40,60,80,99)")
for ams in (1,2,3):
    print(f"ams{ams}: "+"  ".join(f"{res[(ams,amd)]:5.1f}" for amd in (10,25,40,60,80,99)))
json.dump({f"{k[0]},{k[1]}":float(v) for k,v in res.items()},open('build/am.json','w'))
