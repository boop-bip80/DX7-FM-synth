import numpy as np, dx7, lab, json
SR=lab.SR
def inst_freq(x):
    n=len(x); X=np.fft.fft(x); h=np.zeros(n); h[0]=1; h[1:n//2]=2; h[n//2]=1
    z=np.fft.ifft(X*h); ph=np.unwrap(np.angle(z)); return np.diff(ph)*SR/(2*np.pi)
ds=[10,20,30,40,50,60,70,80,90,95]
vs=[]
for d in ds:
    v=lab.simple_voice(32); o=v.op(1); o.ol=99
    v.lfo_speed=40; v.lfo_wave=4; v.lfo_pmd=60; v.pms=5; v.lfo_delay=d   # ~ +/-260 cents once fully faded in
    vs.append(v)
a=lab.render_voices(vs,note=69,hold=8.0,tail=0.1)
out={}
for d,x in zip(ds,a):
    f=inst_freq(x[:int(8*SR)].astype(np.float64))
    k=int(0.003*SR); f=np.convolve(f,np.ones(k)/k,mode='same')
    # vibrato depth envelope: peak-to-peak cents in 0.4 s windows
    w=int(0.4*SR); depth=[]
    for i in range(0,len(f)-w,int(0.1*SR)):
        s=f[i:i+w]; s=s[200:-200]
        depth.append((i/SR, 1200*np.log2(np.percentile(s,95)/np.percentile(s,5))/2))
    t=np.array([p[0] for p in depth]); dp=np.array([p[1] for p in depth])
    full=dp[-5:].mean()
    t10=t[np.argmax(dp>0.1*full)] if (dp>0.1*full).any() else None
    t50=t[np.argmax(dp>0.5*full)] if (dp>0.5*full).any() else None
    t90=t[np.argmax(dp>0.9*full)] if (dp>0.9*full).any() else None
    out[d]=(t10,t50,t90,full)
    print(f"delay {d:2d}: onset(10%)={t10}s half={t50}s full(90%)={t90}s  final depth={full:.0f}c")
