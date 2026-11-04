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
    ctrlBackground = Colour(0xFF17181A);

    // WARNING! If you modify the colour IDs here, please actualize the file ''DexedTheme.md'' 
    // in the subdirectory ``~/dexed/Documentation/``    

    REG_COLOUR(TextButton::buttonColourId, Colour(0xFF34353A));
    REG_COLOUR(TextButton::buttonOnColourId, Colour(0xFF2E7D63));
    REG_COLOUR(TextButton::textColourOnId, Colour(0xFFF4F2EC));
    REG_COLOUR(TextButton::textColourOffId, Colour(0xFFE4E3DF));
    REG_COLOUR(Slider::rotarySliderOutlineColourId, Colour(0xFF121315));
    REG_COLOUR(Slider::rotarySliderFillColourId, fillColour);
    REG_COLOUR(Slider::textBoxTextColourId, Colour(0xFFE4E3DF));
    REG_COLOUR(Slider::textBoxBackgroundColourId, ctrlBackground);
    REG_COLOUR(Slider::textBoxHighlightColourId, fillColour.withAlpha(0.35f));
    REG_COLOUR(Slider::textBoxOutlineColourId, Colour(0xFF4A4B50));
    REG_COLOUR(AlertWindow::backgroundColourId,lightBackground);
    REG_COLOUR(AlertWindow::textColourId, Colour(0xFFE4E3DF));
    REG_COLOUR(TextEditor::backgroundColourId,ctrlBackground);
    REG_COLOUR(TextEditor::textColourId, Colour(0xFFE4E3DF));
    REG_COLOUR(TextEditor::highlightColourId, fillColour);
    REG_COLOUR(TextEditor::outlineColourId, Colour(0xFF4A4B50));
    REG_COLOUR(TextEditor::focusedOutlineColourId, fillColour);
    REG_COLOUR(ComboBox::backgroundColourId, ctrlBackground);
    REG_COLOUR(ComboBox::textColourId, Colour(0xFFE4E3DF));
    REG_COLOUR(ComboBox::buttonColourId, fillColour);
    REG_COLOUR(ComboBox::outlineColourId, Colour(0xFF4A4B50));
    REG_COLOUR(Label::textColourId, Colour(0xFFE4E3DF));
    REG_COLOUR(Label::backgroundColourId, Colours::transparentBlack);
    REG_COLOUR(Label::outlineColourId, Colours::transparentBlack);
    REG_COLOUR(PopupMenu::backgroundColourId, background);
    REG_COLOUR(PopupMenu::textColourId, Colour(0xFFE4E3DF));
    REG_COLOUR(PopupMenu::highlightedTextColourId, Colour(0xFF0F1F19));
    REG_COLOUR(PopupMenu::highlightedBackgroundColourId, fillColour);
    REG_COLOUR(TreeView::backgroundColourId, background);
    REG_COLOUR(DirectoryContentsDisplayComponent::highlightColourId, fillColour);
    REG_COLOUR(DirectoryContentsDisplayComponent::textColourId, Colour(0xFFE4E3DF));
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
        // Embed both weights so bold captions render as real bold glyphs on
        // every OS rather than a synthesised or substituted face.
        static Typeface::Ptr notoRegular = Typeface::createSystemTypefaceFor(
            BinaryData::NotoSansRegular_ttf, BinaryData::NotoSansRegular_ttfSize);
        static Typeface::Ptr notoBold = Typeface::createSystemTypefaceFor(
            BinaryData::NotoSansBold_ttf, BinaryData::NotoSansBold_ttfSize);
        return font.isBold() ? notoBold : notoRegular;
    }

    return LookAndFeel_V4::getTypefaceForFont(font);
}

