/**
 *
 * Copyright (c) 2013-2018 Pascal Gauthier.
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
#include "midi/SysexComm.h"
#include "window/TuningShow.h"
#include "../debugger/Logger.h"
#include "math.h"
#include <fstream>
#include <memory>

#include "../msfa/fm_op_kernel.h"

//==============================================================================
DexedAudioProcessorEditor::DexedAudioProcessorEditor (DexedAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      midiKeyboard (ownerFilter->keyboardState, MidiKeyboardComponent::horizontalKeyboard),
      cartManager(this), global(*ownerFilter, this) {
    processor = ownerFilter;

    // Set the host window size before attaching the keyboard so that hosts
    // which query it during construction see the correct dimensions.
    resetSize();
    setExplicitFocusOrder(1);

    frameComponent.setBounds(0, 0, WINDOW_SIZE_X,
                             processor->showKeyboard ? WINDOW_SIZE_Y : WINDOW_SIZE_Y - KEYBOARD_AREA_HEIGHT);
    addAndMakeVisible(&frameComponent);
    lookAndFeel->setDefaultLookAndFeel(lookAndFeel);
    background = lookAndFeel->background;

    // A readable, musical command bar: everything here delegates to the
    // existing program, cartridge and settings operations.
    voiceNameLabel.setName("Current FM voice");
    voiceNameLabel.setTitle("Current FM voice");
    voiceNameLabel.setJustificationType(Justification::centredLeft);
    voiceNameLabel.setFont(Font(23.0f, Font::bold));
    voiceNameLabel.setColour(Label::textColourId, Colour(0xffe8e6da));
    voiceNameLabel.setBounds(300, 18, 275, 30);
    frameComponent.addAndMakeVisible(voiceNameLabel);

    voiceNumberLabel.setName("Voice position and synthesis type");
    voiceNumberLabel.setJustificationType(Justification::centredLeft);
    voiceNumberLabel.setFont(Font(11.0f, Font::plain));
    voiceNumberLabel.setColour(Label::textColourId, Colour(0xff8ba6ae));
    voiceNumberLabel.setBounds(301, 50, 275, 19);
    frameComponent.addAndMakeVisible(voiceNumberLabel);

    auto styleToolbarButton = [this](TextButton& button, int x, int width, const String& help,
                                     bool emphasized = false) {
        button.setBounds(x, 21, width, 42);
        button.setTooltip(help);
        button.setColour(TextButton::buttonColourId,
                         emphasized ? Colour(0xff22776f) : Colour(0xff273943));
        button.setColour(TextButton::buttonOnColourId, Colour(0xff22776f));
        button.setColour(TextButton::textColourOffId, Colour(0xffe8e6da));
        button.setColour(TextButton::textColourOnId, Colour(0xffe8e6da));
        frameComponent.addAndMakeVisible(button);
    };

    styleToolbarButton(previousVoiceButton, 592, 65, "Previous voice in the loaded cartridge");
    styleToolbarButton(nextVoiceButton, 663, 65, "Next voice in the loaded cartridge");
    styleToolbarButton(libraryButton, 741, 106, "Browse and import DX7 SysEx cartridges");
    styleToolbarButton(saveVoiceButton, 855, 100, "Save this voice or cartridge", true);
    styleToolbarButton(settingsButton, 963, 100, "MIDI, tuning, audio and UI preferences");
    styleToolbarButton(keyboardButton, 1071, 112, "Show or hide the on-screen piano keyboard");

    previousVoiceButton.onClick = [this]() { moveToProgram(-1); };
    nextVoiceButton.onClick = [this]() { moveToProgram(1); };
    libraryButton.onClick = [this]() { cartShow(); };
    saveVoiceButton.onClick = [this]() { storeProgram(); };
    settingsButton.onClick = [this]() { parmShow(); };
    keyboardButton.setClickingTogglesState(true);
    keyboardButton.setToggleState(processor->showKeyboard, dontSendNotification);
    keyboardButton.onClick = [this]() {
        processor->showKeyboard = keyboardButton.getToggleState();
        processor->savePreference();
        resetSize();
    };

    // The six operators form two uniform rows. Their editors own all sound
    // controls, so no audio parameter bindings are moved or duplicated.
    for (int i = 0; i < 6; ++i) {
        operators[i] = std::make_unique<OperatorEditor>(*processor, i);
        frameComponent.addAndMakeVisible(operators[i].get());
    }
    constexpr int operatorColumns[] = { 16, 409, 802 };
    for (int i = 0; i < 6; ++i)
        operators[i]->setBounds(operatorColumns[i % 3], i < 3 ? 110 : 344, 381, 226);

    // Retain the original fully playable MIDI keyboard.
    frameComponent.addAndMakeVisible (&midiKeyboard);
    midiKeyboard.setLowestVisibleKey(24);
    midiKeyboard.setBounds(18, 766, WINDOW_SIZE_X - 36, 69);
    midiKeyboard.setTitle("Keyboard keys");
    midiKeyboard.setColour(MidiKeyboardComponent::whiteNoteColourId, Colour(0xffe8e6da));
    midiKeyboard.setColour(MidiKeyboardComponent::blackNoteColourId, Colour(0xff17242d));
    midiKeyboard.setColour(MidiKeyboardComponent::keySeparatorLineColourId, Colour(0xff576b72));
    midiKeyboard.setColour(MidiKeyboardComponent::keyDownOverlayColourId, Colour(0xff55d7c7));

    // Engine-wide algorithm, LFO, tuning and performance controls.
    frameComponent.addAndMakeVisible(&global);
    global.setBounds(18, 594, WINDOW_SIZE_X - 36, 160);
    global.cartButton->onClick = [this]() { cartShow(); };
    global.parmButton->onClick = [this]() { parmShow(); };
    global.initButton->onClick = [this]() { initProgram(); };
    global.storeButton->onClick = [this]() { storeProgram(); };

    rebuildProgramCombobox();
    global.programs->addListener(this);

    frameComponent.addChildComponent(&cartManagerCover);
    cartManagerCover.addChildComponent(&cartManager);
    cartManager.setVisible(true);

    AffineTransform scale = AffineTransform::scale(processor->getZoomFactor());
    frameComponent.setTransform(scale);
    resetSize();
    addKeyListener(this);
    updateUI();
    startTimer(20);
}

DexedAudioProcessorEditor::~DexedAudioProcessorEditor() {
    stopTimer();
    setLookAndFeel(nullptr);
}

//==============================================================================
void DexedAudioProcessorEditor::paint (Graphics& g) {
    // Draw at logical coordinates, then apply the same user-selected scale as
    // the child frame. Every vector border remains sharp on HiDPI hosts.
    g.fillAll(Colour(0xff101820));
    g.addTransform(AffineTransform::scale(static_cast<float>(getWidth()) / WINDOW_SIZE_X));

    ColourGradient backdrop(Colour(0xff15232e), 600.0f, 0.0f,
                            Colour(0xff0d151c), 600.0f, WINDOW_SIZE_Y, false);
    g.setGradientFill(backdrop);
    g.fillRect(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y);

    g.setColour(Colour(0xff1a2b35));
    g.fillRoundedRectangle(12.0f, 11.0f, 1176.0f, 67.0f, 11.0f);
    g.setColour(Colour(0xff345360));
    g.drawRoundedRectangle(12.5f, 11.5f, 1175.0f, 66.0f, 10.0f, 1.0f);

    // An original product mark, intentionally not a recreation of another
    // manufacturer's artwork or logo.
    g.setColour(Colour(0xff55d7c7));
    g.fillRoundedRectangle(25.0f, 22.0f, 4.0f, 43.0f, 2.0f);
    g.setColour(Colour(0xffe8e6da));
    g.setFont(Font(28.0f, Font::bold));
    g.drawText("DX7", 40, 17, 93, 36, Justification::centredLeft);
    g.setColour(Colour(0xff55d7c7));
    g.setFont(Font(15.0f, Font::bold));
    g.drawText("/ FM", 127, 24, 77, 23, Justification::centredLeft);
    g.setColour(Colour(0xff8ba6ae));
    g.setFont(Font(10.0f, Font::plain));
    g.drawText("SIX-OPERATOR SYNTHESIZER", 40, 49, 230, 17, Justification::centredLeft);

    g.setColour(Colour(0xff243842));
    g.fillRoundedRectangle(286.0f, 17.0f, 293.0f, 55.0f, 7.0f);
    g.setColour(Colour(0xff36515b));
    g.drawRoundedRectangle(286.5f, 17.5f, 292.0f, 54.0f, 7.0f, 1.0f);

    auto sectionHeader = [&g](const String& name, const String& helper, float top) {
        g.setColour(Colour(0xff55d7c7));
        g.fillRoundedRectangle(18.0f, top + 4.0f, 4.0f, 10.0f, 2.0f);
        g.setColour(Colour(0xffe8e6da));
        g.setFont(Font(13.0f, Font::bold));
        g.drawText(name, 31, static_cast<int>(top), 350, 19, Justification::centredLeft);
        g.setColour(Colour(0xff8298a3));
        g.setFont(Font(10.0f, Font::plain));
        g.drawText(helper, 570, static_cast<int>(top + 1), 612, 18, Justification::centredRight);
    };
    sectionHeader("OPERATOR LAB", "FREQUENCY  /  AMPLITUDE  /  ENVELOPES  /  KEY SCALING", 87.0f);
    sectionHeader("PERFORMANCE & GLOBAL", "ALGORITHM  /  LFO  /  PITCH  /  OUTPUT", 574.0f);

    // Subtle elevation separates the six editable sound modules.
    g.setColour(Colour(0x30000000));
    for (int row = 0; row < 2; ++row)
        for (int col = 0; col < 3; ++col)
            g.fillRoundedRectangle(18.0f + col * 393.0f, 114.0f + row * 234.0f,
                                   381.0f, 226.0f, 9.0f);

    if (processor->showKeyboard) {
        g.setColour(Colour(0xff1a2b35));
        g.fillRoundedRectangle(13.0f, 761.0f, 1174.0f, 78.0f, 8.0f);
        g.setColour(Colour(0xff2e4852));
        g.drawRoundedRectangle(13.5f, 761.5f, 1173.0f, 77.0f, 8.0f, 1.0f);
    }
}

void DexedAudioProcessorEditor::cartShow() {
    stopTimer();
    cartManager.resetActiveSysex();
    cartManagerCover.setBounds(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y - KEYBOARD_AREA_HEIGHT);
    // The cartridge browser retains its deliberately compact 800px data grid.
    // Centre it as a modal workspace instead of stretching its rows and cells.
    cartManager.setBounds((WINDOW_SIZE_X - 844) / 2, 124, 844, 556);
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
    global.programs->setSelectedId(processor->getCurrentProgram()+1, dontSendNotification);
    processor->updateHostDisplay();

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
    options.dialogBackgroundColour = Colour(0xFF323E44);
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
    param->setColour(AlertWindow::backgroundColourId, Colour(0xFF323E44));
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

                                   if ( ret == false ) {
                                       AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Midi Interface", "Error opening midi ports");
                                   }
                               };
    param->setGeneralCallback(generalCallback);

    dexedParameterDialog = std::make_unique<DexedDialogWindow>("dexed Parameters", Colour(0xFF323E44));
    dexedParameterDialog->setContentOwned(param, true);
    frameComponent.addAndMakeVisible(dexedParameterDialog.get());
    dexedParameterDialog->centreAroundComponent(&frameComponent, param->getWidth(), param->getHeight() + dexedParameterDialog->getTitleBarHeight());
    dexedParameterDialog->enterModalState(true,{});
}

void DexedAudioProcessorEditor::initProgram() {
    processor->resetToInitVoice();
}

void DexedAudioProcessorEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged) {
    processor->setCurrentProgram(global.programs->getSelectedId()-1);
    processor->updateHostDisplay();
    updateHeader();
}

void DexedAudioProcessorEditor::timerCallback() {
    if ( processor->programChanged.compareAndSetBool(false, true) ) {
        updateUI();
    }

    if ( ! processor->peekVoiceStatus() )
        return;

    for(int i=0;i<6;i++) {
        //operators[i].updateGain(sqrt(processor->voiceStatus.amp[5 - i]) / 8196);        // TODO: FUGLY !!!! change this sqrt nonsense

        const int amp_min = 1036152;
        const float one_per_amp_diff = (float)(1.0 / (259037922 - amp_min));
        // Note, that minimum and maximum values of ``amp`` are different and depend on the engines. 
        // These two constants were determined from the results produced by the OPL engine,
        // because its minimum is smaller, its maximum is higher than in the other cases. 

        int amp = processor->voiceStatus.amp[5 - i] - amp_min;
        if (amp <= 0) amp = 0;
        operators[i]->updateGain(amp * one_per_amp_diff);
        operators[i]->updateEnvPos(processor->voiceStatus.ampStep[5 - i]);
    }
    global.updatePitchPos(processor->voiceStatus.pitchStep);
    global.updateVu(processor->vuSignal);
}

void DexedAudioProcessorEditor::updateUI() {
    for(int i=0;i<6;i++) {
        operators[i]->updateDisplay();
    }
    rebuildProgramCombobox();
    global.updateDisplay();
    cartManager.updateCartFilename();
    updateHeader();
}

void DexedAudioProcessorEditor::updateHeader() {
    auto name = processor->activeProgram.getName().trim();
    voiceNameLabel.setText(name.isEmpty() ? "INIT VOICE" : name, dontSendNotification);

    const int current = processor->getCurrentProgram() + 1;
    const int count = processor->getNumPrograms();
    voiceNumberLabel.setText("VOICE " + String(current).paddedLeft('0', 2)
                            + " / " + String(count).paddedLeft('0', 2)
                            + "     -     6 OP / 32 ALGORITHMS", dontSendNotification);
}

void DexedAudioProcessorEditor::moveToProgram(int direction) {
    const int count = processor->getNumPrograms();
    if (count <= 0)
        return;

    const int next = (processor->getCurrentProgram() + direction + count) % count;
    processor->setCurrentProgram(next);
    processor->updateHostDisplay();
    updateUI();
}

void DexedAudioProcessorEditor::rebuildProgramCombobox() {
    global.programs->clear(dontSendNotification);

    processor->currentCart.getProgramNames(processor->programNames);

    for(int i=0;i<processor->getNumPrograms();i++) {
        String id;
        id << (i+1) << ". " << processor->getProgramName(i);
        global.programs->addItem(id, i+1);
    }

    global.programs->setSelectedId(processor->getCurrentProgram()+1, dontSendNotification);

    String name = processor->activeProgram.getName();
    cartManager.setActiveProgram(processor->getCurrentProgram(), name);
    if ( name != processor->getProgramName(processor->getCurrentProgram()) )
        global.programs->setText("**. " + name, dontSendNotification);

    cartManager.resetActiveSysex();
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

bool DexedAudioProcessorEditor::keyPressed(const KeyPress& key, Component* originatingComponent) {
    int keycode = key.getKeyCode();
    ModifierKeys mods = key.getModifiers();

    #ifdef DEXED_EVENT_DEBUG
        TRACE("key pressed: %d\n", keycode);
    #endif

    if ( (keycode >= '1' && keycode <= '6') && mods.isCtrlDown() ) {
        int op = keycode - '1';

        if ( mods.isShiftDown() ) {
            operators[op]->toggleOpSwitch();
            return true;
        }

        operators[op]->grabKeyboardFocus();
        return true;
    }

    if ( keycode == 'G' && mods.isCtrlDown() ) {
        global.grabKeyboardFocus();
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

    if ( key.getKeyCode() == KeyPress::escapeKey ) {
        cartManager.hideCartridgeManager();
        return true;
    }

    return false;
}

void DexedAudioProcessorEditor::resetSize() {
    float factor = processor->getZoomFactor();
    const int logicalHeight = processor->showKeyboard ? WINDOW_SIZE_Y
                                                      : WINDOW_SIZE_Y - KEYBOARD_AREA_HEIGHT;
    frameComponent.setBounds(0, 0, WINDOW_SIZE_X, logicalHeight);
    midiKeyboard.setVisible(processor->showKeyboard);
    keyboardButton.setToggleState(processor->showKeyboard, dontSendNotification);
    setSize(roundToInt(WINDOW_SIZE_X * factor), roundToInt(logicalHeight * factor));
}
