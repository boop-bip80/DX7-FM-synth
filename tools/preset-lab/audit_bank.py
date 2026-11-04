import sys, importlib; sys.path.insert(0,'.')
import numpy as np, audit, viz
mod=importlib.import_module('banks.'+sys.argv[1]); idx=int(sys.argv[2]) if len(sys.argv)>2 else 0
sec,title,vs=mod.BANKS[idx]
print(sec,title,len(vs))
assert len(vs)==32, len(vs)
audio,mets=audit.run(vs)
for i,(v,m) in enumerate(zip(vs,mets)):
    flag=("S" if m['silent'] else "")+("C" if m['clipped'] else "")
    print(f"{i:2d} {v.name:10s}", audit.fmt(m))
viz.sheet(audio,[f"{i} {v.name}" for i,v in enumerate(vs)],'build/bank_sheet.png',cols=8,dur=2.4)
fp=audit.fingerprints(audio); pairs=audit.nearest_pairs(fp, float(sys.argv[3]) if len(sys.argv)>3 else 1.6)
print("near-duplicate pairs (<thresh dB):"); 
for i,j,d in pairs[:15]: print(f"  {vs[i].name:10s} ~ {vs[j].name:10s} d={d:.2f}")
