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

#ifndef PROGRAMLISTBOX_H_INCLUDED
#define PROGRAMLISTBOX_H_INCLUDED

#include "JuceHeader.h"
#include "PluginData.h"
#include "../DXLookNFeel.h"

class ProgramListBox;
class ProgramListBoxListener {
public:
    virtual ~ProgramListBoxListener() = default;
    virtual void programSelected(ProgramListBox *source, int pos) = 0;
    virtual void programRightClicked(ProgramListBox *source, int pos) = 0;
    virtual void programDragged(ProgramListBox *destListBox, int dest, char *packedPgm) = 0;
};

class ProgramLabel;
class ProgramListBox : public Component, public KeyListener {
    ProgramListBoxListener *listener = nullptr;
    Cartridge cartContent;
    std::unique_ptr<ProgramLabel> labels[32];

    bool hasContent;
    int cols, rows;
    float cellWidth, cellHeight;
    int activePgm;
    
    friend class ProgramLabel;
public:
    StringArray programNames;
    bool readOnly;
    
    ProgramListBox(const String name, int numCols);
    void addListener(ProgramListBoxListener *listener);
    void resized() override;
    void paint(Graphics &g) override;
    void setActive(int idx);
    
    Cartridge &getCurrentCart();
    void setCartridge(Cartridge &cart);
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;

    ProgramLabel *getProgramComponent(int idx) {
        return labels[idx].get();
    }
};

class ProgramLabel : public Component, public DragAndDropTarget, public juce::SettableTooltipClient {
    ProgramListBox *pgmListBox;
    bool inDrag = false;

public:
    int idx;

    ProgramLabel(ProgramListBox *pgmListBox, int idx) {
        this->pgmListBox = pgmListBox;
        this->idx = idx;
        setWantsKeyboardFocus(true);
        setExplicitFocusOrder(idx+1);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
        setTooltip("Double-click to load a patch. Use the arrow keys and Enter to browse.");
    }

    void paint(Graphics &g) override {
        if ( ! pgmListBox->hasContent || inDrag )
            return;

        const bool selected = idx == pgmListBox->activePgm;
        const bool focused = hasKeyboardFocus(false);
        const bool hovered = isMouseOver(false);
        const auto bounds = getLocalBounds().toFloat().reduced(0.5f);
        const auto accent = juce::Colour(0xff55d7c7);
        const auto darkCard = juce::Colour(0xff17242d);

        g.setColour(selected ? juce::Colour(0xff235d60)
                             : hovered ? juce::Colour(0xff29404c)
                                       : (idx % 2 == 0 ? darkCard : juce::Colour(0xff20303b)));
        g.fillRoundedRectangle(bounds, 4.0f);

        if (selected) {
            g.setColour(accent);
            g.fillRoundedRectangle(0.5f, 3.0f, 2.5f, juce::jmax(0.0f, (float)getHeight() - 6.0f), 1.0f);
        }
        if (focused || selected) {
            g.setColour(focused ? juce::Colour(0xfff2b563) : accent.withAlpha(0.55f));
            g.drawRoundedRectangle(bounds, 4.0f, focused ? 1.5f : 1.0f);
        }

        const bool compact = getWidth() < 69;
        const int numberWidth = compact ? 19 : 26;
        g.setColour(selected ? accent : juce::Colour(0xff8198a8));
        g.setFont(juce::Font(10.5f, juce::Font::bold));
        g.drawFittedText(juce::String(idx + 1).paddedLeft('0', 2),
                         4, 1, numberWidth - 4, getHeight() - 2,
                         juce::Justification::centredLeft, 1);

        g.setColour(juce::Colour(0xffe8e6da));
        g.setFont(juce::Font(11.5f, selected ? juce::Font::bold : juce::Font::plain));
        g.drawFittedText(getProgramName().trim(),
                         numberWidth, 1, juce::jmax(0, getWidth() - numberWidth - 4),
                         getHeight() - 2, juce::Justification::centredLeft, 1, 0.7f);
    }

    void focusGained(FocusChangeType cause) override {
        repaint();
    }

    void focusLost(FocusChangeType cause) override {
        repaint();
    }

    void mouseEnter(const MouseEvent&) override { repaint(); }
    void mouseExit(const MouseEvent&) override { repaint(); }

    String getProgramName() {
        return pgmListBox->cartContent.getProgramName(idx);
    }

    void loadProgram() {
        if ( ! pgmListBox->hasContent || pgmListBox->listener == nullptr )
            return;        
        pgmListBox->listener->programSelected(pgmListBox, idx);
    }

    void mouseDown(const MouseEvent &event) override {
        if ( ! pgmListBox->hasContent )
            return;

        grabKeyboardFocus();
        if ( event.mods.isPopupMenu()) {
            if (pgmListBox->listener != nullptr)
                pgmListBox->listener->programRightClicked(pgmListBox, idx);
            return;
        }
        if ( event.getNumberOfClicks() == 2 )
            loadProgram();
    }

    void mouseDrag(const MouseEvent &event) override {
        if ( ! pgmListBox->hasContent )
            return;
        if ( event.getDistanceFromDragStart() < 7 )
            return;
    
        if (DragAndDropContainer* const dragContainer = DragAndDropContainer::findParentDragContainerFor(this)) {
            ScaledImage snapshot;
            void *src = pgmListBox->cartContent.getRawVoice() + (idx*128);
            var description = var(src, 128);
            dragContainer->startDragging(description, this, snapshot, false);
        }        
    }

    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override {
        if ( pgmListBox->readOnly )
            return false;
        if ( ! pgmListBox->hasContent )
            return false;

        Component *comp = dragSourceDetails.sourceComponent.get();

        if ( comp == this )
            return false;
        if ( dynamic_cast<ProgramLabel*>(comp) == nullptr )
            return false;

        return true;
    }

    void itemDragEnter(const SourceDetails &dragSourceDetails) override {
        inDrag = true;
        repaint();
    }

    void itemDragMove(const SourceDetails &dragSourceDetails) override {
    }

    void itemDragExit(const SourceDetails &dragSourceDetails) override {
        inDrag = false;
        repaint();
    }

    void itemDropped(const SourceDetails& dragSourceDetails) override {
        inDrag = false;

        MemoryBlock* block = dragSourceDetails.description.getBinaryData();
        if ( block != nullptr && block->getSize() >= 128 && pgmListBox->listener != nullptr )
            pgmListBox->listener->programDragged(pgmListBox, idx, (char *)block->getData());

        repaint();
    }

    bool isActive() {
        return pgmListBox->hasContent;
    }

    struct ProgramLabelAH : public juce::AccessibilityHandler {
        explicit ProgramLabelAH(ProgramLabel *s): program(s), juce::AccessibilityHandler(*s, juce::AccessibilityRole::cell, getAccessibilityActions(s)) {
        }

        virtual String getTitle() const override {
            return "Patch " + String(program->idx + 1) + ": " + program->getProgramName();
        }

        ProgramLabel *program;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgramLabelAH);

        static AccessibilityActions getAccessibilityActions (ProgramLabel* label) {
            return AccessibilityActions().addAction(AccessibilityActionType::press, [label] { label->loadProgram(); });
        }        
    };

    std::unique_ptr< AccessibilityHandler> createAccessibilityHandler() override {
        return std::make_unique<ProgramLabelAH>(this);
    }
};

#endif  // PROGRAMLISTBOX_H_INCLUDED
