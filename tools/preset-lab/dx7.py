"""
dx7.py - minimal, dependency-free DX7 voice model and SysEx cartridge writer.

Everything here follows doc/sysex-format.txt and source/core/Cartridge.cpp:
  * a cartridge is F0 43 00 09 20 00 + 32 x 128 packed voice bytes + checksum + F7
  * packed voice byte order is OP6 first ... OP1 last, then pitch EG, algorithm,
    feedback, LFO, transpose and a 10 character name.

Operators are indexed 1..6 exactly as printed on the DX7 front panel (OP1 is the
operator nearest the output on most algorithms).  The engine algorithm table
below is copied from source/msfa/fm_core.cc and is *simulated* to derive each
algorithm's carriers and modulation routing, so recipes can reason about
topology instead of hard-coding it.
"""
from __future__ import annotations

import math
from dataclasses import dataclass, field
from typing import List, Sequence

SYSEX_HEADER = bytes([0xF0, 0x43, 0x00, 0x09, 0x20, 0x00])
BANK_SIZE = 32
VOICE_BYTES = 128

# --------------------------------------------------------------------------- #
# Algorithm topology (copied from source/msfa/fm_core.cc, index 0 == OP6)
# --------------------------------------------------------------------------- #
_ALG_FLAGS = [
    (0xc1, 0x11, 0x11, 0x14, 0x01, 0x14), (0x01, 0x11, 0x11, 0x14, 0xc1, 0x14),
    (0xc1, 0x11, 0x14, 0x01, 0x11, 0x14), (0xc1, 0x11, 0x94, 0x01, 0x11, 0x14),
    (0xc1, 0x14, 0x01, 0x14, 0x01, 0x14), (0xc1, 0x94, 0x01, 0x14, 0x01, 0x14),
    (0xc1, 0x11, 0x05, 0x14, 0x01, 0x14), (0x01, 0x11, 0xc5, 0x14, 0x01, 0x14),
    (0x01, 0x11, 0x05, 0x14, 0xc1, 0x14), (0x01, 0x05, 0x14, 0xc1, 0x11, 0x14),
    (0xc1, 0x05, 0x14, 0x01, 0x11, 0x14), (0x01, 0x05, 0x05, 0x14, 0xc1, 0x14),
    (0xc1, 0x05, 0x05, 0x14, 0x01, 0x14), (0xc1, 0x05, 0x11, 0x14, 0x01, 0x14),
    (0x01, 0x05, 0x11, 0x14, 0xc1, 0x14), (0xc1, 0x11, 0x02, 0x25, 0x05, 0x14),
    (0x01, 0x11, 0x02, 0x25, 0xc5, 0x14), (0x01, 0x11, 0x11, 0xc5, 0x05, 0x14),
    (0xc1, 0x14, 0x14, 0x01, 0x11, 0x14), (0x01, 0x05, 0x14, 0xc1, 0x14, 0x14),
    (0x01, 0x14, 0x14, 0xc1, 0x14, 0x14), (0xc1, 0x14, 0x14, 0x14, 0x01, 0x14),
    (0xc1, 0x14, 0x14, 0x01, 0x14, 0x04), (0xc1, 0x14, 0x14, 0x14, 0x04, 0x04),
    (0xc1, 0x14, 0x14, 0x04, 0x04, 0x04), (0xc1, 0x05, 0x14, 0x01, 0x14, 0x04),
    (0x01, 0x05, 0x14, 0xc1, 0x14, 0x04), (0x04, 0xc1, 0x11, 0x14, 0x01, 0x14),
    (0xc1, 0x14, 0x01, 0x14, 0x04, 0x04), (0x04, 0xc1, 0x11, 0x14, 0x04, 0x04),
    (0xc1, 0x14, 0x04, 0x04, 0x04, 0x04), (0xc4, 0x04, 0x04, 0x04, 0x04, 0x04),
]


@dataclass(frozen=True)
class Topology:
    algo: int                       # 1..32
    carriers: tuple                 # operator numbers (1..6) that reach the output
    mods: dict                      # op -> tuple of ops that modulate it (excluding feedback)
    feedback_op: int                # operator that owns the feedback path
    feedback_ops: tuple             # all operators inside the feedback loop

    def depth(self, op: int) -> int:
        """Length of the longest modulation chain feeding `op` (0 = unmodulated)."""
        m = self.mods.get(op, ())
        return 0 if not m else 1 + max(self.depth(x) for x in m)


