/**
 *
 * Copyright (c) 2013-2024 Pascal Gauthier.
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
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 */
 
#include <inttypes.h>
#include <stdint.h>
#include <cmath>

#include "DXLookNFeel.h"
#include "component/DXComponents.h"
#include "../debugger/Logger.h"
#include "PluginProcessor.h"

#define REG_COLOUR(id, value) setColour(id, value); colourMap.set(#id, id)

/*
// Loads an image from the file specified by ``path`` and 
// returns it. 
// Otherwise it returns ``nullptr``, if the image was not loaded successfully.
//
// I propose it for deprecation.
//
Image findImage(String path) {
    Image img;
    if ( path.length() <= 3 ) 
        return img;
    //File imgFile(path); // it might cause an assertion in juce_File.cpp
    File imgFile = File::getCurrentWorkingDirectory().getChildFile(path);
    img = ImageCache::getFromFile(imgFile);
    if (img.isNull()) {
        TRACE("img.isNull() == true: cannot load image from the path=%s", path.toRawUTF8());
    }
    return img;
}
*/


// Loads an image from the file specified by ``path`` and 
// strore it in variable ``image``, only if the image was 
// loaded successfully from the file.
// Otherwise, content of variable ``image`` is not altered.
void replaceImage(String path, Image &image) {
    Image img;
    if (path.length() <= 3) {
        return;
    }
    File imgFile = File::getCurrentWorkingDirectory().getChildFile(path);
    img = ImageCache::getFromFile(imgFile);
    if (!img.isNull()) {
        // check sizes
        int hnew = img.getHeight();
        int wnew = img.getWidth();        
        int horig = image.getHeight();
        int worig = image.getWidth();
        if (horig != hnew || worig != wnew) {
            TRACE("WARNING: new image sizes (h=%d,w=%d) differ from original ones (h=%d, w=%d)", hnew, wnew, horig, worig);
        }

        // store the new image
        // 
        // TODO: if we really do not want allow to replace the original image
        // with a new one having different size, then the  previous ``if`` statement 
        // should be supplemented with an ``else`` branch, and the following 
        // assignment should be moved into this ``else`` branch.
        image = img;
    }
    else {
        TRACE("ERROR: img.isNull() == true: cannot load image from the path=%s", path.toRawUTF8());
    }
}

