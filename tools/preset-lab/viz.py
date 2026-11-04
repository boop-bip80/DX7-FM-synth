"""viz.py - spectrogram contact sheets (numpy + PIL only) for eyeballing generated voices."""
import numpy as np
from PIL import Image, ImageDraw
import lab

SR = lab.SR


def _cmap(x):
    # 0..1 -> inferno-ish RGB
    x = np.clip(x, 0, 1)
    r = np.clip(1.5 * x, 0, 1)
    g = np.clip(2.0 * x - 0.6, 0, 1)
    b = np.clip(np.where(x < 0.5, 2 * x, 1.2 - 1.4 * x) + 0.1 * x, 0, 1)
    return (np.stack([r, g, b], axis=-1) * 255).astype(np.uint8)


def spec_image(a, dur=2.4, w=300, h=110, fmax=12000, dyn=70):
    n = 1024
    hop = int(dur * SR / w)
    a = a[: int(dur * SR) + n]
    cols = []
    win = np.hanning(n)
    for i in range(w):
        s = a[i * hop:i * hop + n]
        if len(s) < n:
            s = np.pad(s, (0, n - len(s)))
        cols.append(np.abs(np.fft.rfft(s * win)))
    S = np.array(cols).T                      # (bins, w)
    f = np.fft.rfftfreq(n, 1 / SR)
    # log-frequency resampling to h rows
    fr = np.geomspace(50, fmax, h)
    rows = np.array([S[np.searchsorted(f, x)] for x in fr[::-1]])
    d = 20 * np.log10(rows + 1e-7)
    d = (d - (d.max() - dyn)) / dyn
    img = _cmap(d)
    # waveform envelope strip
    return Image.fromarray(img)


def sheet(audio, names, path, cols=4, dur=2.4):
    tiles = []
    for a, nm in zip(audio, names):
        im = spec_image(a, dur)
        t = Image.new("RGB", (im.width, im.height + 14), (20, 20, 20))
        t.paste(im, (0, 14))
        ImageDraw.Draw(t).text((3, 1), nm, fill=(255, 255, 255))
        tiles.append(t)
    rows = (len(tiles) + cols - 1) // cols
    W, H = tiles[0].size
    out = Image.new("RGB", (W * cols, H * rows), (0, 0, 0))
    for i, t in enumerate(tiles):
        out.paste(t, ((i % cols) * W, (i // cols) * H))
    out.save(path)
