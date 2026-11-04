#pragma once

#include "parameter/DexedApvts.h"
#include "../util/CachedParameter.h"

/**
 * Graphical DX7 four-rate / four-level envelope.
 *
 * When editable, the break points can be dragged: vertical movement sets the
 * level of the segment, horizontal movement its rate (dragging to the right
 * makes the segment slower). Edits go through the host-automatable
 * parameters with proper begin/end gestures.
 */
class EnvelopeViewBase : public juce::Component, public juce::SettableTooltipClient {
public:
    char vPos = 0;

    void setEditable(bool shouldBeEditable) { editable = shouldBeEditable; setMouseCursor(editable ? juce::MouseCursor::PointingHandCursor : juce::MouseCursor::NormalCursor); }
    void setAccent(juce::Colour c) { accent = c; repaint(); }
    void setShowGrid(bool show) { showLabels = show; repaint(); }

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

protected:
    juce::RangedAudioParameter* rateParams[4] = {};
    juce::RangedAudioParameter* levelParams[4] = {};
    bool editable = false;
    bool showLabels = false;
    juce::Colour accent { 0xff5ad4a6 };

    // Screen points of the drawn curve and which (rate, level) pair each
    // draggable point edits. -1 marks a point that is not draggable.
    juce::Array<juce::Point<float>> points;
    juce::Array<int> pointStage;
    int hoverPoint = -1, dragPoint = -1;
    int dragStartRate = 0, dragStartLevel = 0;

    void drawFrame(juce::Graphics& g, juce::Rectangle<float> graph);
    void drawCurve(juce::Graphics& g, juce::Rectangle<float> graph, int activeIndex);
    virtual float levelToY(juce::Rectangle<float> graph, int level) const;
    int findPoint(juce::Point<float> p) const;
    juce::Rectangle<float> graphArea() const;
    static void setParam(juce::RangedAudioParameter* p, int value);
    static int getParam(juce::RangedAudioParameter* p);
};

class EnvDisplay : public EnvelopeViewBase {
    CachedParameter<int> rates[4];
    CachedParameter<int> levels[4];
public:
    EnvDisplay(DexedApvts &apvts, int op);
    void paint(juce::Graphics &g) override;
};

class PitchEnvDisplay : public EnvelopeViewBase {
    CachedParameter<int> rates[4];
    CachedParameter<int> levels[4];
public:
    PitchEnvDisplay(DexedApvts &apvts);
    void paint(juce::Graphics &g) override;
protected:
    float levelToY(juce::Rectangle<float> graph, int level) const override;
};
