

#include "OperatorEditor.h"
#include "PluginEditor.h"
#include "component/OperatorDisplay.h"
#include "util/ContextMenuAdapter.h"

#ifndef M_LN10
#define M_LN10 2.30258509299404568402
#endif

class OperatorSwitch : public ToggleButton {
public:
    explicit OperatorSwitch(juce::String name) : ToggleButton(name) {
        setTooltip("Enable or disable this FM operator");
    }

    void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown) override {
        const auto on = getToggleState();
        const auto bounds = getLocalBounds().toFloat().reduced(3.5f);
        g.setColour(on ? Colour(0xff55d7c7) : Colour(0xff465765));
        g.fillEllipse(bounds);
        g.setColour(Colour(0xff101820));
        g.fillEllipse(bounds.reduced(on ? 4.0f : 3.0f));
        g.setColour(isButtonDown ? Colour(0xfff2b563)
                                  : (isMouseOverButton ? Colour(0xffe8e6da)
                                                       : (on ? Colour(0xff55d7c7) : Colour(0xff9eafbf))));
        const auto cx = bounds.getCentreX();
        const auto cy = bounds.getCentreY();
        Path power;
        power.addCentredArc(cx, cy + 1.3f, 6.1f, 6.1f, 0.0f, 0.68f, MathConstants<float>::twoPi - 0.68f, true);
        g.strokePath(power, PathStrokeType(2.0f, PathStrokeType::curved, PathStrokeType::rounded));
        g.drawLine(cx, cy - 8.0f, cx, cy + 1.0f, 2.0f);
    }
};

// The DX7's ratio/fixed distinction deserves an actual control instead of
// two tiny bitmap lamps.  The underlying parameter and automation stay the
// same; the two positions are simply made legible.
class OperatorModeSwitch : public ToggleButton {
public:
    explicit OperatorModeSwitch(const juce::String& name) : ToggleButton(name) {
        setTooltip("Operator frequency mode: Ratio or Fixed Hz");
    }

    void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown) override {
        auto bounds = getLocalBounds().toFloat().reduced(1.0f);
        const auto half = bounds.getWidth() * 0.5f;
        g.setColour(Colour(0xff0d1820));
        g.fillRoundedRectangle(bounds, 5.0f);
        g.setColour(Colour(0xff405663));
        g.drawRoundedRectangle(bounds, 5.0f, 1.0f);

        auto selected = bounds.withX(getToggleState() ? bounds.getX() + half : bounds.getX())
                              .withWidth(half);
        g.setColour(isButtonDown ? Colour(0xfff2b563)
                                 : (isMouseOverButton ? Colour(0xff71e6d8) : Colour(0xff55d7c7)));
        g.fillRoundedRectangle(selected.reduced(2.0f), 3.5f);

        g.setFont(juce::Font(juce::FontOptions().withHeight(10.5f).withStyle("Bold")));
        g.setColour(getToggleState() ? Colour(0xffa9b9c4) : Colour(0xff101820));
        g.drawFittedText("RATIO", bounds.withWidth(half).toNearestInt(), Justification::centred, 1);
        g.setColour(getToggleState() ? Colour(0xff101820) : Colour(0xffa9b9c4));
        g.drawFittedText("FIXED", bounds.withX(bounds.getX() + half).withWidth(half).toNearestInt(),
                         Justification::centred, 1);
    }
};

//[/MiscUserDefs]


class OperatorContextMenu : public ContextMenuAdapter {
    int op;
public:
    OperatorContextMenu(DexedAudioProcessor &processor, int op) : ContextMenuAdapter(processor), op(op) {
    }

    void addSpecificItems(PopupMenu &menu, juce::Component *source) override {
        menu.addItem("Copy Operator Values", [this] {
            processor.copyToClipboard(op);
        });
        menu.addItem("Paste Envelope Values", DexedClipboard().isOperatorData(), false, [this] {
            processor.pasteEnvFromClipboard(op);
        });
        menu.addItem("Paste Operator Values", DexedClipboard().isOperatorData(), false, [this] {
            processor.pasteOpFromClipboard(op);
        });
        menu.addSeparator();
    }
};

