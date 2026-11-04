/**
 *
 * Copyright (c) 2014-2025 Pascal Gauthier.
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

#include <juce_audio_processors/juce_audio_processors.h>
#include "parameter/DexedApvts.h"
#include "parameter/Model.h"
#include "../util/CachedParameter.h"

#include "../../debugger/Logger.h"

class AlgoDisplay : public juce::Component, public juce::SettableTooltipClient {
    DexedApvts &apvts;
    CachedParameter<int> algoValue;
    CachedParameter<bool> opSwitch[6];

    // Operator chip bounds in the 152 x 91 design space, refreshed on paint
    // and used for hit-testing clicks on the routing chart.
    juce::Rectangle<float> chipBounds[6];
    juce::AffineTransform designTransform;
    int selectedOp = -1;

    void displayOp(juce::Graphics &g, char id, int x, int y, char link, char fb);
    void drawAlgorithm(juce::Graphics &g, int algo);
    int operatorAt(juce::Point<float> position) const;
public:
    /** Called with a zero-based operator index when an operator chip is clicked. */
    std::function<void(int)> onOperatorSelected;

    AlgoDisplay(DexedApvts &apvts) : apvts(apvts) {
        algoValue.referTo(apvts, IDs::algorithm.name);
        algoValue.callback = [this]() {
            repaint();
        };

        for (int i=0;i<6;i++) {
            opSwitch[i].referTo(apvts, IDs::on.op(i).name);
            opSwitch[i].callback = [this]() {
                repaint();
            };
        }
        setTitle("Algorithm routing");
        setTooltip("Click an operator to edit it, right-click to mute or unmute it");
    }

    void setSelectedOperator(int op) {
        if (op != selectedOp) {
            selectedOp = op;
            repaint();
        }
    }

    void paint(juce::Graphics &g) override;
    void mouseDown(const juce::MouseEvent &event) override;
};