def _simulate(algo_index: int) -> Topology:
    flags = _ALG_FLAGS[algo_index]
    bus = {1: set(), 2: set()}
    carriers, mods, fb_ops = [], {}, []
    fb_op = None
    for i, fl in enumerate(flags):            # i == 0 -> OP6
        op = 6 - i
        inbus = (fl >> 4) & 3
        outbus = fl & 3
        add = bool(fl & 4)
        mods[op] = tuple(sorted(bus[inbus])) if inbus else ()
        if fl & 0x40:                          # FB_IN
            fb_op = op
        if fl & 0xC0:
            fb_ops.append(op)
        if outbus == 0:
            carriers.append(op)
        else:
            if add:
                bus[outbus].add(op)
            else:
                bus[outbus] = {op}
    return Topology(algo_index + 1, tuple(sorted(carriers)), mods, fb_op, tuple(sorted(fb_ops)))


TOPOLOGY = {a + 1: _simulate(a) for a in range(32)}


def topo(algo: int) -> Topology:
    return TOPOLOGY[algo]


# --------------------------------------------------------------------------- #
# Voice model
# --------------------------------------------------------------------------- #
@dataclass
class Op:
    rates: List[int] = field(default_factory=lambda: [99, 99, 99, 99])
    levels: List[int] = field(default_factory=lambda: [99, 99, 99, 0])
    bp: int = 39            # keyboard level scaling break point (39 == C3)
    ld: int = 0             # left depth
    rd: int = 0             # right depth
    lc: int = 0             # left curve 0=-LIN 1=-EXP 2=+EXP 3=+LIN
    rc: int = 0             # right curve
    rs: int = 0             # rate scaling 0..7
    ams: int = 0            # amp mod sensitivity 0..3
    kvs: int = 0            # key velocity sensitivity 0..7
    ol: int = 0             # output level 0..99
    mode: int = 0           # 0 ratio, 1 fixed
    coarse: int = 1         # 0..31
    fine: int = 0           # 0..99
    det: int = 7            # 0..14, 7 = centre

    def clamp(self) -> "Op":
        c = lambda v, lo, hi: max(lo, min(hi, int(round(v))))
        self.rates = [c(v, 0, 99) for v in self.rates]
        self.levels = [c(v, 0, 99) for v in self.levels]
        self.bp, self.ld, self.rd = c(self.bp, 0, 99), c(self.ld, 0, 99), c(self.rd, 0, 99)
        self.lc, self.rc = c(self.lc, 0, 3), c(self.rc, 0, 3)
        self.rs, self.ams, self.kvs = c(self.rs, 0, 7), c(self.ams, 0, 3), c(self.kvs, 0, 7)
        self.ol, self.mode = c(self.ol, 0, 99), c(self.mode, 0, 1)
        self.coarse, self.fine, self.det = c(self.coarse, 0, 31), c(self.fine, 0, 99), c(self.det, 0, 14)
        return self


@dataclass
class Voice:
    name: str = "INIT VOICE"
    ops: List[Op] = field(default_factory=lambda: [Op() for _ in range(6)])  # ops[0] == OP1
    peg_rates: List[int] = field(default_factory=lambda: [99, 99, 99, 99])
    peg_levels: List[int] = field(default_factory=lambda: [50, 50, 50, 50])
    algo: int = 1           # 1..32
    fb: int = 0             # 0..7
    oks: int = 1            # oscillator key sync
    lfo_speed: int = 35
    lfo_delay: int = 0
    lfo_pmd: int = 0
    lfo_amd: int = 0
    lfo_sync: int = 1
    lfo_wave: int = 0       # 0 TRI 1 SAW-DN 2 SAW-UP 3 SQR 4 SIN 5 S&H
    pms: int = 0            # pitch mod sensitivity 0..7
    transpose: int = 24     # 0..48, 24 == no transpose (C3)
    meta: dict = field(default_factory=dict, compare=False, repr=False)  # not written to SysEx

    def op(self, n: int) -> Op:
        """Operator by panel number 1..6."""
        return self.ops[n - 1]

    def clamp(self) -> "Voice":
        c = lambda v, lo, hi: max(lo, min(hi, int(round(v))))
        for o in self.ops:
            o.clamp()
        self.peg_rates = [c(v, 0, 99) for v in self.peg_rates]
        self.peg_levels = [c(v, 0, 99) for v in self.peg_levels]
        self.algo, self.fb, self.oks = c(self.algo, 1, 32), c(self.fb, 0, 7), c(self.oks, 0, 1)
        self.lfo_speed, self.lfo_delay = c(self.lfo_speed, 0, 99), c(self.lfo_delay, 0, 99)
        self.lfo_pmd, self.lfo_amd = c(self.lfo_pmd, 0, 99), c(self.lfo_amd, 0, 99)
        self.lfo_sync, self.lfo_wave, self.pms = c(self.lfo_sync, 0, 1), c(self.lfo_wave, 0, 5), c(self.pms, 0, 7)
        self.transpose = c(self.transpose, 0, 48)
        return self


