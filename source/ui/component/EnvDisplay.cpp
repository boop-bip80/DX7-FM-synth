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

//==============================================================================
juce::Rectangle<float> EnvelopeViewBase::graphArea() const {
    auto frame = getLocalBounds().toFloat().reduced(0.5f);
    if (showLabels)
        return frame.reduced(14.0f, 12.0f).withTrimmedLeft(10.0f);
    return frame.reduced(5.0f, 4.0f);
}

float EnvelopeViewBase::levelToY(juce::Rectangle<float> graph, int level) const {
    return graph.getBottom() - graph.getHeight() * jlimit(0, 99, level) / 99.0f;
}

void EnvelopeViewBase::setParam(juce::RangedAudioParameter* p, int value) {
    if (p == nullptr)
        return;
    const auto range = p->getNormalisableRange();
    p->setValueNotifyingHost(p->convertTo0to1(jlimit(range.start, range.end, (float) value)));
}

int EnvelopeViewBase::getParam(juce::RangedAudioParameter* p) {
    return p == nullptr ? 0 : roundToInt(p->convertFrom0to1(p->getValue()));
}

int EnvelopeViewBase::findPoint(juce::Point<float> p) const {
    int best = -1;
    float bestDistance = 11.0f;
    for (int i = 0; i < points.size(); ++i) {
        if (pointStage[i] < 0)
            continue;
        const float d = points[i].getDistanceFrom(p);
        if (d < bestDistance) {
            bestDistance = d;
            best = i;
        }
    }
    return best;
}

void EnvelopeViewBase::mouseMove(const juce::MouseEvent& e) {
    if (!editable)
        return;
    const int p = findPoint(e.position);
    if (p != hoverPoint) {
        hoverPoint = p;
        repaint();
    }
}

void EnvelopeViewBase::mouseExit(const juce::MouseEvent&) {
    if (hoverPoint != -1) {
        hoverPoint = -1;
        repaint();
    }
}

void EnvelopeViewBase::mouseDown(const juce::MouseEvent& e) {
    if (!editable || e.mods.isPopupMenu())
        return;
    dragPoint = findPoint(e.position);
    if (dragPoint < 0)
        return;
    const int stage = pointStage[dragPoint];
    dragStartRate = getParam(rateParams[stage]);
    dragStartLevel = getParam(levelParams[stage]);
    if (rateParams[stage] != nullptr) rateParams[stage]->beginChangeGesture();
    if (levelParams[stage] != nullptr) levelParams[stage]->beginChangeGesture();
}

void EnvelopeViewBase::mouseDrag(const juce::MouseEvent& e) {
    if (dragPoint < 0)
        return;
    const int stage = pointStage[dragPoint];
    const auto graph = graphArea();
    const float levelDelta = -e.getDistanceFromDragStartY() * 99.0f / jmax(1.0f, graph.getHeight());
    const float rateDelta = -e.getDistanceFromDragStartX() * 99.0f / jmax(1.0f, graph.getWidth() * 0.8f);
    setParam(levelParams[stage], roundToInt(dragStartLevel + levelDelta));
    if (!e.mods.isShiftDown()) // Shift locks the rate so only the level moves.
        setParam(rateParams[stage], roundToInt(dragStartRate + rateDelta));
}

void EnvelopeViewBase::mouseUp(const juce::MouseEvent&) {
    if (dragPoint < 0)
        return;
    const int stage = pointStage[dragPoint];
    if (rateParams[stage] != nullptr) rateParams[stage]->endChangeGesture();
    if (levelParams[stage] != nullptr) levelParams[stage]->endChangeGesture();
    dragPoint = -1;
}

