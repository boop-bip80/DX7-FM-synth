#include "EnvDisplay.h"
#include "../DXLookNFeel.h"
#include "parameter/Model.h"
#include "msfa/pitchenv.h"

static float EG_rate_rise_duration[128] = {
    38.00000 ,34.96000 ,31.92000 ,28.88000 ,25.84000 ,
    22.80000 ,20.64000 ,18.48000 ,16.32000 ,14.16000 ,
    12.00000 ,11.10000 ,10.20000 ,9.30000 ,8.40000 ,
    7.50000 ,6.96000 ,6.42000 ,5.88000 ,5.34000 ,
    4.80000 ,4.38000 ,3.96000 ,3.54000 ,3.12000 ,
    2.70000 ,2.52000 ,2.34000 ,2.16000 ,1.98000 ,
    1.80000 ,1.70000 ,1.60000 ,1.50000 ,1.40000 ,
    1.30000 ,1.22962 ,1.15925 ,1.08887 ,1.01850 ,
    0.94813 ,0.87775 ,0.80737 ,0.73700 ,0.69633 ,
    0.65567 ,0.61500 ,0.57833 ,0.54167 ,0.50500 ,
    0.47300 ,0.44100 ,0.40900 ,0.37967 ,0.35033 ,
    0.32100 ,0.28083 ,0.24067 ,0.20050 ,0.16033 ,
    0.12017 ,0.08000 ,0.07583 ,0.07167 ,0.06750 ,
    0.06333 ,0.05917 ,0.05500 ,0.04350 ,0.03200 ,
    0.02933 ,0.02667 ,0.02400 ,0.02200 ,0.02000 ,
    0.01800 ,0.01667 ,0.01533 ,0.01400 ,0.01300 ,
    0.01200 ,0.01100 ,0.01000 ,0.00900 ,0.00800 ,
    0.00800 ,0.00800 ,0.00800 ,0.00767 ,0.00733 ,
    0.00700 ,0.00633 ,0.00567 ,0.00500 ,0.00433 ,
    0.00367 ,0.00300 ,0.00300 ,0.00300 ,0.00300 ,
    0.00300 ,0.00300 ,0.00300 ,0.00300 ,0.00300 ,
    0.00300 ,0.00300 ,0.00300 ,0.00300 ,0.00300 ,
    0.00300 ,0.00300 ,0.00300 ,0.00300 ,0.00300 ,
    0.00300 ,0.00300 ,0.00300 ,0.00300 ,0.00300 ,
    0.00300 ,0.00300 ,0.00300 ,0.00300 ,0.00300 ,
    0.00300 ,0.00300 ,0.00300
};

static float EG_rate_decay_duration[128] = {
    318.00000 ,283.75000 ,249.50000 ,215.25000 ,181.00000 ,
    167.80000 ,154.60001 ,141.39999 ,128.20000 ,115.00000 ,
    104.60000 ,94.20000 ,83.80000 ,73.40000 ,63.00000 ,
    58.34000 ,53.68000 ,49.02000 ,44.36000 ,39.70000 ,
    35.76000 ,31.82000 ,27.88000 ,23.94000 ,20.00000 ,
    18.24000 ,16.48000 ,14.72000 ,12.96000 ,11.20000 ,
    10.36000 ,9.52000 ,8.68000 ,7.84000 ,7.00000 ,
    6.83250 ,6.66500 ,6.49750 ,6.33000 ,6.16250 ,
    5.99500 ,5.82750 ,5.66000 ,5.10000 ,4.54000 ,
    3.98000 ,3.64833 ,3.31667 ,2.98500 ,2.65333 ,
    2.32167 ,1.99000 ,1.77333 ,1.55667 ,1.34000 ,
    1.22333 ,1.10667 ,0.99000 ,0.89667 ,0.80333 ,
    0.71000 ,0.65000 ,0.59000 ,0.53000 ,0.47000 ,
    0.41000 ,0.32333 ,0.23667 ,0.15000 ,0.12700 ,
    0.10400 ,0.08100 ,0.07667 ,0.07233 ,0.06800 ,
    0.06100 ,0.05400 ,0.04700 ,0.04367 ,0.04033 ,
    0.03700 ,0.03300 ,0.02900 ,0.02500 ,0.02333 ,
    0.02167 ,0.02000 ,0.01767 ,0.01533 ,0.01300 ,
    0.01133 ,0.00967 ,0.00800 ,0.00800 ,0.00800 ,
    0.00800 ,0.00800 ,0.00800 ,0.00800 ,0.00800 ,
    0.00800 ,0.00800 ,0.00800 ,0.00800 ,0.00800 ,
    0.00800 ,0.00800 ,0.00800 ,0.00800 ,0.00800 ,
    0.00800 ,0.00800 ,0.00800 ,0.00800 ,0.00800 ,
    0.00800 ,0.00800 ,0.00800 ,0.00800 ,0.00800 ,
    0.00800 ,0.00800 ,0.00800 ,0.00800 ,0.00800 ,
    0.00800 ,0.00800 ,0.00800
};

