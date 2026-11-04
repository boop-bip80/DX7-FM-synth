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

#pragma once

#include "parameter/DexedApvts.h"
#include "DXComponents.h"
#include "VintageControls.h"
#include "../util/CachedParameter.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

/**
 * Rotary knob with a caption underneath. While the knob is hovered or dragged
 * the caption is replaced by the parameter's current value.
 */
class LabeledKnob : public juce::Component {
public:
    LabeledKnob(DexedApvts& apvts, const juce::String& paramId, const juce::String& caption,
                juce::Colour accent = vt::accent, bool bipolar = false);
    ~LabeledKnob() override;

    DXSlider slider;

    void setKnobSize(int size) { knobSize = size; resized(); }
    void resized() override;
    void paint(juce::Graphics& g) override;
    void mouseEnter(const juce::MouseEvent&) override { repaint(); }
    void mouseExit(const juce::MouseEvent&) override { repaint(); }

private:
    juce::String caption;
    juce::RangedAudioParameter* parameter = nullptr;
    std::unique_ptr<SliderAttachment> attachment;
    int knobSize = 0;
};

/** DeckToggle attached to a boolean parameter. */
class ParamSwitch : public DeckToggle {
public:
    ParamSwitch(DexedApvts& apvts, const juce::String& paramId, const juce::String& caption);
private:
    std::unique_ptr<ButtonAttachment> attachment;
};

/** Combo box attached to a choice parameter, with a caption above it. */
class ParamChoice : public juce::Component {
public:
    ParamChoice(DexedApvts& apvts, const juce::String& paramId, const juce::String& caption);
    ~ParamChoice() override;
    juce::ComboBox box;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    juce::String caption;
    std::unique_ptr<ComboBoxAttachment> attachment;
};

/** Horizontal slider with caption and live value, for breakpoint-like values. */
class LabeledHSlider : public juce::Component {
public:
    LabeledHSlider(DexedApvts& apvts, const juce::String& paramId, const juce::String& caption,
                   juce::Colour accent = vt::accent);
    ~LabeledHSlider() override;
    DXSlider slider;
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    juce::String caption;
    juce::RangedAudioParameter* parameter = nullptr;
    std::unique_ptr<SliderAttachment> attachment;
};

/** Six LFO waveform buttons drawn as icons, bound to the LFO waveform parameter. */
class WaveSelector : public juce::Component {
public:
    explicit WaveSelector(DexedApvts& apvts);
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;
    static void drawWave(juce::Graphics& g, int wave, juce::Rectangle<float> r, float stroke);
private:
    CachedParameter<int> waveform;
    juce::RangedAudioParameter* parameter = nullptr;
    int hover = -1;
    void select(int wave);
};

/** LFO shape preview including the delay fade-in. */
class LfoPreview : public juce::Component {
public:
    explicit LfoPreview(DexedApvts& apvts);
    void paint(juce::Graphics& g) override;
private:
    CachedParameter<int> waveform, speed, delay, pmd, amd;
};

/** Keyboard level scaling curve for one operator. */
class KeyScalingDisplay : public juce::Component {
public:
    KeyScalingDisplay(DexedApvts& apvts, int op, juce::Colour accent);
    void paint(juce::Graphics& g) override;
private:
    CachedParameter<int> breakpoint, leftDepth, rightDepth, leftCurve, rightCurve;
    juce::Colour accent;
};

/** Low-pass response sketch for the output filter. */
class FilterResponseDisplay : public juce::Component {
public:
    explicit FilterResponseDisplay(DexedApvts& apvts);
    void paint(juce::Graphics& g) override;
private:
    CachedParameter<float> cutoff, resonance;
};

/** Ratio/fixed frequency mode switch for an operator. */
class FrequencyModeSwitch : public juce::ToggleButton {
public:
    FrequencyModeSwitch(DexedApvts& apvts, const juce::String& paramId);
    void paintButton(juce::Graphics& g, bool over, bool down) override;
private:
    std::unique_ptr<ButtonAttachment> attachment;
};

/** Round power button for operator on/off. */
class OperatorPowerButton : public juce::ToggleButton {
public:
    OperatorPowerButton(DexedApvts& apvts, const juce::String& paramId, juce::Colour accent);
    void paintButton(juce::Graphics& g, bool over, bool down) override;
private:
    juce::Colour accent;
    std::unique_ptr<ButtonAttachment> attachment;
};
