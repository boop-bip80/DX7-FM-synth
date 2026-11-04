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

#include "HardwarePanel.h"
#include "parameter/Model.h"

using namespace juce;

namespace {
// Layout grid of the front panel (logical pixels).
constexpr int voiceFieldX = 622;
constexpr int voiceKeyPitch = 32;
constexpr int advancedTabX = 1146;
}

HardwarePanel::HardwarePanel(DexedAudioProcessor& p) : processor(p) {
    setTitle("Front panel");
    setFocusContainerType(FocusContainerType::focusContainer);
    binder = std::make_unique<AudioComponentContainer>(*this, processor.parameters);

    addAndMakeVisible(voiceLed);
    voiceLed.setTitle("Voice number");
    addAndMakeVisible(lcd);

    auto volumeSlider = std::make_unique<DXSlider>(IDs::output.name);
    volumeSlider->setSliderStyle(Slider::LinearVertical);
    volumeSlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    volumeSlider->getProperties().set("fader", true);
    volumeSlider->setTooltip("Master volume");
    volumeSlider->setDoubleClickReturnValue(true, 1.0);
    volume = volumeSlider.get();
    binder->addAndAttach(std::move(volumeSlider));

    dataEntry.setRange(0.0, 1.0, 0.0);
    dataEntry.getProperties().set("fader", true);
    dataEntry.setTitle("Data entry");
    dataEntry.setTooltip("Data entry: edits the parameter shown on the display (the last control you touched)");
    addAndMakeVisible(dataEntry);

    for (auto* b : { &noButton, &yesButton })
        b->setShowLed(false);
    noButton.onClick = [this] { if (onDataStep) onDataStep(-1); };
    yesButton.onClick = [this] { if (onDataStep) onDataStep(1); };
    noButton.setTooltip("Decrease the displayed parameter by one step");
    yesButton.setTooltip("Increase the displayed parameter by one step");
    addAndMakeVisible(noButton);
    addAndMakeVisible(yesButton);

    libraryButton.onClick = [this] { if (onLibrary) onLibrary(); };
    storeButton.onClick = [this] { if (onStore) onStore(); };
    initButton.onClick = [this] { if (onInit) onInit(); };
    functionButton.onClick = [this] { if (onFunction) onFunction(); };
    libraryButton.setTooltip("Browse and load DX7 SysEx cartridges (Ctrl+L)");
    storeButton.setTooltip("Store the current voice into the cartridge");
    initButton.setTooltip("Reset the current voice to the initial FM patch");
    functionButton.setTooltip("MIDI, tuning, controller and UI settings (Ctrl+P)");
    for (auto* b : { &libraryButton, &storeButton, &initButton, &functionButton }) {
        b->setShowLed(false);
        addAndMakeVisible(b);
    }

    for (int i = 0; i < 32; ++i) {
        auto* b = voiceButtons.add(new MembraneButton("Voice " + String(i + 1), vt::membraneMint, String(i + 1)));
        b->setLegendHeight(12.0f);
        b->onClick = [this, i] { if (onVoiceSelected) onVoiceSelected(i); };
        addAndMakeVisible(b);
    }

    for (int i = 0; i < 6; ++i) {
        auto sw = std::make_unique<OperatorMembraneSwitch>(IDs::on.op(i).name, i + 1);
        opSwitches[i] = sw.get();
        binder->addAndAttach(std::move(sw));
    }

    for (auto* led : { &algorithmLed, &feedbackLed })
        addAndMakeVisible(*led);
    algorithmLed.setTitle("Algorithm");
    feedbackLed.setTitle("Feedback");
    algorithm.referTo(processor.parameters, IDs::algorithm.name);
    feedback.referTo(processor.parameters, IDs::feedback.name);
    algorithm.callback = [this] { algorithmLed.setValue(algorithm); };
    feedback.callback = [this] { feedbackLed.setValue(feedback); };
    algorithmLed.setValue(algorithm);
    feedbackLed.setValue(feedback);

    algorithmDown.onClick = [this] { stepParameter(IDs::algorithm.name, -1); };
    algorithmUp.onClick = [this] { stepParameter(IDs::algorithm.name, 1); };
    feedbackDown.onClick = [this] { stepParameter(IDs::feedback.name, -1); };
    feedbackUp.onClick = [this] { stepParameter(IDs::feedback.name, 1); };
    for (auto* b : { &algorithmDown, &algorithmUp, &feedbackDown, &feedbackUp }) {
        b->setShowLed(false);
        addAndMakeVisible(b);
    }

    advancedTab.onClick = [this] { if (onAdvancedToggled) onAdvancedToggled(advancedTab.getToggleState()); };
    addAndMakeVisible(advancedTab);
}

HardwarePanel::~HardwarePanel() {
    binder->freeAttachments();
}

void HardwarePanel::stepParameter(const String& id, int delta) {
    if (auto* param = processor.parameters.getParameter(id)) {
        const auto range = param->getNormalisableRange();
        const float current = param->convertFrom0to1(param->getValue());
        const float next = jlimit(range.start, range.end, current + (float) delta);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(next));
        param->endChangeGesture();
    }
}

void HardwarePanel::toggleOperator(int op) {
    if (op >= 0 && op < 6)
        opSwitches[op]->triggerClick();
}

void HardwarePanel::setAdvancedOpen(bool open) {
    advancedTab.setToggleState(open, dontSendNotification);
}