DXLookNFeel::DXLookNFeel() {
    Colour ctrlBackground;
    
    DexedAudioProcessor::dexedAppDir.setAsCurrentWorkingDirectory();
    ctrlBackground = Colour(0xFF0D151C);

    // WARNING! If you modify the colour IDs here, please actualize the file ''DexedTheme.md'' 
    // in the subdirectory ``~/dexed/Documentation/``    

    REG_COLOUR(TextButton::buttonColourId, Colour(0xFF24343F));
    REG_COLOUR(TextButton::buttonOnColourId, Colour(0xFF247D79));
    REG_COLOUR(TextButton::textColourOnId, Colour(0xFF101820));
    REG_COLOUR(TextButton::textColourOffId, Colour(0xFFE8E6DA));
    REG_COLOUR(Slider::rotarySliderOutlineColourId, Colour(0xFF405663));
    REG_COLOUR(Slider::rotarySliderFillColourId, fillColour);
    REG_COLOUR(Slider::textBoxTextColourId, Colour(0xFFE8E6DA));
    REG_COLOUR(Slider::textBoxBackgroundColourId, ctrlBackground);
    REG_COLOUR(Slider::textBoxHighlightColourId, fillColour.withAlpha(0.35f));
    REG_COLOUR(Slider::textBoxOutlineColourId, Colour(0xFF405663));
    REG_COLOUR(AlertWindow::backgroundColourId,lightBackground);
    REG_COLOUR(AlertWindow::textColourId, Colour(0xFFE8E6DA));
    REG_COLOUR(TextEditor::backgroundColourId,ctrlBackground);
    REG_COLOUR(TextEditor::textColourId, Colour(0xFFE8E6DA));
    REG_COLOUR(TextEditor::highlightColourId, fillColour);
    REG_COLOUR(TextEditor::outlineColourId, Colour(0xFF3E5961));
    REG_COLOUR(TextEditor::focusedOutlineColourId, fillColour);
    REG_COLOUR(ComboBox::backgroundColourId, ctrlBackground);
    REG_COLOUR(ComboBox::textColourId, Colour(0xFFE8E6DA));
    REG_COLOUR(ComboBox::buttonColourId, fillColour);
    REG_COLOUR(ComboBox::outlineColourId, Colour(0xFF405663));
    REG_COLOUR(Label::textColourId, Colour(0xFFE8E6DA));
    REG_COLOUR(Label::backgroundColourId, Colours::transparentBlack);
    REG_COLOUR(Label::outlineColourId, Colours::transparentBlack);
    REG_COLOUR(PopupMenu::backgroundColourId, background);
    REG_COLOUR(PopupMenu::textColourId, Colour(0xFFE8E6DA));
    REG_COLOUR(PopupMenu::highlightedTextColourId, Colour(0xFF101820));
    REG_COLOUR(PopupMenu::highlightedBackgroundColourId, fillColour);
    REG_COLOUR(TreeView::backgroundColourId, background);
    REG_COLOUR(DirectoryContentsDisplayComponent::highlightColourId, fillColour);
    REG_COLOUR(DirectoryContentsDisplayComponent::textColourId, Colour(0xFFE8E6DA));
    REG_COLOUR(DialogWindow::backgroundColourId, background);
    REG_COLOUR(ListBox::backgroundColourId, ctrlBackground);

    // Register ``Scrollbar::thumbColourId`` to allow its redefinion in ``DexedTheme.xml``.
    REG_COLOUR(ScrollBar::thumbColourId, background.darker());

    // WARNING! If you modify the images here, please actualize the file ''DexedTheme.md'' 
    // in the subdirectory ``~/dexed/Documentation/``
    imageKnob = ImageCache::getFromMemory(BinaryData::Knob_68x68_png, BinaryData::Knob_68x68_pngSize); // 2x
    imageSwitch = ImageCache::getFromMemory(BinaryData::Switch_96x52_png, BinaryData::Switch_96x52_pngSize); // 2x
    imageSwitchLighted = ImageCache::getFromMemory(BinaryData::SwitchLighted_48x26_png, BinaryData::SwitchLighted_48x26_pngSize);
    imageSwitchOperator = ImageCache::getFromMemory(BinaryData::Switch_64x64_png, BinaryData::Switch_64x64_pngSize); // 2x
    imageButton = ImageCache::getFromMemory(BinaryData::ButtonUnlabeled_50x30_png, BinaryData::ButtonUnlabeled_50x30_pngSize);
    imageSlider = ImageCache::getFromMemory(BinaryData::Slider_52x52_png, BinaryData::Slider_52x52_pngSize); // 2x
    imageScaling = ImageCache::getFromMemory(BinaryData::Scaling_36_26_png, BinaryData::Scaling_36_26_pngSize); // 2x
    imageLight = ImageCache::getFromMemory(BinaryData::Light_28x28_png, BinaryData::Light_28x28_pngSize); // 2x
    imageLFO = ImageCache::getFromMemory(BinaryData::LFO_36_26_png, BinaryData::LFO_36_26_pngSize);
    imageOperator =  ImageCache::getFromMemory(BinaryData::OperatorEditor_574x436_png, BinaryData::OperatorEditor_574x436_pngSize); // 2x
    imageGlobal = ImageCache::getFromMemory (BinaryData::GlobalEditor_1728x288_png, BinaryData::GlobalEditor_1728x288_pngSize); // 2x

    //---
    // load and parse the file ``DexedTheme.xml``
    //---

    File dexedTheme = DexedAudioProcessor::dexedAppDir.getChildFile("DexedTheme.xml");

    if ( ! dexedTheme.existsAsFile() ) {
        TRACE("file \"%s\" does not exists", dexedTheme.getFullPathName().toRawUTF8());
        return;
    }

    std::unique_ptr<XmlElement> root = XmlDocument::parse(dexedTheme);
    if ( root == NULL ) {
        TRACE("ERROR: XmlDocument::parse(): failed");
        return;
    }

    //--- 
    // Get custom ARGB color codes specified by the
    // ``colour id`` - ``value`` pairs in ``DexedTheme.xml`` file.
    //---

    //forEachXmlChildElementWithTagName(*root, colour, "colour") { // deprecated!
    for (XmlElement* colour = root->getFirstChildElement(); colour != nullptr; colour = colour->getNextElement()) {
        if (colour->hasTagName("colour")) {
            String name = colour->getStringAttribute("id", "");
            if ( name == "" )
                continue;
            String value = colour->getStringAttribute("value", "");
            if ( value == "" )
                continue;
            if ( value.length() < 8 ) {
                TRACE("ERROR: illegal value=\"%s\" at color id=\"%s\"; skipped", value.toRawUTF8(), name.toRawUTF8());
                continue;
            }
            //int conv = strtol(value.toRawUTF8(), NULL, 16); // as alpha (MSB) could be above 0x7F, hence ``strtol()`` is inappropiate to convert values exceeding ``0x7FFFFFFF`` 
            char* endptr = NULL;
            uint64_t conv = strtoull(value.toRawUTF8(), &endptr, 16);
            //TRACE("color id=\"%s\" value=\"%s\": conv=0x%" PRIx64 "", name.toRawUTF8(), value.toRawUTF8(), conv);
            if (endptr != nullptr && *endptr != '\0') {
                TRACE("ERROR: illegal char #%d in value=\"%s\" at color id=\"%s\"; skipped", (int)(*endptr), value.toRawUTF8(), name.toRawUTF8());
                continue;
            }
            if (conv > 0xFFFFFFFFULL) {
                TRACE("ERROR: value 0x%" PRIx64 " exceeded the limit at color id=\"%s\"; skipped", conv, name.toRawUTF8());
                continue;
            }
            if ( colourMap.contains(name) ) {
                setColour(colourMap[name], Colour((uint32_t)conv));
            } else {
                if ( name == "Dexed::backgroundId" ) {
                    background = Colour((uint32_t)conv);
                    continue;
                }
                else if ( name == "Dexed::fillColourId" ) {
                    fillColour = Colour((uint32_t)conv);
                    continue;
                }
                TRACE("ERROR: color id=\"%s\" not found in colourMap; skipped.", name.toRawUTF8());
            }
        }        
    }

    //---
    // Get the custom images specified by the 
    // ``image`` - ``path`` pairs in the "Dexed.xml" file.
    //---

    //forEachXmlChildElementWithTagName(*root, image, "image") { // deprecated!
    for (XmlElement* image = root->getFirstChildElement(); image != nullptr; image = image->getNextElement()) {
        if (image->hasTagName("image")) {
            String name = image->getStringAttribute("id", "");
            String path = image->getStringAttribute("path", "");
            //TRACE("image id=\'%s\' path=\'%s\'", name.toRawUTF8(), path.toRawUTF8());
            if (name == /*"Knob_34x34.png"*/"Knob_68x68.png") { // 2x                
                //imageKnob = findImage(path); // if the new image was not loaded successfully, ``findImage()`` returns ``nullptr``, so the existing ``imageKnob`` is lost
                replaceImage(path, imageKnob);
                continue;
            }
            if (name == /*"Switch_48x26.png"*/ "Switch_96x52.png") { // 2x
                //imageSwitch = findImage(path);
                replaceImage(path, imageSwitch);
                continue;
            }
            if (name == "SwitchLighted_48x26.png") {
                //imageSwitchLighted = findImage(path);
                replaceImage(path, imageSwitchLighted);
                continue;
            }
            if (name == /*"Switch_32x64.png"*/ "Switch_64x64.png") { // 2x
                //imageSwitchOperator = findImage(path);
                replaceImage(path, imageSwitchOperator);
                continue;
            }
            if (name == "ButtonUnlabeled_50x30.png") {
                //imageButton = findImage(path);
                replaceImage(path, imageButton);
                continue;
            }
            if (name == /*"Slider_26x26.png"*/ "Slider_52x52.png") { // 2x
                //imageSlider = findImage(path);
                replaceImage(path, imageSlider);
                continue;
            }
            if (name == "Scaling_36_26.png") {
                //imageScaling = findImage(path);
                replaceImage(path, imageScaling);
                continue;
            }
            if (name == /*"Light_14x14.png"*/ "Light_28x28.png") { // 2x
                //imageLight = findImage(path);
                replaceImage(path, imageLight);
                continue;
            }
            if (name == "LFO_36_26.png") {
                //imageLFO = findImage(path);
                replaceImage(path, imageLFO);
                continue;
            }
            if (name == /*"OperatorEditor_287x218.png"*/ "OperatorEditor_574x436_png") { // 2x
                //imageOperator = findImage(path);
                replaceImage(path, imageOperator);
                continue;
            }
            if (name == /*"GlobalEditor_864x144.png"*/ "GlobalEditor_1728x288_png") { // 2x
                //imageGlobal = findImage(path);
                replaceImage(path, imageGlobal);
                continue;
            }
            TRACE("ERROR: unknown image id=\"%s\"; skipped", name.toRawUTF8());
        }        
    }
}

