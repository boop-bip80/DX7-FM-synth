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
 *
 */

#include "PluginEditor.h"
#include "window/ParamDialog.h"
#include "window/AboutBox.h"
#include "midi/SysexComm.h"
#include "window/TuningShow.h"
#include "parameter/Model.h"
#include "../debugger/Logger.h"
#include "../debugger/ValueTreeDebugger.h"
#include "math.h"
#include <fstream>
#include <memory>

#include "../msfa/fm_op_kernel.h"

//==============================================================================
void PresetNameField::paint(Graphics& g) {
    auto r = getLocalBounds().toFloat().reduced(0.5f);
    const bool over = isMouseOver(true);
    g.setColour(vt::toolbarField);
    g.fillRoundedRectangle(r, 4.0f);
    g.setColour(over ? vt::accent.withAlpha(0.7f) : vt::toolbarLine);
    g.drawRoundedRectangle(r, 4.0f, 1.0f);
    auto textArea = r.reduced(12.0f, 0.0f);
    vt::text(g, name, textArea.withTrimmedRight(150.0f), vt::toolbarText, 14.0f, true, Justification::centredLeft);
    vt::text(g, info, textArea.withTrimmedRight(16.0f).withTrimmedLeft(textArea.getWidth() - 160.0f),
             vt::toolbarDim, 10.0f, true, Justification::centredRight);
    Path arrow;
    const float ax = r.getRight() - 12.0f, ay = r.getCentreY();
    arrow.addTriangle(ax - 4.0f, ay - 2.0f, ax + 4.0f, ay - 2.0f, ax, ay + 3.0f);
    g.setColour(vt::toolbarDim);
    g.fillPath(arrow);
}

void PresetNameField::mouseWheelMove(const MouseEvent&, const MouseWheelDetails& wheel) {
    wheelAccumulator += wheel.deltaY;
    if (std::abs(wheelAccumulator) > 0.2f) {
        if (onWheel) onWheel(wheelAccumulator > 0.0f ? -1 : 1);
        wheelAccumulator = 0.0f;
    }
}

void DexedAudioProcessorEditor::TouchTracker::mouseDown(const MouseEvent& event) {
    // Walk up from the clicked component to the nearest parameter-bound one.
    for (auto* c = event.eventComponent; c != nullptr && c != &editor; c = c->getParentComponent()) {
        if (auto* param = editor.processor->parameters.getParameter(c->getName())) {
            editor.setDataParameter(param);
            return;
        }
    }
}

