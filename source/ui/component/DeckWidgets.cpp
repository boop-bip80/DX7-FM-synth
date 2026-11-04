/**
 *
 * Copyright (c) 2026 DX7 FM Synth contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */

#include "DeckWidgets.h"
#include "parameter/Model.h"

using namespace juce;

//==============================================================================
LabeledKnob::LabeledKnob(DexedApvts& apvts, const String& paramId, const String& captionText,
                         Colour accent, bool bipolar)
    : slider(paramId), caption(captionText) {
    slider.setSliderStyle(Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    slider.setColour(Slider::rotarySliderFillColourId, accent);
    slider.getProperties().set("bipolar", bipolar);
    slider.addMouseListener(this, false);
    slider.onValueChange = [this] { repaint(); };
    addAndMakeVisible(slider);

    parameter = apvts.getParameter(paramId);
    jassert(parameter != nullptr);
    if (parameter != nullptr) {
        attachment = std::make_unique<SliderAttachment>(apvts, paramId, slider);
        slider.setTitle(parameter->getName(100));
        slider.setTooltip(parameter->getName(100) + ": drag, scroll or use the arrow keys (Shift for coarse steps)");
        slider.setDoubleClickReturnValue(true, parameter->convertFrom0to1(parameter->getDefaultValue()));
    }
}

LabeledKnob::~LabeledKnob() {
    slider.removeMouseListener(this);
    attachment.reset();
}

void LabeledKnob::resized() {
    auto r = getLocalBounds();
    r.removeFromBottom(15);
    const int size = knobSize > 0 ? jmin(knobSize, jmin(r.getWidth(), r.getHeight()))
                                  : jmin(r.getWidth(), r.getHeight());
    slider.setBounds(r.withSizeKeepingCentre(size, size));
}

void LabeledKnob::paint(Graphics& g) {
    auto r = getLocalBounds().toFloat().removeFromBottom(15.0f);
    const bool active = slider.isMouseOverOrDragging();
    if (active && parameter != nullptr)
        vt::text(g, parameter->getCurrentValueAsText(), r, slider.findColour(Slider::rotarySliderFillColourId),
                 10.0f, true);
    else
        vt::text(g, caption, r, vt::deckDim, 9.5f, true);
}

//==============================================================================
ParamSwitch::ParamSwitch(DexedApvts& apvts, const String& paramId, const String& caption)
    : DeckToggle(paramId, caption) {
    if (auto* p = apvts.getParameter(paramId)) {
        attachment = std::make_unique<ButtonAttachment>(apvts, paramId, *this);
        setTitle(p->getName(100));
        setTooltip(p->getName(100));
    } else {
        jassertfalse;
    }
}

//==============================================================================
ParamChoice::ParamChoice(DexedApvts& apvts, const String& paramId, const String& captionText)
    : box(paramId), caption(captionText) {
    if (auto* choice = dynamic_cast<AudioParameterChoice*>(apvts.getParameter(paramId))) {
        for (int i = 0; i < choice->choices.size(); ++i)
            box.addItem(choice->choices[i], i + 1);
        attachment = std::make_unique<ComboBoxAttachment>(apvts, paramId, box);
        box.setTitle(choice->getName(100));
        box.setTooltip(choice->getName(100));
    } else {
        jassertfalse;
    }
    box.setJustificationType(Justification::centred);
    addAndMakeVisible(box);
}

ParamChoice::~ParamChoice() {
    attachment.reset();
}

void ParamChoice::resized() {
    auto r = getLocalBounds();
    r.removeFromTop(15);
    box.setBounds(r.withSizeKeepingCentre(r.getWidth(), jmin(r.getHeight(), 26)));
}

void ParamChoice::paint(Graphics& g) {
    vt::text(g, caption, getLocalBounds().toFloat().removeFromTop(14.0f), vt::deckDim, 9.5f, true);
}

//==============================================================================
LabeledHSlider::LabeledHSlider(DexedApvts& apvts, const String& paramId, const String& captionText, Colour accent)
    : slider(paramId), caption(captionText) {
    slider.setSliderStyle(Slider::LinearHorizontal);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    slider.setColour(Slider::rotarySliderFillColourId, accent);
    slider.onValueChange = [this] { repaint(); };
    addAndMakeVisible(slider);
    parameter = apvts.getParameter(paramId);
    if (parameter != nullptr) {
        attachment = std::make_unique<SliderAttachment>(apvts, paramId, slider);
        slider.setTitle(parameter->getName(100));
        slider.setTooltip(parameter->getName(100));
    } else {
        jassertfalse;
    }
}

LabeledHSlider::~LabeledHSlider() {
    attachment.reset();
}

void LabeledHSlider::resized() {
    auto r = getLocalBounds();
    r.removeFromTop(15);
    slider.setBounds(r);
}

void LabeledHSlider::paint(Graphics& g) {
    auto top = getLocalBounds().toFloat().removeFromTop(14.0f);
    vt::text(g, caption, top, vt::deckDim, 9.5f, true, Justification::centredLeft);
    if (parameter != nullptr)
        vt::text(g, parameter->getCurrentValueAsText(), top,
                 slider.findColour(Slider::rotarySliderFillColourId), 10.0f, true, Justification::centredRight);
}

//==============================================================================
WaveSelector::WaveSelector(DexedApvts& apvts) {
    waveform.referTo(apvts, IDs::lfoWaveform.name);
    waveform.callback = [this] { repaint(); };
    parameter = apvts.getParameter(IDs::lfoWaveform.name);
    setName(IDs::lfoWaveform.name);
    setTitle("LFO waveform");
    setWantsKeyboardFocus(true);
}

void WaveSelector::drawWave(Graphics& g, int wave, Rectangle<float> r, float stroke) {
    Path p;
    const float x0 = r.getX(), x1 = r.getRight(), w = r.getWidth();
    const float top = r.getY(), bottom = r.getBottom(), mid = r.getCentreY();
    switch (wave) {
        case 0: // triangle
            p.startNewSubPath(x0, mid);
            p.lineTo(x0 + w * 0.25f, top);
            p.lineTo(x0 + w * 0.75f, bottom);
            p.lineTo(x1, mid);
            break;
        case 1: // saw down
            p.startNewSubPath(x0, top);
            p.lineTo(x0 + w * 0.5f, bottom);
            p.lineTo(x0 + w * 0.5f, top);
            p.lineTo(x1, bottom);
            break;
        case 2: // saw up
            p.startNewSubPath(x0, bottom);
            p.lineTo(x0 + w * 0.5f, top);
            p.lineTo(x0 + w * 0.5f, bottom);
            p.lineTo(x1, top);
            break;
        case 3: // square
            p.startNewSubPath(x0, top);
            p.lineTo(x0 + w * 0.5f, top);
            p.lineTo(x0 + w * 0.5f, bottom);
            p.lineTo(x1, bottom);
            break;
        case 4: // sine
            p.startNewSubPath(x0, mid);
            for (int i = 1; i <= 32; ++i) {
                const float t = i / 32.0f;
                p.lineTo(x0 + w * t, mid - std::sin(t * MathConstants<float>::twoPi) * r.getHeight() * 0.5f);
            }
            break;
        default: { // sample & hold
            const float levels[] = { 0.3f, 0.9f, 0.1f, 0.6f, 0.75f };
            p.startNewSubPath(x0, top + r.getHeight() * levels[0]);
            for (int i = 0; i < 5; ++i) {
                const float y = top + r.getHeight() * levels[i];
                p.lineTo(x0 + w * i / 5.0f, y);
                p.lineTo(x0 + w * (i + 1) / 5.0f, y);
            }
            break;
        }
    }
    g.strokePath(p, PathStrokeType(stroke, PathStrokeType::mitered, PathStrokeType::rounded));
}

void WaveSelector::paint(Graphics& g) {
    const float cell = getWidth() / 6.0f;
    for (int i = 0; i < 6; ++i) {
        auto r = Rectangle<float>(i * cell, 0.0f, cell, (float) getHeight()).reduced(2.0f);
        const bool selected = i == (int) waveform;
        g.setColour(selected ? vt::accentDeep.withAlpha(0.6f) : (i == hover ? vt::deckPanelHi : vt::deckInset));
        g.fillRoundedRectangle(r, 4.0f);
        g.setColour(selected ? vt::accent : vt::deckLine);
        g.drawRoundedRectangle(r, 4.0f, 1.0f);
        g.setColour(selected ? vt::deckText : vt::deckDim);
        drawWave(g, i, r.reduced(r.getWidth() * 0.22f, r.getHeight() * 0.3f), 1.6f);
    }
    if (hasKeyboardFocus(true)) {
        g.setColour(vt::accent.withAlpha(0.6f));
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f), 5.0f, 1.2f);
    }
}

