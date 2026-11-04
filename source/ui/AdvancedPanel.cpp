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

#include "AdvancedPanel.h"
#include "parameter/Model.h"
#include "component/AlgoDisplay.h"
#include "component/DeckWidgets.h"
#include "component/EnvDisplay.h"
#include "component/OperatorDisplay.h"
#include "component/VUMeter.h"
#include "util/ContextMenuAdapter.h"

using namespace juce;

namespace {

constexpr int contentTop = 40;
constexpr int contentMargin = 12;

/** Right-click menu for operator areas: MIDI learn plus copy/paste. */
class OperatorContextMenu : public ContextMenuAdapter {
    int op;
public:
    OperatorContextMenu(DexedAudioProcessor& processor, int opIndex) : ContextMenuAdapter(processor), op(opIndex) {}

    void addSpecificItems(PopupMenu& menu, juce::Component*) override {
        // The clipboard API uses SysEx operator order (operator 6 first).
        const int sysexOp = 5 - op;
        menu.addItem("Copy Operator " + String(op + 1) + " Values", [this, sysexOp] {
            processor.copyToClipboard(sysexOp);
        });
        menu.addItem("Paste Envelope Values", DexedClipboard().isOperatorData(), false, [this, sysexOp] {
            processor.pasteEnvFromClipboard(sysexOp);
        });
        menu.addItem("Paste Operator Values", DexedClipboard().isOperatorData(), false, [this, sysexOp] {
            processor.pasteOpFromClipboard(sysexOp);
        });
        menu.addSeparator();
    }
};

/** Base for a page: owns child widgets and the right-click adapter. */
class DeckPage : public Component {
public:
    explicit DeckPage(DexedAudioProcessor& p) : processor(p) {
        setFocusContainerType(FocusContainerType::focusContainer);
    }
    ~DeckPage() override {
        for (auto& adapter : adapters)
            if (adapter.first != nullptr)
                adapter.first->removeMouseListener(adapter.second.get());
    }

    template <typename T>
    T* own(std::unique_ptr<T> component) {
        auto* raw = component.get();
        addAndMakeVisible(raw);
        owned.add(component.release());
        return raw;
    }

    void addContextMenu(Component& target, std::unique_ptr<MouseListener> adapter) {
        target.addMouseListener(adapter.get(), true);
        adapters.emplace_back(&target, std::move(adapter));
    }

protected:
    DexedAudioProcessor& processor;
    DexedApvts& apvts() { return processor.parameters; }
    OwnedArray<Component> owned;
    std::vector<std::pair<Component*, std::unique_ptr<MouseListener>>> adapters;
};

LabeledKnob* makeKnob(DeckPage& page, DexedApvts& apvts, const String& id, const String& caption,
                      Colour accent = vt::accent, bool bipolar = false) {
    return page.own(std::make_unique<LabeledKnob>(apvts, id, caption, accent, bipolar));
}

} // namespace

//==============================================================================
/** One operator column on the overview page. */
class OperatorStrip : public Component {
public:
    OperatorStrip(DexedAudioProcessor& p, int opIndex)
        : processor(p), op(opIndex), colour(vt::operatorColour(opIndex)),
          freq(p.parameters, opIndex),
          mode(p.parameters, IDs::mode.op(opIndex).name),
          power(p.parameters, IDs::on.op(opIndex).name, vt::operatorColour(opIndex)),
          env(p.parameters, opIndex),
          coarse(p.parameters, IDs::frequencyCoarse.op(opIndex).name, "COARSE", colour),
          fine(p.parameters, IDs::frequencyFine.op(opIndex).name, "FINE", colour),
          detune(p.parameters, IDs::detune.op(opIndex).name, "DETUNE", colour, true),
          level(p.parameters, IDs::outputLevel.op(opIndex).name, "LEVEL", colour),
          velocity(p.parameters, IDs::keyVelocity.op(opIndex).name, "VELOCITY", colour),
          amSens(p.parameters, IDs::ampModeSens.op(opIndex).name, "AM SENS", colour),
          rateScale(p.parameters, IDs::rateScaling.op(opIndex).name, "KEY RATE", colour) {
        setTitle("Operator " + String(op + 1));
        setFocusContainerType(FocusContainerType::focusContainer);
        setWantsKeyboardFocus(true);
        for (Component* c : std::initializer_list<Component*> { &vu, &freq, &mode, &power, &env, &coarse, &fine, &detune,
                                                                 &level, &velocity, &amSens, &rateScale })
            addAndMakeVisible(c);
        env.setAccent(colour);
        env.setTooltip("Click to edit this envelope on the ENVELOPES tab");
        addMouseListener(&selectListener, true);
        level.setKnobSize(60);
        contextMenu = std::make_unique<OperatorContextMenu>(processor, op);
        addMouseListener(contextMenu.get(), true);
    }

