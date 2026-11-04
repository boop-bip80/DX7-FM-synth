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

#include <juce_gui_basics/juce_gui_basics.h>
#include "../VintageTheme.h"

/** Flat membrane switch with an optional printed legend above it. */
class MembraneButton : public juce::Button {
public:
    MembraneButton(const juce::String& name, juce::Colour faceColour, const juce::String& legendText = {},
                   const juce::String& innerText = {})
        : juce::Button(name), face(faceColour), legend(legendText), inner(innerText) {
        setTitle(name);
        setWantsKeyboardFocus(true);
        setMouseClickGrabsKeyboardFocus(false);
    }

    void setLit(bool shouldBeLit) {
        if (lit != shouldBeLit) {
            lit = shouldBeLit;
            repaint();
        }
    }
    bool isLit() const { return lit; }
    void setLegend(const juce::String& s) { legend = s; repaint(); }
    void setLegendHeight(float h) { legendHeight = h; repaint(); }
    void setShowLed(bool shouldShow) { showLed = shouldShow; repaint(); }

    void paintButton(juce::Graphics& g, bool over, bool down) override;

private:
    juce::Colour face;
    juce::String legend, inner;
    float legendHeight = 13.0f;
    bool lit = false;
    bool showLed = true;
};

/**
 * Operator on/off membrane switch. Its component name is the parameter ID so
 * the APVTS ButtonAttachment drives it like any other switch.
 */
class OperatorMembraneSwitch : public juce::ToggleButton {
public:
    OperatorMembraneSwitch(const juce::String& paramId, int opNumber)
        : juce::ToggleButton(paramId), number(opNumber) {
        setMouseClickGrabsKeyboardFocus(false);
        setTooltip("Operator " + juce::String(opNumber) + " on/off (Ctrl+Shift+" + juce::String(opNumber) + ")");
    }
    void paintButton(juce::Graphics& g, bool over, bool down) override;
private:
    int number;
};

/** Two-line backlit character LCD. */
class LcdDisplay : public juce::Component {
public:
    LcdDisplay() { setTitle("Voice display"); }
    void setLines(const juce::String& top, const juce::String& bottom);
    void paint(juce::Graphics& g) override;
    static constexpr int columns = 20;
private:
    juce::String line1, line2;
};

/** Red two-digit seven segment display (voice number). */
class LedDisplay : public juce::Component {
public:
    void setValue(int v) {
        if (v != value) {
            value = v;
            repaint();
        }
    }
    void paint(juce::Graphics& g) override;
private:
    int value = 1;
    void drawDigit(juce::Graphics& g, juce::Rectangle<float> r, int digit);
};

/** Vertical tab that opens or closes the advanced workspace. */
class AdvancedTab : public juce::Button {
public:
    AdvancedTab() : juce::Button("Advanced") {
        setClickingTogglesState(true);
        setMouseClickGrabsKeyboardFocus(false);
        setTooltip("Open or close the advanced editing workspace (Ctrl+E)");
    }
    void paintButton(juce::Graphics& g, bool over, bool down) override;
};

/** Toolbar icon button. */
class IconButton : public juce::Button {
public:
    enum class Icon { menu, library, previous, next, save, gear, keyboard, panic, none };

    IconButton(const juce::String& name, Icon iconType, const juce::String& caption = {})
        : juce::Button(name), icon(iconType), label(caption) {
        setTitle(name);
        setMouseClickGrabsKeyboardFocus(false);
    }
    void setLabel(const juce::String& s) { label = s; repaint(); }
    void paintButton(juce::Graphics& g, bool over, bool down) override;
private:
    Icon icon;
    juce::String label;
};

/** Labelled pill toggle used on the advanced workspace. */
class DeckToggle : public juce::ToggleButton {
public:
    DeckToggle(const juce::String& paramId, const juce::String& caption)
        : juce::ToggleButton(paramId) {
        setButtonText(caption);
        setMouseClickGrabsKeyboardFocus(false);
    }
    void paintButton(juce::Graphics& g, bool over, bool down) override;
};

/** Tab header button in the advanced workspace. */
class DeckTabButton : public juce::Button {
public:
    explicit DeckTabButton(const juce::String& name) : juce::Button(name) {
        // Selection is managed explicitly by the owning panel.
        setMouseClickGrabsKeyboardFocus(false);
        setTitle(name + " tab");
    }
    void paintButton(juce::Graphics& g, bool over, bool down) override;
};
