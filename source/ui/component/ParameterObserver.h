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

    void mouseExit(const juce::MouseEvent &event) override {
        if ( parameter != nullptr && event.eventComponent != nullptr
             && event.eventComponent->getName() == parameter->getParameterID() ) {
            parameter = nullptr;
            repaint();
        }
    }

    void paint(juce::Graphics &g) override {
        const auto ivory = juce::Colour(0xffdedcd6);
        const auto muted = juce::Colour(0xff8c8a86);
        const auto accent = juce::Colour(0xff5ad4a6);
        const int height = getHeight();

        // Status-bar readout of the control under the pointer.
        juce::RangedAudioParameter *param = parameter;
        if (param == nullptr) {
            g.setColour(muted);
            g.setFont(juce::Font(juce::FontOptions().withHeight(11.0f)));
            g.drawFittedText("Hover a control to see its name and value. Right-click a control for MIDI learn.",
                             0, 0, getWidth(), height, juce::Justification::centredLeft, 1);
            return;
        }

        const auto name = param->getName(100);
        g.setFont(juce::Font(juce::FontOptions().withHeight(11.5f).withStyle("Bold")));
        const int nameWidth = juce::jmin(getWidth() - 80,
            juce::roundToInt(juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), name)) + 4);
        g.setColour(ivory);
        g.drawFittedText(name, 0, 0, nameWidth, height, juce::Justification::centredLeft, 1);
        g.setColour(accent);
        g.drawFittedText(param->getCurrentValueAsText(), nameWidth + 10, 0, 120, height,
                         juce::Justification::centredLeft, 1);
    }

    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override {
        juce::RangedAudioParameter *param = parameter;
        if ( apvts.pushToParameterInProgress )
            return;
        repaint();

    }
};
