/**
 *
 * Copyright (c) 2013-2018 Pascal Gauthier.
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
 */

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "component/DXComponents.h"
#include "component/VintageControls.h"
#include "component/ParameterObserver.h"
#include "DXLookNFeel.h"
#include "HardwarePanel.h"
#include "AdvancedPanel.h"
#include "window/CartManager.h"

/** Clickable preset name in the toolbar; opens a list of the cartridge's voices. */
class PresetNameField : public juce::Component, public juce::SettableTooltipClient {
public:
    std::function<void()> onClick;
    std::function<void(int)> onWheel;
    void setText(const juce::String& voiceName, const juce::String& detail) {
        name = voiceName;
        info = detail;
        setTitle("Current voice: " + name);
        repaint();
    }
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override { if (!e.mods.isPopupMenu() && onClick) onClick(); }
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override;
    void mouseEnter(const juce::MouseEvent&) override { repaint(); }
    void mouseExit(const juce::MouseEvent&) override { repaint(); }
private:
    juce::String name, info;
    float wheelAccumulator = 0.0f;
};

//==============================================================================
class DexedAudioProcessorEditor  : public AudioProcessorEditor, public Timer,
                                   public FileDragAndDropTarget, public KeyListener {
    SharedResourcePointer<DXLookNFeel> lookAndFeel;
    Component frameComponent;

    // Upper toolbar.
    IconButton menuButton { "Main menu", IconButton::Icon::menu, "DX7  FM" };
    IconButton libraryButton { "Cartridge library", IconButton::Icon::library };
    IconButton previousVoiceButton { "Previous voice", IconButton::Icon::previous };
    IconButton nextVoiceButton { "Next voice", IconButton::Icon::next };
    IconButton saveVoiceButton { "Store voice", IconButton::Icon::save };
    IconButton settingsButton { "Settings", IconButton::Icon::gear };
    PresetNameField presetField;

    HardwarePanel hardware;
    AdvancedPanel advanced;
    MidiKeyboardComponent midiKeyboard;

    // Lower status bar.
    std::unique_ptr<ParameterObserver> statusReadout;
    IconButton panicButton { "All notes off", IconButton::Icon::panic, "PANIC" };
    IconButton keyboardButton { "Keyboard", IconButton::Icon::keyboard };

    CartManager cartManager;

    // This cover is used to disable main window when cart manager is shown
    class ModalVeil : public Component {
    public:
        void paint(Graphics& g) override { g.fillAll(Colour(0xe0121315)); }
    } cartManagerCover;

    // Data-entry target: the parameter of the last control that was touched.
    class TouchTracker : public MouseListener {
    public:
        explicit TouchTracker(DexedAudioProcessorEditor& e) : editor(e) {}
        void mouseDown(const MouseEvent& event) override;
    private:
        DexedAudioProcessorEditor& editor;
    } touchTracker { *this };
    RangedAudioParameter* dataParameter = nullptr;
    float lastDataValue = -1.0f;
    String lastLcdTop, lastLcdBottom;

    TooltipWindow tooltipWindow { this, 700 };
    std::unique_ptr<juce::DialogWindow> dexedParameterDialog;
    std::unique_ptr<FileChooser> fileChooser;
    std::unique_ptr<Component> aboutBox;
    std::unique_ptr<juce::DocumentWindow> debugger;

    void resetSize();
    void layoutFrame();
    void updateHeader();
    void updateLcd();
    void moveToProgram(int direction);
    void selectProgram(int index);
    void showProgramMenu();
    void showMainMenu();
    void setAdvancedOpen(bool open);
    void setKeyboardVisible(bool visible);
    void setDataParameter(RangedAudioParameter* parameter);
    void stepDataParameter(int delta);
    void applyCart(Cartridge cart, File file);
    void storeProgramShowDialog(Cartridge destSysex, File externalFile);
    void storeProgramResponse(AlertWindow *dialog, int response, Cartridge destSysex, File externalFile, bool hasExternalFile);
    int logicalHeight() const;
public:
    DexedAudioProcessor *processor;

    DexedAudioProcessorEditor (DexedAudioProcessor* ownerFilter);
    ~DexedAudioProcessorEditor() override;
    void timerCallback() override;

    void paint (Graphics& g) override;
    void updateUI();
    void rebuildProgramCombobox();
    void loadCart(File file);
    void saveCart();
    void initProgram();
    void storeProgram();
    void cartShow();
    void parmShow();
    void tuningShow();

    static float getLargestScaleFactor();
    void resetZoomFactor();

    bool isInterestedInFileDrag (const StringArray &files) override;
    void filesDropped (const StringArray &files, int x, int y ) override;
    std::unique_ptr<ComponentTraverser> createFocusTraverser() override;

    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;

    static const int WINDOW_SIZE_X = 1200;
    static const int TOOLBAR_HEIGHT = 44;
    static const int STATUS_BAR_HEIGHT = 28;
    static const int KEYBOARD_AREA_HEIGHT = 86;

private:
#ifdef DEXED_EVENT_DEBUG
    FocusLogger focusLogger;
#endif
};


#endif  // PLUGINEDITOR_H_INCLUDED
