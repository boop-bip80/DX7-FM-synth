/**
 *
 * Copyright (c) 2014-2018 Pascal Gauthier.
 *
 * DX7 Envelope Tables from legasynth-0.4.1 / DX7 Patch Editor
 * Copyright (C) 2002 Juan Linietsky <coding@reduz.com.ar>
 * Copyright (C) 2006 Mark-André Hopf <mhopf@mark13.org>
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

#include "../../debugger/Logger.h"
#include "DXComponents.h"
#include "../DXLookNFeel.h"
#include "PluginProcessor.h"
#include "EnvDisplay.h"

static void comboBoxPopupMenuFinishedCallback(int result, ComboBoxImage* combo) {
    if (combo != nullptr) {
        combo->hidePopup();

        if (result != 0)
            combo->setSelectedId (result);
    }
}

ComboBoxImage::ComboBoxImage() {
    itemPos[0] = -1;
    itemHeight = 0;
}

void ComboBoxImage::paint(Graphics &g) {
    if (items.isNull() || itemHeight <= 0)
        return;

    int idx = getSelectedItemIndex();
    if ( itemPos[0] != -1 ) {
        if ( idx >= 0 && idx < 4 )
            idx = itemPos[idx];
    }

    if (idx < 0 || idx * itemHeight >= items.getHeight())
        return;

    g.drawImage(items, 0, 0, getWidth(), getHeight(), 0, idx * itemHeight, items.getWidth(), itemHeight);
}

void ComboBoxImage::showPopup() {
    popup.showMenuAsync (PopupMenu::Options().withTargetComponent (this)
                         .withItemThatMustBeVisible(getSelectedId())
                         .withMinimumWidth(getWidth())
                         .withMaximumNumColumns(1)
                         .withStandardItemHeight(itemHeight),
                         ModalCallbackFunction::forComponent(comboBoxPopupMenuFinishedCallback, this));
}

void ComboBoxImage::setImage(Image image) {
    items = image;
    
    int numItems = getNumItems();
    if (numItems == 0 || image.isNull())
        return;
    itemHeight = image.getHeight() / numItems;

    for(int i=0;i<numItems;i++) {
        Image tmp = image.getClippedImage(Rectangle<int>(0,itemHeight*i, image.getWidth(), itemHeight));
        popup.addItem(i+1, getItemText(i), true, false, tmp);
    }
}

void ComboBoxImage::setImage(Image image, int pos[]) {
    items = image;

    int numItems = getNumItems();
    if (numItems == 0 || image.isNull())
        return;
    itemHeight = 26;

    for(int i=0;i<numItems;i++) {
        Image tmp = image.getClippedImage(Rectangle<int>(0,itemHeight*pos[i], image.getWidth(), itemHeight));
        popup.addItem(i+1, getItemText(i), true, false, tmp);
    }

    for(int i=0;i<4;i++)
        itemPos[i] = pos[i];
}

void ProgramSelector::mouseDown(const MouseEvent &event) {
    if (event.x < getWidth() - 24 || getNumItems() == 0) {
        ComboBox::mouseDown(event);
        return;
    }
    
    int cur = getSelectedItemIndex();
    const int last = getNumItems() - 1;
    if ( event.y < getHeight() / 2 ) {
        if ( cur == 0 )
            cur = last;
        else
            cur--;
    } else {
        if ( cur == last )
            cur = 0;
        else
            cur++;
    }
    setSelectedItemIndex(jlimit(0, last, cur));
}

void ProgramSelector::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) {
    if (getNumItems() == 0)
        return;
    accum_wheel += wheel.deltaY;

    float wheelFactor = 0.2; // higher is slower
#if JUCE_WINDOWS
    wheelFactor = 0.4;
#endif    
    
    bool up = (accum_wheel > wheelFactor);
    bool dn = (accum_wheel < -wheelFactor);
    int sn = 1;
#if ! JUCE_MAC
    auto t = up;
    up = dn;
    dn = t;
    sn = -1;
#endif    
    
    if( up ) {
        accum_wheel -= sn * wheelFactor;
        int c = getSelectedItemIndex();
        if( c == getNumItems() - 1 ) c = 0;
        else c++;
        setSelectedItemIndex(c);
    }
    else if( dn ) {
        accum_wheel += sn * wheelFactor;
        int c = getSelectedItemIndex();
        if( c == 0 ) c = getNumItems() - 1;
        else c--;
        setSelectedItemIndex(jlimit(0, getNumItems() - 1, c));
    }
}

void ProgramSelector::paint(Graphics &g) {
    const float width = static_cast<float>(getWidth());
    const float height = static_cast<float>(getHeight());
    if (width < 30.0f || height < 12.0f)
        return;

    const auto bounds = getLocalBounds().toFloat().reduced(0.5f);
    g.setColour(Colour(0xFF0D151C));
    g.fillRoundedRectangle(bounds, 5.0f);
    g.setColour(hasKeyboardFocus(true) ? DXLookNFeel::fillColour : Colour(0xFF49616B));
    g.drawRoundedRectangle(bounds, 5.0f, hasKeyboardFocus(true) ? 1.6f : 1.0f);

    g.setFont(juce::Font(jmax(11.0f, jmin(14.0f, height * 0.58f))));
    g.setColour(Colour(0xFFE8E6DA));
    g.drawFittedText(getText(), 10, 1, getWidth() - 36, getHeight() - 2,
                     Justification::centredLeft, 1, 0.7f);

    const float arrowCenterX = width - 12.0f;
    Path arrows;
    arrows.addTriangle(arrowCenterX, height * 0.16f,
                       arrowCenterX - 4.0f, height * 0.41f,
                       arrowCenterX + 4.0f, height * 0.41f);
    arrows.addTriangle(arrowCenterX - 4.0f, height * 0.59f,
                       arrowCenterX + 4.0f, height * 0.59f,
                       arrowCenterX, height * 0.84f);
    g.setColour(DXLookNFeel::fillColour);
    g.fillPath(arrows);
}