//==============================================================================
DexedAudioProcessorEditor::DexedAudioProcessorEditor (DexedAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      hardware(*ownerFilter),
      advanced(*ownerFilter),
      midiKeyboard (ownerFilter->keyboardState, MidiKeyboardComponent::horizontalKeyboard),
      cartManager(this) {
    processor = ownerFilter;
    lookAndFeel->setDefaultLookAndFeel(lookAndFeel);

    // Set the host window size before attaching children so that hosts
    // which query it during construction see the correct dimensions.
    resetSize();
    setExplicitFocusOrder(1);
    addAndMakeVisible(&frameComponent);

    // ---- Upper toolbar ----------------------------------------------------
    menuButton.setTooltip("Main menu: voice, cartridge, settings and view options");
    menuButton.onClick = [this] { showMainMenu(); };
    libraryButton.setTooltip("Cartridge library: browse and load DX7 SysEx banks (Ctrl+L)");
    libraryButton.onClick = [this] { cartShow(); };
    previousVoiceButton.setTooltip("Previous voice");
    previousVoiceButton.onClick = [this] { moveToProgram(-1); };
    nextVoiceButton.setTooltip("Next voice");
    nextVoiceButton.onClick = [this] { moveToProgram(1); };
    saveVoiceButton.setTooltip("Store this voice in the cartridge or a .syx file");
    saveVoiceButton.onClick = [this] { storeProgram(); };
    settingsButton.setTooltip("Settings: MIDI, controllers, tuning and UI scaling (Ctrl+P)");
    settingsButton.onClick = [this] { parmShow(); };
    presetField.setTooltip("Click to choose a voice of the loaded cartridge; scroll to step through voices");
    presetField.onClick = [this] { showProgramMenu(); };
    presetField.onWheel = [this](int direction) { moveToProgram(direction); };
    for (auto* c : std::initializer_list<Component*> { &menuButton, &libraryButton, &presetField, &previousVoiceButton,
                                                       &nextVoiceButton, &saveVoiceButton, &settingsButton })
        frameComponent.addAndMakeVisible(c);

    // ---- Front panel ------------------------------------------------------
    hardware.onVoiceSelected = [this](int index) { selectProgram(index); };
    hardware.onLibrary = [this] { cartShow(); };
    hardware.onStore = [this] { storeProgram(); };
    hardware.onInit = [this] { initProgram(); };
    hardware.onFunction = [this] { parmShow(); };
    hardware.onAdvancedToggled = [this](bool open) { setAdvancedOpen(open); };
    hardware.onDataStep = [this](int delta) { stepDataParameter(delta); };
    auto& dataSlider = hardware.getDataEntrySlider();
    dataSlider.onDragStart = [this] { if (dataParameter != nullptr) dataParameter->beginChangeGesture(); };
    dataSlider.onDragEnd = [this] { if (dataParameter != nullptr) dataParameter->endChangeGesture(); };
    dataSlider.onValueChange = [this, &dataSlider] {
        if (dataParameter != nullptr)
            dataParameter->setValueNotifyingHost((float) dataSlider.getValue());
    };
    hardware.setAdvancedOpen(processor->showAdvanced);
    frameComponent.addAndMakeVisible(hardware);

    // ---- Advanced workspace ----------------------------------------------
    frameComponent.addChildComponent(advanced);

    // ---- Keyboard -----------------------------------------------------------
    frameComponent.addAndMakeVisible(&midiKeyboard);
    midiKeyboard.setLowestVisibleKey(24);
    midiKeyboard.setTitle("Keyboard keys");
    midiKeyboard.setColour(MidiKeyboardComponent::whiteNoteColourId, Colour(0xffefede6));
    midiKeyboard.setColour(MidiKeyboardComponent::blackNoteColourId, Colour(0xff17181a));
    midiKeyboard.setColour(MidiKeyboardComponent::keySeparatorLineColourId, Colour(0xff8a8884));
    midiKeyboard.setColour(MidiKeyboardComponent::keyDownOverlayColourId, vt::accent.withAlpha(0.85f));
    midiKeyboard.setColour(MidiKeyboardComponent::mouseOverKeyOverlayColourId, vt::accent.withAlpha(0.25f));
    midiKeyboard.setColour(MidiKeyboardComponent::shadowColourId, Colours::black.withAlpha(0.35f));
    midiKeyboard.setColour(MidiKeyboardComponent::textLabelColourId, Colour(0xff55565b));

    // ---- Lower status bar ---------------------------------------------------
    statusReadout = std::make_unique<ParameterObserver>(&frameComponent, processor->parameters);
    frameComponent.addAndMakeVisible(statusReadout.get());
    panicButton.setTooltip("Panic: release all sounding notes");
    panicButton.onClick = [this] {
        processor->command.push([](DexedAudioProcessor& proc) { proc.panic(); });
    };
    keyboardButton.setClickingTogglesState(true);
    keyboardButton.setTooltip("Show or hide the on-screen keyboard");
    keyboardButton.setToggleState(processor->showKeyboard, dontSendNotification);
    keyboardButton.onClick = [this] { setKeyboardVisible(keyboardButton.getToggleState()); };
    frameComponent.addAndMakeVisible(panicButton);
    frameComponent.addAndMakeVisible(keyboardButton);

    frameComponent.addChildComponent(&cartManagerCover);
    cartManagerCover.addChildComponent(&cartManager);
    cartManager.setVisible(true);

    frameComponent.addMouseListener(&touchTracker, true);
    setDataParameter(processor->parameters.getParameter(IDs::algorithm.name));

    AffineTransform scale = AffineTransform::scale(processor->getZoomFactor());
    frameComponent.setTransform(scale);
    resetSize();
    addKeyListener(this);
    updateUI();
    startTimer(20);
}

DexedAudioProcessorEditor::~DexedAudioProcessorEditor() {
    stopTimer();
    frameComponent.removeMouseListener(&touchTracker);
    statusReadout = nullptr;
    setLookAndFeel(nullptr);
}

//==============================================================================
int DexedAudioProcessorEditor::logicalHeight() const {
    int h = TOOLBAR_HEIGHT + HardwarePanel::panelHeight + STATUS_BAR_HEIGHT;
    if (processor->showAdvanced)
        h += AdvancedPanel::panelHeight;
    if (processor->showKeyboard)
        h += KEYBOARD_AREA_HEIGHT;
    return h;
}

void DexedAudioProcessorEditor::layoutFrame() {
    const int w = WINDOW_SIZE_X;
    const int h = logicalHeight();
    frameComponent.setBounds(0, 0, w, h);

    menuButton.setBounds(8, 6, 112, 32);
    libraryButton.setBounds(126, 6, 36, 32);
    presetField.setBounds(366, 6, 404, 32);
    previousVoiceButton.setBounds(776, 6, 32, 32);
    nextVoiceButton.setBounds(810, 6, 32, 32);
    saveVoiceButton.setBounds(852, 6, 36, 32);
    settingsButton.setBounds(w - 46, 6, 38, 32);

    int y = TOOLBAR_HEIGHT;
    hardware.setBounds(0, y, w, HardwarePanel::panelHeight);
    y += HardwarePanel::panelHeight;

    advanced.setVisible(processor->showAdvanced);
    if (processor->showAdvanced) {
        advanced.setBounds(0, y, w, AdvancedPanel::panelHeight);
        y += AdvancedPanel::panelHeight;
    }

    midiKeyboard.setVisible(processor->showKeyboard);
    if (processor->showKeyboard) {
        midiKeyboard.setBounds(14, y + 9, w - 28, KEYBOARD_AREA_HEIGHT - 16);
        midiKeyboard.setKeyWidth((w - 28) / 52.0f);
        y += KEYBOARD_AREA_HEIGHT;
    }

    if (statusReadout != nullptr)
        statusReadout->setBounds(14, y + 4, 640, STATUS_BAR_HEIGHT - 8);
    panicButton.setBounds(w - 140, y + 3, 88, STATUS_BAR_HEIGHT - 6);
    keyboardButton.setBounds(w - 46, y + 3, 38, STATUS_BAR_HEIGHT - 6);
    keyboardButton.setToggleState(processor->showKeyboard, dontSendNotification);
}

void DexedAudioProcessorEditor::paint (Graphics& g) {
    // Draw at logical coordinates with the same scale as the child frame so
    // every vector border stays sharp on HiDPI hosts.
    g.fillAll(vt::toolbarBottom);
    g.addTransform(AffineTransform::scale(processor->getZoomFactor()));
    const int w = WINDOW_SIZE_X;
    const int h = logicalHeight();

    // Upper toolbar.
    g.setGradientFill(ColourGradient(vt::toolbarTop, 0.0f, 0.0f, vt::toolbarBottom, 0.0f, (float) TOOLBAR_HEIGHT, false));
    g.fillRect(0, 0, w, TOOLBAR_HEIGHT);
    g.setColour(Colours::black);
    g.fillRect(0, TOOLBAR_HEIGHT - 1, w, 1);
    g.setColour(vt::toolbarLine);
    g.fillRect(170, 10, 1, 24);
    g.fillRect(844, 10, 1, 24);

    // Keyboard bed.
    if (processor->showKeyboard) {
        const int ky = h - STATUS_BAR_HEIGHT - KEYBOARD_AREA_HEIGHT;
        g.setGradientFill(ColourGradient(Colour(0xff1b1c1f), 0.0f, (float) ky, Colour(0xff111214), 0.0f,
                                         (float) (ky + KEYBOARD_AREA_HEIGHT), false));
        g.fillRect(0, ky, w, KEYBOARD_AREA_HEIGHT);
        g.setColour(Colours::black);
        g.fillRect(12, ky + 7, w - 24, KEYBOARD_AREA_HEIGHT - 12);
        g.setColour(vt::membraneMaroon.withAlpha(0.85f));
        g.fillRect(14, ky + 7, w - 28, 2);
    }

    // Lower status bar.
    const int sy = h - STATUS_BAR_HEIGHT;
    g.setGradientFill(ColourGradient(vt::toolbarTop, 0.0f, (float) sy, vt::toolbarBottom, 0.0f, (float) h, false));
    g.fillRect(0, sy, w, STATUS_BAR_HEIGHT);
    g.setColour(Colours::black);
    g.fillRect(0, sy, w, 1);
    g.setColour(vt::toolbarLine);
    g.fillRect(w - 148, sy + 6, 1, STATUS_BAR_HEIGHT - 12);
}

void DexedAudioProcessorEditor::setAdvancedOpen(bool open) {
    processor->showAdvanced = open;
    processor->savePreference();
    hardware.setAdvancedOpen(open);
    resetSize();
    repaint();
}

void DexedAudioProcessorEditor::setKeyboardVisible(bool visible) {
    processor->showKeyboard = visible;
    processor->savePreference();
    resetSize();
    repaint();
}

void DexedAudioProcessorEditor::showMainMenu() {
    PopupMenu menu;
    menu.addSectionHeader("VOICE");
    menu.addItem("Initialise Voice", [this] { initProgram(); });
    menu.addItem("Store Voice...", [this] { storeProgram(); });
    menu.addItem("Send Voice to DX7", [this] { processor->sendCurrentSysexProgram(); });
    menu.addSectionHeader("CARTRIDGE");
    menu.addItem("Cartridge Library...", [this] { cartShow(); });
    menu.addItem("Export Cartridge (.syx)...", [this] { saveCart(); });
    menu.addSectionHeader("SETTINGS");
    menu.addItem("MIDI, Controllers && Tuning...", [this] { parmShow(); });
    menu.addItem("Show Current Tuning...", [this] { tuningShow(); });
    menu.addSeparator();
    menu.addItem("Advanced Workspace", true, processor->showAdvanced, [this] { setAdvancedOpen(!processor->showAdvanced); });
    menu.addItem("On-screen Keyboard", true, processor->showKeyboard, [this] { setKeyboardVisible(!processor->showKeyboard); });
    if (processor->getZoomFactor() != 1.0f)
        menu.addItem("Reset UI Scaling", [this] { resetZoomFactor(); });
    menu.addSeparator();
    menu.addItem("About...", [this] {
        if (ModifierKeys::getCurrentModifiers().isShiftDown()) {
            debugger.reset(new ValueTreeDebugger(processor->parameters.rootVt));
            return;
        }
        aboutBox = std::make_unique<AboutBox>();
        addAndMakeVisible(aboutBox.get());
        aboutBox->centreWithSize(aboutBox->getWidth(), aboutBox->getHeight());
        aboutBox->enterModalState(true, {});
    });
    menu.showMenuAsync(PopupMenu::Options().withTargetComponent(&menuButton));
}

void DexedAudioProcessorEditor::showProgramMenu() {
    PopupMenu menu;
    const int current = processor->getCurrentProgram();
    for (int i = 0; i < processor->getNumPrograms(); ++i) {
        const auto label = String(i + 1).paddedLeft('0', 2) + "   " + processor->getProgramName(i).trim();
        menu.addItem(label, true, i == current, [this, i] { selectProgram(i); });
    }
    menu.showMenuAsync(PopupMenu::Options().withTargetComponent(&presetField)
                                           .withMinimumWidth(presetField.getWidth())
                                           .withMaximumNumColumns(2));
}

void DexedAudioProcessorEditor::cartShow() {
    // The library needs the full workspace height.
    if (!processor->showAdvanced)
        setAdvancedOpen(true);
    stopTimer();
    cartManager.resetActiveSysex();
    const int coverTop = TOOLBAR_HEIGHT;
    const int coverHeight = logicalHeight() - TOOLBAR_HEIGHT - STATUS_BAR_HEIGHT;
    cartManagerCover.setBounds(0, coverTop, WINDOW_SIZE_X, coverHeight);
    // The cartridge browser retains its compact 800px data grid; centre it.
    cartManager.setBounds((WINDOW_SIZE_X - 844) / 2, jmax(8, (coverHeight - 556) / 2), 844, 556);
    cartManager.updateCartFilename();
    cartManagerCover.setVisible(true);
    cartManager.initialFocus();
}

std::unique_ptr<ComponentTraverser> DexedAudioProcessorEditor::createFocusTraverser() {
    return std::make_unique<FocusTraverser>();
}

void DexedAudioProcessorEditor::loadCart(File file) {
    Cartridge cart;

    int rc = cart.load(file);

    if ( rc < 0 ) {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Error",
                                          "Unable to open: " + file.getFullPathName());
        return;
    }

    if ( rc != 0 ) {
        AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon, "Unable to find DX7 sysex cartridge in file",
                                     "This sysex file is not for the DX7 or it is corrupted. "
                                     "Do you still want to load this file as random data ?", {}, {}, this,
                                     ModalCallbackFunction::create([this, cart, file](int result) {
                                         if ( result != 0 )
                                             applyCart(cart, file);
                                     }));
        return;
    }

    applyCart(cart, file);
}