def sanitize_name(name: str) -> str:
    """DX7 names are 10 printable ASCII characters (0x20..0x7E)."""
    n = "".join(ch if 32 <= ord(ch) < 127 else " " for ch in name.upper())
    return n[:10].ljust(10)


def pack_voice(v: Voice) -> bytes:
    v.clamp()
    out = bytearray(VOICE_BYTES)
    for i in range(6):                         # packed order: OP6 .. OP1
        o = v.ops[5 - i]
        p = i * 17
        out[p:p + 4] = bytes(o.rates)
        out[p + 4:p + 8] = bytes(o.levels)
        out[p + 8], out[p + 9], out[p + 10] = o.bp, o.ld, o.rd
        out[p + 11] = (o.lc & 3) | ((o.rc & 3) << 2)
        out[p + 12] = (o.rs & 7) | ((o.det & 15) << 3)
        out[p + 13] = (o.ams & 3) | ((o.kvs & 7) << 2)
        out[p + 14] = o.ol
        out[p + 15] = (o.mode & 1) | ((o.coarse & 31) << 1)
        out[p + 16] = o.fine
    out[102:106] = bytes(v.peg_rates)
    out[106:110] = bytes(v.peg_levels)
    out[110] = (v.algo - 1) & 31
    out[111] = (v.fb & 7) | ((v.oks & 1) << 3)
    out[112], out[113], out[114], out[115] = v.lfo_speed, v.lfo_delay, v.lfo_pmd, v.lfo_amd
    out[116] = (v.lfo_sync & 1) | ((v.lfo_wave & 7) << 1) | ((v.pms & 7) << 4)
    out[117] = v.transpose
    out[118:128] = sanitize_name(v.name).encode("ascii")
    return bytes(out)


def unpack_voice(b: bytes) -> Voice:
    assert len(b) == VOICE_BYTES
    v = Voice()
    for i in range(6):
        o = v.ops[5 - i]
        p = i * 17
        o.rates = list(b[p:p + 4])
        o.levels = list(b[p + 4:p + 8])
        o.bp, o.ld, o.rd = b[p + 8], b[p + 9], b[p + 10]
        o.lc, o.rc = b[p + 11] & 3, (b[p + 11] >> 2) & 3
        o.rs, o.det = b[p + 12] & 7, (b[p + 12] >> 3) & 15
        o.ams, o.kvs = b[p + 13] & 3, (b[p + 13] >> 2) & 7
        o.ol = b[p + 14]
        o.mode, o.coarse = b[p + 15] & 1, (b[p + 15] >> 1) & 31
        o.fine = b[p + 16]
    v.peg_rates, v.peg_levels = list(b[102:106]), list(b[106:110])
    v.algo = (b[110] & 31) + 1
    v.fb, v.oks = b[111] & 7, (b[111] >> 3) & 1
    v.lfo_speed, v.lfo_delay, v.lfo_pmd, v.lfo_amd = b[112], b[113], b[114], b[115]
    v.lfo_sync, v.lfo_wave, v.pms = b[116] & 1, (b[116] >> 1) & 7, (b[116] >> 4) & 7
    v.transpose = b[117]
    v.name = bytes(b[118:128]).decode("ascii", "replace")
    return v


def checksum(data: bytes) -> int:
    return (-sum(data)) & 0x7F


def write_bank(voices: Sequence[Voice]) -> bytes:
    if len(voices) != BANK_SIZE:
        raise ValueError(f"a cartridge holds exactly {BANK_SIZE} voices, got {len(voices)}")
    body = b"".join(pack_voice(v) for v in voices)
    return SYSEX_HEADER + body + bytes([checksum(body), 0xF7])