void WaveSelector::select(int wave) {
    if (parameter == nullptr)
        return;
    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost(parameter->convertTo0to1((float) jlimit(0, 5, wave)));
    parameter->endChangeGesture();
}

void WaveSelector::mouseDown(const MouseEvent& e) {
    if (e.mods.isPopupMenu())
        return;
    select((int) (e.position.x / (getWidth() / 6.0f)));
}

void WaveSelector::mouseMove(const MouseEvent& e) {
    const int h = (int) (e.position.x / (getWidth() / 6.0f));
    if (h != hover) {
        hover = h;
        repaint();
    }
}

void WaveSelector::mouseExit(const MouseEvent&) {
    hover = -1;
    repaint();
}

bool WaveSelector::keyPressed(const KeyPress& key) {
    if (key.getKeyCode() == KeyPress::leftKey) { select((int) waveform - 1); return true; }
    if (key.getKeyCode() == KeyPress::rightKey) { select((int) waveform + 1); return true; }
    return false;
}

//==============================================================================
LfoPreview::LfoPreview(DexedApvts& apvts) {
    waveform.referTo(apvts, IDs::lfoWaveform.name);
    speed.referTo(apvts, IDs::lfoRate.name);
    delay.referTo(apvts, IDs::lfoDelay.name);
    pmd.referTo(apvts, IDs::lfoPmDepth.name);
    amd.referTo(apvts, IDs::lfoAmpDepth.name);
    for (auto* p : { &waveform, &speed, &delay, &pmd, &amd })
        p->callback = [this] { repaint(); };
    setTitle("LFO preview");
}

