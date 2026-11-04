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

#include "VintageControls.h"

using namespace juce;

//==============================================================================
void MembraneButton::paintButton(Graphics& g, bool over, bool down) {
    auto r = getLocalBounds().toFloat();
    if (legend.isNotEmpty()) {
        vt::text(g, legend, r.removeFromTop(legendHeight), lit ? vt::printCream : vt::printDim,
                 jmin(9.5f, legendHeight - 2.0f), true);
        r.removeFromTop(2.0f);
    }
    auto key = r.reduced(1.0f, 0.5f);
    const bool pressed = down || getToggleState();
    vt::drawMembrane(g, key, face, pressed, over, lit);

    if (inner.isNotEmpty()) {
        const auto ink = face.getPerceivedBrightness() > 0.55f ? Colour(0xff2a221d) : vt::printCream;
        vt::text(g, inner, key.reduced(3.0f, 1.0f), ink, jmin(10.0f, key.getHeight() * 0.42f), true);
    }

    if (showLed)
        vt::drawLed(g, { key.getRight() - 6.0f, key.getY() + 6.0f }, 2.2f, lit);

    if (hasKeyboardFocus(true)) {
        g.setColour(vt::accent.withAlpha(0.9f));
        g.drawRoundedRectangle(key.expanded(1.5f), 3.0f, 1.4f);
    }
}

//==============================================================================
void OperatorMembraneSwitch::paintButton(Graphics& g, bool over, bool down) {
    auto r = getLocalBounds().toFloat();
    const bool on = getToggleState();
    vt::text(g, String(number), r.removeFromTop(12.0f), on ? vt::printCream : vt::printDim, 9.5f, true);
    r.removeFromTop(2.0f);
    auto key = r.reduced(1.0f, 0.5f);
    vt::drawMembrane(g, key, vt::membraneViolet, down, over, on);
    vt::drawLed(g, { key.getCentreX(), key.getCentreY() }, 2.6f, on, vt::accent);
    if (hasKeyboardFocus(true)) {
        g.setColour(vt::accent.withAlpha(0.9f));
        g.drawRoundedRectangle(key.expanded(1.5f), 3.0f, 1.4f);
    }
}

//==============================================================================
void LcdDisplay::setLines(const String& top, const String& bottom) {
    auto fit = [](const String& s) {
        return s.toUpperCase().substring(0, columns).paddedRight(' ', columns);
    };
    const auto a = fit(top), b = fit(bottom);
    if (a != line1 || b != line2) {
        line1 = a;
        line2 = b;
        setTitle(line1.trim() + ". " + line2.trim());
        repaint();
    }
}

void LcdDisplay::paint(Graphics& g) {
    auto bezel = getLocalBounds().toFloat();
    g.setColour(Colour(0xff0d0b0a));
    g.fillRoundedRectangle(bezel, 4.0f);
    g.setColour(Colour(0xff5a4b40));
    g.drawRoundedRectangle(bezel.reduced(0.5f), 4.0f, 1.0f);

    auto glass = bezel.reduced(6.0f, 6.0f);
    g.setGradientFill(ColourGradient(vt::lcdLight, glass.getX(), glass.getY(),
                                     vt::lcdDark, glass.getX(), glass.getBottom(), false));
    g.fillRoundedRectangle(glass, 2.0f);

    // Character cells: unlit pixels are faintly visible like a real STN LCD.
    const auto area = glass.reduced(8.0f, 6.0f);
    const float cellW = area.getWidth() / columns;
    const float cellH = area.getHeight() / 2.0f;
    const auto font = vt::monoFont(cellH * 0.78f, true);
    for (int row = 0; row < 2; ++row) {
        const auto& line = row == 0 ? line1 : line2;
        for (int col = 0; col < columns; ++col) {
            auto cell = Rectangle<float>(area.getX() + col * cellW, area.getY() + row * cellH,
                                         cellW, cellH).reduced(1.0f, 2.0f);
            g.setColour(vt::lcdInk.withAlpha(0.06f));
            g.fillRect(cell);
            const auto ch = line.substring(col, col + 1);
            if (ch.isNotEmpty() && ch != " ") {
                g.setColour(vt::lcdInk.withAlpha(0.92f));
                g.setFont(font);
                g.drawText(ch, cell.toNearestInt(), Justification::centred, false);
            }
        }
    }

    // Glass reflection.
    g.setGradientFill(ColourGradient(Colours::white.withAlpha(0.16f), glass.getX(), glass.getY(),
                                     Colours::transparentWhite, glass.getX(), glass.getCentreY(), false));
    g.fillRoundedRectangle(glass.withHeight(glass.getHeight() * 0.5f), 2.0f);
}