Typeface::Ptr DXLookNFeel::getTypefaceForFont(const Font &font) {
    if (font.getTypefaceName() == Font::getDefaultSansSerifFontName()) {
        static Typeface::Ptr noto = Typeface::createSystemTypefaceFor(
            BinaryData::NotoSansRegular_ttf, BinaryData::NotoSansRegular_ttfSize);
        return noto;
    }

    return LookAndFeel_V4::getTypefaceForFont(font);
}

void DXLookNFeel::drawRotarySlider( Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
     float rotaryStartAngle, float rotaryEndAngle,  Slider &slider ) {
    // Vector controls stay sharp at any host zoom level. The generous dark face,
    // restrained graduated ring and bright value arc are deliberately readable
    // even on the compact 34px controls used by the six DX7 operators.
    const float diameter = static_cast<float>(jmin(width, height));
    if (diameter < 8.0f)
        return;

    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float radius = diameter * 0.5f - 1.7f;
    const float trackRadius = radius * 0.83f;
    const float ringWidth = jlimit(1.6f, 3.0f, diameter * 0.077f);
    const float angle = rotaryStartAngle + jlimit(0.0f, 1.0f, sliderPosProportional)
                                               * (rotaryEndAngle - rotaryStartAngle);
    const bool highlighted = slider.isMouseOverOrDragging() || slider.hasKeyboardFocus(true);
    const auto accent = slider.findColour(Slider::rotarySliderFillColourId, true);
    const auto muted = slider.findColour(Slider::rotarySliderOutlineColourId, true);

    g.setColour(Colours::black.withAlpha(0.30f));
    g.fillEllipse(centreX - radius + 0.5f, centreY - radius + 1.7f,
                  radius * 2.0f, radius * 2.0f);
    g.setGradientFill(ColourGradient(Colour(0xFF344653), centreX - radius, centreY - radius,
                                     Colour(0xFF111C26), centreX + radius, centreY + radius, false));
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);
    g.setColour(Colour(0xFF52626A).withAlpha(highlighted ? 0.85f : 0.45f));
    g.drawEllipse(centreX - radius + 0.7f, centreY - radius + 0.7f,
                  radius * 2.0f - 1.4f, radius * 2.0f - 1.4f, 0.9f);

    Path track, active;
    track.addCentredArc(centreX, centreY, trackRadius, trackRadius, 0.0f,
                        rotaryStartAngle, rotaryEndAngle, true);
    active.addCentredArc(centreX, centreY, trackRadius, trackRadius, 0.0f,
                         rotaryStartAngle, angle, true);
    const PathStrokeType stroke(ringWidth, PathStrokeType::curved, PathStrokeType::rounded);
    g.setColour(muted.withAlpha(0.75f));
    g.strokePath(track, stroke);
    g.setColour(accent.withAlpha(slider.isEnabled() ? 1.0f : 0.30f));
    g.strokePath(active, stroke);

    const float faceRadius = radius * 0.61f;
    g.setGradientFill(ColourGradient(Colour(0xFF344954), centreX - faceRadius, centreY - faceRadius,
                                     Colour(0xFF111A23), centreX + faceRadius, centreY + faceRadius, false));
    g.fillEllipse(centreX - faceRadius, centreY - faceRadius,
                  faceRadius * 2.0f, faceRadius * 2.0f);
    g.setColour(Colour(0xFF52616A));
    g.drawEllipse(centreX - faceRadius, centreY - faceRadius,
                  faceRadius * 2.0f, faceRadius * 2.0f, 0.8f);

    const auto pointerX = centreX + std::sin(angle) * faceRadius * 0.74f;
    const auto pointerY = centreY - std::cos(angle) * faceRadius * 0.74f;
    g.setColour(highlighted ? accent.brighter(0.35f) : Colour(0xFFE8E6DA));
    g.drawLine(centreX, centreY, pointerX, pointerY, jlimit(1.6f, 2.5f, diameter * 0.067f));

    if (slider.hasKeyboardFocus(true)) {
        g.setColour(accent.withAlpha(0.80f));
        g.drawEllipse(centreX - radius - 0.7f, centreY - radius - 0.7f,
                      (radius + 0.7f) * 2.0f, (radius + 0.7f) * 2.0f, 1.3f);
    }
};