    ~OperatorStrip() override {
        removeMouseListener(contextMenu.get());
        removeMouseListener(&selectListener);
    }

    std::function<void(int)> onSelect;
    std::function<void(int)> onOpenEnvelope;

    void setSelected(bool shouldBeSelected) {
        if (selected != shouldBeSelected) {
            selected = shouldBeSelected;
            repaint();
        }
    }

    void setStatus(float gain, char stage) {
        vu.v = gain;
        vu.repaint();
        env.vPos = stage;
        env.repaint();
    }

    /** Any click inside the strip selects its operator; a click on the
        mini envelope opens the operator's envelope page. */
    struct SelectListener : public MouseListener {
        explicit SelectListener(OperatorStrip& s) : strip(s) {}
        void mouseDown(const MouseEvent& e) override {
            if (e.mods.isPopupMenu())
                return;
            if (e.eventComponent == &strip.env) {
                if (strip.onOpenEnvelope) strip.onOpenEnvelope(strip.op);
            } else if (strip.onSelect) {
                strip.onSelect(strip.op);
            }
        }
        OperatorStrip& strip;
    };

    void resized() override {
        const int w = getWidth();
        power.setBounds(w - 36, 6, 28, 28);
        vu.setBounds(10, 38, w - 20, 6);
        freq.setBounds(10, 50, w - 20, 26);
        mode.setBounds(10, 82, w - 20, 22);
        const int cell = (w - 12) / 3;
        coarse.setBounds(6, 110, cell, 56);
        fine.setBounds(6 + cell, 110, cell, 56);
        detune.setBounds(6 + cell * 2, 110, cell, 56);
        level.setBounds(w / 2 - 42, 170, 84, 78);
        velocity.setBounds(6, 252, cell, 54);
        amSens.setBounds(6 + cell, 252, cell, 54);
        rateScale.setBounds(6 + cell * 2, 252, cell, 54);
        env.setBounds(8, 314, w - 16, getHeight() - 322);
    }

    void paint(Graphics& g) override {
        auto r = getLocalBounds().toFloat();
        vt::drawDeckCard(g, r);
        if (selected) {
            g.setColour(colour);
            g.drawRoundedRectangle(r.reduced(1.0f), 6.0f, 2.0f);
        }
        g.setColour(colour);
        g.fillRoundedRectangle(10.0f, 10.0f, 4.0f, 20.0f, 2.0f);
        vt::text(g, "OP " + String(op + 1), { 20.0f, 7.0f, 70.0f, 26.0f }, vt::deckText, 16.0f, true,
                 Justification::centredLeft);
    }

    void paintOverChildren(Graphics& g) override {
        if (hasKeyboardFocus(false)) {
            g.setColour(vt::accent.withAlpha(0.7f));
            g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 6.0f, 1.4f);
        }
    }

    Component* firstControl() { return &mode; }

private:
    DexedAudioProcessor& processor;
    int op;
    Colour colour;
    bool selected = false;
    VuMeter vu;
    OperatorDisplay freq;
    FrequencyModeSwitch mode;
    OperatorPowerButton power;
    EnvDisplay env;
    LabeledKnob coarse, fine, detune, level, velocity, amSens, rateScale;
    std::unique_ptr<MouseListener> contextMenu;
    SelectListener selectListener { *this };
};

//==============================================================================
class OverviewPage : public DeckPage {
public:
    explicit OverviewPage(DexedAudioProcessor& p) : DeckPage(p) {
        setTitle("Overview");
        algo = own(std::make_unique<AlgoDisplay>(apvts()));
        algoKnob = makeKnob(*this, apvts(), IDs::algorithm.name, "ALGORITHM");
        feedbackKnob = makeKnob(*this, apvts(), IDs::feedback.name, "FEEDBACK");
        oscSync = own(std::make_unique<ParamSwitch>(apvts(), IDs::oscKeySync.name, "OSC SYNC"));
        for (auto* c : std::initializer_list<Component*> { algoKnob, feedbackKnob, oscSync })
            addContextMenu(*c, std::make_unique<ContextMenuAdapter>(processor));

        for (int i = 0; i < 6; ++i) {
            strips[i] = own(std::make_unique<OperatorStrip>(processor, i));
            strips[i]->onSelect = [this](int op) { if (onOperatorSelected) onOperatorSelected(op); };
            strips[i]->onOpenEnvelope = [this](int op) { if (onOpenEnvelope) onOpenEnvelope(op); };
        }
        algo->onOperatorSelected = [this](int op) { if (onOperatorSelected) onOperatorSelected(op); };
        algorithm.referTo(apvts(), IDs::algorithm.name);
        algorithm.callback = [this] { repaint(); };
    }

    std::function<void(int)> onOperatorSelected, onOpenEnvelope;

    void setSelectedOperator(int op) {
        algo->setSelectedOperator(op);
        for (int i = 0; i < 6; ++i)
            strips[i]->setSelected(i == op);
    }