void DexedAudioProcessorEditor::applyCart(Cartridge cart, File file) {
    processor->loadCartridge(cart);
    rebuildProgramCombobox();
    processor->setCurrentProgram(0);
    processor->updateHostDisplay();
    updateUI();

    processor->activeFileCartridge = file;
}

void DexedAudioProcessorEditor::saveCart() {
    fileChooser = std::make_unique<FileChooser>("Export DX sysex...", processor->dexedCartDir, "*.syx;*.SYX", true);
    fileChooser->launchAsync(FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::warnAboutOverwriting,
                             [this](const FileChooser& fc) {
                                 if ( fc.getResults().isEmpty() )
                                     return;
                                 if ( ! processor->currentCart.saveVoice(fc.getResult()) ) {
                                     AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                                                       "Error",
                                                                       "Unable to write: " + fc.getResult().getFullPathName());
                                 }
                             });
}

void DexedAudioProcessorEditor::tuningShow() {
    auto te = new TuningShow();
    te->setTuning( processor->synthTuningState->getTuning() );

    DialogWindow::LaunchOptions options;
    options.content.setOwned(te);
    options.dialogTitle = "Current Tuning";
    options.dialogBackgroundColour = vt::deckPanel;
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;

    auto dialogwindow = options.launchAsync();
}