//==============================================================================
void LedDisplay::drawDigit(Graphics& g, Rectangle<float> r, int digit) {
    // a b c d e f g
    static const bool segments[10][7] = {
        {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1},
        {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
    };
    const float t = r.getWidth() * 0.17f;       // segment thickness
    const float w = r.getWidth(), h = r.getHeight();
    const float x = r.getX(), y = r.getY();
    const float slant = w * 0.08f;

    auto seg = [&](float x1, float y1, float x2, float y2, bool horizontal) {
        Path p;
        if (horizontal) {
            p.startNewSubPath(x1 + t * 0.5f, y1);
            p.lineTo(x1 + t, y1 - t * 0.5f);
            p.lineTo(x2 - t, y2 - t * 0.5f);
            p.lineTo(x2 - t * 0.5f, y2);
            p.lineTo(x2 - t, y2 + t * 0.5f);
            p.lineTo(x1 + t, y1 + t * 0.5f);
        } else {
            p.startNewSubPath(x1, y1 + t * 0.5f);
            p.lineTo(x1 + t * 0.5f, y1 + t);
            p.lineTo(x2 + t * 0.5f, y2 - t);
            p.lineTo(x2, y2 - t * 0.5f);
            p.lineTo(x2 - t * 0.5f, y2 - t);
            p.lineTo(x1 - t * 0.5f, y1 + t);
        }
        p.closeSubPath();
        return p;
    };

    const float l = x + t * 0.5f, rr = x + w - t * 0.5f;
    const float top = y + t * 0.5f, mid = y + h * 0.5f, bot = y + h - t * 0.5f;
    Path paths[7] = {
        seg(l + slant, top, rr + slant, top, true),          // a
        seg(rr + slant, top, rr + slant * 0.5f, mid, false), // b
        seg(rr + slant * 0.5f, mid, rr, bot, false),         // c
        seg(l, bot, rr, bot, true),                          // d
        seg(l + slant * 0.5f, mid, l, bot, false),           // e
        seg(l + slant, top, l + slant * 0.5f, mid, false),   // f
        seg(l + slant * 0.5f, mid, rr + slant * 0.5f, mid, true) // g
    };
    for (int i = 0; i < 7; ++i) {
        const bool on = digit >= 0 && segments[digit][i];
        if (on) {
            g.setColour(vt::ledRed.withAlpha(0.25f));
            g.strokePath(paths[i], PathStrokeType(t * 0.9f));
        }
        g.setColour(on ? vt::ledRed : vt::ledOff);
        g.fillPath(paths[i]);
    }
}

void LedDisplay::paint(Graphics& g) {
    auto r = getLocalBounds().toFloat();
    g.setColour(Colour(0xff0d0b0a));
    g.fillRoundedRectangle(r, 4.0f);
    g.setColour(Colour(0xff5a4b40));
    g.drawRoundedRectangle(r.reduced(0.5f), 4.0f, 1.0f);
    auto window = r.reduced(6.0f);
    g.setColour(Colour(0xff1c0705));
    g.fillRoundedRectangle(window, 2.0f);
    auto digits = window.reduced(window.getWidth() * 0.14f, window.getHeight() * 0.16f);
    const float dw = digits.getWidth() * 0.40f;
    drawDigit(g, { digits.getX(), digits.getY(), dw, digits.getHeight() }, (value / 10) % 10);
    drawDigit(g, { digits.getRight() - dw, digits.getY(), dw, digits.getHeight() }, value % 10);
    g.setGradientFill(ColourGradient(Colours::white.withAlpha(0.08f), window.getX(), window.getY(),
                                     Colours::transparentWhite, window.getX(), window.getCentreY(), false));
    g.fillRoundedRectangle(window.withHeight(window.getHeight() * 0.5f), 2.0f);
}

//==============================================================================
void AdvancedTab::paintButton(Graphics& g, bool over, bool down) {
    auto r = getLocalBounds().toFloat().reduced(3.0f, 6.0f);
    const bool open = getToggleState();
    g.setColour(Colours::black.withAlpha(0.25f));
    g.fillRoundedRectangle(r, 5.0f);
    g.setColour(vt::printRule.withAlpha(over ? 0.9f : 0.55f));
    g.drawRoundedRectangle(r, 5.0f, 1.0f);

    auto box = r.withSizeKeepingCentre(26.0f, 34.0f).translated(0.0f, -10.0f);
    if (open) {
        g.setColour(vt::accent);
        g.fillRoundedRectangle(box, 4.0f);
    } else if (over) {
        g.setColour(vt::printCream.withAlpha(0.12f));
        g.fillRoundedRectangle(box, 4.0f);
    }
    const auto ink = open ? Colour(0xff10241c) : (down ? vt::accent : vt::printCream);
    g.setColour(ink);
    const float cx = box.getCentreX();
    for (int i = 0; i < 2; ++i) {
        const float cy = box.getCentreY() - 6.0f + i * 10.0f;
        Path chevron;
        if (open) { // up
            chevron.startNewSubPath(cx - 7.0f, cy + 3.5f);
            chevron.lineTo(cx, cy - 3.5f);
            chevron.lineTo(cx + 7.0f, cy + 3.5f);
        } else {    // down
            chevron.startNewSubPath(cx - 7.0f, cy - 3.5f);
            chevron.lineTo(cx, cy + 3.5f);
            chevron.lineTo(cx + 7.0f, cy - 3.5f);
        }
        g.strokePath(chevron, PathStrokeType(2.2f, PathStrokeType::curved, PathStrokeType::rounded));
    }

    Graphics::ScopedSaveState state(g);
    const auto labelCentre = Point<float>(r.getCentreX(), box.getBottom() + 42.0f);
    g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, labelCentre.x, labelCentre.y));
    vt::text(g, "ADVANCED", Rectangle<float>(80.0f, 14.0f).withCentre(labelCentre),
             open ? vt::accent : vt::printDim, 9.5f, true);
}