void DXLookNFeel::drawToggleButton(Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown) {
    const auto bounds = button.getLocalBounds().toFloat().reduced(1.5f);
    if (bounds.getWidth() < 12.0f || bounds.getHeight() < 10.0f)
        return;

    const bool on = button.getToggleState();
    const bool focused = button.hasKeyboardFocus(true);
    const bool isLighted = dynamic_cast<LightedToggleButton*>(&button) != nullptr;
    const auto accent = isLighted ? Colour(0xFFF2B563) : fillColour;
    const auto corner = jmin(7.0f, bounds.getHeight() * 0.36f);
    g.setColour(on ? Colour(0xFF245950) : Colour(0xFF0B151C));
    g.fillRoundedRectangle(bounds, corner);
    g.setColour(on ? accent.withAlpha(0.90f)
                   : Colour(0xFF5B707A).withAlpha(isMouseOverButton ? 0.85f : 0.55f));
    g.drawRoundedRectangle(bounds, corner, focused ? 1.9f : 1.1f);

    // A clear physical travel direction is easier to parse at a glance than
    // a colour-only state indication, and is accessible to colour-blind users.
    const float thumbSize = jmin(bounds.getHeight() - 6.0f, bounds.getWidth() * 0.32f);
    const float thumbX = on ? bounds.getRight() - thumbSize - 4.0f : bounds.getX() + 4.0f;
    const float thumbY = bounds.getCentreY() - thumbSize * 0.5f;
    g.setGradientFill(ColourGradient(on ? Colour(0xFFFCF8E8) : Colour(0xFF98A8AC),
                                     thumbX, thumbY, on ? Colour(0xFFD4E6DE) : Colour(0xFF5D707A),
                                     thumbX, thumbY + thumbSize, false));
    g.fillRoundedRectangle(thumbX, thumbY, thumbSize, thumbSize, jmin(4.0f, thumbSize * 0.35f));
    g.setColour(accent.withAlpha(on ? 1.0f : 0.18f));
    const float ledX = on ? bounds.getX() + 7.5f : bounds.getRight() - 7.5f;
    g.fillEllipse(ledX - 1.5f, bounds.getCentreY() - 1.5f, 3.0f, 3.0f);

    if (isButtonDown) {
        g.setColour(Colours::black.withAlpha(0.18f));
        g.fillRoundedRectangle(bounds, corner);
    }
}

