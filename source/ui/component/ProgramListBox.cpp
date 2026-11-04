/**
 *
 * Copyright (c) 2015-2024 Pascal Gauthier.
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

#include "ProgramListBox.h"
#include "PluginData.h"

ProgramListBox::ProgramListBox(const String name, int numCols) : Component(name) {
    cols = juce::jlimit(1, 32, numCols);
    rows = 32 / cols;
    activePgm = -1;
    hasContent = false;
    readOnly = false;
    programNames.clear();
    
    for(int i=0;i<32;i++) {
        labels[i].reset(new ProgramLabel(this, i));
        addAndMakeVisible(labels[i].get());
    }

    setTitle(name);
    setWantsKeyboardFocus(true);
    addKeyListener(this);
    setFocusContainerType(FocusContainerType::focusContainer);
}

void ProgramListBox::resized() {
    if (getWidth() <= 0 || getHeight() <= 0)
        return;

    cellWidth = (float) getWidth() / cols;
    cellHeight = (float) getHeight() / rows;

    for(int i=0;i<32;i++) {
        int targetCols = i / rows;
        int targetRow = i % rows;
        labels[i].get()->setBounds(juce::roundToInt(cellWidth * targetCols + 1),
                                   juce::roundToInt(cellHeight * targetRow + 1),
                                   juce::roundToInt(cellWidth - 2),
                                   juce::roundToInt(cellHeight - 2));
    }
}

void ProgramListBox::setCartridge(Cartridge &cart) {
    cartContent = cart;
    cartContent.getProgramNames(programNames);
    hasContent = true;
    for (int i = 0; i < 32; ++i) {
        labels[i]->setTooltip("Patch " + juce::String(i + 1) + ": " +
                              cartContent.getProgramName(i) +
                              ". Double-click to load; drag to move or copy.");
        labels[i]->repaint();
    }
    repaint();
}

void ProgramListBox::addListener(ProgramListBoxListener *listener) {
    this->listener = listener;
}

void ProgramListBox::setActive(int idx) {
    const int previous = activePgm;
    activePgm = idx;
    if (juce::isPositiveAndBelow(previous, 32))
        labels[previous]->repaint();
    if (juce::isPositiveAndBelow(activePgm, 32))
        labels[activePgm]->repaint();
    repaint();
}

Cartridge &ProgramListBox::getCurrentCart() {
    return cartContent;
}

bool ProgramListBox::keyPressed(const KeyPress &key, Component *originatingComponent) {
    ProgramLabel *programLabel = dynamic_cast<ProgramLabel*>(getCurrentlyFocusedComponent());
    if ( programLabel == nullptr ) 
        return false;

    if ( key.isKeyCode(KeyPress::returnKey) || key.isKeyCode(KeyPress::spaceKey) ) {
        activePgm = programLabel->idx;
        if ( activePgm != -1 && listener != nullptr ) {
            listener->programSelected(this, activePgm);
        }
        return true;
    }

    int currentIdx = programLabel->idx;
    int column = currentIdx / rows;
    int row = currentIdx % rows;

    if ( key.isKeyCode(KeyPress::upKey) ) {
        row = (row + rows - 1) % rows;
    } else if ( key.isKeyCode(KeyPress::downKey) ) {
        row = (row + 1) % rows;
    } else if ( key.isKeyCode(KeyPress::leftKey) ) {
        column = (column + cols - 1) % cols;
    } else if ( key.isKeyCode(KeyPress::rightKey) ) {
        column = (column + 1) % cols;
    } else if ( key.isKeyCode(KeyPress::homeKey) ) {
        column = 0;
        row = 0;
    } else if ( key.isKeyCode(KeyPress::endKey) ) {
        column = cols - 1;
        row = rows - 1;
    } else if ( key.isKeyCode(KeyPress::pageUpKey) ) {
        row = 0;
    } else if ( key.isKeyCode(KeyPress::pageDownKey) ) {
        row = rows - 1;
    } else {
        return false;
    }

    currentIdx = column * rows + row;
    labels[currentIdx]->grabKeyboardFocus();

    repaint();
    return true;
}

void ProgramListBox::paint(Graphics &g) {
    g.fillAll(juce::Colour(0xff17181a));
    g.setColour(juce::Colour(0xff3c3d42));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f), 5.0f, 1.0f);
    if (!hasContent) {
        g.setColour(juce::Colour(0xff9a9995));
        g.setFont(juce::Font(12.0f));
        g.drawFittedText("Choose a cartridge to browse its 32 voices",
                         getLocalBounds().reduced(8), juce::Justification::centred, 2);
    }
}