    void setOperatorStatus(int op, float gain, char stage) { strips[op]->setStatus(gain, stage); }
    Component* operatorStrip(int op) { return strips[op]; }
    Component* firstGlobalControl() { return &algoKnob->slider; }

    void resized() override {
        algo->setBounds(12, 34, 276, 226);
        algoKnob->setBounds(12, 270, 90, 72);
        feedbackKnob->setBounds(104, 270, 90, 72);
        oscSync->setBounds(200, 290, 88, 28);
        const int stripX = 310, stripW = 140, gap = 5;
        for (int i = 0; i < 6; ++i)
            strips[i]->setBounds(stripX + i * (stripW + gap), 0, stripW, getHeight());
    }

    void paint(Graphics& g) override {
        vt::drawDeckCard(g, { 0.0f, 0.0f, 300.0f, (float) getHeight() }, "ALGORITHM");
        vt::text(g, String((int) algorithm).paddedLeft('0', 2) + " / 32", { 196.0f, 7.0f, 92.0f, 17.0f },
                 vt::accent, 12.0f, true, Justification::centredRight);
        vt::text(g, "CLICK AN OPERATOR TO EDIT  -  RIGHT-CLICK TO MUTE",
                 { 12.0f, (float) getHeight() - 30.0f, 276.0f, 18.0f }, vt::deckFaint, 8.5f, true);
    }

private:
    AlgoDisplay* algo = nullptr;
    LabeledKnob *algoKnob = nullptr, *feedbackKnob = nullptr;
    ParamSwitch* oscSync = nullptr;
    OperatorStrip* strips[6] = {};
    CachedParameter<int> algorithm;
};

//==============================================================================
/** Envelope editor + keyboard scaling for one operator. */
class OperatorEnvelopePage : public DeckPage {
public:
    OperatorEnvelopePage(DexedAudioProcessor& p, int opIndex) : DeckPage(p), op(opIndex) {
        const auto colour = vt::operatorColour(op);
        setTitle("Operator " + String(op + 1) + " envelope");
        env = own(std::make_unique<EnvDisplay>(apvts(), op));
        env->setEditable(true);
        env->setShowGrid(true);
        env->setAccent(colour);
        env->setTooltip("Drag a point: up/down sets the level, left/right sets the rate. Hold Shift to change only the level.");
        for (int i = 0; i < 4; ++i) {
            rates[i] = makeKnob(*this, apvts(), IDs::egRate.op(op).idx(i).name, "RATE " + String(i + 1), vt::warm);
            levels[i] = makeKnob(*this, apvts(), IDs::egLevel.op(op).idx(i).name, "LEVEL " + String(i + 1), colour);
        }
        scaling = own(std::make_unique<KeyScalingDisplay>(apvts(), op, colour));
        breakpoint = own(std::make_unique<LabeledHSlider>(apvts(), IDs::breakpoint.op(op).name, "BREAK POINT", vt::warm));
        leftCurve = own(std::make_unique<ParamChoice>(apvts(), IDs::lKeyScale.op(op).name, "L CURVE"));
        rightCurve = own(std::make_unique<ParamChoice>(apvts(), IDs::rKeyScale.op(op).name, "R CURVE"));
        leftDepth = makeKnob(*this, apvts(), IDs::lScaleDepth.op(op).name, "L DEPTH", colour);
        rightDepth = makeKnob(*this, apvts(), IDs::rScaleDepth.op(op).name, "R DEPTH", colour);
        rateScaling = makeKnob(*this, apvts(), IDs::rateScaling.op(op).name, "RATE SCALING", colour);
        outputLevel = makeKnob(*this, apvts(), IDs::outputLevel.op(op).name, "OUTPUT LEVEL", colour);
        velocity = makeKnob(*this, apvts(), IDs::keyVelocity.op(op).name, "VELOCITY", colour);
        addContextMenu(*this, std::make_unique<OperatorContextMenu>(processor, op));
    }

    void setStage(char stage) { env->vPos = stage; env->repaint(); }

    void resized() override {
        env->setBounds(12, 32, 636, 226);
        const float cell = 636.0f / 8.0f;
        for (int i = 0; i < 4; ++i) {
            rates[i]->setBounds(roundToInt(12 + cell * i), 270, roundToInt(cell), 72);
            levels[i]->setBounds(roundToInt(12 + cell * (i + 4)), 270, roundToInt(cell), 72);
        }
        const int bx = 670;
        scaling->setBounds(bx + 12, 32, 360, 128);
        breakpoint->setBounds(bx + 12, 168, 360, 38);
        leftCurve->setBounds(bx + 12, 222, 92, 44);
        leftDepth->setBounds(bx + 110, 214, 76, 64);
        rightDepth->setBounds(bx + 192, 214, 76, 64);
        rightCurve->setBounds(bx + 280, 222, 92, 44);
        rateScaling->setBounds(bx + 30, 290, 96, 72);
        outputLevel->setBounds(bx + 144, 290, 96, 72);
        velocity->setBounds(bx + 258, 290, 96, 72);
    }