static float EG_rate_decay_percent[128] = {
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00501 ,0.01001 ,0.01500 ,
    0.02000 ,0.02800 ,0.03600 ,0.04400 ,0.05200 ,
    0.06000 ,0.06800 ,0.07600 ,0.08400 ,0.09200 ,
    0.10000 ,0.10800 ,0.11600 ,0.12400 ,0.13200 ,
    0.14000 ,0.15000 ,0.16000 ,0.17000 ,0.18000 ,
    0.19000 ,0.20000 ,0.21000 ,0.22000 ,0.23000 ,
    0.24000 ,0.25100 ,0.26200 ,0.27300 ,0.28400 ,
    0.29500 ,0.30600 ,0.31700 ,0.32800 ,0.33900 ,
    0.35000 ,0.36500 ,0.38000 ,0.39500 ,0.41000 ,
    0.42500 ,0.44000 ,0.45500 ,0.47000 ,0.48500 ,
    0.50000 ,0.52000 ,0.54000 ,0.56000 ,0.58000 ,
    0.60000 ,0.62000 ,0.64000 ,0.66000 ,0.68000 ,
    0.70000 ,0.73200 ,0.76400 ,0.79600 ,0.82800 ,
    0.86000 ,0.89500 ,0.93000 ,0.96500 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000

};

static float EG_rate_rise_percent[128] = {
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00001 ,0.00001 ,0.00001 ,
    0.00001 ,0.00001 ,0.00501 ,0.01001 ,0.01500 ,
    0.02000 ,0.02800 ,0.03600 ,0.04400 ,0.05200 ,
    0.06000 ,0.06800 ,0.07600 ,0.08400 ,0.09200 ,
    0.10000 ,0.10800 ,0.11600 ,0.12400 ,0.13200 ,
    0.14000 ,0.15000 ,0.16000 ,0.17000 ,0.18000 ,
    0.19000 ,0.20000 ,0.21000 ,0.22000 ,0.23000 ,
    0.24000 ,0.25100 ,0.26200 ,0.27300 ,0.28400 ,
    0.29500 ,0.30600 ,0.31700 ,0.32800 ,0.33900 ,
    0.35000 ,0.36500 ,0.38000 ,0.39500 ,0.41000 ,
    0.42500 ,0.44000 ,0.45500 ,0.47000 ,0.48500 ,
    0.50000 ,0.52000 ,0.54000 ,0.56000 ,0.58000 ,
    0.60000 ,0.62000 ,0.64000 ,0.66000 ,0.68000 ,
    0.70000 ,0.73200 ,0.76400 ,0.79600 ,0.82800 ,
    0.86000 ,0.89500 ,0.93000 ,0.96500 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000 ,1.00000 ,1.00000 ,
    1.00000 ,1.00000 ,1.00000
};

