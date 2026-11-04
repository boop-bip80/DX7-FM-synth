#include "tuning.h"
struct StandardTuning : public TuningState {
    int t[128];
    StandardTuning() {
        const int base = 50857777; const int step = (1 << 24) / 12;
        for (int mn = 0; mn < 128; ++mn) t[mn] = base + step * mn;
    }
    int32_t midinote_to_logfreq(int n) override { return t[n]; }
};
std::shared_ptr<TuningState> createStandardTuning() { return std::make_shared<StandardTuning>(); }