    void paint(Graphics& g) override {
        const auto colour = vt::operatorColour(op);
        vt::drawDeckCard(g, { 0.0f, 0.0f, 660.0f, (float) getHeight() },
                         "OPERATOR " + String(op + 1) + "  -  AMPLITUDE ENVELOPE", colour);
        vt::text(g, "DRAG POINTS TO SHAPE  -  SHIFT LOCKS RATE", { 360.0f, 7.0f, 288.0f, 17.0f },
                 vt::deckFaint, 9.0f, true, Justification::centredRight);
        vt::drawDeckCard(g, { 670.0f, 0.0f, 384.0f, (float) getHeight() }, "KEYBOARD SCALING", colour);
        g.setColour(vt::deckLine);
        g.fillRect(682.0f, 283.0f, 360.0f, 1.0f);
    }

private:
    int op;
    EnvDisplay* env = nullptr;
    LabeledKnob* rates[4] = {};
    LabeledKnob* levels[4] = {};
    KeyScalingDisplay* scaling = nullptr;
    LabeledHSlider* breakpoint = nullptr;
    ParamChoice *leftCurve = nullptr, *rightCurve = nullptr;
    LabeledKnob *leftDepth = nullptr, *rightDepth = nullptr, *rateScaling = nullptr, *outputLevel = nullptr,
                *velocity = nullptr;
};

/** Pitch envelope editor page. */
class PitchEnvelopePage : public DeckPage {
public:
    explicit PitchEnvelopePage(DexedAudioProcessor& p) : DeckPage(p) {
        setTitle("Pitch envelope");
        env = own(std::make_unique<PitchEnvDisplay>(apvts()));
        env->setEditable(true);
        env->setShowGrid(true);
        env->setTooltip("Drag a point: up/down sets the pitch level (50 = no shift), left/right sets the rate.");
        for (int i = 0; i < 4; ++i) {
            rates[i] = makeKnob(*this, apvts(), IDs::pitchEgRate.idx(i).name, "RATE " + String(i + 1), vt::warm);
            levels[i] = makeKnob(*this, apvts(), IDs::pitchEgLevel.idx(i).name, "LEVEL " + String(i + 1), vt::warm);
        }
        pitchSens = makeKnob(*this, apvts(), IDs::pitchModSens.name, "PITCH MOD SENS", vt::warm);
        lfoPitch = makeKnob(*this, apvts(), IDs::lfoPmDepth.name, "LFO PITCH DEPTH", vt::warm);
        transpose = makeKnob(*this, apvts(), IDs::transpose.name, "TRANSPOSE", vt::warm, true);
        addContextMenu(*this, std::make_unique<ContextMenuAdapter>(processor));
    }

    void setStage(int stage) { env->vPos = (char) stage; env->repaint(); }

    void resized() override {
        env->setBounds(12, 32, 636, 226);
        const float cell = 636.0f / 8.0f;
        for (int i = 0; i < 4; ++i) {
            rates[i]->setBounds(roundToInt(12 + cell * i), 270, roundToInt(cell), 72);
            levels[i]->setBounds(roundToInt(12 + cell * (i + 4)), 270, roundToInt(cell), 72);
        }
        pitchSens->setBounds(670 + 20, 200, 110, 86);
        lfoPitch->setBounds(670 + 137, 200, 110, 86);
        transpose->setBounds(670 + 254, 200, 110, 86);
    }

    void paint(Graphics& g) override {
        vt::drawDeckCard(g, { 0.0f, 0.0f, 660.0f, (float) getHeight() }, "PITCH ENVELOPE", vt::warm);
        vt::text(g, "LEVEL 50 = ORIGINAL PITCH", { 360.0f, 7.0f, 288.0f, 17.0f },
                 vt::deckFaint, 9.0f, true, Justification::centredRight);
        vt::drawDeckCard(g, { 670.0f, 0.0f, 384.0f, (float) getHeight() }, "PITCH MODULATION", vt::warm);
        g.setColour(vt::deckDim);
        g.setFont(vt::font(11.0f, false));
        g.drawFittedText("The pitch envelope bends all six operators together - ideal for "
                         "attack chirps, drops and slow detuned swells.\n\n"
                         "Pitch mod sensitivity scales how strongly the LFO and the assigned "
                         "performance controllers move the pitch.",
                         Rectangle<int>(690, 40, 344, 140), Justification::topLeft, 8);
    }

private:
    PitchEnvDisplay* env = nullptr;
    LabeledKnob* rates[4] = {};
    LabeledKnob* levels[4] = {};
    LabeledKnob *pitchSens = nullptr, *lfoPitch = nullptr, *transpose = nullptr;
};

