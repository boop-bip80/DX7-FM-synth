// Offline renderer for DX7 32-voice cartridges using the repo's msfa engine.
// usage: dx7render cart.syx out.f32 midinote velocity hold_seconds tail_seconds [sr]
// Writes 32 consecutive mono float32 renders, each (hold+tail)*sr samples long.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <memory>
#include <cmath>
#include "synth.h"
#include "exp2.h"
#include "sin.h"
#include "freqlut.h"
#include "lfo.h"
#include "env.h"
#include "pitchenv.h"
#include "porta.h"
#include "controllers.h"
#include "fm_core.h"
#include "dx7note.h"

void dexed_trace(const char*, const char*, ...) {}

static uint8_t normparm(uint8_t value, char max) {
    if (value <= max) return value;
    return ((float)value) / 255 * max;
}

// identical to Cartridge::unpackProgram
static void unpack(const uint8_t *bulk, uint8_t *ret) {
    for (int op = 0; op < 6; op++) {
        for (int i = 0; i < 11; i++) {
            uint8_t cp = bulk[op * 17 + i] & 0x7F;
            ret[op * 21 + i] = normparm(cp, 99);
        }
        memcpy(ret + op * 21, bulk + op * 17, 11);
        char lr = bulk[op * 17 + 11] & 0xF;
        ret[op * 21 + 11] = lr & 3;
        ret[op * 21 + 12] = (lr >> 2) & 3;
        char det_rs = bulk[op * 17 + 12] & 0x7F;
        ret[op * 21 + 13] = det_rs & 7;
        char kvs_ams = bulk[op * 17 + 13] & 0x1F;
        ret[op * 21 + 14] = kvs_ams & 3;
        ret[op * 21 + 15] = (kvs_ams >> 2) & 7;
        ret[op * 21 + 16] = bulk[op * 17 + 14] & 0x7F;
        char fc_mode = bulk[op * 17 + 15] & 0x3F;
        ret[op * 21 + 17] = fc_mode & 1;
        ret[op * 21 + 18] = (fc_mode >> 1) & 0x1F;
        ret[op * 21 + 19] = bulk[op * 17 + 16] & 0x7F;
        ret[op * 21 + 20] = (det_rs >> 3) & 0x7F;
    }
    for (int i = 0; i < 8; i++) {
        uint8_t cp = bulk[102 + i] & 0x7F;
        ret[126 + i] = normparm(cp, 99);
    }
    ret[134] = normparm(bulk[110] & 0x1F, 31);
    char oks_fb = bulk[111] & 0xF;
    ret[135] = oks_fb & 7;
    ret[136] = oks_fb >> 3;
    ret[137] = bulk[112] & 0x7F;
    ret[138] = bulk[113] & 0x7F;
    ret[139] = bulk[114] & 0x7F;
    ret[140] = bulk[115] & 0x7F;
    char lpms_lfw_lks = bulk[116] & 0x7F;
    ret[141] = lpms_lfw_lks & 1;
    ret[142] = (lpms_lfw_lks >> 1) & 7;
    ret[143] = lpms_lfw_lks >> 4;
    ret[144] = bulk[117] & 0x7F;
    for (int i = 0; i < 10; i++) ret[145 + i] = bulk[118 + i] & 0x7F;
    ret[155] = 0x3f;
}

int main(int argc, char **argv) {
    if (argc < 7) { fprintf(stderr, "usage\n"); return 2; }
    const char *in = argv[1], *outp = argv[2];
    int note = atoi(argv[3]), vel = atoi(argv[4]);
    double hold = atof(argv[5]), tail = atof(argv[6]);
    double sr = argc > 7 ? atof(argv[7]) : 44100.0;

    FILE *f = fopen(in, "rb");
    if (!f) { perror("open"); return 1; }
    static uint8_t buf[65535];
    size_t sz = fread(buf, 1, sizeof buf, f);
    fclose(f);
    if (sz < 4104 || buf[0] != 0xF0) { fprintf(stderr, "not a 32-voice cart\n"); return 1; }

    Exp2::init(); Sin::init();
    Freqlut::init(sr); Lfo::init(sr); PitchEnv::init(sr); Env::init_sr(sr);
    Porta::init_sr(sr);

    auto tuning = createStandardTuning();
    MTSClient mts;
    FmCore core;
    Controllers ctrls;
    ctrls.pitch_cc = 0x2000; ctrls.pitch_range_up = 3; ctrls.pitch_range_dn = 3; ctrls.pitch_step = 0;
    ctrls.modwheel_cc = ctrls.foot_cc = ctrls.breath_cc = ctrls.aftertouch_cc = 0;
    ctrls.portamento_enable_cc = false; ctrls.portamento_cc = 0; ctrls.portamento_gliss_cc = false;
    ctrls.masterTune = 0; ctrls.core = &core;
    ctrls.refresh();

    int total = (int)((hold + tail) * sr);
    total = (total / N + 1) * N;
    int holdSamples = (int)(hold * sr);
    FILE *o = fopen(outp, "wb");
    std::vector<float> out(total);

    for (int v = 0; v < 32; v++) {
        uint8_t patch[156];
        memset(patch, 0, sizeof patch);
        unpack(buf + 6 + v * 128, patch);

        Lfo lfo; lfo.reset(patch + 137); lfo.keydown();
        Dx7Note dn(tuning, &mts);
        int pn = note + (int)patch[144] - 24; if (pn < 0) pn = 0; if (pn > 127) pn = 127;  // same as DexedAudioProcessor::tuningTranspositionShift
        dn.init(patch, pn, vel, 1, &ctrls);
        if (patch[136]) dn.oscSync();
        bool released = false;
        for (int i = 0; i < total; i += N) {
            if (!released && i >= holdSamples) { dn.keyup(); released = true; }
            AlignedBuf<int32_t, N> audiobuf;
            for (int j = 0; j < N; j++) audiobuf.get()[j] = 0;
            int32_t lv = lfo.getsample(), ld = lfo.getdelay();
            dn.compute(audiobuf.get(), lv, ld, &ctrls);
            for (int j = 0; j < N; j++) {
                int32_t val = audiobuf.get()[j] >> 4;
                int clip = val < -(1 << 24) ? 0x8000 : val >= (1 << 24) ? 0x7fff : val >> 9;
                float fl = (float)clip / (float)0x8000;
                if (fl > 1) fl = 1; if (fl < -1) fl = -1;
                out[i + j] = fl;
            }
        }
        fwrite(out.data(), sizeof(float), total, o);
    }
    fclose(o);
    return 0;
}
