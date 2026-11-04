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

/**
 * Shared palette and vector drawing helpers for the "vintage FM" editor.
 *
 * The editor follows the familiar two-level layout of modern hardware
 * emulations: a classic 1980s style front panel on top and a modern,
 * expandable "advanced" workspace below it. Everything is drawn with vector
 * primitives; no third-party artwork, bitmaps or logos are used.
 */
namespace vt {

using juce::Colour;

// ---------------------------------------------------------------------------
// Toolbar / status bar (neutral graphite)
inline const Colour toolbarTop      { 0xff202022 };
inline const Colour toolbarBottom   { 0xff151516 };
inline const Colour toolbarLine     { 0xff2e2e31 };
inline const Colour toolbarField    { 0xff0f0f10 };
inline const Colour toolbarText     { 0xffdedcd6 };
inline const Colour toolbarDim      { 0xff8c8a86 };

// ---------------------------------------------------------------------------
// Classic front panel (espresso chassis, cream print, membrane switches)
inline const Colour chassisTop      { 0xff342b26 };
inline const Colour chassisBottom   { 0xff231d1a };
inline const Colour chassisEdge     { 0xff4a3e36 };
inline const Colour printCream      { 0xffeadfc6 };
inline const Colour printDim        { 0xffb6a68b };
inline const Colour printRule       { 0xff6b5b4c };
inline const Colour membraneMaroon  { 0xff93323d };
inline const Colour membraneMint    { 0xff78bfa6 };
inline const Colour membraneCream   { 0xffdccfb2 };
inline const Colour membraneViolet  { 0xff74639a };
inline const Colour membraneBrown   { 0xff5a4a3f };
inline const Colour lcdLight        { 0xffbccd55 };
inline const Colour lcdDark         { 0xff98af38 };
inline const Colour lcdInk          { 0xff1d2610 };
inline const Colour ledRed          { 0xffff4433 };
inline const Colour ledOff          { 0xff3a110d };

// ---------------------------------------------------------------------------
// Advanced workspace (charcoal with a mint accent)
inline const Colour deckBackground  { 0xff1f2023 };
inline const Colour deckPanel       { 0xff2a2b2f };
inline const Colour deckPanelHi     { 0xff313236 };
inline const Colour deckInset       { 0xff17181a };
inline const Colour deckLine        { 0xff3c3d42 };
inline const Colour deckText        { 0xffe4e3df };
inline const Colour deckDim         { 0xff9a9995 };
inline const Colour deckFaint       { 0xff66666a };
inline const Colour accent          { 0xff5ad4a6 };
inline const Colour accentDeep      { 0xff2e7d63 };
inline const Colour warm            { 0xfff0b35e };

// Operator identity colours, used consistently in the algorithm chart,
// operator strips and envelope pages.
inline Colour operatorColour(int opIndex) {
    static const Colour colours[6] = {
        Colour(0xff5ad4a6), Colour(0xff7fb4ea), Colour(0xffefb766),
        Colour(0xffc09df0), Colour(0xffea8b96), Colour(0xffb6d684)
    };
    return colours[juce::jlimit(0, 5, opIndex)];
}

inline juce::Font font(float height, bool bold = false) {
    return juce::Font(juce::FontOptions().withHeight(height).withStyle(bold ? "Bold" : "Regular"));
}

inline juce::Font monoFont(float height, bool bold = true) {
    return juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), height,
                                        bold ? juce::Font::bold : juce::Font::plain));
}

inline void text(juce::Graphics& g, const juce::String& s, juce::Rectangle<float> r, Colour c,
                 float height, bool bold = true,
                 juce::Justification j = juce::Justification::centred) {
    g.setColour(c);
    g.setFont(font(height, bold));
    g.drawFittedText(s, r.toNearestInt(), j, 1, 0.8f);
}

// Brushed/grained chassis surface. Fine horizontal grain gives the flat
// colour some material without any bitmap texture.
inline void drawChassis(juce::Graphics& g, juce::Rectangle<float> r) {
    g.setGradientFill(juce::ColourGradient(chassisTop, r.getX(), r.getY(),
                                           chassisBottom, r.getX(), r.getBottom(), false));
    g.fillRect(r);
    juce::Random grain(7);
    for (float y = r.getY() + 1.0f; y < r.getBottom(); y += 2.0f) {
        g.setColour(juce::Colours::white.withAlpha(0.006f + grain.nextFloat() * 0.012f));
        g.fillRect(r.getX(), y, r.getWidth(), 1.0f);
    }
    g.setColour(chassisEdge);
    g.fillRect(r.getX(), r.getY(), r.getWidth(), 1.0f);
    g.setColour(juce::Colours::black.withAlpha(0.55f));
    g.fillRect(r.getX(), r.getBottom() - 2.0f, r.getWidth(), 2.0f);
}