/** Selector button for the envelope page (OP 1..6, PITCH). */
class EnvelopeSelectButton : public Button {
public:
    EnvelopeSelectButton(const String& name, Colour c) : Button(name), colour(c) {
        setMouseClickGrabsKeyboardFocus(false);
        setTitle(name + " envelope");
    }
    void paintButton(Graphics& g, bool over, bool down) override {
        auto r = getLocalBounds().toFloat().reduced(0.5f);
        const bool on = getToggleState();
        g.setColour(on ? vt::deckPanelHi : (over ? vt::deckPanel : vt::deckInset));
        g.fillRoundedRectangle(r, 5.0f);
        g.setColour(on ? colour : vt::deckLine);
        g.drawRoundedRectangle(r, 5.0f, on ? 1.6f : 1.0f);
        g.setColour(colour.withAlpha(on ? 1.0f : 0.55f));
        g.fillRoundedRectangle(r.getX() + 8.0f, r.getCentreY() - 9.0f, 4.0f, 18.0f, 2.0f);
        vt::text(g, getName(), r.withTrimmedLeft(18.0f), on ? vt::deckText : vt::deckDim, 13.0f, true,
                 Justification::centredLeft);
        if (down || hasKeyboardFocus(true)) {
            g.setColour(vt::accent.withAlpha(0.6f));
            g.drawRoundedRectangle(r.reduced(1.0f), 5.0f, 1.2f);
        }
    }
private:
    Colour colour;
};

class EnvelopesPage : public DeckPage {
public:
    explicit EnvelopesPage(DexedAudioProcessor& p) : DeckPage(p) {
        setTitle("Envelopes");
        for (int i = 0; i < 7; ++i) {
            auto* b = own(std::make_unique<EnvelopeSelectButton>(i < 6 ? "OP " + String(i + 1) : String("PITCH"),
                                                                 i < 6 ? vt::operatorColour(i) : vt::warm));
            b->onClick = [this, i] { if (onSelectionChanged) onSelectionChanged(i); };
            selectors[i] = b;
        }
        for (int i = 0; i < 6; ++i) {
            opPages[i] = own(std::make_unique<OperatorEnvelopePage>(processor, i));
            opPages[i]->setVisible(false);
        }
        pitchPage = own(std::make_unique<PitchEnvelopePage>(processor));
        pitchPage->setVisible(false);
        show(0);
    }

    std::function<void(int)> onSelectionChanged;

    void show(int index) {
        index = jlimit(0, 6, index);
        for (int i = 0; i < 7; ++i)
            selectors[i]->setToggleState(i == index, dontSendNotification);
        for (int i = 0; i < 6; ++i)
            opPages[i]->setVisible(i == index);
        pitchPage->setVisible(index == 6);
    }

    void setOperatorStage(int op, char stage) { opPages[op]->setStage(stage); }
    void setPitchStage(int stage) { pitchPage->setStage(stage); }

    void resized() override {
        for (int i = 0; i < 7; ++i)
            selectors[i]->setBounds(0, i * 52 + (i == 6 ? 10 : 0), 110, 44);
        for (auto* page : opPages)
            page->setBounds(122, 0, getWidth() - 122, getHeight());
        pitchPage->setBounds(122, 0, getWidth() - 122, getHeight());
    }

private:
    EnvelopeSelectButton* selectors[7] = {};
    OperatorEnvelopePage* opPages[6] = {};
    PitchEnvelopePage* pitchPage = nullptr;
};