void LfoPreview::paint(Graphics& g) {
    auto frame = getLocalBounds().toFloat().reduced(0.5f);
    g.setColour(Colour(0xff141517));
    g.fillRoundedRectangle(frame, 4.0f);
    g.setColour(vt::deckLine);
    g.drawRoundedRectangle(frame, 4.0f, 1.0f);
    auto r = frame.reduced(10.0f, 12.0f);
    g.setColour(Colours::white.withAlpha(0.08f));
    g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY(), 0.8f);

    // Number of cycles grows with speed; the delay produces a fade-in ramp.
    const float cycles = jmap((float) (int) speed, 0.0f, 99.0f, 1.0f, 9.0f);
    const float delayFraction = jmap((float) (int) delay, 0.0f, 99.0f, 0.0f, 0.6f);
    const float depth = jmax(0.18f, jmax((int) pmd, (int) amd) / 99.0f);
    juce::Random sh(3);
    float held = 0.0f;
    int lastStep = -1;
    Path p;
    const int steps = 400;
    for (int i = 0; i <= steps; ++i) {
        const float t = i / (float) steps;
        const float phase = std::fmod(t * cycles, 1.0f);
        float v = 0.0f;
        switch ((int) waveform) {
            case 0: v = phase < 0.25f ? phase * 4.0f : (phase < 0.75f ? 2.0f - phase * 4.0f : phase * 4.0f - 4.0f); break;
            case 1: v = 1.0f - 2.0f * phase; break;
            case 2: v = 2.0f * phase - 1.0f; break;
            case 3: v = phase < 0.5f ? 1.0f : -1.0f; break;
            case 4: v = std::sin(phase * MathConstants<float>::twoPi); break;
            default: {
                const int step = (int) (t * cycles);
                if (step != lastStep) { held = sh.nextFloat() * 2.0f - 1.0f; lastStep = step; }
                v = held;
                break;
            }
        }
        const float fade = delayFraction <= 0.0f ? 1.0f : jlimit(0.0f, 1.0f, t / delayFraction);
        const float y = r.getCentreY() - v * fade * depth * r.getHeight() * 0.5f;
        const float x = r.getX() + t * r.getWidth();
        if (i == 0) p.startNewSubPath(x, y); else p.lineTo(x, y);
    }
    g.setColour(vt::accent);
    g.strokePath(p, PathStrokeType(1.8f, PathStrokeType::mitered, PathStrokeType::rounded));
    if (delayFraction > 0.0f) {
        g.setColour(vt::warm.withAlpha(0.4f));
        const float dash[] = { 3.0f, 3.0f };
        const float dx = r.getX() + delayFraction * r.getWidth();
        g.drawDashedLine(Line<float>(dx, r.getY(), dx, r.getBottom()), dash, 2, 1.0f);
        vt::text(g, "DELAY", { dx + 4.0f, r.getY() - 8.0f, 50.0f, 12.0f }, vt::warm.withAlpha(0.7f),
                 8.5f, true, Justification::centredLeft);
    }
}