void DXLookNFeel::drawButtonBackground(Graphics &g, Button &button, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) {
    const auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    if (bounds.isEmpty())
        return;

    const bool active = button.getToggleState();
    const auto surface = active ? Colour(0xFF28635D)
                                : isButtonDown ? Colour(0xFF111E26)
                                               : isMouseOverButton ? Colour(0xFF334854)
                                                                   : backgroundColour;
    g.setColour(Colours::black.withAlpha(0.18f));
    g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), 5.0f);
    g.setGradientFill(ColourGradient(surface.brighter(0.13f), bounds.getX(), bounds.getY(),
                                     surface.darker(0.20f), bounds.getX(), bounds.getBottom(), false));
    g.fillRoundedRectangle(bounds, 5.0f);
    g.setColour(active ? fillColour.withAlpha(0.8f)
                       : Colour(0xFF5A6C74).withAlpha(isMouseOverButton ? 0.9f : 0.50f));
    g.drawRoundedRectangle(bounds, 5.0f, button.hasKeyboardFocus(true) ? 2.0f : 1.0f);
}

void DXLookNFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                 float sliderPos, float minSliderPos, float maxSliderPos,
                                 const Slider::SliderStyle style, Slider& slider) {
    const bool horizontal = style == Slider::LinearHorizontal || style == Slider::LinearBar;
    const bool vertical = style == Slider::LinearVertical || style == Slider::LinearBarVertical;
    if (!horizontal && !vertical) {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos,
                                         minSliderPos, maxSliderPos, style, slider);
        return;
    }

    const auto accent = slider.findColour(Slider::rotarySliderFillColourId, true);
    const auto rail = Colour(0xFF0C151C);
    const auto edge = Colour(0xFF546775);
    if (horizontal) {
        const float centreY = y + height * 0.5f;
        const float start = x + 5.0f, end = x + width - 5.0f;
        const float thumbX = jlimit(start, end, sliderPos);
        g.setColour(rail);
        g.fillRoundedRectangle(start, centreY - 3.0f, end - start, 6.0f, 3.0f);
        g.setColour(accent.withAlpha(0.72f));
        g.fillRoundedRectangle(start, centreY - 2.0f, jmax(0.0f, thumbX - start), 4.0f, 2.0f);
        g.setColour(Colour(0xFFE8E6DA));
        g.fillRoundedRectangle(thumbX - 4.5f, centreY - 8.0f, 9.0f, 16.0f, 3.0f);
        g.setColour(edge);
        g.drawRoundedRectangle(thumbX - 4.5f, centreY - 8.0f, 9.0f, 16.0f, 3.0f, 1.0f);
    } else {
        const float centreX = x + width * 0.5f;
        const float top = y + 5.0f, bottom = y + height - 5.0f;
        const float thumbY = jlimit(top, bottom, sliderPos);
        g.setColour(rail);
        g.fillRoundedRectangle(centreX - 3.0f, top, 6.0f, bottom - top, 3.0f);
        g.setColour(accent.withAlpha(0.72f));
        g.fillRoundedRectangle(centreX - 2.0f, thumbY, 4.0f, jmax(0.0f, bottom - thumbY), 2.0f);
        g.setColour(Colour(0xFFE8E6DA));
        g.fillRoundedRectangle(centreX - 8.0f, thumbY - 4.5f, 16.0f, 9.0f, 3.0f);
        g.setColour(edge);
        g.drawRoundedRectangle(centreX - 8.0f, thumbY - 4.5f, 16.0f, 9.0f, 3.0f, 1.0f);
    }

    if (slider.hasKeyboardFocus(true)) {
        g.setColour(accent.withAlpha(0.70f));
        g.drawRoundedRectangle(slider.getLocalBounds().toFloat().reduced(0.8f), 4.0f, 1.3f);
    }
}