//==============================================================================
class ModulationPage : public DeckPage {
public:
    explicit ModulationPage(DexedAudioProcessor& p) : DeckPage(p) {
        setTitle("Modulation");
        wave = own(std::make_unique<WaveSelector>(apvts()));
        preview = own(std::make_unique<LfoPreview>(apvts()));
        lfoKnobs[0] = makeKnob(*this, apvts(), IDs::lfoRate.name, "SPEED");
        lfoKnobs[1] = makeKnob(*this, apvts(), IDs::lfoDelay.name, "DELAY");
        lfoKnobs[2] = makeKnob(*this, apvts(), IDs::lfoPmDepth.name, "PITCH DEPTH");
        lfoKnobs[3] = makeKnob(*this, apvts(), IDs::lfoAmpDepth.name, "AMP DEPTH");
        lfoKnobs[4] = makeKnob(*this, apvts(), IDs::pitchModSens.name, "PITCH SENS");
        lfoSync = own(std::make_unique<ParamSwitch>(apvts(), IDs::lfoKeySync.name, "KEY SYNC"));

        const MetaParameterID* rows[4][4] = {
            { &IDs::modWheel, &IDs::modWheelPitch, &IDs::modWheelAmp, &IDs::modWheelEgBias },
            { &IDs::modFoot, &IDs::modFootPitch, &IDs::modFootAmp, &IDs::modFootEgBias },
            { &IDs::modBreath, &IDs::modBreathPitch, &IDs::modBreathAmp, &IDs::modBreathEgBias },
            { &IDs::modAftertouch, &IDs::modAftertouchPitch, &IDs::modAftertouchAmp, &IDs::modAftertouchEgBias },
        };
        for (int r = 0; r < 4; ++r) {
            ranges[r] = makeKnob(*this, apvts(), rows[r][0]->name, "RANGE", vt::warm);
            pitch[r] = own(std::make_unique<ParamSwitch>(apvts(), rows[r][1]->name, "PITCH"));
            amp[r] = own(std::make_unique<ParamSwitch>(apvts(), rows[r][2]->name, "AMP"));
            egBias[r] = own(std::make_unique<ParamSwitch>(apvts(), rows[r][3]->name, "EG BIAS"));
        }

        bendUp = makeKnob(*this, apvts(), IDs::pitchRangeUp.name, "BEND UP");
        bendDown = makeKnob(*this, apvts(), IDs::pitchRangeDown.name, "BEND DOWN");
        bendStep = makeKnob(*this, apvts(), IDs::pitchStep.name, "STEP");
        portamento = makeKnob(*this, apvts(), IDs::portamentoTm.name, "PORTAMENTO TIME");
        portamento->setKnobSize(56);
        glissando = own(std::make_unique<ParamSwitch>(apvts(), IDs::glissando.name, "GLISSANDO"));
        mono = own(std::make_unique<ParamSwitch>(apvts(), IDs::monoMode.name, "MONO"));
        addContextMenu(*this, std::make_unique<ContextMenuAdapter>(processor));
    }

    Component* firstControl() { return wave; }

    void resized() override {
        wave->setBounds(12, 32, 416, 40);
        preview->setBounds(12, 80, 416, 128);
        const float cell = 416.0f / 5.0f;
        for (int i = 0; i < 5; ++i)
            lfoKnobs[i]->setBounds(roundToInt(12 + cell * i), 216, roundToInt(cell), 76);
        lfoSync->setBounds(12, 312, 120, 28);

        const int cx = 450;
        for (int r = 0; r < 4; ++r) {
            const int y = 50 + r * 80;
            ranges[r]->setBounds(cx + 124, y, 64, 64);
            pitch[r]->setBounds(cx + 196, y + 18, 76, 28);
            amp[r]->setBounds(cx + 278, y + 18, 70, 28);
            egBias[r]->setBounds(cx + 354, y + 18, 84, 28);
        }

        const int bx = 910;
        bendUp->setBounds(bx + 10, 34, 80, 72);
        bendDown->setBounds(bx + 93, 34, 80, 72);
        bendStep->setBounds(bx + 176, 34, 80, 72);
        portamento->setBounds(bx + 58, 136, 150, 86);
        glissando->setBounds(bx + 20, 244, 108, 28);
        mono->setBounds(bx + 138, 244, 108, 28);
    }

    void paint(Graphics& g) override {
        vt::drawDeckCard(g, { 0.0f, 0.0f, 440.0f, (float) getHeight() }, "LFO");
        vt::text(g, "RESTARTS THE LFO WITH EVERY NOTE", { 140.0f, 312.0f, 288.0f, 28.0f }, vt::deckFaint, 8.5f, true,
                 Justification::centredLeft);

        vt::drawDeckCard(g, { 450.0f, 0.0f, 450.0f, (float) getHeight() }, "PERFORMANCE CONTROLLERS", vt::warm);
        const char* names[] = { "MOD WHEEL", "FOOT", "BREATH", "AFTERTOUCH" };
        for (int r = 0; r < 4; ++r) {
            const float y = 50.0f + r * 80.0f;
            if (r > 0) {
                g.setColour(vt::deckLine);
                g.fillRect(462.0f, y - 8.0f, 426.0f, 1.0f);
            }
            vt::text(g, names[r], { 466.0f, y + 18.0f, 104.0f, 28.0f }, vt::deckText, 11.0f, true,
                     Justification::centredLeft);
        }
        vt::text(g, "ROUTE TO", { 646.0f, 28.0f, 242.0f, 14.0f }, vt::deckFaint, 8.5f, true);

        vt::drawDeckCard(g, { 910.0f, 0.0f, 266.0f, (float) getHeight() }, "PITCH BEND  -  PORTAMENTO");
        g.setColour(vt::deckLine);
        g.fillRect(922.0f, 122.0f, 242.0f, 1.0f);
        g.fillRect(922.0f, 232.0f, 242.0f, 1.0f);
        g.setColour(vt::deckDim);
        g.setFont(vt::font(10.5f, false));
        g.drawFittedText("STEP > 0 quantises pitch bend to semitone steps. Portamento time 0 disables the glide.",
                         Rectangle<int>(924, 290, 240, 70), Justification::topLeft, 4);
    }

private:
    WaveSelector* wave = nullptr;
    LfoPreview* preview = nullptr;
    LabeledKnob* lfoKnobs[5] = {};
    ParamSwitch* lfoSync = nullptr;
    LabeledKnob* ranges[4] = {};
    ParamSwitch *pitch[4] = {}, *amp[4] = {}, *egBias[4] = {};
    LabeledKnob *bendUp = nullptr, *bendDown = nullptr, *bendStep = nullptr, *portamento = nullptr;
    ParamSwitch *glissando = nullptr, *mono = nullptr;
};

