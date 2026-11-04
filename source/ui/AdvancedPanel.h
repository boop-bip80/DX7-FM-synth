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
#include "component/VintageControls.h"

class OverviewPage;
class EnvelopesPage;
class ModulationPage;
class OutputPage;

/**
 * The expandable advanced workspace beneath the front panel. Four tabs give
 * direct access to every DX7 voice parameter plus the engine-level controls:
 *
 *  - OVERVIEW    clickable algorithm chart and six operator strips
 *  - ENVELOPES   large editable amplitude/pitch envelopes and key scaling
 *  - MODULATION  LFO, performance controllers, pitch bend and portamento
 *  - OUTPUT      filter, amplifier, tuning and engine type
 */
class AdvancedPanel : public juce::Component {
public:
    enum Tab { overviewTab = 0, envelopesTab, modulationTab, outputTab, numTabs };

    explicit AdvancedPanel(DexedAudioProcessor& processor);
    ~AdvancedPanel() override;

    void selectTab(int tab);
    int getCurrentTab() const { return currentTab; }
    void selectOperator(int op);
    int getSelectedOperator() const { return selectedOp; }

    void setOperatorStatus(int op, float gain, char envelopeStage);
    void setPitchEnvelopeStage(int stage);
    void setOutputLevel(float level);

    void focusOperator(int op);
    void focusGlobal();

    void paint(juce::Graphics& g) override;
    void resized() override;

    static constexpr int panelHeight = 424;

private:
    DexedAudioProcessor& processor;
    juce::OwnedArray<DeckTabButton> tabs;
    std::unique_ptr<OverviewPage> overview;
    std::unique_ptr<EnvelopesPage> envelopes;
    std::unique_ptr<ModulationPage> modulation;
    std::unique_ptr<OutputPage> output;
    int currentTab = overviewTab;
    int selectedOp = 0;

    juce::Component* pageFor(int tab) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdvancedPanel)
};