// I don't know why closeButtonPressed is not implemented in the standard DialogWindow version.
class DexedDialogWindow : public juce::DialogWindow {
public:
    DexedDialogWindow(const juce::String& title, juce::Colour backgroundColour)
        : juce::DialogWindow(title, backgroundColour, true, false) {

    }
    void closeButtonPressed() override {
        setVisible(false);
    }
};

void DexedAudioProcessorEditor::parmShow() {
    auto param = new ParamDialog(processor->parameters, *processor);
    param->setColour(AlertWindow::backgroundColourId, vt::deckPanel);
    param->setDialogValues(processor->controllers, processor->sysexComm, processor->showKeyboard, processor->getZoomFactor());
    param->setIsStandardTuning(processor->synthTuningState->is_standard_tuning() );
    param->setTuningCallback([this](ParamDialog *p, ParamDialog::TuningAction which) {
                                switch(which) {
                                case ParamDialog::LOAD_SCL:
                                    this->processor->applySCLTuning();
                                    break;
                                case ParamDialog::LOAD_KBM:
                                    this->processor->applyKBMMapping();
                                    break;
                                case ParamDialog::RESET_TUNING:
                                    this->processor->retuneToStandard();
                                    break;
                                case ParamDialog::SHOW_TUNING:
                                    // consider https://forum.juce.com/t/closing-a-modal-dialog-window/2961
                                    this->tuningShow();
                                    break;
                                }
                                p->setIsStandardTuning(this->processor->synthTuningState->is_standard_tuning() );
                            } );

    auto generalCallback = [this](ParamDialog *param) {
                                   float scale = this->processor->getZoomFactor();
                                   bool ret = param->getDialogValues(this->processor->controllers, this->processor->sysexComm, &this->processor->showKeyboard, &scale);
                                   this->processor->setZoomFactor(scale);
                                   this->processor->savePreference();

                                   AffineTransform scaleAffine = AffineTransform::scale(this->processor->getZoomFactor());
                                   frameComponent.setTransform(scaleAffine);
                                   resetSize();
                                   repaint();

                                   if ( ret == false ) {
                                       AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Midi Interface", "Error opening midi ports");
                                   }
                               };
    param->setGeneralCallback(generalCallback);

    dexedParameterDialog = std::make_unique<DexedDialogWindow>("Settings", vt::deckPanel);
    dexedParameterDialog->setContentOwned(param, true);
    frameComponent.addAndMakeVisible(dexedParameterDialog.get());
    dexedParameterDialog->centreAroundComponent(&frameComponent, param->getWidth(), param->getHeight() + dexedParameterDialog->getTitleBarHeight());
    dexedParameterDialog->enterModalState(true,{});
}

