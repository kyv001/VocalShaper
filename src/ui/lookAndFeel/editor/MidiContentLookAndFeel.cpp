#include "MidiContentLookAndFeel.h"
#include "../../misc/ColorMap.h"

MidiContentLookAndFeel::MidiContentLookAndFeel()
	: MainLookAndFeel() {
	/** Background */
	this->setColour(juce::ResizableWindow::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));

	/** Ruler Line */
	this->setColour(juce::TableListBox::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));
	this->setColour(juce::TableListBox::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB5"));/** Line */
	this->setColour(juce::TableListBox::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorB10"));

	/** Key Line */
	this->setColour(juce::MidiKeyboardComponent::ColourIds::whiteNoteColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));/**< White Key Line */
	this->setColour(juce::MidiKeyboardComponent::ColourIds::blackNoteColourId,
		ColorMap::getInstance()->get("ThemeColorB2"));/**< Black Key Line */
	this->setColour(juce::MidiKeyboardComponent::ColourIds::keySeparatorLineColourId,
		ColorMap::getInstance()->get("ThemeColorB1"));/**< Line Split */
	this->setColour(juce::MidiKeyboardComponent::ColourIds::mouseOverKeyOverlayColourId,
		ColorMap::getInstance()->get("ThemeColorB5").withAlpha(0.5f));
	this->setColour(juce::MidiKeyboardComponent::ColourIds::keyDownOverlayColourId,
		ColorMap::getInstance()->get("ThemeColorB5").withAlpha(0.5f));
	this->setColour(juce::MidiKeyboardComponent::ColourIds::textLabelColourId,
		ColorMap::getInstance()->get("ThemeColorB0"));/**< Light Note Label */
	this->setColour(juce::MidiKeyboardComponent::ColourIds::textLabelColourId + 2,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Dark Note Label */
	this->setColour(juce::MidiKeyboardComponent::ColourIds::textLabelColourId + 3,
		ColorMap::getInstance()->get("ThemeColorB10"));/**< Note Lyrics */

	/** Lines */
	this->setColour(juce::Label::ColourIds::backgroundColourId,
		ColorMap::getInstance()->get("ThemeColorB0"));/**< Notes Opaque Base */
	this->setColour(juce::Label::ColourIds::textColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));/**< Cursor */
	this->setColour(juce::Label::ColourIds::outlineColourId,
		ColorMap::getInstance()->get("ThemeColorB3"));/**< Note Outline */
	this->setColour(juce::Label::ColourIds::backgroundWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB9"));/**< Notes In High Channel */
	this->setColour(juce::Label::ColourIds::textWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorB1").withAlpha(0.8f));/**< Time Out of Blocks */
	this->setColour(juce::Label::ColourIds::outlineWhenEditingColourId,
		ColorMap::getInstance()->get("ThemeColorA2"));
}