//==============================================================================
void IconButton::paintButton(Graphics& g, bool over, bool down) {
    auto r = getLocalBounds().toFloat().reduced(1.0f);
    const bool on = getToggleState();
    if (over || down || on) {
        g.setColour(on ? vt::accent.withAlpha(0.18f) : Colours::white.withAlpha(down ? 0.12f : 0.07f));
        g.fillRoundedRectangle(r, 4.0f);
    }
    if (hasKeyboardFocus(true)) {
        g.setColour(vt::accent.withAlpha(0.8f));
        g.drawRoundedRectangle(r, 4.0f, 1.2f);
    }
    const auto ink = on ? vt::accent : (over ? Colours::white : vt::toolbarText);
    g.setColour(ink);

    auto iconArea = label.isEmpty() ? r : r.removeFromLeft(r.getHeight());
    const auto c = iconArea.getCentre();
    const PathStrokeType stroke(1.7f, PathStrokeType::curved, PathStrokeType::rounded);
    Path p;
    switch (icon) {
        case Icon::menu:
            for (int i = -1; i <= 1; ++i)
                g.fillRoundedRectangle(c.x - 7.5f, c.y + i * 5.0f - 1.0f, 15.0f, 2.0f, 1.0f);
            break;
        case Icon::library:
            for (int i = 0; i < 3; ++i)
                g.fillRoundedRectangle(c.x - 8.0f + i * 5.0f, c.y - 7.0f, 2.6f, 14.0f, 1.0f);
            p.startNewSubPath(c.x + 5.0f, c.y - 6.5f);
            p.lineTo(c.x + 9.0f, c.y + 6.5f);
            g.strokePath(p, PathStrokeType(2.4f, PathStrokeType::curved, PathStrokeType::rounded));
            break;
        case Icon::previous:
        case Icon::next: {
            const float dir = icon == Icon::next ? 1.0f : -1.0f;
            p.startNewSubPath(c.x - 3.0f * dir, c.y - 6.0f);
            p.lineTo(c.x + 3.0f * dir, c.y);
            p.lineTo(c.x - 3.0f * dir, c.y + 6.0f);
            g.strokePath(p, PathStrokeType(2.0f, PathStrokeType::curved, PathStrokeType::rounded));
            break;
        }
        case Icon::save:
            p.addRoundedRectangle(c.x - 7.0f, c.y - 7.0f, 14.0f, 14.0f, 2.0f);
            g.strokePath(p, stroke);
            g.fillRect(c.x - 4.0f, c.y - 7.0f, 8.0f, 4.5f);
            g.fillRoundedRectangle(c.x - 4.0f, c.y + 1.5f, 8.0f, 4.0f, 1.0f);
            break;
        case Icon::gear: {
            p.addCentredArc(c.x, c.y, 4.0f, 4.0f, 0.0f, 0.0f, MathConstants<float>::twoPi, true);
            g.strokePath(p, stroke);
            for (int i = 0; i < 8; ++i) {
                const float a = i * MathConstants<float>::twoPi / 8.0f;
                g.drawLine(c.x + std::sin(a) * 5.5f, c.y - std::cos(a) * 5.5f,
                           c.x + std::sin(a) * 8.0f, c.y - std::cos(a) * 8.0f, 2.4f);
            }
            break;
        }
        case Icon::keyboard: {
            auto kb = Rectangle<float>(16.0f, 11.0f).withCentre(c);
            g.drawRoundedRectangle(kb, 1.5f, 1.4f);
            for (int i = 1; i < 4; ++i)
                g.fillRect(kb.getX() + kb.getWidth() * i / 4.0f - 0.5f, kb.getY(), 1.0f, kb.getHeight());
            for (int i : { 1, 2 })
                g.fillRect(kb.getX() + kb.getWidth() * i / 4.0f - 1.5f, kb.getY(), 3.0f, kb.getHeight() * 0.55f);
            break;
        }
        case Icon::panic:
            p.addCentredArc(c.x, c.y, 7.0f, 7.0f, 0.0f, 0.0f, MathConstants<float>::twoPi, true);
            g.strokePath(p, stroke);
            g.fillRoundedRectangle(c.x - 1.0f, c.y - 4.5f, 2.0f, 6.0f, 1.0f);
            g.fillEllipse(c.x - 1.2f, c.y + 2.6f, 2.4f, 2.4f);
            break;
        case Icon::none:
            break;
    }

    if (label.isNotEmpty())
        vt::text(g, label, r, ink, 11.0f, true, Justification::centredLeft);
}