void DXLookNFeel::drawRotarySlider( Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
     float rotaryStartAngle, float rotaryEndAngle,  Slider &slider ) {
    // Neutral graphite knob with a coloured value ring: the ring carries the
    // value, the white pointer carries the angle. Drawn entirely as vectors.
    const float diameter = static_cast<float>(jmin(width, height));
    if (diameter < 8.0f)
        return;

    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float radius = diameter * 0.5f - 1.5f;
    const float ringRadius = radius - jlimit(1.5f, 3.0f, diameter * 0.06f);
    const float ringWidth = jlimit(2.0f, 4.0f, diameter * 0.085f);
    const float proportion = jlimit(0.0f, 1.0f, sliderPosProportional);
    const float angle = rotaryStartAngle + proportion * (rotaryEndAngle - rotaryStartAngle);
    const bool highlighted = slider.isMouseOverOrDragging() || slider.hasKeyboardFocus(true);
    const bool bipolar = (bool) slider.getProperties().getWithDefault("bipolar", false);
    const auto accent = slider.findColour(Slider::rotarySliderFillColourId, true);

    // Track and value arc.
    Path track, active;
    track.addCentredArc(centreX, centreY, ringRadius, ringRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    const float originAngle = bipolar ? (rotaryStartAngle + rotaryEndAngle) * 0.5f : rotaryStartAngle;
    active.addCentredArc(centreX, centreY, ringRadius, ringRadius, 0.0f,
                         jmin(originAngle, angle), jmax(originAngle, angle), true);
    const PathStrokeType stroke(ringWidth, PathStrokeType::curved, PathStrokeType::butt);
    g.setColour(Colour(0xFF121315));
    g.strokePath(track, stroke);
    g.setColour(accent.withAlpha(slider.isEnabled() ? (highlighted ? 1.0f : 0.92f) : 0.3f));
    g.strokePath(active, stroke);

    // Knob body.
    const float bodyRadius = ringRadius - ringWidth * 0.5f - jlimit(1.5f, 3.5f, diameter * 0.07f);
    g.setColour(Colours::black.withAlpha(0.45f));
    g.fillEllipse(centreX - bodyRadius, centreY - bodyRadius + 1.6f, bodyRadius * 2.0f, bodyRadius * 2.0f);
    g.setGradientFill(ColourGradient(Colour(0xFF55565B), centreX - bodyRadius, centreY - bodyRadius,
                                     Colour(0xFF1D1E21), centreX + bodyRadius * 0.6f, centreY + bodyRadius, false));
    g.fillEllipse(centreX - bodyRadius, centreY - bodyRadius, bodyRadius * 2.0f, bodyRadius * 2.0f);
    const float capRadius = bodyRadius * 0.80f;
    g.setGradientFill(ColourGradient(Colour(0xFF3F4045), centreX, centreY - capRadius,
                                     Colour(0xFF26272A), centreX, centreY + capRadius, false));
    g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    g.setColour(Colours::white.withAlpha(highlighted ? 0.22f : 0.10f));
    g.drawEllipse(centreX - bodyRadius + 0.5f, centreY - bodyRadius + 0.5f,
                  bodyRadius * 2.0f - 1.0f, bodyRadius * 2.0f - 1.0f, 0.8f);

    const float inner = bodyRadius * 0.30f, outer = bodyRadius * 0.92f;
    g.setColour(highlighted ? Colours::white : Colour(0xFFE9E7E1));
    g.drawLine(centreX + std::sin(angle) * inner, centreY - std::cos(angle) * inner,
               centreX + std::sin(angle) * outer, centreY - std::cos(angle) * outer,
               jlimit(1.5f, 2.6f, diameter * 0.065f));

    if (slider.hasKeyboardFocus(true)) {
        g.setColour(accent.withAlpha(0.75f));
        g.drawEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 1.2f);
    }
}