void DexedAudioProcessorEditor::initProgram() {
    processor->resetToInitVoice();
}

void DexedAudioProcessorEditor::storeProgram() {
    storeProgramShowDialog(processor->currentCart, File());
}

void DexedAudioProcessorEditor::storeProgramShowDialog(Cartridge destSysex, File externalFile) {
    String currentName = processor->activeProgram.getName();
    bool activeCartridgeFound = processor->activeFileCartridge.exists();
    bool hasExternalFile = externalFile.exists();
    String msg;

    if ( ! hasExternalFile ) {
        if ( activeCartridgeFound )
            msg = "Store program to current (" + processor->activeFileCartridge.getFileName() + ") / new cartridge";
        else
            msg = "Store program to current / new cartridge";
    } else {
        msg = "Store program to " + externalFile.getFileName();
    }

    auto dialog = new AlertWindow("Store Program", msg, AlertWindow::NoIcon, this);
    dialog->addTextEditor("Name", currentName, String("Name"), false);
    dialog->getTextEditor("Name")->setInputRestrictions(10);

    StringArray programs;
    destSysex.getProgramNames(programs);
    dialog->addComboBox("Dest", programs, "Program Destination");

    if ( ! hasExternalFile ) {
        StringArray saveAction;
        saveAction.add("Store program to DAW plugin song state");
        saveAction.add("Store program and create a new copy of the .syx cartridge");
        if ( activeCartridgeFound )
            saveAction.add("Store program and overwrite current .syx cartridge");

        dialog->addComboBox("SaveAction", saveAction, "Store Action");
    }

    dialog->addButton("OK", 0, KeyPress(KeyPress::returnKey));
    dialog->addButton("CANCEL", 1, KeyPress(KeyPress::escapeKey));
    dialog->addButton("EXTERNAL FILE", 2, KeyPress());

    dialog->enterModalState(true, ModalCallbackFunction::create(
        [this, dialog, destSysex, externalFile, hasExternalFile](int response) {
            storeProgramResponse(dialog, response, destSysex, externalFile, hasExternalFile);
        }), true);
}