//==============================================================================
OperatorEditor::OperatorEditor(DexedAudioProcessor &processor, int num) : processor(processor), internalOp(num) {
    SharedResourcePointer<DXLookNFeel> lookAndFeel;

    binder = std::make_unique<AudioComponentContainer>(*this, processor.parameters);
    opNum << num + 1;
    internalOp = 5-num;
    setTitle("Operator " + opNum);
    setFocusContainerType(FocusContainerType::focusContainer);

    std::unique_ptr<DXSlider> eglevel[4];
    std::unique_ptr<DXSlider> egrate[4];
    for (int i=0; i<4; i++) {
        eglevel[i] = std::make_unique<DXSlider>(IDs::egLevel.op(num).idx(i).name);
        egrate[i] = std::make_unique<DXSlider>(IDs::egRate.op(num).idx(i).name);
        eglevel[i]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        eglevel[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
        egrate[i]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        egrate[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    }

    eglevel[0]->setExplicitFocusOrder(4);
    eglevel[0]->setBounds(5, 128, 34, 34);
    binder->addAndAttach(std::move(eglevel[0]));
    eglevel[1]->setExplicitFocusOrder(6);
    eglevel[1]->setBounds(33, 128, 34, 34);
    binder->addAndAttach(std::move(eglevel[1]));
    eglevel[2]->setExplicitFocusOrder(8);
    eglevel[2]->setBounds(61, 128, 34, 34);
    binder->addAndAttach(std::move(eglevel[2]));
    eglevel[3]->setExplicitFocusOrder(10);
    eglevel[3]->setBounds(89, 128, 34, 34);
    binder->addAndAttach(std::move(eglevel[3]));
    egrate[0]->setExplicitFocusOrder(5);
    egrate[0]->setBounds(5, 169, 34, 34);
    binder->addAndAttach(std::move(egrate[0]));
    egrate[1]->setExplicitFocusOrder(7);
    egrate[1]->setBounds(33, 169, 34, 34);
    binder->addAndAttach(std::move(egrate[1]));
    egrate[2]->setExplicitFocusOrder(9);
    egrate[2]->setBounds(61, 169, 34, 34);
    binder->addAndAttach(std::move(egrate[2]));
    egrate[3]->setExplicitFocusOrder(11);
    egrate[3]->setBounds(89, 169, 34, 34);
    binder->addAndAttach(std::move(egrate[3]));

    auto opLevel = std::make_unique<DXSlider>(IDs::outputLevel.op(num).name);
    opLevel->setExplicitFocusOrder(15);
    opLevel->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    opLevel->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    opLevel->setBounds(245, 76, 34, 34);
    binder->addAndAttach(std::move(opLevel));

    auto opFine = std::make_unique<DXSlider>(IDs::frequencyFine.op(num).name);
    opFine->setExplicitFocusOrder(3);
    opFine->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    opFine->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    opFine->setBounds(78, 24, 34, 34);
    binder->addAndAttach(std::move(opFine));

    auto opCoarse = std::make_unique<DXSlider>(IDs::frequencyCoarse.op(num).name);
    opCoarse->setExplicitFocusOrder(2);
    opCoarse->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    opCoarse->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    opCoarse->setBounds(43, 24, 34, 34);
    binder->addAndAttach(std::move(opCoarse));

    auto detune = std::make_unique<DXSlider>(IDs::detune.op(num).name);
    detune->setExplicitFocusOrder(1);
    detune->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    detune->setTextBoxStyle(juce::Slider::NoTextBox, true, 80, 20);
    detune->setBounds(6, 24, 34, 34);
    binder->addAndAttach(std::move(detune));

    auto sclLeftLevel = std::make_unique<DXSlider>(IDs::lScaleDepth.op(num).name);
    sclLeftLevel->setTooltip(TRANS("Keyboard Scale Level Left Depth "));
    sclLeftLevel->setExplicitFocusOrder(16);
    sclLeftLevel->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    sclLeftLevel->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    sclLeftLevel->setBounds(131, 115, 34, 34);
    binder->addAndAttach(std::move(sclLeftLevel));

    auto sclRightLevel = std::make_unique<DXSlider>(IDs::rScaleDepth.op(num).name);
    sclRightLevel->setTooltip(TRANS("Keyboard Scale Level Right Depth "));
    sclRightLevel->setExplicitFocusOrder(18);
    sclRightLevel->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    sclRightLevel->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    sclRightLevel->setBounds(241, 115, 34, 34);
    binder->addAndAttach(std::move(sclRightLevel));

    auto sclLvlBrkPt = std::make_unique<DXSlider>(IDs::breakpoint.op(num).name);
    sclLvlBrkPt->setTooltip(TRANS("Scale Level Breakpoint"));
    sclLvlBrkPt->setExplicitFocusOrder(17);
    sclLvlBrkPt->setSliderStyle(juce::Slider::LinearHorizontal);
    sclLvlBrkPt->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    sclLvlBrkPt->setBounds(178, 130, 54, 24);
    binder->addAndAttach(std::move(sclLvlBrkPt));

    auto sclRateScaling = std::make_unique<DXSlider>(IDs::rateScaling.op(num).name);
    sclRateScaling->setTooltip(TRANS("Keyboard Rate Scaling"));
    sclRateScaling->setExplicitFocusOrder(20);
    sclRateScaling->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    sclRateScaling->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    sclRateScaling->setBounds(186, 179, 34, 34);
    binder->addAndAttach(std::move(sclRateScaling));

    auto keyVelSens = std::make_unique<DXSlider>(IDs::keyVelocity.op(num).name);
    keyVelSens->setExplicitFocusOrder(14);
    keyVelSens->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    keyVelSens->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    keyVelSens->setBounds(204, 76, 34, 34);
    binder->addAndAttach(std::move(keyVelSens));

    auto ampModeSens = std::make_unique<DXSlider>(IDs::ampModeSens.op(num).name);
    ampModeSens->setExplicitFocusOrder(13);
    ampModeSens->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    ampModeSens->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    ampModeSens->setBounds(140, 76, 34, 34);
    binder->addAndAttach(std::move(ampModeSens));

    opMode = std::make_unique<OperatorModeSwitch>(IDs::mode.op(num).name);
    opMode->setExplicitFocusOrder(12);
    opMode->setButtonText(juce::String());
    opMode->setBounds(137, 19, 74, 25);
    opMode->onClick = [this](){
        repaint();
    };

    addAndMakeVisible(opMode.get());
    binder->attach(opMode.get());

    auto kbdLeftCurve = std::make_unique<ComboBoxImage>();
    kbdLeftCurve->setName(IDs::lKeyScale.op(internalOp).name);
    kbdLeftCurve->setExplicitFocusOrder(19);
    kbdLeftCurve->setBounds(128, 170, 36, 26);
    binder->attach(kbdLeftCurve.get());
    int posLeft[] = {0,5,4,3};
    kbdLeftCurve->setImage(lookAndFeel->imageScaling, posLeft);
    binder->add(std::move(kbdLeftCurve));

    auto kbdRightCurve = std::make_unique<ComboBoxImage>();
    kbdRightCurve->setName(IDs::rKeyScale.op(internalOp).name);
    kbdRightCurve->setExplicitFocusOrder(21);
    kbdRightCurve->setBounds(240, 170, 36, 26);
    binder->attach(kbdRightCurve.get());
    int posRight[] = {3,2,1,0};
    kbdRightCurve->setImage(lookAndFeel->imageScaling, posRight);
    binder->add(std::move(kbdRightCurve));

    auto operatorDetails = std::make_unique<OperatorDisplay>(processor.parameters, num);
    operatorDetails->setBounds(15, 10, 95, 10);
    binder->add(std::move(operatorDetails));

    opSwitch = std::make_unique<OperatorSwitch>(IDs::on.op(5-internalOp).name);
    addAndMakeVisible(opSwitch.get());
    opSwitch->setTitle("Operator switch");
    opSwitch->setBounds(251, 10, 30, 30);
    binder->attach(opSwitch.get());

    envDisplay = std::make_unique<EnvDisplay>(processor.parameters, num);
    addAndMakeVisible(envDisplay.get());
    envDisplay->setBounds(16, 83, 94, 30);

    vu = std::make_unique<VuMeter>();
    addAndMakeVisible(vu.get());
    vu->setBounds(132, 52, 140, 8);

    contextMenuAdapter = std::make_unique<OperatorContextMenu>(processor, internalOp);
    addMouseListener(contextMenuAdapter.get(), true);
    setWantsKeyboardFocus(true);

    // Retain a clean, logical design grid even when the host or the new main
    // editor changes the size of each operator card.  We capture the original
    // coordinates once so repeated resizes never accumulate rounding errors.
    for (int i = 0; i < getNumChildComponents(); ++i) {
        auto* child = getChildComponent(i);
        if (child != opMode.get() && child != opSwitch.get()
            && child != envDisplay.get() && child != vu.get()) {
            designBounds.emplace_back(child, child->getBounds());
        }
    }
}

OperatorEditor::~OperatorEditor() {
    envDisplay = nullptr;
    vu = nullptr;
    contextMenuAdapter = nullptr;
}

//==============================================================================
void OperatorEditor::paint(juce::Graphics& g) {
    if (getWidth() == 0 || getHeight() == 0)
        return;

    const Colour dark(0xff101820), panel(0xff17242d), inner(0xff20303b);
    const Colour ink(0xffe8e6da), dim(0xff9eafbf), teal(0xff55d7c7), amber(0xfff2b563);

    // Draw in 287 x 218 design units; child controls are laid out separately
    // to retain their proper (circular) knob geometry at any card aspect ratio.
    Graphics::ScopedSaveState saved(g);
    g.addTransform(AffineTransform::scale(getWidth() / 287.0f, getHeight() / 218.0f));
    g.setGradientFill(ColourGradient(panel.brighter(0.09f), 0.0f, 0.0f, dark, 287.0f, 218.0f, false));
    g.fillRoundedRectangle(1.0f, 1.0f, 285.0f, 216.0f, 8.0f);
    g.setColour(Colour(0xff40505c));
    g.drawRoundedRectangle(1.0f, 1.0f, 285.0f, 216.0f, 8.0f, 1.0f);

    const auto section = [&g, &inner](float x, float y, float w, float h) {
        g.setColour(inner.withAlpha(0.78f));
        g.fillRoundedRectangle(x, y, w, h, 5.0f);
        g.setColour(Colour(0xff344955));
        g.drawRoundedRectangle(x + 0.5f, y + 0.5f, w - 1.0f, h - 1.0f, 5.0f, 0.7f);
    };
    section(6.0f, 6.0f, 115.0f, 67.0f);     // Tuning
    section(125.0f, 6.0f, 155.0f, 105.0f);   // Output
    section(6.0f, 77.0f, 115.0f, 135.0f);    // Envelope
    section(125.0f, 115.0f, 155.0f, 97.0f);  // Keyboard scaling

    const auto label = [&g](const String& text, int x, int y, int width, int height,
                            Colour colour, float size = 8.5f,
                            Justification alignment = Justification::centredLeft) {
        g.setColour(colour);
        g.setFont(juce::Font(juce::FontOptions().withHeight(size).withStyle("Bold")));
        g.drawFittedText(text, x, y, width, height, alignment, 1);
    };

    g.setColour(teal);
    g.fillRoundedRectangle(10.0f, 8.0f, 2.0f, 12.0f, 1.0f);
    label("TUNE", 17, 8, 35, 11, dim, 9.2f);
    label("DETUNE", 7, 59, 34, 10, dim, 7.5f, Justification::centred);
    label("COARSE", 43, 59, 34, 10, dim, 7.5f, Justification::centred);
    label("FINE", 80, 59, 33, 10, dim, 7.5f, Justification::centred);

    label("PERFORMANCE", 132, 8, 80, 10, dim, 8.7f);
    label("OP " + opNum, 213, 11, 35, 28, ink, 15.0f, Justification::centredRight);
    label("FREQUENCY MODE", 135, 44, 89, 9, dim, 7.4f);
    label("SIGNAL", 231, 44, 40, 9, dim, 7.4f, Justification::centredRight);
    label("AM MOD", 132, 65, 50, 10, dim, 7.9f, Justification::centred);
    label("VELOCITY", 196, 65, 51, 10, dim, 7.9f, Justification::centred);
    label("OUTPUT", 240, 65, 42, 10, dim, 7.9f, Justification::centred);

    g.setColour(amber);
    g.fillRoundedRectangle(10.0f, 80.0f, 2.0f, 10.0f, 1.0f);
    label("AMPLITUDE ENVELOPE", 16, 79, 100, 10, dim, 8.2f);
    for (int i = 0; i < 4; ++i) {
        label("L" + String(i + 1), 8 + i * 28, 118, 30, 10,
              teal, 8.2f, Justification::centred);
        label("R" + String(i + 1), 8 + i * 28, 159, 30, 10,
              amber, 8.2f, Justification::centred);
    }

    // Each caption sits in its own row.  The original bitmap layered the
    // breakpoint heading over the larger KEYBOARD SCALING label and both
    // landed directly on top of sliders at the wider editor size.
    label("KEY SCALE", 132, 118, 66, 11, dim, 9.1f);
    label("BREAK", 191, 125, 37, 10, dim, 8.1f, Justification::centred);
    label("L DEPTH", 125, 166, 51, 10, dim, 8.4f, Justification::centred);
    label("RATE", 179, 166, 49, 10, dim, 8.4f, Justification::centred);
    label("R DEPTH", 232, 166, 51, 10, dim, 8.4f, Justification::centred);
    label("L CURVE", 125, 204, 48, 9, dim, 8.3f, Justification::centred);
    label("R CURVE", 234, 204, 49, 9, dim, 8.3f, Justification::centred);

    // Low-cost active status stripe provides an immediate at-a-glance overview
    // of which of the six FM operators are contributing to the patch.
    g.setColour(opSwitch->getToggleState() ? teal : amber.withAlpha(0.45f));
    g.fillRoundedRectangle(8.0f, 215.0f, 271.0f, 1.5f, 0.75f);
}

void OperatorEditor::resized() {
    if (getWidth() <= 0 || getHeight() <= 0)
        return;

    const float sx = getWidth() / 287.0f;
    const float sy = getHeight() / 218.0f;
    for (const auto& control : designBounds) {
        const auto& base = control.second;
        auto bounds = base;
        bounds.setPosition(roundToInt(base.getX() * sx), roundToInt(base.getY() * sy));
        // Sliders remain 34 x 34, so the new wide operator cards give more
        // breathing room without distorting the rotary controls into ellipses.
        if (dynamic_cast<juce::Slider*>(control.first) == nullptr)
            bounds.setSize(roundToInt(base.getWidth() * sx), roundToInt(base.getHeight() * sy));
        if (dynamic_cast<OperatorDisplay*>(control.first) != nullptr)
            bounds = {roundToInt(58.0f * sx), roundToInt(8.0f * sy),
                      roundToInt(57.0f * sx), jmax(13, roundToInt(14.0f * sy))};
        control.first->setBounds(bounds);
    }
    // Create a proper label band above keyboard-scaling controls.  Reposition
    // just these six controls; their existing APVTS attachments stay untouched.
    const auto place = [this, sx, sy](const String& name, int x, int y, int w, int h) {
        for (const auto& control : designBounds) {
            if (control.first->getName() == name) {
                control.first->setBounds(roundToInt(x * sx), roundToInt(y * sy), w, h);
                return;
            }
        }
    };
    const int num = 5 - internalOp;
    place(IDs::lScaleDepth.op(num).name, 131, 132, 34, 34);
    place(IDs::rScaleDepth.op(num).name, 241, 132, 34, 34);
    place(IDs::breakpoint.op(num).name, 178, 136, 54, 24);
    place(IDs::rateScaling.op(num).name, 186, 175, 34, 34);
    place(IDs::lKeyScale.op(internalOp).name, 128, 178, roundToInt(36 * sx), 26);
    place(IDs::rKeyScale.op(internalOp).name, 240, 178, roundToInt(36 * sx), 26);
    opMode->setBounds(roundToInt(137.0f * sx), roundToInt(19.0f * sy),
                      roundToInt(74.0f * sx), roundToInt(25.0f * sy));
    opSwitch->setBounds(getWidth() - 37, roundToInt(10.0f * sy), 30, 30);
    envDisplay->setBounds(roundToInt(15.0f * sx), roundToInt(88.0f * sy),
                          roundToInt(98.0f * sx), roundToInt(29.0f * sy));
    vu->setBounds(roundToInt(132.0f * sx), roundToInt(53.0f * sy),
                  roundToInt(140.0f * sx), jmax(7, roundToInt(8.0f * sy)));
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void OperatorEditor::updateGain(float v) {
    vu->v = v;
    vu->repaint();
}

void OperatorEditor::updateDisplay() {
}


void OperatorEditor::updateEnvPos(char pos) {
    envDisplay->vPos = pos;
    envDisplay->repaint();
}

void OperatorEditor::mouseDown(const MouseEvent &event) {

}

void OperatorEditor::toggleOpSwitch() {
    opSwitch->setToggleState(!opSwitch->getToggleState(), dontSendNotification);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="OperatorEditor" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="2" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="287" initialHeight="218">
  <BACKGROUND backgroundColour="ffffff"/>
  <SLIDER name="egl1" id="dc070cc41347df47" memberName="s_egl1" virtualName=""
          explicitFocusOrder="4" pos="5 128 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="egl2" id="66f5195e9c374029" memberName="s_egl2" virtualName=""
          explicitFocusOrder="6" pos="33 128 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="egl3" id="9d57bd53203dcdb4" memberName="s_egl3" virtualName=""
          explicitFocusOrder="8" pos="61 128 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="egl4" id="4f7c3ece3ea2cf9c" memberName="s_egl4" virtualName=""
          explicitFocusOrder="10" pos="89 128 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="egr1" id="2ca8137d80da46fb" memberName="s_egv1" virtualName=""
          explicitFocusOrder="5" pos="5 169 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="egr3" id="4ad6d0c532d15973" memberName="s_egv2" virtualName=""
          explicitFocusOrder="7" pos="33 169 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="egr3" id="8a2027f9ede16b4f" memberName="s_egv3" virtualName=""
          explicitFocusOrder="9" pos="61 169 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="egr4" id="8c04f1c943d837e8" memberName="s_egv4" virtualName=""
          explicitFocusOrder="11" pos="89 169 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="opLevel" id="f8521c8214fb8993" memberName="opLevel" virtualName=""
          explicitFocusOrder="15" pos="245 76 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="opFine" id="e445aa61bd6cddcb" memberName="opFine" virtualName=""
          explicitFocusOrder="3" pos="78 24 34 34" min="0.0" max="99.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="opCoarse" id="4eec63d30d7488d2" memberName="opCoarse" virtualName=""
          explicitFocusOrder="2" pos="43 24 34 34" min="0.0" max="31.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="khz" id="eb961eed8902a6fc" memberName="khzDisplay" virtualName=""
         explicitFocusOrder="0" pos="15 10 95 10" bkgCol="6a000000" textCol="ffffffff"
         outlineCol="0" edTextCol="ff000000" edBkgCol="0" labelText="1,000 kHz"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="12.6" kerning="0.0" bold="0"
         italic="0" justification="36"/>
  <SLIDER name="detune" id="f093ec8defca2fc2" memberName="detune" virtualName=""
          explicitFocusOrder="1" pos="6 24 34 34" min="-7.0" max="7.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <GENERICCOMPONENT name="envDisplay" id="b18856de924c6340" memberName="envDisplay"
                    virtualName="" explicitFocusOrder="0" pos="16 83 94 30" class="EnvDisplay"
                    params=""/>
  <SLIDER name="sclLeftLevel" id="bd6f338ae68e454f" memberName="sclLeftLevel"
          virtualName="" explicitFocusOrder="16" pos="131 115 34 34" tooltip="Keyboard Scale Level Left Depth "
          min="0.0" max="99.0" int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="sclRightLevel" id="b9e23ed5187fc7e5" memberName="sclRightLevel"
          virtualName="" explicitFocusOrder="18" pos="241 115 34 34" tooltip="Keyboard Scale Level Right Depth "
          min="0.0" max="99.0" int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="sclLvlBrkPt" id="c563ac3116923bbc" memberName="sclLvlBrkPt"
          virtualName="" explicitFocusOrder="17" pos="178 130 54 24" tooltip="Scale Level Breakpoint"
          min="0.0" max="99.0" int="1.0" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="sclRateScaling" id="f0d17c8e09aa4c49" memberName="sclRateScaling"
          virtualName="" explicitFocusOrder="20" pos="186 179 34 34" tooltip="Keyboard Rate Scaling"
          min="0.0" max="7.0" int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="keyVelSens" id="21795d045d07602b" memberName="keyVelSens"
          virtualName="" explicitFocusOrder="14" pos="204 76 34 34" min="0.0"
          max="7.0" int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="ampModSens" id="634ceaa7b0f81a6c" memberName="ampModSens"
          virtualName="" explicitFocusOrder="13" pos="140 76 34 34" min="0.0"
          max="3.0" int="1.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <GENERICCOMPONENT name="vu" id="6f952594ea99dc1e" memberName="vu" virtualName=""
                    explicitFocusOrder="0" pos="132 52 140 8" class="VuMeter" params=""/>
  <TOGGLEBUTTON name="opMode" id="15db8e5c9dd13966" memberName="opMode" virtualName=""
                explicitFocusOrder="12" pos="146 19 48 26" buttonText="" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <GENERICCOMPONENT name="kbdLeftCurve" id="ee4e60dfcd6cb790" memberName="kbdLeftCurve"
                    virtualName="" explicitFocusOrder="19" pos="128 170 36 26" class="ComboBoxImage"
                    params=""/>
  <GENERICCOMPONENT name="kbdRightCurve" id="f5ca44d67593488a" memberName="kbdRightCurve"
                    virtualName="" explicitFocusOrder="21" pos="240 170 36 26" class="ComboBoxImage"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