// A printed panel rule with a label, the way silk-screened sections are
// separated on classic synthesizers.
inline void drawPrintedHeader(juce::Graphics& g, const juce::String& label,
                              juce::Rectangle<float> r, Colour c = printCream) {
    g.setFont(font(9.5f, true));
    const float labelWidth = juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), label) + 12.0f;
    const float cy = r.getCentreY();
    g.setColour(printRule);
    g.fillRect(r.getX(), cy, juce::jmax(0.0f, (r.getWidth() - labelWidth) * 0.5f), 1.0f);
    g.fillRect(r.getCentreX() + labelWidth * 0.5f, cy,
               juce::jmax(0.0f, (r.getWidth() - labelWidth) * 0.5f), 1.0f);
    g.setColour(c);
    g.drawText(label, r.withSizeKeepingCentre(labelWidth, r.getHeight()).toNearestInt(),
               juce::Justification::centred, false);
}

// Flat membrane switch: a coloured rectangle under a thin clear overlay.
inline void drawMembrane(juce::Graphics& g, juce::Rectangle<float> r, Colour face,
                         bool down, bool hover, bool lit) {
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(r.translated(0.0f, 1.2f), 2.0f);
    auto body = down ? face.darker(0.25f) : (hover ? face.brighter(0.10f) : face);
    if (lit)
        body = body.brighter(0.18f);
    g.setGradientFill(juce::ColourGradient(body.brighter(0.08f), r.getX(), r.getY(),
                                           body.darker(0.12f), r.getX(), r.getBottom(), false));
    g.fillRoundedRectangle(r, 2.0f);
    g.setColour(juce::Colours::white.withAlpha(down ? 0.04f : 0.14f));
    g.fillRoundedRectangle(r.withHeight(r.getHeight() * 0.42f).reduced(1.0f, 0.0f).translated(0.0f, 1.0f), 1.5f);
    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.drawRoundedRectangle(r, 2.0f, 0.8f);
}

inline void drawLed(juce::Graphics& g, juce::Point<float> c, float radius, bool on,
                    Colour colour = ledRed) {
    if (on) {
        g.setColour(colour.withAlpha(0.28f));
        g.fillEllipse(c.x - radius * 2.1f, c.y - radius * 2.1f, radius * 4.2f, radius * 4.2f);
    }
    g.setColour(on ? colour : colour.darker(2.4f).withSaturation(0.45f));
    g.fillEllipse(c.x - radius, c.y - radius, radius * 2.0f, radius * 2.0f);
    g.setColour(juce::Colours::white.withAlpha(on ? 0.55f : 0.10f));
    g.fillEllipse(c.x - radius * 0.45f, c.y - radius * 0.6f, radius * 0.7f, radius * 0.55f);
}

// Rounded charcoal card used throughout the advanced workspace.
inline void drawDeckCard(juce::Graphics& g, juce::Rectangle<float> r, const juce::String& title = {},
                         Colour stripe = accent) {
    g.setColour(juce::Colours::black.withAlpha(0.28f));
    g.fillRoundedRectangle(r.translated(0.0f, 1.5f), 6.0f);
    g.setGradientFill(juce::ColourGradient(deckPanelHi, r.getX(), r.getY(),
                                           deckPanel, r.getX(), r.getBottom(), false));
    g.fillRoundedRectangle(r, 6.0f);
    g.setColour(deckLine);
    g.drawRoundedRectangle(r.reduced(0.5f), 6.0f, 1.0f);
    if (title.isNotEmpty()) {
        g.setColour(stripe);
        g.fillRoundedRectangle(r.getX() + 10.0f, r.getY() + 10.0f, 3.0f, 11.0f, 1.5f);
        text(g, title, { r.getX() + 19.0f, r.getY() + 7.0f, r.getWidth() - 28.0f, 17.0f },
             deckText, 11.0f, true, juce::Justification::centredLeft);
    }
}

inline void drawCaption(juce::Graphics& g, const juce::String& s, juce::Rectangle<float> r,
                        Colour c = deckDim, float height = 9.5f) {
    text(g, s, r, c, height, true, juce::Justification::centred);
}

} // namespace vt
