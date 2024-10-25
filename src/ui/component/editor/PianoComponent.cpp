#include "PianoComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"

PianoComponent::PianoComponent() {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forPiano());
}

void PianoComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float blackCornerSize = screenSize.getHeight() * 0.0025;
	float whiteCornerSize = screenSize.getHeight() * 0.0025;
	float lineThickness = screenSize.getHeight() * 0.0005;

	float blackLength = 0.7;

	float keyLabelFontHeight = screenSize.getHeight() * 0.015;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour blackKeyColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::blackNoteColourId);
	juce::Colour whiteKeyColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::whiteNoteColourId);
	juce::Colour lineColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::keySeparatorLineColourId);
	juce::Colour labelColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::textLabelColourId);

	/** Font */
	juce::Font labelFont(juce::FontOptions{ keyLabelFontHeight });

	/** Keys */
	const int keysPerOctave = this->blackKeys.size() + this->whiteKeys.size();

	float sizePerOctave = this->itemSize * keysPerOctave;
	float sizePerWhiteKey = sizePerOctave / this->whiteKeys.size();

	int topKey = this->totalKeys - std::floor(this->pos / this->itemSize);
	int bottomKey = this->totalKeys - std::ceil((this->pos + this->getHeight()) / this->itemSize);
	int startOctave = std::floor(bottomKey / (double)keysPerOctave);
	int endOctave = std::ceil(topKey / (double)keysPerOctave);
	int octaveNum = endOctave - startOctave;

	float startOctavePos = (-this->pos) + this->totalKeys * this->itemSize - startOctave * sizePerOctave;

	for (int i = 0; i < octaveNum; i++) {
		int octave = startOctave + i;
		juce::String octaveName = juce::String{ octave - 1 };
		float octavePos = startOctavePos - i * sizePerOctave;

		/** White Keys */
		for (int j = 0; j < this->whiteKeys.size(); j++) {
			int keyInOctave = this->whiteKeys[j];
			int keyNumber = octave * keysPerOctave + keyInOctave;
			if (keyNumber < this->totalKeys) {
				/** Key Rect */
				juce::Rectangle<float> keyRect(
					0, octavePos - (j + 1) * sizePerWhiteKey,
					this->getWidth(), sizePerWhiteKey);
				g.setColour(whiteKeyColor);
				g.fillRoundedRectangle(keyRect, whiteCornerSize);
				g.setColour(lineColor);
				g.drawRoundedRectangle(keyRect, whiteCornerSize, lineThickness);

				/** Key Name */
				/*if (keyInOctave == 0 || keyNumber == this->keyHovered) {
					juce::String keyName = this->keyNames[keyInOctave] + octaveName;
					juce::Rectangle<int> keyLabelRect = keyRect.withTrimmedLeft(blackLength * this->getWidth()).toNearestInt();
					g.setFont(labelFont);
					g.setColour(labelColor);
					g.drawFittedText(keyName, keyLabelRect,
						juce::Justification::centred, 1, 0.75f);
				}*/
			}
		}

		/** Black Keys */
		for (int j = 0; j < this->blackKeys.size(); j++) {
			int keyInOctave = this->blackKeys[j];
			int keyNumber = octave * keysPerOctave + keyInOctave;
			if (keyNumber < this->totalKeys) {
				/** Key Rect */
				juce::Rectangle<float> keyRect(
					0, octavePos - (keyInOctave + 1) * this->itemSize,
					this->getWidth() * blackLength, this->itemSize);
				g.setColour(blackKeyColor);
				g.fillRoundedRectangle(keyRect, blackCornerSize);
				g.setColour(lineColor);
				g.drawRoundedRectangle(keyRect, blackCornerSize, lineThickness);

				/** Key Name */
				/*if (keyInOctave == 0 || keyNumber == this->keyHovered) {
					juce::String keyName = this->keyNames[keyInOctave] + octaveName;
					juce::Rectangle<int> keyLabelRect = keyRect.withTrimmedLeft((blackLength - (1 - blackLength)) * this->getWidth()).toNearestInt();
					g.setFont(labelFont);
					g.setColour(labelColor);
					g.drawFittedText(keyName, keyLabelRect,
						juce::Justification::centred, 1, 0.75f);
				}*/
			}
		}

		/** Octave C Label */
		{
			juce::String keyName = this->keyNames[0] + octaveName;
			juce::Rectangle<int> keyLabelRect(
				blackLength * this->getWidth(), octavePos - this->itemSize,
				(1 - blackLength) * this->getWidth(), this->itemSize);
			g.setFont(labelFont);
			g.setColour(labelColor);
			g.drawFittedText(keyName, keyLabelRect,
				juce::Justification::centred, 1, 0.75f);
		}
	}
}

void PianoComponent::setPos(double pos, double itemSize) {
	this->pos = pos;
	this->itemSize = itemSize;
	this->repaint();
}
