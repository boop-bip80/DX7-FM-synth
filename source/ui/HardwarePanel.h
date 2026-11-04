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

#include "PluginProcessor.h"
#include "component/DXComponents.h"
#include "component/VintageControls.h"
#include "util/AudioComponentContainer.h"
#include "util/CachedParameter.h"

/**
 * The classic front panel: voice LED + LCD, volume and data-entry faders,
 * mode switches, 32 voice-select membrane switches, operator on/off switches
 * and the tab that unfolds the advanced workspace.
 */
class HardwarePanel : public juce::Component {
public:
    explicit HardwarePanel(DexedAudioProcessor& processor);
    ~HardwarePanel() override;

    std::function<void(int)> onVoiceSelected;
    std::function<void()> onLibrary, onStore, onInit, onFunction;
    std::function<void(bool)> onAdvancedToggled;
    std::function<void(int)> onDataStep;

    void setAdvancedOpen(bool open);
    void setCurrentVoice(int index, const juce::StringArray& names);
    void setLcd(const juce::String& top, const juce::String& bottom) { lcd.setLines(top, bottom); }
    juce::Slider& getDataEntrySlider() { return dataEntry; }
    juce::Component* getFirstFocusable() { return voiceButtons.getFirst(); }
    void toggleOperator(int op);

    void paint(juce::Graphics& g) override;
    void resized() override;

    static constexpr int panelHeight = 214;

private:
    DexedAudioProcessor& processor;
    std::unique_ptr<AudioComponentContainer> binder;

    LedDisplay voiceLed;
    LcdDisplay lcd;
    DXSlider* volume = nullptr;
    juce::Slider dataEntry { juce::Slider::LinearVertical, juce::Slider::NoTextBox };
    MembraneButton noButton { "Decrement", vt::membraneCream, "-1  NO" };
    MembraneButton yesButton { "Increment", vt::membraneCream, "+1  YES" };
    MembraneButton libraryButton { "Cartridge library", vt::membraneMaroon, "CARTRIDGE" };
    MembraneButton storeButton { "Store voice", vt::membraneMaroon, "STORE" };
    MembraneButton initButton { "Initialise voice", vt::membraneMaroon, "INIT VOICE" };
    MembraneButton functionButton { "Function settings", vt::membraneMaroon, "FUNCTION" };
    juce::OwnedArray<MembraneButton> voiceButtons;
    OperatorMembraneSwitch* opSwitches[6] = {};

    LedDisplay algorithmLed, feedbackLed;
    MembraneButton algorithmDown { "Previous algorithm", vt::membraneBrown, {}, "-" };
    MembraneButton algorithmUp { "Next algorithm", vt::membraneBrown, {}, "+" };
    MembraneButton feedbackDown { "Less feedback", vt::membraneBrown, {}, "-" };
    MembraneButton feedbackUp { "More feedback", vt::membraneBrown, {}, "+" };
    CachedParameter<int> algorithm, feedback;

    AdvancedTab advancedTab;
    int currentVoice = -1;

    void stepParameter(const juce::String& id, int delta);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HardwarePanel)
};