static double getDuration(int p_rate, int p_level_l, int p_level_r) {
    float *duration_table=(p_level_r>p_level_l) ? EG_rate_rise_duration : EG_rate_decay_duration;
    double duration=duration_table[p_rate];

    float* percent_table =(p_level_r>p_level_l) ? EG_rate_rise_percent  : EG_rate_decay_percent;
    duration *= fabs(percent_table[p_level_r]-percent_table[p_level_l]);
    return duration;
}

EnvDisplay::EnvDisplay(DexedApvts &apvts, int op) {
    for (int i=0; i<4; i++) {
        rates[i].referTo(apvts, IDs::egRate.op(op).idx(i).name);
        levels[i].referTo(apvts, IDs::egLevel.op(op).idx(i).name);
        rates[i].callback = [this] { repaint(); };
        levels[i].callback = [this] { repaint(); };
    }
}

void EnvDisplay::paint(Graphics &g) {
    const auto frame = getLocalBounds().toFloat().reduced(0.5f);
    if (frame.getWidth() < 12.0f || frame.getHeight() < 12.0f)
        return;

    g.setColour(Colour(0xff0e1b22));
    g.fillRoundedRectangle(frame, 4.0f);
    g.setColour(Colour(0xff3c5360));
    g.drawRoundedRectangle(frame, 4.0f, 0.8f);

    const auto graph = frame.reduced(4.0f, 3.0f);
    for (int tick = 1; tick < 4; ++tick) {
        const float x = graph.getX() + graph.getWidth() * tick * 0.25f;
        g.setColour(Colour(0xff38505c).withAlpha(0.46f));
        g.drawLine(x, graph.getY(), x, graph.getBottom(), 0.65f);
    }
    g.setColour(Colour(0xff3e5661).withAlpha(0.58f));
    g.drawLine(graph.getX(), graph.getCentreY(), graph.getRight(), graph.getCentreY(), 0.7f);

    double durations[4] = {
        getDuration(rates[0], levels[3], levels[0]),
        getDuration(rates[1], levels[0], levels[1]),
        getDuration(rates[2], levels[1], levels[2]),
        getDuration(rates[3], levels[2], levels[3])
    };
    for (auto& duration : durations)
        duration = jmax(0.0001, duration);

    const double sustainTime = durations[0] + durations[1] + durations[2];
    const double holdTime = jmax(0.12, sustainTime * 0.16);
    const double totalTime = sustainTime + holdTime + durations[3];
    const double pixelsPerSecond = graph.getWidth() / totalTime;
    const auto y = [&graph](int level) {
        return graph.getBottom() - graph.getHeight() * jlimit(0, 99, level) / 99.0f;
    };

    Point<float> points[6] = {
        {graph.getX(), y(levels[3])},
        {graph.getX() + (float)(durations[0] * pixelsPerSecond), y(levels[0])},
        {graph.getX() + (float)((durations[0] + durations[1]) * pixelsPerSecond), y(levels[1])},
        {graph.getX() + (float)(sustainTime * pixelsPerSecond), y(levels[2])},
        {graph.getX() + (float)((sustainTime + holdTime) * pixelsPerSecond), y(levels[2])},
        {graph.getRight(), y(levels[3])}
    };

    g.setColour(Colour(0xfff2b563).withAlpha(0.30f));
    g.drawLine(points[4].x, graph.getY(), points[4].x, graph.getBottom(), 1.0f);

    Path envelope;
    envelope.startNewSubPath(points[0]);
    for (int i = 1; i < 6; ++i)
        envelope.lineTo(points[i]);

    Path fill(envelope);
    fill.lineTo(graph.getRight(), graph.getBottom());
    fill.lineTo(graph.getX(), graph.getBottom());
    fill.closeSubPath();
    g.setGradientFill(ColourGradient(Colour(0x8555d7c7), graph.getCentreX(), graph.getY(),
                                      Colour(0x1155d7c7), graph.getCentreX(), graph.getBottom(), false));
    g.fillPath(fill);
    g.setColour(Colour(0xff6ee7d8));
    g.strokePath(envelope, PathStrokeType(1.75f, PathStrokeType::curved, PathStrokeType::rounded));

    const auto active = jlimit(0, 5, static_cast<int>(vPos));
    for (int i = 0; i < 6; ++i) {
        if (i != active && i != active + 1)
            continue;
        g.setColour(i == active ? Colour(0xfff2b563) : Colour(0xffe8e6da));
        const float radius = i == active ? 2.6f : 1.6f;
        g.fillEllipse(points[i].x - radius, points[i].y - radius, radius * 2.0f, radius * 2.0f);
    }
}