void DexedAudioProcessorEditor::storeProgramResponse(AlertWindow *dialog, int response, Cartridge destSysex, File externalFile, bool hasExternalFile) {
    if ( response == 2 ) {
        fileChooser = std::make_unique<FileChooser>("Destination Sysex", processor->dexedCartDir, "*.syx;*.SYX;*.*", true);
        fileChooser->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                 [this, destSysex](const FileChooser& fc) mutable {
                                     if ( fc.getResults().isEmpty() ) {
                                         cartManager.resetActiveSysex();
                                         return;
                                     }
                                     File chosenFile = fc.getResult();
                                     if ( destSysex.load(chosenFile) == 0 ) {
                                         storeProgramShowDialog(destSysex, chosenFile);
                                         return;
                                     }
                                     AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Read error", "Unable to read file");
                                     cartManager.resetActiveSysex();
                                 });
        return;
    }

    if ( response == 0 ) {
        TextEditor *name = dialog->getTextEditor("Name");
        ComboBox *dest = dialog->getComboBoxComponent("Dest");

        int programNum = dest->getSelectedItemIndex();
        String programName(name->getText());

        if ( ! hasExternalFile ) {
            // TODO: build something in the processor to do this...
            // processor->currentCart.packProgram((uint8_t *) processor->data, programNum, programName, processor->controllers.opSwitch);
            rebuildProgramCombobox();
            processor->setCurrentProgram(programNum);
            processor->updateHostDisplay();

            int action = dialog->getComboBoxComponent("SaveAction")->getSelectedItemIndex();
            if ( action > 0 ) {
                if ( action == 1 ) {
                    fileChooser = std::make_unique<FileChooser>("Destination Sysex", processor->dexedCartDir, "*.syx;*.SYX", true);
                    fileChooser->launchAsync(FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::warnAboutOverwriting,
                                             [this](const FileChooser& fc) {
                                                 if ( ! fc.getResults().isEmpty() ) {
                                                     File destination = fc.getResult();
                                                     processor->currentCart.saveVoice(destination);
                                                     processor->activeFileCartridge = destination;
                                                 }
                                                 cartManager.resetActiveSysex();
                                             });
                    return;
                }

                File destination = processor->activeFileCartridge;
                processor->currentCart.saveVoice(destination);
                processor->activeFileCartridge = destination;
            }
        } else {
            // TODO: build something in the processor to do this...
            // destSysex.packProgram((uint8_t *) processor->data, programNum, programName, processor->controllers.opSwitch);
            if ( ! destSysex.saveVoice(externalFile)) {
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Write error", "Unable to write file");
            }
        }
    }

    cartManager.resetActiveSysex();
}

float DexedAudioProcessorEditor::getLargestScaleFactor() {
    // constexpr float TESTING_SCALE_FACTOR[] = { 4.0f, 3.0f, 2.0f, 1.5f, 1.0f };
    //
    // for (float factor: TESTING_SCALE_FACTOR) {
    //     const juce::Rectangle<int> rect(WINDOW_SIZE_X * factor, WINDOW_SIZE_Y * factor);
    //
    //     // validate if there is really a display that can show the complete plugin size
    //     for (auto& display : Desktop::getInstance().getDisplays().displays) {
    //         int height = display.userArea.getHeight();
    //         int width = display.userArea.getWidth();
    //
    //         TRACE("Testing size %d x %d < Dexed Window %d x %d", height, width, rect.getWidth(), rect.getHeight() );
    //         if ( height > rect.getHeight() && width > rect.getWidth() ) {
    //             TRACE("Found factor %f for display %s with size %d x %d", factor, display.userArea.toString().toRawUTF8(), height, width );
    //             return factor;
    //         }
    //     }
    // }
    //
    // TRACE("No suitable display found, returning default scale factor 1.0");

    // For now, always return 4.0f as the maximum scale factor.
    return 4.0f;
}

void DexedAudioProcessorEditor::resetZoomFactor() {
    TRACE("Resetting zoom factor to 1.0");
    processor->setZoomFactor(1.0);
    processor->savePreference();
    AffineTransform scale = AffineTransform::scale(processor->getZoomFactor());
    frameComponent.setTransform(scale);
    resetSize();
}

bool DexedAudioProcessorEditor::isInterestedInFileDrag (const StringArray &files) {
    if( files.size() != 1 ) return false;

    for( auto i = files.begin(); i != files.end(); ++i ) {
        if( i->endsWithIgnoreCase( ".scl" ) || i->endsWithIgnoreCase( ".kbm" ) )
            return true;
    }
    return false;
}