void DXLookNFeel::drawComboBox(Graphics& g, int width, int height, bool isButtonDown,
                              int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box) {
    const auto bounds = Rectangle<float>(0.8f, 0.8f,
                                         static_cast<float>(width) - 1.6f,
                                         static_cast<float>(height) - 1.6f);
    if (bounds.isEmpty())
        return;

    g.setColour(Colour(0xFF0D151C));
    g.fillRoundedRectangle(bounds, 5.0f);
    g.setColour(box.hasKeyboardFocus(true) ? fillColour : Colour(0xFF405663));
    g.drawRoundedRectangle(bounds, 5.0f, box.hasKeyboardFocus(true) ? 1.8f : 1.0f);

    const float arrowX = buttonX + buttonW * 0.5f;
    const float arrowY = buttonY + buttonH * 0.5f;
    Path arrow;
    arrow.addTriangle(arrowX - 4.0f, arrowY - 1.5f,
                      arrowX + 4.0f, arrowY - 1.5f,
                      arrowX, arrowY + 2.5f);
    g.setColour(isButtonDown ? fillColour : Colour(0xFF9EAFBF));
    g.fillPath(arrow);
}

void DXLookNFeel::positionComboBoxText(ComboBox& box, Label& label) {
    ComboBox *src = &box;

    // I'm not proud of this one, but really... it must be another way to do this....
    ComboBoxImage* img = dynamic_cast<ComboBoxImage*>(src);
    if( img != 0 ) {
        return;
    }

    LookAndFeel_V4::positionComboBoxText(box, label);
}

Colour DXLookNFeel::fillColour = Colour(0xFF55D7C7);
Colour DXLookNFeel::lightBackground = Colour(0xFF17242D);
Colour DXLookNFeel::background = Colour(0xFF101820);
Colour DXLookNFeel::roundBackground = Colour(0xFF20303B);