void EnvelopeViewBase::drawFrame(juce::Graphics& g, juce::Rectangle<float> graph) {
    const auto frame = getLocalBounds().toFloat().reduced(0.5f);
    g.setColour(Colour(0xff141517));
    g.fillRoundedRectangle(frame, 4.0f);
    g.setColour(Colour(0xff3c3d42));
    g.drawRoundedRectangle(frame, 4.0f, 1.0f);

    const int divisions = showLabels ? 8 : 4;
    for (int tick = 1; tick < divisions; ++tick) {
        const float x = graph.getX() + graph.getWidth() * tick / (float) divisions;
        g.setColour(Colours::white.withAlpha(0.05f));
        g.drawLine(x, graph.getY(), x, graph.getBottom(), 0.7f);
    }
    for (int tick = 0; tick <= 4; ++tick) {
        const float y = graph.getY() + graph.getHeight() * tick / 4.0f;
        g.setColour(Colours::white.withAlpha(tick == 4 ? 0.12f : 0.05f));
        g.drawLine(graph.getX(), y, graph.getRight(), y, 0.7f);
        if (showLabels) {
            g.setColour(Colour(0xff66666a));
            g.setFont(juce::Font(juce::FontOptions().withHeight(8.5f)));
            g.drawText(String(roundToInt(99.0f * (1.0f - tick / 4.0f))),
                       Rectangle<float>(graph.getX() - 24.0f, y - 6.0f, 20.0f, 12.0f).toNearestInt(),
                       Justification::centredRight, false);
        }
    }
}

void EnvelopeViewBase::drawCurve(juce::Graphics& g, juce::Rectangle<float> graph, int activeIndex) {
    if (points.size() < 2)
        return;
    Path envelope;
    envelope.startNewSubPath(points[0]);
    for (int i = 1; i < points.size(); ++i)
        envelope.lineTo(points[i]);

    Path fill(envelope);
    fill.lineTo(points.getLast().x, graph.getBottom());
    fill.lineTo(points[0].x, graph.getBottom());
    fill.closeSubPath();
    g.setGradientFill(ColourGradient(accent.withAlpha(0.45f), graph.getCentreX(), graph.getY(),
                                     accent.withAlpha(0.04f), graph.getCentreX(), graph.getBottom(), false));
    g.fillPath(fill);
    g.setColour(accent);
    g.strokePath(envelope, PathStrokeType(showLabels ? 2.2f : 1.6f, PathStrokeType::curved, PathStrokeType::rounded));

    for (int i = 0; i < points.size(); ++i) {
        const bool draggable = editable && pointStage[i] >= 0;
        const bool active = i == activeIndex;
        if (!draggable && !active)
            continue;
        float radius = showLabels ? 4.5f : 2.4f;
        if (i == hoverPoint || i == dragPoint)
            radius += 1.5f;
        if (active) {
            g.setColour(Colour(0xfff0b35e));
            g.fillEllipse(points[i].x - radius, points[i].y - radius, radius * 2.0f, radius * 2.0f);
        } else {
            g.setColour(Colour(0xff141517));
            g.fillEllipse(points[i].x - radius, points[i].y - radius, radius * 2.0f, radius * 2.0f);
            g.setColour(i == hoverPoint || i == dragPoint ? Colours::white : accent);
            g.drawEllipse(points[i].x - radius, points[i].y - radius, radius * 2.0f, radius * 2.0f, 1.6f);
            if (showLabels) {
                g.setFont(juce::Font(juce::FontOptions().withHeight(9.0f).withStyle("Bold")));
                g.setColour(Colour(0xff9a9995));
                const float labelY = points[i].y - radius - 14.0f < graph.getY() - 10.0f
                                         ? points[i].y + radius + 2.0f : points[i].y - radius - 14.0f;
                g.drawText(String(pointStage[i] + 1),
                           Rectangle<float>(points[i].x - 10.0f, labelY, 20.0f, 12.0f).toNearestInt(),
                           Justification::centred, false);
            }
        }
    }
}

//==============================================================================
EnvDisplay::EnvDisplay(DexedApvts &apvts, int op) {
    for (int i=0; i<4; i++) {
        rates[i].referTo(apvts, IDs::egRate.op(op).idx(i).name);
        levels[i].referTo(apvts, IDs::egLevel.op(op).idx(i).name);
        rates[i].callback = [this] { repaint(); };
        levels[i].callback = [this] { repaint(); };
        rateParams[i] = apvts.getParameter(IDs::egRate.op(op).idx(i).name);
        levelParams[i] = apvts.getParameter(IDs::egLevel.op(op).idx(i).name);
    }
    setTitle("Operator " + String(op + 1) + " amplitude envelope");
}