void DexedAudioProcessorEditor::filesDropped (const StringArray &files, int x, int y ) {
    if( files.size() != 1 ) return;
    auto fn = files[0];
    try {
        std::ifstream in(fn.toStdString(), std::ifstream::ate | std::ifstream::binary);
        std::ifstream::pos_type filesize = in.tellg();
        if (fn.endsWithIgnoreCase(".scl")) {
            if (filesize == 0) {
                AlertWindow::showMessageBoxAsync(
                    AlertWindow::WarningIcon,
                    "File size error!",
                    "File \'" + fn.toStdString() + "\' is empty."
                );
            }
            else if (filesize > MAX_SCL_KBM_FILE_SIZE) {
                AlertWindow::showMessageBoxAsync(
                    AlertWindow::WarningIcon,
                    "File size error!",
                    "File \'" + fn.toStdString() + "\' has " + std::to_string(filesize) + " bytes, exceeding the maximum limit ("+std::to_string(MAX_SCL_KBM_FILE_SIZE)+")."
                );
            }
            else {
                processor->applySCLTuning(File(fn));
            }
        }
        if (fn.endsWithIgnoreCase(".kbm")) {
            if (filesize == 0) {
                AlertWindow::showMessageBoxAsync(
                    AlertWindow::WarningIcon,
                    "File size error!",
                    "File \'" + fn.toStdString() + "\' is empty."
                );
            }
            else if (filesize > MAX_SCL_KBM_FILE_SIZE) {
                AlertWindow::showMessageBoxAsync(
                    AlertWindow::WarningIcon,
                    "File size error!",
                    "File \'" + fn.toStdString() + "\' has " + std::to_string(filesize) + " bytes, exceeding the maximum limit (" + std::to_string(MAX_SCL_KBM_FILE_SIZE) + ")."
                );
            }
            else {
                processor->applyKBMMapping(File(fn));
            }
        }
    }
    catch (const std::ios_base::failure& ex) {
        AlertWindow::showMessageBoxAsync(
            AlertWindow::WarningIcon, 
            "I/O error!", 
            "Related to file \'" + fn.toStdString() + "\', an exception (std::ios_base::failure) occured: " + ex.what()
        );
    }
    catch (std::bad_alloc& ex) {
        AlertWindow::showMessageBoxAsync(
            AlertWindow::WarningIcon, 
            "I/O error!", 
            "Related to file \'" + fn.toStdString() + "\', an exception (std::bad_alloc) occured: " + ex.what());
    }
    catch (...) {
        AlertWindow::showMessageBoxAsync(
            AlertWindow::WarningIcon, 
            "I/O error!", 
            "Related to file \'"+fn.toStdString()+"\', an unknown exception occured.");
    };
}


//==============================================================================
void DexedAudioProcessorEditor::timerCallback() {
    if ( processor->programChanged.compareAndSetBool(false, true) ) {
        updateUI();
    }

    // Keep the data-entry fader and the LCD in sync with the selected
    // parameter, whatever changed it (mouse, host automation, MIDI CC...).
    if (dataParameter != nullptr) {
        const float value = dataParameter->getValue();
        if (value != lastDataValue) {
            lastDataValue = value;
            auto& slider = hardware.getDataEntrySlider();
            if (slider.getThumbBeingDragged() < 0)
                slider.setValue(value, dontSendNotification);
        }
    }
    updateLcd();

    if ( ! processor->peekVoiceStatus() )
        return;

    for(int i=0;i<6;i++) {
        const int amp_min = 1036152;
        const float one_per_amp_diff = (float)(1.0 / (259037922 - amp_min));
        // Note, that minimum and maximum values of ``amp`` are different and depend on the engines.
        // These two constants were determined from the results produced by the OPL engine,
        // because its minimum is smaller, its maximum is higher than in the other cases.

        int amp = processor->voiceStatus.amp[5 - i] - amp_min;
        if (amp <= 0) amp = 0;
        advanced.setOperatorStatus(i, amp * one_per_amp_diff, processor->voiceStatus.ampStep[5 - i]);
    }
    advanced.setPitchEnvelopeStage(processor->voiceStatus.pitchStep);
    advanced.setOutputLevel(processor->vuSignal);
}

void DexedAudioProcessorEditor::setDataParameter(RangedAudioParameter* parameter) {
    if (parameter == nullptr || parameter == dataParameter)
        return;
    dataParameter = parameter;
    lastDataValue = -1.0f;
    auto& slider = hardware.getDataEntrySlider();
    slider.setValue(parameter->getValue(), dontSendNotification);
    slider.setTitle("Data entry: " + parameter->getName(100));
    updateLcd();
}

void DexedAudioProcessorEditor::stepDataParameter(int delta) {
    if (dataParameter == nullptr)
        return;
    const auto range = dataParameter->getNormalisableRange();
    const float span = range.end - range.start;
    const float step = range.interval > 0.0f ? range.interval : span / 100.0f;
    const float current = dataParameter->convertFrom0to1(dataParameter->getValue());
    const float next = jlimit(range.start, range.end, current + step * (float) delta);
    dataParameter->beginChangeGesture();
    dataParameter->setValueNotifyingHost(dataParameter->convertTo0to1(next));
    dataParameter->endChangeGesture();
}