//==============================================================================
class OutputPage : public DeckPage {
public:
    explicit OutputPage(DexedAudioProcessor& p) : DeckPage(p) {
        setTitle("Output");
        response = own(std::make_unique<FilterResponseDisplay>(apvts()));
        cutoff = makeKnob(*this, apvts(), IDs::cutoff.name, "CUTOFF");
        resonance = makeKnob(*this, apvts(), IDs::resonance.name, "RESONANCE");
        cutoff->setKnobSize(76);
        resonance->setKnobSize(76);
        output = makeKnob(*this, apvts(), IDs::output.name, "OUTPUT");
        output->setKnobSize(76);
        meter = own(std::make_unique<VuMeterOutput>());
        tune = makeKnob(*this, apvts(), IDs::masterTuneAdj.name, "MASTER TUNE", vt::accent, true);
        transpose = makeKnob(*this, apvts(), IDs::transpose.name, "TRANSPOSE", vt::accent, true);
        engine = own(std::make_unique<ParamChoice>(apvts(), IDs::engineType.name, "ENGINE RESOLUTION"));
        mono = own(std::make_unique<ParamSwitch>(apvts(), IDs::monoMode.name, "MONO"));
        scaleTranspose = own(std::make_unique<ParamSwitch>(apvts(), IDs::transposeTuningScale.name, "SCALE TRANSP."));
        addContextMenu(*this, std::make_unique<ContextMenuAdapter>(processor));
    }

    void setLevel(float v) { meter->v = v; meter->repaint(); }

    void resized() override {
        response->setBounds(12, 32, 476, 214);
        cutoff->setBounds(80, 256, 120, 110);
        resonance->setBounds(300, 256, 120, 110);
        output->setBounds(510 + 105, 34, 120, 108);
        meter->setBounds(510 + 24, 168, 282, 10);
        tune->setBounds(510 + 40, 212, 110, 96);
        transpose->setBounds(510 + 180, 212, 110, 96);
        engine->setBounds(850 + 20, 36, 286, 44);
        mono->setBounds(850 + 20, 250, 136, 28);
        scaleTranspose->setBounds(850 + 170, 250, 136, 28);
    }

    void paint(Graphics& g) override {
        vt::drawDeckCard(g, { 0.0f, 0.0f, 500.0f, (float) getHeight() }, "FILTER");
        vt::drawDeckCard(g, { 510.0f, 0.0f, 330.0f, (float) getHeight() }, "AMPLIFIER  -  TUNING");
        vt::text(g, "OUTPUT LEVEL", { 534.0f, 182.0f, 282.0f, 14.0f }, vt::deckFaint, 8.5f, true);
        vt::drawDeckCard(g, { 850.0f, 0.0f, 326.0f, (float) getHeight() }, "ENGINE");
        g.setColour(vt::deckDim);
        g.setFont(vt::font(11.0f, false));
        g.drawFittedText("MODERN - clean 24-bit operator math with the widest dynamic range.\n\n"
                         "MARK I - emulates the original hardware's reduced resolution, with its "
                         "characteristic grit and aliasing.\n\n"
                         "OPL SERIES - the brighter, lo-fi flavour of the later two-operator chips.",
                         Rectangle<int>(870, 92, 286, 140), Justification::topLeft, 12);
        g.setColour(vt::deckLine);
        g.fillRect(870.0f, 238.0f, 286.0f, 1.0f);
        vt::text(g, "SCALE TRANSP. - transpose follows the loaded microtuning scale",
                 { 870.0f, 288.0f, 286.0f, 16.0f }, vt::deckFaint, 8.5f, true, Justification::centredLeft);
    }

private:
    FilterResponseDisplay* response = nullptr;
    LabeledKnob *cutoff = nullptr, *resonance = nullptr, *output = nullptr, *tune = nullptr, *transpose = nullptr;
    VuMeterOutput* meter = nullptr;
    ParamChoice* engine = nullptr;
    ParamSwitch *mono = nullptr, *scaleTranspose = nullptr;
};