//==============================================================================
void DeckToggle::paintButton(Graphics& g, bool over, bool down) {
    auto r = getLocalBounds().toFloat().reduced(1.0f);
    const bool on = getToggleState();
    g.setColour(on ? vt::accentDeep.withAlpha(0.55f) : vt::deckInset);
    g.fillRoundedRectangle(r, 5.0f);
    g.setColour(on ? vt::accent : (over ? vt::deckDim : vt::deckLine));
    g.drawRoundedRectangle(r, 5.0f, hasKeyboardFocus(true) ? 2.0f : 1.0f);
    vt::drawLed(g, { r.getX() + 10.0f, r.getCentreY() }, 3.0f, on, vt::accent);
    vt::text(g, getButtonText(), r.withTrimmedLeft(18.0f).reduced(3.0f, 0.0f),
             on ? vt::deckText : vt::deckDim, 10.0f, true, Justification::centred);
    if (down) {
        g.setColour(Colours::black.withAlpha(0.2f));
        g.fillRoundedRectangle(r, 5.0f);
    }
}

//==============================================================================
void DeckTabButton::paintButton(Graphics& g, bool over, bool down) {
    auto r = getLocalBounds().toFloat();
    const bool on = getToggleState();
    if (on) {
        g.setColour(vt::deckPanel);
        g.fillRoundedRectangle(r.withTrimmedBottom(-4.0f), 5.0f);
        g.setColour(vt::accent);
        g.fillRoundedRectangle(r.getX() + 12.0f, r.getBottom() - 3.0f, r.getWidth() - 24.0f, 3.0f, 1.5f);
    } else if (over || down) {
        g.setColour(Colours::white.withAlpha(0.05f));
        g.fillRoundedRectangle(r, 5.0f);
    }
    if (hasKeyboardFocus(true)) {
        g.setColour(vt::accent.withAlpha(0.7f));
        g.drawRoundedRectangle(r.reduced(1.0f), 5.0f, 1.2f);
    }
    vt::text(g, getName().toUpperCase(), r, on ? vt::deckText : vt::deckDim, 11.5f, true);
}
