DX7 FM theme customization
==========================
Updated: October 2026 — vector UI refresh


The redesigned DX7 FM interface uses resolution-independent, vector-painted knobs, switches,
sliders, buttons, and selection indicators. You can still customize the registered JUCE
colours through the existing `DexedTheme.xml` mechanism. This is a continuation of the
original Dexed skin format, not a separate skinning system.

Create `DexedTheme.xml` beside your existing `Dexed.xml` settings file.
Historically, Dexed stores its settings under `%APPDATA%\DigitalSuburban\Dexed`
(Windows), `~/Library/Application Support/DigitalSuburban/Dexed` (macOS), or
`~/.local/share/DigitalSuburban/Dexed` (Linux). The actual settings location is
determined by the running app and your OS.

Colour keys are JUCE colour IDs registered by [DXLookNFeel](../source/ui/DXLookNFeel.cpp).
Values are hexadecimal `0xAARRGGBB` (alpha, red, green, blue). Alpha `00` means fully
transparent; `FF` means fully opaque.

The default design palette is graphite `#101820`, raised graphite `#17242D`, ivory
`#E8E6DA`, teal `#55D7C7`, and amber status lights `#F2B563`. Teal appears on control
value arcs and active states. Focus outlines remain visible for keyboard users.

Known colour keys; simply override those you want different.

```cpp
TextButton::buttonColourId
TextButton::buttonOnColourId
TextButton::textColourOnId
TextButton::textColourOffId
Slider::rotarySliderOutlineColourId
Slider::rotarySliderFillColourId
Slider::textBoxTextColourId
Slider::textBoxBackgroundColourId
Slider::textBoxHighlightColourId
Slider::textBoxOutlineColourId
AlertWindow::backgroundColourId
AlertWindow::textColourId
TextEditor::backgroundColourId
TextEditor::textColourId
TextEditor::highlightColourId
TextEditor::outlineColourId
TextEditor::focusedOutlineColourId
ComboBox::backgroundColourId
ComboBox::textColourId
ComboBox::buttonColourId
ComboBox::outlineColourId
Label::textColourId
Label::backgroundColourId
Label::outlineColourId
PopupMenu::backgroundColourId
PopupMenu::textColourId
PopupMenu::highlightedTextColourId
PopupMenu::highlightedBackgroundColourId
TreeView::backgroundColourId
DirectoryContentsDisplayComponent::highlightColourId
DirectoryContentsDisplayComponent::textColourId
DialogWindow::backgroundColourId
ListBox::backgroundColourId
ScrollBar::thumbColourId
Dexed::backgroundId
Dexed::fillColourId
```

### Legacy image overrides

The XML parser still accepts the historic image IDs listed below, with paths resolved
relative to the folder containing `DexedTheme.xml`. Missing images leave the bundled
fallback image in place. The unmodified source files are available in [assets/ui](../assets/ui).

**Important:** Knobs, normal toggles, buttons, linear sliders, and the main panels now
render as vector UI. Their former PNG skin overrides may load successfully but do not
replace the new vector painting. The scaling/LFO icon selectors and some indicator
sprites continue to use legacy images. Treat image overrides as a compatibility
feature, not as a complete way to reskin the redesigned interface.

Accepted legacy IDs:

Knob_68x68.png
Switch_96x52.png
SwitchLighted_48x26.png
Switch_64x64.png
ButtonUnlabeled_50x30.png
Slider_52x52.png
Scaling_36_26.png
Light_28x28.png
LFO_36_26.png
OperatorEditor_574x436.png
GlobalEditor_1728x288.png


Example ``DexedTheme.xml`` configuration
----------------------------------------

```xml
<dexedTheme>
	<colour id="PopupMenu::backgroundColourId"            value="0xff0000ff"/>
	<colour id="PopupMenu::textColourId"                  value="0xffffffff"/>
	<colour id="PopupMenu::highlightedTextColourId"       value="0xffffff00"/>
	<colour id="PopupMenu::highlightedBackgroundColourId" value="0xff00ffff"/>
	<colour id="ScrollBar::thumbColourId"                 value="0xFFC0C0C0"/>
	<image id="Light_28x28.png" path="myTheme/GreenLight_28x28.png"/>
</dexedTheme>
```

In this example the menu, highlighted items and scrollbar thumb change colours. The
legacy LED override can still affect indicator sprites used in portions of the app,
but it does not change the vector-drawn button indicator dots.

`DexedTheme.xml` is read when the standalone app or plugin initializes; restart the
editor/host to reload changes. Although colour overrides are supported, the refreshed
control geometry, typography, labels and panel layout are currently code-defined.
