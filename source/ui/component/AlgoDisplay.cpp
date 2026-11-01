/**
 *
 * Copyright (c) 2014, 2025 Pascal Gauthier.
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

#include "AlgoDisplay.h"
#include "../DXLookNFeel.h"

void AlgoDisplay::displayOp(juce::Graphics &g, char id, int x, int y, char link, char fb) {
    const float LINE_SZ = 1.7f;
    String t(static_cast<int>(id));
    bool opOn = opSwitch[id-1];
    
    x *= 25;
    x += 3;
    y *= 21;
    y += 5;
    
    // Routing strokes go behind the operator chips. Colour changes convey
    // bypass state without relying solely on an on/off indicator.
    g.setColour(opOn ? Colour(0xff68c6bd) : Colour(0xff47545d));
    
    switch(link) {
        case 0 : // LINE DOWN
            g.drawLine(x+8, y+12, x+8, y+21, LINE_SZ);
            break;
        case 1: // ARROW TO RIGHT
            g.drawLine(x+8, y+12, x+8,  y+18, LINE_SZ);
            g.drawLine(x+7, y+18, x+34, y+18, LINE_SZ);
            break;
        case 2: // ARROW TO RIGHT JOIN
            g.drawLine(x+8,  y+12, x+8, y+19, LINE_SZ);
            break;
        case 3: // ARROW TO RIGHT AND DOWN
            g.drawLine(x+8, y+12, x+8, y+21, LINE_SZ);
            g.drawLine(x+7, y+18, x+34, y+18, LINE_SZ);
            g.drawLine(x+34, y+17, x+34, y+21, LINE_SZ);
            break;
        case 4: // ARROW TO RIGHT+LEFT AND DOWN
            g.drawLine(x+8, y+12, x+8, y+21, LINE_SZ);
            g.drawLine(x+7, y+18, x+34, y+18, LINE_SZ);
            g.drawLine(x+34, y+17, x+34, y+21, LINE_SZ);
            g.drawLine(x-17, y+18, x+8, y+18, LINE_SZ);
            g.drawLine(x-17, y+17, x-17, y+21, LINE_SZ);
            break;
        case 6:
            g.drawLine(x+8, y+12, x+8,  y+18, LINE_SZ);
            g.drawLine(x+7, y+18, x+58, y+18, LINE_SZ);
            break;
        case 7: // ARROW TO LEFT
            g.drawLine(x+8,  y+12, x+8, y+19, LINE_SZ);
            g.drawLine(x-17, y+18, x+9, y+18, LINE_SZ);
            break;
    }
    
    switch(fb) {
        case 0:
            break;
        case 1:
            g.drawLine(x+7, y, x+8, y-5, LINE_SZ);
            g.drawLine(x+8, y-4, x+21, y-4, LINE_SZ);
            g.drawLine(x+20, y-4, x+20, y+15, LINE_SZ);
            g.drawLine(x+19, y+15, x+20, y+16, LINE_SZ);
            g.drawLine(x+8, y+15, x+20, y+15, LINE_SZ);
            break;
        case 2: // ALGO 4
            g.drawLine(x+7, y, x+8, y-5, LINE_SZ);
            g.drawLine(x+8, y-4, x+20, y-4, LINE_SZ);
            g.drawLine(x+19, y-4, x+19, y+59, LINE_SZ);
            g.drawLine(x+8, y+58, x+19, y+58, LINE_SZ);
            break;
        case 3: // ALGO 6
            g.drawLine(x+7, y, x+8, y-5, LINE_SZ);
            g.drawLine(x+8, y-4, x+20, y-4, LINE_SZ);
            g.drawLine(x+19, y-4, x+19, y+37, LINE_SZ);
            g.drawLine(x+8, y+36, x+19, y+36, LINE_SZ);
            break;
        case 4:
            g.drawLine(x+7, y, x+8, y-5, LINE_SZ);
            g.drawLine(x+8, y-4, x-4, y-4, LINE_SZ);
            g.drawLine(x-3, y-4, x-3, y+15, LINE_SZ);
            g.drawLine(x-3, y+15, x+8, y+15, LINE_SZ);
            g.drawLine(x+8, y+15, x+8, y+12, LINE_SZ);
            break;
    }

    const Colour operatorColours[6] = {
        Colour(0xff68d9c6), Colour(0xff85baec), Colour(0xffe8b66e),
        Colour(0xffbd9cf2), Colour(0xffe98d97), Colour(0xffb4d484)
    };
    const auto chip = Rectangle<float>((float)x - 1.0f, (float)y - 1.5f, 19.0f, 15.5f);
    g.setColour(Colour(0xff10202a));
    g.fillRoundedRectangle(chip, 3.0f);
    g.setColour(opOn ? operatorColours[id - 1] : Colour(0xff53616c));
    if (opOn)
        g.fillRoundedRectangle(chip.reduced(1.5f), 2.3f);
    else
        g.drawRoundedRectangle(chip, 3.0f, 1.0f);
    g.setColour(opOn ? Colour(0xff101820) : Colour(0xff95a1aa));
    g.setFont(juce::Font(juce::FontOptions().withHeight(10.5f).withStyle("Bold")));
    g.drawFittedText(t, chip.toNearestInt(), Justification::centred, 1);
    
}

void AlgoDisplay::paint(Graphics &g) {
    if (getWidth() <= 0 || getHeight() <= 0)
        return;

    const auto frame = getLocalBounds().toFloat().reduced(0.5f);
    g.setGradientFill(ColourGradient(Colour(0xff20303b), 0.0f, 0.0f,
                                      Colour(0xff101820), (float)getWidth(), (float)getHeight(), false));
    g.fillRoundedRectangle(frame, 6.0f);
    g.setColour(Colour(0xff3e5360));
    g.drawRoundedRectangle(frame, 6.0f, 0.9f);

    // Preserve all 32 original hardware routing diagrams, scaling their
    // geometry uniformly to whatever space the global editor provides.
    Graphics::ScopedSaveState state(g);
    const float scale = jmin(getWidth() / 152.0f, getHeight() / 91.0f);
    g.addTransform(AffineTransform::scale(scale).translated((getWidth() - 152.0f * scale) / 2.0f,
                                                          (getHeight() - 91.0f * scale) / 2.0f));
    const int algo = jlimit(1, 32, static_cast<int>(algoValue));
    g.setColour(Colour(0xff55d7c7));
    g.fillRoundedRectangle(2.0f, 3.0f, 20.0f, 16.0f, 3.5f);
    g.setColour(Colour(0xff101820));
    g.setFont(juce::Font(juce::FontOptions().withHeight(10.5f).withStyle("Bold")));
    g.drawFittedText(juce::String(algo), 2, 3, 20, 16, Justification::centred, 1);
    
    switch(algo-1) {
        case 0:
            displayOp(g, 6, 3, 0, 0, 1);
            displayOp(g, 5, 3, 1, 0, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 2, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 1:
            displayOp(g, 6, 3, 0, 0, 0);
            displayOp(g, 5, 3, 1, 0, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 2, 0);
            displayOp(g, 2, 2, 2, 0, 1);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 2:
            displayOp(g, 6, 3, 1, 0, 1);
            displayOp(g, 5, 3, 2, 0, 0);
            displayOp(g, 4, 3, 3, 2, 0);
            displayOp(g, 3, 2, 1, 0, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 3: //
            displayOp(g, 6, 3, 1, 0, 2);
            displayOp(g, 5, 3, 2, 0, 0);
            displayOp(g, 4, 3, 3, 2, 0);
            displayOp(g, 3, 2, 1, 0, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 4:
            displayOp(g, 6, 4, 2, 0, 1);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 1, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 5: //
            displayOp(g, 6, 4, 2, 0, 3);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 1, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 6:
            displayOp(g, 6, 4, 1, 0, 1);
            displayOp(g, 5, 4, 2, 7, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 2, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 7:
            displayOp(g, 6, 4, 1, 0, 0);
            displayOp(g, 5, 4, 2, 7, 0);
            displayOp(g, 4, 3, 2, 0, 4);
            displayOp(g, 3, 3, 3, 2, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 8:
            displayOp(g, 6, 4, 1, 0, 0);
            displayOp(g, 5, 4, 2, 7, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 2, 0);
            displayOp(g, 2, 2, 2, 0, 1);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 9:
            displayOp(g, 6, 2, 2, 0, 0);
            displayOp(g, 5, 1, 2, 1, 0);
            displayOp(g, 4, 2, 3, 1, 0);
            displayOp(g, 3, 3, 1, 0, 1);
            displayOp(g, 2, 3, 2, 0, 0);
            displayOp(g, 1, 3, 3, 2, 0);
            break;
        case 10:
            displayOp(g, 6, 2, 2, 0, 1);
            displayOp(g, 5, 1, 2, 1, 0);
            displayOp(g, 4, 2, 3, 1, 0);
            displayOp(g, 3, 3, 1, 0, 0);
            displayOp(g, 2, 3, 2, 0, 0);
            displayOp(g, 1, 3, 3, 2, 0);
            break;
        case 11:
            displayOp(g, 6, 3, 2, 7, 0);
            displayOp(g, 5, 2, 2, 0, 0);
            displayOp(g, 4, 1, 2, 1, 0);
            displayOp(g, 3, 2, 3, 6, 0);
            displayOp(g, 2, 4, 2, 0, 1);
            displayOp(g, 1, 4, 3, 2, 0);
            break;
        case 12:
            displayOp(g, 6, 3, 2, 7, 1);
            displayOp(g, 5, 2, 2, 0, 0);
            displayOp(g, 4, 1, 2, 1, 0);
            displayOp(g, 3, 2, 3, 6, 0);
            displayOp(g, 2, 4, 2, 0, 0);
            displayOp(g, 1, 4, 3, 2, 0);
            break;
        case 13:                                                                       
            displayOp(g, 6, 4, 1, 7, 1);// displayOp(g, 6, 3, 1, 0, 1);
            displayOp(g, 5, 3, 1, 0, 0);// displayOp(g, 5, 2, 1, 1, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 2, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 14:
            displayOp(g, 6, 4, 1, 7, 0);// displayOp(g, 6, 3, 1, 0, 0);
            displayOp(g, 5, 3, 1, 0, 0);// displayOp(g, 5, 2, 1, 1, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 2, 0);
            displayOp(g, 2, 2, 2, 0, 4);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 15:
            displayOp(g, 6, 4, 1, 0, 1);
            displayOp(g, 5, 4, 2, 7, 0);
            displayOp(g, 4, 3, 1, 0, 0);
            displayOp(g, 3, 3, 2, 0, 0);
            displayOp(g, 2, 2, 2, 1, 0);
            displayOp(g, 1, 3, 3, 0, 0);
            break;
        case 16:
            displayOp(g, 6, 4, 1, 0, 0);
            displayOp(g, 5, 4, 2, 7, 0);
            displayOp(g, 4, 3, 1, 0, 0);
            displayOp(g, 3, 3, 2, 0, 0);
            displayOp(g, 2, 2, 2, 1, 4);
            displayOp(g, 1, 3, 3, 0, 0);
            break;
        case 17:
            displayOp(g, 6, 4, 0, 0, 0);
            displayOp(g, 5, 4, 1, 0, 0);
            displayOp(g, 4, 4, 2, 7, 0);
            displayOp(g, 3, 3, 2, 0, 4);
            displayOp(g, 2, 2, 2, 1, 0);
            displayOp(g, 1, 3, 3, 0, 0);
            break;
        case 18:
            displayOp(g, 6, 3, 2, 3, 1);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 3, 1, 0);
            displayOp(g, 3, 2, 1, 0, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 19:
            displayOp(g, 6, 4, 2, 0, 0);
            displayOp(g, 5, 3, 2, 1, 0);
            displayOp(g, 4, 4, 3, 2, 0);
            displayOp(g, 3, 1, 2, 3, 1);
            displayOp(g, 2, 2, 3, 6, 0);
            displayOp(g, 1, 1, 3, 1, 0);
            break;
        case 20:
            displayOp(g, 6, 3, 2, 3, 0);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 3, 1, 0);
            displayOp(g, 3, 1, 2, 3, 1);
            displayOp(g, 2, 2, 3, 1, 0);
            displayOp(g, 1, 1, 3, 1, 0);
            break;
        case 21:
            displayOp(g, 6, 3, 2, 4, 1);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 3, 1, 0);
            displayOp(g, 3, 2, 3, 1, 0);
            displayOp(g, 2, 1, 2, 0, 0);
            displayOp(g, 1, 1, 3, 1, 0);
            break;
        case 22: // CC
            displayOp(g, 6, 3, 2, 3, 1);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 3, 1, 0);
            displayOp(g, 3, 2, 2, 0, 0);
            displayOp(g, 2, 2, 3, 1, 0);
            displayOp(g, 1, 1, 3, 1, 0);
            break;
        case 23: // CC
            displayOp(g, 6, 3, 2, 4, 1);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 3, 1, 0);
            displayOp(g, 3, 2, 3, 1, 0);
            displayOp(g, 2, 1, 3, 1, 0);
            displayOp(g, 1, 0, 3, 1, 0);
            break;
        case 24: // CC
            displayOp(g, 6, 3, 2, 3, 1);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 3, 1, 0);
            displayOp(g, 3, 2, 3, 1, 0);
            displayOp(g, 2, 1, 3, 1, 0);
            displayOp(g, 1, 0, 3, 1, 0);
            break;
        case 25:
            displayOp(g, 6, 4, 2, 0, 1);
            displayOp(g, 5, 3, 2, 1, 0);
            displayOp(g, 4, 4, 3, 2, 0);
            displayOp(g, 3, 2, 2, 0, 0);
            displayOp(g, 2, 2, 3, 6, 0);
            displayOp(g, 1, 1, 3, 1, 0);
            break;
        case 26:
            displayOp(g, 6, 4, 2, 0, 0);
            displayOp(g, 5, 3, 2, 1, 0);
            displayOp(g, 4, 4, 3, 2, 0);
            displayOp(g, 3, 2, 2, 0, 1);
            displayOp(g, 2, 2, 3, 6, 0);
            displayOp(g, 1, 1, 3, 1, 0);
            break;
        case 27:
            displayOp(g, 6, 4, 3, 2, 0);
            displayOp(g, 5, 3, 1, 0, 1);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 1, 0);
            displayOp(g, 2, 2, 2, 0, 0);
            displayOp(g, 1, 2, 3, 1, 0);
            break;
        case 28:
            displayOp(g, 6, 4, 2, 0, 1);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 1, 0);
            displayOp(g, 2, 2, 3, 1, 0);
            displayOp(g, 1, 1, 3, 1, 0);
            break;
        case 29:
            displayOp(g, 6, 4, 3, 2, 0);
            displayOp(g, 5, 3, 1, 0, 1);
            displayOp(g, 4, 3, 2, 0, 0);
            displayOp(g, 3, 3, 3, 1, 0);
            displayOp(g, 2, 2, 3, 1, 0);
            displayOp(g, 1, 1, 3, 1, 0);
            break;
        case 30:
            displayOp(g, 6, 4, 2, 0, 1);
            displayOp(g, 5, 4, 3, 2, 0);
            displayOp(g, 4, 3, 3, 1, 0);
            displayOp(g, 3, 2, 3, 1, 0);
            displayOp(g, 2, 1, 3, 1, 0);
            displayOp(g, 1, 0, 3, 1, 0);
            break;
        case 31:
            displayOp(g, 6, 5, 3, 2, 1);
            displayOp(g, 5, 4, 3, 1, 0);
            displayOp(g, 4, 3, 3, 1, 0);
            displayOp(g, 3, 2, 3, 1, 0);
            displayOp(g, 2, 1, 3, 1, 0);
            displayOp(g, 1, 0, 3, 1, 0);       
            break;
        default:
            break;
    }
}

