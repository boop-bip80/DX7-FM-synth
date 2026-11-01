#pragma once

#include "../util/CachedParameter.h"
#include "parameter/DexedApvts.h"

class OperatorDisplay : public juce::Component {
    CachedParameter<int> opFine, opCoarse, detune, opMode;
    String message;
public:
    OperatorDisplay(DexedApvts &apvts, int opNum) {
        opFine.referTo(apvts, IDs::frequencyFine.op(opNum).name);
        opFine.callback = [this]() {
            updateDisplay();
        };

        opCoarse.referTo(apvts, IDs::frequencyCoarse.op(opNum).name);
        opCoarse.callback = [this]() {
            updateDisplay();
        };

        detune.referTo(apvts, IDs::detune.op(opNum).name);
        detune.callback = [this]() {
            updateDisplay();
        };

        opMode.referTo(apvts, IDs::mode.op(opNum).name);
        opMode.callback = [this]() {
            updateDisplay();
        };

        updateDisplay();
    }

    void updateDisplay() {
        float freq = opCoarse;
        float fine = opFine;
        String txtFreq;
        const double m_ln10 = 2.30258509299404568402;

        if (opMode == 0) {
            if (freq == 0)
                freq = 0.5;
            // A compact, explicit ratio readout fits alongside the TUNE label
            // in every operator card without covering either heading.
            txtFreq << String(freq + (freq * (fine / 100)), 2) << "x";
        } else {
            freq = pow(10, ((int) freq) & 3);
            freq = freq * exp(m_ln10 * (fine / 100));
            txtFreq << String(freq, freq < 10.0f ? 2 : (freq < 100.0f ? 1 : 0)) << " Hz";
        }

        message = txtFreq;
        String details = opMode == 0 ? "Frequency ratio " : "Fixed frequency ";
        details << txtFreq << ", detune " << (detune > 0 ? "+" : "") << detune;
        setTitle(details);  // Exposed to screen readers; base Component is not SettableTooltipClient.
        repaint();
    }

    void paint(juce::Graphics &g) override {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff102027));
        g.fillRoundedRectangle(bounds, 2.7f);
        g.setColour(juce::Colour(0xff405d67));
        g.drawRoundedRectangle(bounds.reduced(0.4f), 2.7f, 0.7f);
        g.setColour(juce::Colour(0xff79e7d8));
        g.setFont(juce::Font(juce::FontOptions().withHeight(11.0f).withStyle("Bold")));
        g.drawFittedText(message, 2, 1, getWidth() - 4, getHeight() - 2,
                         juce::Justification::centred, 1);
    }
};