void DexedAudioProcessorEditor::updateLcd() {
    constexpr int columns = LcdDisplay::columns;
    const int current = processor->getCurrentProgram();
    auto name = processor->activeProgram.getName().trim().toUpperCase();
    if (name.isEmpty())
        name = "INIT VOICE";
    String algo;
    if (auto* p = processor->parameters.getParameter(IDs::algorithm.name))
        algo = "A" + String(roundToInt(p->convertFrom0to1(p->getValue()))).paddedLeft('0', 2);
    const auto top = String(current + 1).paddedLeft('0', 2) + " " + name.substring(0, 10).paddedRight(' ', 10)
                   + String().paddedRight(' ', columns - 13 - algo.length()) + algo;

    String bottom;
    if (dataParameter != nullptr) {
        const auto value = dataParameter->getCurrentValueAsText().trim().toUpperCase();
        const int nameSpace = jmax(1, columns - value.length() - 1);
        auto label = dataParameter->getName(100).toUpperCase().substring(0, nameSpace);
        bottom = label.paddedRight(' ', columns - value.length()) + value;
    }
    if (top != lastLcdTop || bottom != lastLcdBottom) {
        lastLcdTop = top;
        lastLcdBottom = bottom;
        hardware.setLcd(top, bottom);
    }
}

void DexedAudioProcessorEditor::updateUI() {
    rebuildProgramCombobox();
    cartManager.updateCartFilename();
    updateHeader();
    updateLcd();
}

void DexedAudioProcessorEditor::updateHeader() {
    auto name = processor->activeProgram.getName().trim();
    const int current = processor->getCurrentProgram() + 1;
    const int count = processor->getNumPrograms();
    String cart = processor->activeFileCartridge.existsAsFile()
                      ? processor->activeFileCartridge.getFileNameWithoutExtension().toUpperCase()
                      : String("INTERNAL");
    presetField.setText(name.isEmpty() ? "INIT VOICE" : name,
                        cart.substring(0, 14) + "   " + String(current).paddedLeft('0', 2) + " / "
                        + String(count).paddedLeft('0', 2));
    hardware.setCurrentVoice(processor->getCurrentProgram(), processor->programNames);
}

void DexedAudioProcessorEditor::selectProgram(int index) {
    const int count = processor->getNumPrograms();
    if (count <= 0)
        return;
    processor->setCurrentProgram(jlimit(0, count - 1, index));
    processor->updateHostDisplay();
    updateUI();
}

void DexedAudioProcessorEditor::moveToProgram(int direction) {
    const int count = processor->getNumPrograms();
    if (count <= 0)
        return;
    selectProgram((processor->getCurrentProgram() + direction + count) % count);
}

void DexedAudioProcessorEditor::rebuildProgramCombobox() {
    processor->currentCart.getProgramNames(processor->programNames);
    String name = processor->activeProgram.getName();
    cartManager.setActiveProgram(processor->getCurrentProgram(), name);
    cartManager.resetActiveSysex();
}

bool DexedAudioProcessorEditor::keyPressed(const KeyPress& key, Component* originatingComponent) {
    int keycode = key.getKeyCode();
    ModifierKeys mods = key.getModifiers();

    #ifdef DEXED_EVENT_DEBUG
        TRACE("key pressed: %d\n", keycode);
    #endif

    if ( (keycode >= '1' && keycode <= '6') && mods.isCtrlDown() ) {
        int op = keycode - '1';

        if ( mods.isShiftDown() ) {
            hardware.toggleOperator(op);
            return true;
        }

        if (!processor->showAdvanced)
            setAdvancedOpen(true);
        advanced.focusOperator(op);
        return true;
    }

    if ( keycode == 'G' && mods.isCtrlDown() ) {
        if (!processor->showAdvanced)
            setAdvancedOpen(true);
        advanced.focusGlobal();
        return true;
    }

    if ( keycode == 'L' && mods.isCtrlDown() ) {
        cartShow();
        return true;
    }

    if ( keycode == 'P' && mods.isCtrlDown() ) {
        parmShow();
        return true;
    }

    if ( keycode == 'E' && mods.isCtrlDown() ) {
        setAdvancedOpen(!processor->showAdvanced);
        return true;
    }

    if ( mods.isCtrlDown() && (keycode == KeyPress::leftKey || keycode == KeyPress::rightKey) ) {
        moveToProgram(keycode == KeyPress::leftKey ? -1 : 1);
        return true;
    }

    if ( key.getKeyCode() == KeyPress::escapeKey ) {
        cartManager.hideCartridgeManager();
        return true;
    }

    return false;
}

void DexedAudioProcessorEditor::resetSize() {
    const float factor = processor->getZoomFactor();
    layoutFrame();
    setSize(roundToInt(WINDOW_SIZE_X * factor), roundToInt(logicalHeight() * factor));
}