def read_bank(raw: bytes) -> List[Voice]:
    if len(raw) != 4104 or raw[:6] != SYSEX_HEADER or raw[-1] != 0xF7:
        raise ValueError("not a 32-voice DX7 cartridge")
    body = raw[6:6 + 4096]
    if checksum(body) != raw[4102]:
        raise ValueError("bad checksum")
    return [unpack_voice(body[i * 128:(i + 1) * 128]) for i in range(32)]


# --------------------------------------------------------------------------- #
# Unit helpers (all derived from the engine source, see calibrate.py)
# --------------------------------------------------------------------------- #
_LEVELLUT = [0, 5, 9, 13, 17, 20, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 42, 43, 45, 46]


def level_units(l: int) -> int:
    """Engine 'scaleoutlevel': 0..99 -> 0..127 steps of 0.75 dB."""
    l = max(0, min(99, int(round(l))))
    return 28 + l if l >= 20 else _LEVELLUT[l]


def level_db(l: int) -> float:
    return -0.75 * (127 - level_units(l))


def level_for_db(db: float) -> int:
    """Smallest 0..99 level that is no louder than `db` (<= 0)."""
    best, besterr = 0, 1e9
    for l in range(100):
        e = abs(level_db(l) - db)
        if e < besterr:
            best, besterr = l, e
    return best


def ratio_to_coarse_fine(r: float):
    """Frequency ratio -> (coarse, fine) in ratio mode (coarse 0 == x0.5)."""
    if r <= 0:
        raise ValueError("ratio must be positive")
    if r < 1.0:
        coarse = 0
        base = 0.5
    else:
        coarse = min(31, int(r))
        base = float(coarse)
    fine = round((r / base - 1.0) * 100.0)
    if fine > 99:
        # fall through to next coarse step
        if coarse == 0:
            coarse, base = 1, 1.0
            fine = round((r / base - 1.0) * 100.0)
        elif coarse < 31:
            coarse += 1
            fine = max(0, round((r / coarse - 1.0) * 100.0))
    fine = max(0, min(99, fine))
    return coarse, fine


def coarse_fine_to_ratio(coarse: int, fine: int) -> float:
    return (0.5 if coarse == 0 else float(coarse)) * (1.0 + fine / 100.0)


def hz_to_fixed(hz: float):
    """Fixed-frequency mode: f = 10^(coarse%4 + fine/100) Hz."""
    hz = max(1.0, min(9772.0, hz))
    e = math.log10(hz)
    coarse = min(3, int(e))
    fine = min(99, round((e - coarse) * 100))
    return coarse, fine


def midi_to_bp(note: int) -> int:
    """Break-point value for a MIDI note (C3 = 60 -> 39)."""
    return max(0, min(99, note - 21))


# Pitch EG: level -> semitones, rate -> octaves per second (engine tables)
_PITCHENV_TAB = [
    -128, -116, -104, -95, -85, -76, -68, -61, -56, -52, -49, -46, -43, -41, -39, -37, -35, -33, -32, -31, -30,
    -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8,
    -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 38, 40, 43, 46, 49, 53, 58, 65, 73, 82, 92, 103, 115, 127,
]
_PITCHENV_RATE = [
    1, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 16, 16, 17, 18, 18,
    19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 30, 31, 33, 34, 36, 37, 38, 39, 41, 42, 44, 46, 47, 49, 51, 53, 54, 56,
    58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 79, 82, 85, 88, 91, 94, 98, 102, 106, 110, 115, 120, 125, 130, 135,
    141, 147, 153, 159, 165, 171, 178, 185, 193, 202, 211, 232, 243, 254, 255,
]
assert len(_PITCHENV_TAB) == 100 and len(_PITCHENV_RATE) == 100


def peg_semitones(level: int) -> float:
    return _PITCHENV_TAB[max(0, min(99, level))] * 12.0 / 32.0


def peg_level_for_semitones(semi: float) -> int:
    best, err = 50, 1e9
    for l in range(100):
        e = abs(peg_semitones(l) - semi)
        if e < err:
            best, err = l, e
    return best


def peg_rate_for_time(semitones: float, seconds: float) -> int:
    """Pitch EG rate that sweeps `semitones` in about `seconds`."""
    oct_per_s = abs(semitones) / 12.0 / max(seconds, 1e-3)
    want = oct_per_s * 21.3
    best, err = 99, 1e9
    for r in range(100):
        e = abs(_PITCHENV_RATE[r] - want)
        if e < err:
            best, err = r, e
    return best