void DXLookNFeel::drawToggleButton(Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown) {
    const auto bounds = button.getLocalBounds().toFloat().reduced(1.5f);
    if (bounds.getWidth() < 12.0f || bounds.getHeight() < 10.0f)
        return;

    const bool on = button.getToggleState();
    const auto corner = jmin(6.0f, bounds.getHeight() * 0.36f);
    g.setColour(on ? Colour(0xFF1F4B3D) : Colour(0xFF17181A));
    g.fillRoundedRectangle(bounds, corner);
    g.setColour(on ? fillColour : Colour(0xFF4A4B50).withAlpha(isMouseOverButton ? 1.0f : 0.7f));
    g.drawRoundedRectangle(bounds, corner, button.hasKeyboardFocus(true) ? 1.9f : 1.0f);

    const float thumbSize = jmin(bounds.getHeight() - 6.0f, bounds.getWidth() * 0.34f);
    const float thumbX = on ? bounds.getRight() - thumbSize - 3.0f : bounds.getX() + 3.0f;
    const float thumbY = bounds.getCentreY() - thumbSize * 0.5f;
    g.setGradientFill(ColourGradient(on ? Colour(0xFFF4F2EC) : Colour(0xFF8C8D91), thumbX, thumbY,
                                     on ? Colour(0xFFC9D8D0) : Colour(0xFF55565B), thumbX, thumbY + thumbSize, false));
    g.fillRoundedRectangle(thumbX, thumbY, thumbSize, thumbSize, jmin(4.0f, thumbSize * 0.35f));

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
    const auto surface = active ? Colour(0xFF2E7D63)
                                : isButtonDown ? backgroundColour.darker(0.3f)
                                               : isMouseOverButton ? backgroundColour.brighter(0.15f)
                                                                   : backgroundColour;
    g.setColour(Colours::black.withAlpha(0.25f));
    g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), 4.0f);
    g.setGradientFill(ColourGradient(surface.brighter(0.10f), bounds.getX(), bounds.getY(),
                                     surface.darker(0.18f), bounds.getX(), bounds.getBottom(), false));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(active ? fillColour.withAlpha(0.85f)
                       : Colour(0xFF56575C).withAlpha(isMouseOverButton ? 0.95f : 0.6f));
    g.drawRoundedRectangle(bounds, 4.0f, button.hasKeyboardFocus(true) ? 2.0f : 1.0f);
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
    const bool fader = (bool) slider.getProperties().getWithDefault("fader", false);

    if (vertical && fader) {
        // Hardware-style fader: dark slot, printed scale, ribbed cap.
        const float centreX = x + width * 0.5f;
        const float top = (float) y + 4.0f, bottom = (float) (y + height) - 4.0f;
        const float thumbY = jlimit(top, bottom, sliderPos);
        const auto print = Colour(0xFFB6A68B);
        for (int i = 0; i <= 10; ++i) {
            const float ty = top + (bottom - top) * i / 10.0f;
            const float len = (i % 5 == 0) ? 7.0f : 4.0f;
            g.setColour(print.withAlpha(0.75f));
            g.fillRect(centreX - 11.0f - len, ty - 0.5f, len, 1.0f);
            g.fillRect(centreX + 11.0f, ty - 0.5f, len, 1.0f);
        }
        g.setColour(Colour(0xFF0B0908));
        g.fillRoundedRectangle(centreX - 3.0f, top - 2.0f, 6.0f, bottom - top + 4.0f, 3.0f);
        g.setColour(Colour(0xFF4A3E36));
        g.drawRoundedRectangle(centreX - 3.0f, top - 2.0f, 6.0f, bottom - top + 4.0f, 3.0f, 0.8f);

        const auto cap = Rectangle<float>(centreX - 10.0f, thumbY - 13.0f, 20.0f, 26.0f);
        g.setColour(Colours::black.withAlpha(0.5f));
        g.fillRoundedRectangle(cap.translated(0.0f, 2.0f), 2.5f);
        g.setGradientFill(ColourGradient(Colour(0xFF4B4440), cap.getX(), cap.getY(),
                                         Colour(0xFF1A1715), cap.getX(), cap.getBottom(), false));
        g.fillRoundedRectangle(cap, 2.5f);
        for (int i = -2; i <= 2; ++i) {
            if (i == 0) continue;
            g.setColour(Colours::black.withAlpha(0.35f));
            g.fillRect(cap.getX() + 3.0f, thumbY + i * 4.0f, cap.getWidth() - 6.0f, 1.0f);
            g.setColour(Colours::white.withAlpha(0.06f));
            g.fillRect(cap.getX() + 3.0f, thumbY + i * 4.0f + 1.0f, cap.getWidth() - 6.0f, 1.0f);
        }
        g.setColour(Colour(0xFFF2EBDC));
        g.fillRect(cap.getX() + 2.0f, thumbY - 1.0f, cap.getWidth() - 4.0f, 2.0f);
        if (slider.hasKeyboardFocus(true)) {
            g.setColour(Colour(0xFF5AD4A6));
            g.drawRoundedRectangle(cap.expanded(1.5f), 3.0f, 1.3f);
        }
        return;
    }

    const auto rail = Colour(0xFF121315);
    if (horizontal) {
        const float centreY = y + height * 0.5f;
        const float start = x + 6.0f, end = x + width - 6.0f;
        const float thumbX = jlimit(start, end, sliderPos);
        g.setColour(rail);
        g.fillRoundedRectangle(start, centreY - 3.0f, end - start, 6.0f, 3.0f);
        g.setColour(accent.withAlpha(0.85f));
        g.fillRoundedRectangle(start, centreY - 2.0f, jmax(0.0f, thumbX - start), 4.0f, 2.0f);
        g.setColour(Colour(0xFFE9E7E1));
        g.fillRoundedRectangle(thumbX - 5.0f, centreY - 8.0f, 10.0f, 16.0f, 3.0f);
        g.setColour(Colour(0xFF1D1E21));
        g.fillRect(thumbX - 0.75f, centreY - 5.0f, 1.5f, 10.0f);
    } else {
        const float centreX = x + width * 0.5f;
        const float top = y + 6.0f, bottom = y + height - 6.0f;
        const float thumbY = jlimit(top, bottom, sliderPos);
        g.setColour(rail);
        g.fillRoundedRectangle(centreX - 3.0f, top, 6.0f, bottom - top, 3.0f);
        g.setColour(accent.withAlpha(0.85f));
        g.fillRoundedRectangle(centreX - 2.0f, thumbY, 4.0f, jmax(0.0f, bottom - thumbY), 2.0f);
        g.setColour(Colour(0xFFE9E7E1));
        g.fillRoundedRectangle(centreX - 8.0f, thumbY - 5.0f, 16.0f, 10.0f, 3.0f);
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

    g.setColour(Colour(0xFF17181A));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(box.hasKeyboardFocus(true) ? fillColour : Colour(0xFF4A4B50));
    g.drawRoundedRectangle(bounds, 4.0f, box.hasKeyboardFocus(true) ? 1.8f : 1.0f);

    const float arrowX = buttonX + buttonW * 0.5f;
    const float arrowY = buttonY + buttonH * 0.5f;
    Path arrow;
    arrow.addTriangle(arrowX - 4.0f, arrowY - 1.5f,
                      arrowX + 4.0f, arrowY - 1.5f,
                      arrowX, arrowY + 2.5f);
    g.setColour(isButtonDown ? fillColour : Colour(0xFFB0AEA8));
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

Colour DXLookNFeel::fillColour = Colour(0xFF5AD4A6);
Colour DXLookNFeel::lightBackground = Colour(0xFF2A2B2F);
Colour DXLookNFeel::background = Colour(0xFF1F2023);
Colour DXLookNFeel::roundBackground = Colour(0xFF313236);