//==============================================================================
KeyScalingDisplay::KeyScalingDisplay(DexedApvts& apvts, int op, Colour accentColour) : accent(accentColour) {
    breakpoint.referTo(apvts, IDs::breakpoint.op(op).name);
    leftDepth.referTo(apvts, IDs::lScaleDepth.op(op).name);
    rightDepth.referTo(apvts, IDs::rScaleDepth.op(op).name);
    leftCurve.referTo(apvts, IDs::lKeyScale.op(op).name);
    rightCurve.referTo(apvts, IDs::rKeyScale.op(op).name);
    for (auto* p : { &breakpoint, &leftDepth, &rightDepth, &leftCurve, &rightCurve })
        p->callback = [this] { repaint(); };
    setTitle("Keyboard level scaling curve");
}

void KeyScalingDisplay::paint(Graphics& g) {
    auto frame = getLocalBounds().toFloat().reduced(0.5f);
    g.setColour(Colour(0xff141517));
    g.fillRoundedRectangle(frame, 4.0f);
    g.setColour(vt::deckLine);
    g.drawRoundedRectangle(frame, 4.0f, 1.0f);

    auto inner = frame.reduced(8.0f, 6.0f);
    auto keys = inner.removeFromBottom(16.0f);
    auto graph = inner.reduced(0.0f, 4.0f);

    // Miniature keyboard from A-1 (MIDI 9) to C8 (MIDI 108), matching the
    // DX7 breakpoint range 0..99.
    const int firstNote = 9, lastNote = 108;
    const auto noteX = [&](float note) {
        return keys.getX() + (note - firstNote) / (float) (lastNote - firstNote) * keys.getWidth();
    };
    g.setColour(Colour(0xffd9d7d0));
    g.fillRect(keys);
    for (int n = firstNote; n <= lastNote; ++n) {
        const int pc = n % 12;
        const bool black = pc == 1 || pc == 3 || pc == 6 || pc == 8 || pc == 10;
        if (black) {
            g.setColour(Colour(0xff1d1e21));
            g.fillRect(noteX((float) n) - 0.9f, keys.getY(), 1.8f, keys.getHeight() * 0.62f);
        } else if (pc == 0) {
            g.setColour(Colour(0xff7a7976));
            g.fillRect(noteX((float) n) - 0.5f, keys.getY(), 1.0f, keys.getHeight());
        }
    }

    const float bpNote = firstNote + jlimit(0, 99, (int) breakpoint);
    const float bpX = noteX(bpNote);
    const float baseY = graph.getCentreY();
    const float maxOffset = graph.getHeight() * 0.48f;

    g.setColour(Colours::white.withAlpha(0.10f));
    g.drawLine(graph.getX(), baseY, graph.getRight(), baseY, 0.8f);

    auto curveValue = [](int curve, float distance) {
        // curve: 0 -LN, 1 -EX, 2 +EX, 3 +LN ; distance 0..1
        const bool positive = curve >= 2;
        const bool exponential = curve == 1 || curve == 2;
        const float shape = exponential ? (std::exp(distance * 3.0f) - 1.0f) / (std::exp(3.0f) - 1.0f) : distance;
        return positive ? shape : -shape;
    };

    Path p;
    const int steps = 120;
    for (int i = 0; i <= steps; ++i) {
        const float note = firstNote + (lastNote - firstNote) * i / (float) steps;
        float offset = 0.0f;
        if (note < bpNote) {
            const float d = (bpNote - note) / jmax(1.0f, bpNote - firstNote);
            offset = curveValue(leftCurve, d) * (int) leftDepth / 99.0f;
        } else {
            const float d = (note - bpNote) / jmax(1.0f, (float) lastNote - bpNote);
            offset = curveValue(rightCurve, d) * (int) rightDepth / 99.0f;
        }
        const float x = noteX(note), y = baseY - offset * maxOffset;
        if (i == 0) p.startNewSubPath(x, y); else p.lineTo(x, y);
    }
    g.setColour(accent);
    g.strokePath(p, PathStrokeType(2.0f, PathStrokeType::curved, PathStrokeType::rounded));

    g.setColour(vt::warm);
    g.drawLine(bpX, graph.getY(), bpX, keys.getBottom(), 1.4f);
    g.fillEllipse(bpX - 4.0f, baseY - 4.0f, 8.0f, 8.0f);
    const float labelX = jlimit(graph.getX(), graph.getRight() - 60.0f, bpX - 30.0f);
    vt::text(g, "BREAK ", { labelX, graph.getY() - 2.0f, 60.0f, 12.0f }, vt::warm, 8.5f, true);
}