PitchEnvDisplay::PitchEnvDisplay(DexedApvts &apvts) {
    for (int i=0; i<4; i++) {
        rates[i].referTo(apvts, IDs::pitchEgRate.idx(i).name);
        levels[i].referTo(apvts, IDs::pitchEgLevel.idx(i).name);
        rates[i].callback = [this] { repaint(); };
        levels[i].callback = [this] { repaint(); };
    }
}

void PitchEnvDisplay::paint(Graphics &g) {
    const auto frame = getLocalBounds().toFloat().reduced(0.5f);
    if (frame.getWidth() < 12.0f || frame.getHeight() < 12.0f)
        return;

    g.setColour(Colour(0xff0e1b22));
    g.fillRoundedRectangle(frame, 4.0f);
    g.setColour(Colour(0xff3c5360));
    g.drawRoundedRectangle(frame, 4.0f, 0.8f);

    const auto graph = frame.reduced(4.0f, 3.0f);
    g.setColour(Colour(0xff425a66).withAlpha(0.53f));
    g.drawLine(graph.getX(), graph.getCentreY(), graph.getRight(), graph.getCentreY(), 0.8f);
    for (int tick = 1; tick < 4; ++tick) {
        const float x = graph.getX() + graph.getWidth() * tick * 0.25f;
        g.drawLine(x, graph.getY(), x, graph.getBottom(), 0.65f);
    }

    float dist[4], total = 0.0f;
    int previous = pitchenv_tab[jlimit(0, 99, static_cast<int>(levels[3]))] + 128;
    for (int i = 0; i < 4; ++i) {
        const int current = pitchenv_tab[jlimit(0, 99, static_cast<int>(levels[i]))] + 128;
        const float speed = jmax(0.0001f, static_cast<float>(pitchenv_rate[jlimit(0, 99, static_cast<int>(rates[i]))]));
        dist[i] = std::abs(current - previous) / speed;
        total += dist[i];
        previous = current;
    }
    if (total < 0.00001f) {
        for (auto& duration : dist)
            duration = 1.0f;
        total = 4.0f;
    }

    const float stepScale = graph.getWidth() / total;
    const auto y = [&graph](int level) {
        const float normalised = (pitchenv_tab[jlimit(0, 99, level)] + 128) / 255.0f;
        return graph.getBottom() - graph.getHeight() * normalised;
    };
    Point<float> points[5] = {{graph.getX(), y(levels[3])}};
    float x = graph.getX();
    for (int i = 0; i < 4; ++i) {
        x += dist[i] * stepScale;
        points[i + 1] = {x, y(levels[i])};
    }

    Path envelope;
    envelope.startNewSubPath(points[0]);
    for (int i = 1; i < 5; ++i)
        envelope.lineTo(points[i]);
    Path fill(envelope);
    fill.lineTo(graph.getRight(), graph.getBottom());
    fill.lineTo(graph.getX(), graph.getBottom());
    fill.closeSubPath();
    g.setGradientFill(ColourGradient(Colour(0x8855d7c7), graph.getCentreX(), graph.getY(),
                                      Colour(0x1155d7c7), graph.getCentreX(), graph.getBottom(), false));
    g.fillPath(fill);
    g.setColour(Colour(0xff6ee7d8));
    g.strokePath(envelope, PathStrokeType(1.75f, PathStrokeType::curved, PathStrokeType::rounded));

    const int active = jlimit(0, 4, static_cast<int>(vPos));
    g.setColour(Colour(0xfff2b563));
    g.fillEllipse(points[active].x - 2.6f, points[active].y - 2.6f, 5.2f, 5.2f);
}