void HardwarePanel::setCurrentVoice(int index, const StringArray& names) {
    currentVoice = index;
    voiceLed.setValue(index + 1);
    for (int i = 0; i < voiceButtons.size(); ++i) {
        voiceButtons[i]->setLit(i == index);
        voiceButtons[i]->setTooltip(String(i + 1).paddedLeft('0', 2) + "  " + names[i].trim());
    }
}

void HardwarePanel::resized() {
    // Left: faders and YES/NO.
    volume->setBounds(22, 96, 40, 108);
    dataEntry.setBounds(80, 96, 40, 108);
    noButton.setBounds(140, 100, 74, 44);
    yesButton.setBounds(140, 154, 74, 44);

    // Display block.
    voiceLed.setBounds(244, 32, 80, 62);
    lcd.setBounds(332, 32, 270, 62);
    const int modeY = 120, modeW = 84;
    libraryButton.setBounds(244, modeY, modeW - 6, 46);
    storeButton.setBounds(244 + modeW, modeY, modeW - 6, 46);
    initButton.setBounds(244 + modeW * 2, modeY, modeW - 6, 46);
    functionButton.setBounds(244 + modeW * 3, modeY, modeW - 6, 46);

    // Voice select field: two rows of sixteen membrane switches.
    for (int i = 0; i < voiceButtons.size(); ++i) {
        const int row = i / 16, col = i % 16;
        voiceButtons[i]->setBounds(voiceFieldX + col * voiceKeyPitch, 30 + row * 52, voiceKeyPitch - 3, 42);
    }

    // Operator on/off and algorithm/feedback steppers.
    for (int i = 0; i < 6; ++i)
        opSwitches[i]->setBounds(voiceFieldX + i * voiceKeyPitch, 156, voiceKeyPitch - 3, 44);

    const int stepY = 160;
    algorithmDown.setBounds(838, stepY, 30, 34);
    algorithmLed.setBounds(872, stepY - 4, 62, 42);
    algorithmUp.setBounds(938, stepY, 30, 34);
    feedbackDown.setBounds(994, stepY, 30, 34);
    feedbackLed.setBounds(1028, stepY - 4, 62, 42);
    feedbackUp.setBounds(1094, stepY, 30, 34);

    advancedTab.setBounds(advancedTabX, 0, getWidth() - advancedTabX - 4, getHeight() - 2);
}

void HardwarePanel::paint(Graphics& g) {
    const auto bounds = getLocalBounds().toFloat();
    vt::drawChassis(g, bounds);

    // Brand block: an original wordmark with a three-colour membrane stripe.
    g.setColour(vt::printCream);
    g.setFont(vt::font(40.0f, true));
    g.drawText("DX7", 20, 10, 110, 44, Justification::centredLeft, false);
    g.setFont(vt::font(13.0f, true));
    g.setColour(vt::membraneMint);
    g.drawText("FM", 122, 15, 40, 16, Justification::centredLeft, false);
    g.setColour(vt::printDim);
    g.drawText("SYNTH", 122, 31, 60, 16, Justification::centredLeft, false);
    const Colour stripe[] = { vt::membraneMaroon, vt::membraneMint, vt::membraneCream, vt::membraneViolet };
    for (int i = 0; i < 4; ++i) {
        g.setColour(stripe[i]);
        g.fillRect(22.0f + i * 48.0f, 58.0f, 44.0f, 3.0f);
    }
    vt::text(g, "SIX-OPERATOR  FM  SYNTHESIZER", { 22.0f, 64.0f, 200.0f, 12.0f },
             vt::printDim, 8.5f, true, Justification::centredLeft);

    vt::text(g, "VOLUME", { 12.0f, 80.0f, 60.0f, 12.0f }, vt::printCream, 9.0f);
    vt::text(g, "DATA ENTRY", { 66.0f, 80.0f, 70.0f, 12.0f }, vt::printCream, 9.0f);

    // Section rules.
    g.setColour(vt::printRule);
    g.fillRect(230.0f, 14.0f, 1.0f, bounds.getHeight() - 28.0f);
    g.fillRect(612.0f, 14.0f, 1.0f, bounds.getHeight() - 28.0f);

    vt::drawPrintedHeader(g, "VOICE  /  DISPLAY", { 244.0f, 10.0f, 358.0f, 14.0f });
    vt::drawPrintedHeader(g, "MODE SELECT", { 244.0f, 101.0f, 330.0f, 14.0f });
    vt::drawPrintedHeader(g, "INTERNAL  VOICE  SELECT", { (float) voiceFieldX, 10.0f, 509.0f, 14.0f });
    vt::drawPrintedHeader(g, "OPERATOR  ON / OFF", { (float) voiceFieldX, 138.0f, 189.0f, 14.0f });
    vt::drawPrintedHeader(g, "ALGORITHM", { 838.0f, 138.0f, 130.0f, 14.0f });
    vt::drawPrintedHeader(g, "FEEDBACK", { 994.0f, 138.0f, 130.0f, 14.0f });

    // Hint under the display block.
    vt::text(g, "TOUCH ANY CONTROL, THEN USE DATA ENTRY OR  -1 / +1",
             { 244.0f, 176.0f, 330.0f, 24.0f }, vt::printDim.withAlpha(0.8f), 8.5f, true);
}
