import sys; sys.path.insert(0,'.')
import numpy as np, random, audit, lab, dx7
from studio import P, shape
random.seed(7)
def flat(a, t0=0.02, dur=0.25):
    f,m=lab.spectrum(a,t0,dur); band=(f>200)&(f<14000); p=m[band]**2+1e-14
    return float(np.exp(np.log(p).mean())/p.mean()), lab.centroid(a,t0,dur)
def chain_voice(freqs, fb=7, b=12.5, algo=1, extra=None):
    p=P("N",algo,fb=fb); env=shape(1,[(-60,0.3)],0.1)
    p.mod(6,fx=freqs[0],beta=b,env=env); p.mod(5,fx=freqs[1],beta=b,env=env); p.mod(4,fx=freqs[2],beta=b,env=env); p.car(3,fx=freqs[3],env=env)
    if extra:
        p.mod(2,fx=extra[0],beta=b,env=env); p.car(1,fx=extra[1],env=env,db=-3)
    return p.done()
results=[]
cands=[]
for _ in range(192):
    fr=[random.uniform(300,9000) for _ in range(4)]
    ex=[random.uniform(300,9000),random.uniform(300,9000)] if random.random()<0.5 else None
    cands.append((fr,ex))
vs=[chain_voice(f,extra=e) for f,e in cands]
audio=lab.render_voices(vs,note=60,hold=0.4,tail=0.1)
sc=[]
for (f,e),a in zip(cands,audio):
    fl,c=flat(a); sc.append((fl,c,f,e))
sc.sort(key=lambda x:-x[0])
print("top 12 flat chains:")
for fl,c,f,e in sc[:12]: print(f"flat={fl:.3f} cent={c:6.0f} f={[int(x) for x in f]} extra={[int(x) for x in e] if e else None}")
for lo,hi,lab_ in ((0,2500,'LOW'),(2500,5000,'MID'),(5000,7500,'HIGH'),(7500,20000,'AIR')):
    sel=[s for s in sc if lo<=s[1]<hi][:3]
    print(lab_,[ (round(s[0],3),int(s[1]),[int(x) for x in s[2]], [int(x) for x in s[3]] if s[3] else None) for s in sel])
