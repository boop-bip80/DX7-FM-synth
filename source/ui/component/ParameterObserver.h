#pragma once

#include "parameter/DexedApvts.h"

/**
 * Component that shows the parameter name and value of the parameter
 * currently under the mouse pointer.
 */
class ParameterObserver : public juce::Component, juce::ValueTree::Listener {
    DexedApvts &apvts;

    juce::RangedAudioParameter *parameter = nullptr;
public:
    ParameterObserver(juce::Component *topComponent, DexedApvts &apvts) : apvts(apvts) {
        topComponent->addMouseListener(this, true);
        apvts.state.addListener(this);
        setTitle("Live parameter information");
    }

    ~ParameterObserver() override {
        apvts.state.removeListener(this);
    }

    void mouseEnter(const juce::MouseEvent &event) override {
        juce::Component *comp = event.eventComponent;
        if ( comp != nullptr ) {
            juce::String name = comp->getName();
            if ( name.isEmpty() )
                return;
            juce::RangedAudioParameter *param = apvts.getParameter(name);
            if ( param != nullptr ) {
                parameter = param;
                repaint();
            }
        }
    }

    void paint(juce::Graphics &g) override {
        const auto ivory = juce::Colour(0xffe8e6da);
        const auto muted = juce::Colour(0xff9eafbf);
        const auto accent = juce::Colour(0xff55d7c7);
        const int height = getHeight();

        // The parameter inspector doubles as a compact idle-ready indicator.
        // It replaces the old unstyled development-build string without
        // changing the plugin identity or any parameter binding.
        g.setColour(accent);
        g.fillEllipse(2.0f, (height - 5.0f) * 0.5f, 5.0f, 5.0f);

        juce::RangedAudioParameter *param = parameter;
        if (param == nullptr) {
            g.setColour(ivory);
            g.setFont(juce::Font(10.0f, juce::Font::bold));
            g.drawFittedText("DX7 FM", 13, 0, getWidth() - 69, height,
                             juce::Justification::centredLeft, 1);
            g.setColour(accent);
            g.setFont(juce::Font(9.2f, juce::Font::bold));
            g.drawFittedText("READY", getWidth() - 56, 0, 52, height,
                             juce::Justification::centredRight, 1);
            return;
        }

        g.setColour(muted);
        g.setFont(juce::Font(9.6f, juce::Font::bold));
        g.drawFittedText(param->getName(100).toUpperCase(),
                         13, 0, juce::jmax(0, getWidth() - 71), height,
                         juce::Justification::centredLeft, 1, 0.75f);
        g.setColour(accent);
        g.setFont(juce::Font(10.4f, juce::Font::bold));
        g.drawFittedText(param->getCurrentValueAsText(),
                         getWidth() - 59, 0, 55, height,
                         juce::Justification::centredRight, 1, 0.7f);
    }

    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override {
        juce::RangedAudioParameter *param = parameter;
        if ( apvts.pushToParameterInProgress )
            return;
        repaint();

    }
};