//==============================================================================
FilterResponseDisplay::FilterResponseDisplay(DexedApvts& apvts) {
    cutoff.referTo(apvts, IDs::cutoff.name);
    resonance.referTo(apvts, IDs::resonance.name);
    cutoff.callback = [this] { repaint(); };
    resonance.callback = [this] { repaint(); };
    setTitle("Filter response");
}

void FilterResponseDisplay::paint(Graphics& g) {
    auto frame = getLocalBounds().toFloat().reduced(0.5f);
    g.setColour(Colour(0xff141517));
    g.fillRoundedRectangle(frame, 4.0f);
    g.setColour(vt::deckLine);
    g.drawRoundedRectangle(frame, 4.0f, 1.0f);
    auto r = frame.reduced(12.0f, 14.0f);

    for (int i = 1; i < 10; ++i) {
        const float x = r.getX() + r.getWidth() * std::log10((float) i * 10.0f) / 3.0f;
        g.setColour(Colours::white.withAlpha(0.04f));
        g.drawLine(x, r.getY(), x, r.getBottom(), 0.7f);
    }
    for (int i = 0; i <= 4; ++i) {
        const float y = r.getY() + r.getHeight() * i / 4.0f;
        g.setColour(Colours::white.withAlpha(0.05f));
        g.drawLine(r.getX(), y, r.getRight(), y, 0.7f);
    }

    // Visual approximation of a resonant 4-pole low-pass.
    const float fc = jlimit(0.02f, 1.0f, (float) cutoff);
    const float q = jlimit(0.0f, 1.0f, (float) resonance);
    Path p;
    const int steps = 200;
    for (int i = 0; i <= steps; ++i) {
        const float t = i / (float) steps;               // log-frequency position 0..1
        const float ratio = std::pow(10.0f, (t - fc) * 3.0f);
        const float lp = 1.0f / std::sqrt(1.0f + std::pow(ratio, 8.0f));
        const float peak = q * 2.2f * std::exp(-std::pow((t - fc) * 14.0f, 2.0f));
        const float gainDb = Decibels::gainToDecibels(lp, -60.0f) + peak * 12.0f;
        const float y = jmap(jlimit(-48.0f, 18.0f, gainDb), -48.0f, 18.0f, r.getBottom(), r.getY());
        const float x = r.getX() + t * r.getWidth();
        if (i == 0) p.startNewSubPath(x, y); else p.lineTo(x, y);
    }
    Path fill(p);
    fill.lineTo(r.getRight(), r.getBottom());
    fill.lineTo(r.getX(), r.getBottom());
    fill.closeSubPath();
    g.setGradientFill(ColourGradient(vt::accent.withAlpha(0.35f), r.getCentreX(), r.getY(),
                                     vt::accent.withAlpha(0.03f), r.getCentreX(), r.getBottom(), false));
    g.fillPath(fill);
    g.setColour(vt::accent);
    g.strokePath(p, PathStrokeType(2.0f, PathStrokeType::curved, PathStrokeType::rounded));
    vt::text(g, "LOW-PASS", { r.getX(), frame.getY() + 2.0f, 80.0f, 12.0f }, vt::deckFaint, 8.5f, true,
             Justification::centredLeft);
}

