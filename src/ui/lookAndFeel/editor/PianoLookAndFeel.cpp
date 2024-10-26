#include "PianoLookAndFeel.h"
#include "../../misc/ColorMap.h"

PianoLookAndFeel::PianoLookAndFeel()
	: MainLookAndFeel() {
	/** Keyboard */
	this->setColour(juce::MidiKeyboardComponent::ColourIds::whiteNoteColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));
	this->setColour(juce::MidiKeyboardComponent::ColourIds::blackNoteColourId,
		ColorMap::getInstance()->get("ThemeColorB0"));
	this->setColour(juce::MidiKeyboardComponent::ColourIds::keySeparatorLineColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));
	this->setColour(juce::MidiKeyboardComponent::ColourIds::mouseOverKeyOverlayColourId,
		ColorMap::getInstance()->get("ThemeColorB5").withAlpha(0.5f));
	this->setColour(juce::MidiKeyboardComponent::ColourIds::keyDownOverlayColourId,
		ColorMap::getInstance()->get("ThemeColorB5").withAlpha(0.5f));
	this->setColour(juce::MidiKeyboardComponent::ColourIds::textLabelColourId,
		ColorMap::getInstance()->get("ThemeColorB4"));
	this->setColour(juce::MidiKeyboardComponent::ColourIds::textLabelColourId + 2,
		ColorMap::getInstance()->get("ThemeColorB8"));/**< Black Key Label */
}