void EnvDisplay::paint(Graphics &g) {
    const auto graph = graphArea();
    if (graph.getWidth() < 12.0f || graph.getHeight() < 8.0f)
        return;
    drawFrame(g, graph);

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
    auto y = [this, &graph](int level) { return levelToY(graph, level); };

    points.clearQuick();
    pointStage.clearQuick();
    points.add({graph.getX(), y(levels[3])});                                                              pointStage.add(-1);
    points.add({graph.getX() + (float)(durations[0] * pixelsPerSecond), y(levels[0])});                   pointStage.add(0);
    points.add({graph.getX() + (float)((durations[0] + durations[1]) * pixelsPerSecond), y(levels[1])});  pointStage.add(1);
    points.add({graph.getX() + (float)(sustainTime * pixelsPerSecond), y(levels[2])});                    pointStage.add(2);
    points.add({graph.getX() + (float)((sustainTime + holdTime) * pixelsPerSecond), y(levels[2])});       pointStage.add(-1);
    points.add({graph.getRight(), y(levels[3])});                                                          pointStage.add(3);

    // Key-off marker at the end of the sustain hold.
    g.setColour(Colour(0xfff0b35e).withAlpha(0.35f));
    const float dash[] = { 3.0f, 3.0f };
    g.drawDashedLine(Line<float>(points[4].x, graph.getY(), points[4].x, graph.getBottom()), dash, 2, 1.0f);
    if (showLabels) {
        g.setFont(juce::Font(juce::FontOptions().withHeight(8.5f).withStyle("Bold")));
        const bool roomRight = points[4].x + 54.0f < graph.getRight();
        g.drawText("KEY OFF", Rectangle<float>(roomRight ? points[4].x + 4.0f : points[4].x - 54.0f,
                                               graph.getY(), 50.0f, 12.0f).toNearestInt(),
                   roomRight ? Justification::centredLeft : Justification::centredRight, false);
    }

    const int active = jlimit(0, 5, static_cast<int>(vPos));
    drawCurve(g, graph, vPos > 0 ? active : -1);
}

PitchEnvDisplay::PitchEnvDisplay(DexedApvts &apvts) {
    for (int i=0; i<4; i++) {
        rates[i].referTo(apvts, IDs::pitchEgRate.idx(i).name);
        levels[i].referTo(apvts, IDs::pitchEgLevel.idx(i).name);
        rates[i].callback = [this] { repaint(); };
        levels[i].callback = [this] { repaint(); };
        rateParams[i] = apvts.getParameter(IDs::pitchEgRate.idx(i).name);
        levelParams[i] = apvts.getParameter(IDs::pitchEgLevel.idx(i).name);
    }
    accent = Colour(0xfff0b35e);
    setTitle("Pitch envelope");
}

float PitchEnvDisplay::levelToY(juce::Rectangle<float> graph, int level) const {
    const float normalised = (pitchenv_tab[jlimit(0, 99, level)] + 128) / 255.0f;
    return graph.getBottom() - graph.getHeight() * normalised;
}

void PitchEnvDisplay::paint(Graphics &g) {
    const auto graph = graphArea();
    if (graph.getWidth() < 12.0f || graph.getHeight() < 8.0f)
        return;
    drawFrame(g, graph);
    g.setColour(Colours::white.withAlpha(0.14f));
    g.drawLine(graph.getX(), levelToY(graph, 50), graph.getRight(), levelToY(graph, 50), 0.9f);

    float dist[4], total = 0.0f;
    int previous = pitchenv_tab[jlimit(0, 99, static_cast<int>(levels[3]))] + 128;
    for (int i = 0; i < 4; ++i) {
        const int current = pitchenv_tab[jlimit(0, 99, static_cast<int>(levels[i]))] + 128;
        const float speed = jmax(0.0001f, static_cast<float>(pitchenv_rate[jlimit(0, 99, static_cast<int>(rates[i]))]));
        dist[i] = std::abs(current - previous) / speed;
        total += dist[i];
        previous = current;
    }
    // Keep every segment visible even when its level does not change.
    const float minimum = jmax(total, 0.0001f) * 0.06f;
    total = 0.0f;
    for (auto& d : dist) {
        d = jmax(d, minimum);
        total += d;
    }

    const float stepScale = graph.getWidth() / total;
    points.clearQuick();
    pointStage.clearQuick();
    points.add({graph.getX(), levelToY(graph, levels[3])});
    pointStage.add(-1);
    float x = graph.getX();
    for (int i = 0; i < 4; ++i) {
        x += dist[i] * stepScale;
        points.add({x, levelToY(graph, levels[i])});
        pointStage.add(i);
    }

    const int active = jlimit(0, 4, static_cast<int>(vPos));
    drawCurve(g, graph, vPos > 0 ? active : -1);
}