//==============================================================================
FrequencyModeSwitch::FrequencyModeSwitch(DexedApvts& apvts, const String& paramId) : ToggleButton(paramId) {
    attachment = std::make_unique<ButtonAttachment>(apvts, paramId, *this);
    setTitle("Frequency mode");
    setTooltip("Operator frequency mode: ratio to the played note, or a fixed frequency in Hz");
}

void FrequencyModeSwitch::paintButton(Graphics& g, bool over, bool down) {
    auto r = getLocalBounds().toFloat().reduced(1.0f);
    g.setColour(vt::deckInset);
    g.fillRoundedRectangle(r, 4.0f);
    g.setColour(hasKeyboardFocus(true) ? vt::accent : vt::deckLine);
    g.drawRoundedRectangle(r, 4.0f, 1.0f);
    const bool fixed = getToggleState();
    auto half = r.withWidth(r.getWidth() * 0.5f);
    auto sel = (fixed ? half.withX(r.getCentreX()) : half).reduced(2.0f);
    g.setColour(down ? vt::warm : (over ? vt::accent.brighter(0.2f) : vt::accent));
    g.fillRoundedRectangle(sel, 3.0f);
    vt::text(g, "RATIO", half, fixed ? vt::deckDim : Colour(0xff10241c), 9.5f, true);
    vt::text(g, "FIXED", half.withX(r.getCentreX()), fixed ? Colour(0xff10241c) : vt::deckDim, 9.5f, true);
}

//==============================================================================
OperatorPowerButton::OperatorPowerButton(DexedApvts& apvts, const String& paramId, Colour accentColour)
    : ToggleButton(paramId), accent(accentColour) {
    attachment = std::make_unique<ButtonAttachment>(apvts, paramId, *this);
    setTitle("Operator switch");
    setTooltip("Enable or disable this operator");
}

void OperatorPowerButton::paintButton(Graphics& g, bool over, bool down) {
    auto r = getLocalBounds().toFloat().reduced(2.0f);
    const float size = jmin(r.getWidth(), r.getHeight());
    r = r.withSizeKeepingCentre(size, size);
    const bool on = getToggleState();
    g.setColour(on ? accent.withAlpha(0.22f) : vt::deckInset);
    g.fillEllipse(r);
    g.setColour(on ? accent : (over ? vt::deckDim : vt::deckLine));
    g.drawEllipse(r.reduced(0.5f), hasKeyboardFocus(true) ? 2.0f : 1.2f);
    g.setColour(down ? vt::warm : (on ? accent : vt::deckDim));
    const auto c = r.getCentre();
    const float rad = size * 0.24f;
    Path arc;
    arc.addCentredArc(c.x, c.y + 0.5f, rad, rad, 0.0f, 0.7f, MathConstants<float>::twoPi - 0.7f, true);
    g.strokePath(arc, PathStrokeType(1.7f, PathStrokeType::curved, PathStrokeType::rounded));
    g.drawLine(c.x, c.y - rad - 1.5f, c.x, c.y - 1.0f, 1.7f);
}