//==============================================================================
AdvancedPanel::AdvancedPanel(DexedAudioProcessor& p) : processor(p) {
    setTitle("Advanced workspace");
    setFocusContainerType(FocusContainerType::focusContainer);

    const char* names[] = { "Overview", "Envelopes", "Modulation", "Output" };
    for (int i = 0; i < numTabs; ++i) {
        auto* tab = tabs.add(new DeckTabButton(names[i]));
        tab->onClick = [this, i] { selectTab(i); };
        addAndMakeVisible(tab);
    }

    overview = std::make_unique<OverviewPage>(processor);
    envelopes = std::make_unique<EnvelopesPage>(processor);
    modulation = std::make_unique<ModulationPage>(processor);
    output = std::make_unique<OutputPage>(processor);
    for (int i = 0; i < numTabs; ++i)
        addChildComponent(pageFor(i));

    overview->onOperatorSelected = [this](int op) { selectOperator(op); };
    overview->onOpenEnvelope = [this](int op) {
        selectOperator(op);
        selectTab(envelopesTab);
    };
    envelopes->onSelectionChanged = [this](int index) {
        if (index < 6)
            selectOperator(index);
        else
            envelopes->show(6);
    };

    selectOperator(0);
    selectTab(overviewTab);
}

AdvancedPanel::~AdvancedPanel() = default;

Component* AdvancedPanel::pageFor(int tab) const {
    switch (tab) {
        case overviewTab: return overview.get();
        case envelopesTab: return envelopes.get();
        case modulationTab: return modulation.get();
        default: return output.get();
    }
}

void AdvancedPanel::selectTab(int tab) {
    currentTab = jlimit(0, numTabs - 1, tab);
    for (int i = 0; i < tabs.size(); ++i)
        tabs[i]->setToggleState(i == currentTab, dontSendNotification);
    // Don't let focus jump to an arbitrary control on the page being shown.
    if (auto* focused = Component::getCurrentlyFocusedComponent())
        if (isParentOf(focused) && !pageFor(currentTab)->isParentOf(focused))
            focused->giveAwayKeyboardFocus();
    for (int i = 0; i < numTabs; ++i)
        pageFor(i)->setVisible(i == currentTab);
    repaint();
}

void AdvancedPanel::selectOperator(int op) {
    selectedOp = jlimit(0, 5, op);
    overview->setSelectedOperator(selectedOp);
    envelopes->show(selectedOp);
    repaint();
}

void AdvancedPanel::setOperatorStatus(int op, float gain, char stage) {
    overview->setOperatorStatus(op, gain, stage);
    envelopes->setOperatorStage(op, stage);
}

void AdvancedPanel::setPitchEnvelopeStage(int stage) {
    envelopes->setPitchStage(stage);
}

void AdvancedPanel::setOutputLevel(float level) {
    output->setLevel(level);
}

void AdvancedPanel::focusOperator(int op) {
    selectOperator(op);
    selectTab(overviewTab);
    overview->operatorStrip(selectedOp)->grabKeyboardFocus();
}

void AdvancedPanel::focusGlobal() {
    selectTab(overviewTab);
    overview->firstGlobalControl()->grabKeyboardFocus();
}

void AdvancedPanel::resized() {
    const int tabWidth = 124;
    for (int i = 0; i < tabs.size(); ++i)
        tabs[i]->setBounds(contentMargin + i * (tabWidth + 4), 6, tabWidth, 30);
    const auto content = Rectangle<int>(contentMargin, contentTop, getWidth() - contentMargin * 2,
                                        getHeight() - contentTop - 6);
    for (int i = 0; i < numTabs; ++i)
        pageFor(i)->setBounds(content);
}

void AdvancedPanel::paint(Graphics& g) {
    auto r = getLocalBounds().toFloat();
    g.setColour(vt::deckBackground);
    g.fillRect(r);
    g.setGradientFill(ColourGradient(Colour(0xff18191b), 0.0f, 0.0f, vt::deckBackground, 0.0f, 36.0f, false));
    g.fillRect(r.withHeight(36.0f));
    g.setColour(vt::deckLine);
    g.fillRect(0.0f, 36.0f, r.getWidth(), 1.0f);

    const bool showOperator = currentTab == overviewTab || currentTab == envelopesTab;
    const String hint = showOperator ? "SELECTED  -  OPERATOR " + String(selectedOp + 1)
                                     : String("ALL VOICE AND ENGINE PARAMETERS ARE HOST-AUTOMATABLE");
    g.setFont(vt::font(10.0f, true));
    const float textWidth = GlyphArrangement::getStringWidth(g.getCurrentFont(), hint);
    if (showOperator) {
        g.setColour(vt::operatorColour(selectedOp));
        g.fillEllipse(r.getRight() - 16.0f - textWidth - 14.0f, 16.0f, 8.0f, 8.0f);
    }
    vt::text(g, hint, { r.getRight() - 440.0f, 8.0f, 428.0f, 24.0f }, vt::deckDim, 10.0f, true,
             Justification::centredRight);
}
