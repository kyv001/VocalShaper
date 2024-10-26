#include "PianoComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"

PianoComponent::PianoComponent(
	const WheelFunc& wheelFunc,
	const WheelAltFunc& wheelAltFunc)
	: wheelFunc(wheelFunc), wheelAltFunc(wheelAltFunc),
	keysPerOctave(this->blackKeys.size() + this->whiteKeys.size()) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forPiano());
}

void PianoComponent::resized() {
	this->updateKeysInternal();
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
	juce::Colour whiteLabelColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::textLabelColourId);
	juce::Colour blackLabelColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::textLabelColourId + 2);
	juce::Colour hoveredColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::mouseOverKeyOverlayColourId);

	/** Font */
	juce::Font labelFont(juce::FontOptions{ keyLabelFontHeight });

	/** Keys */
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
				if (keyNumber == this->keyHovered) {
					g.setColour(hoveredColor);
					g.fillRoundedRectangle(keyRect, whiteCornerSize);
				}
				g.setColour(lineColor);
				g.drawRoundedRectangle(keyRect, whiteCornerSize, lineThickness);

				/** Key Name */
				if (this->showAllKeyNames || keyInOctave == 0 || keyNumber == this->keyHovered) {
					juce::String keyName = this->keyNames[keyInOctave] + octaveName;
					juce::Rectangle<int> keyLabelRect(
						blackLength * this->getWidth(), octavePos - (keyInOctave + 1) * this->itemSize,
						(1 - blackLength) * this->getWidth(), this->itemSize);
					g.setFont(labelFont);
					g.setColour(whiteLabelColor);
					g.drawFittedText(keyName, keyLabelRect,
						juce::Justification::centred, 1, 0.75f);
				}
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
				if (keyNumber == this->keyHovered) {
					g.setColour(hoveredColor);
					g.fillRoundedRectangle(keyRect, blackCornerSize);
				}
				g.setColour(lineColor);
				g.drawRoundedRectangle(keyRect, blackCornerSize, lineThickness);

				/** Key Name */
				if (this->showAllKeyNames || keyInOctave == 0 || keyNumber == this->keyHovered) {
					juce::String keyName = this->keyNames[keyInOctave] + octaveName;
					juce::Rectangle<int> keyLabelRect(
						(blackLength - (1 - blackLength)) * this->getWidth(), octavePos - (keyInOctave + 1) * this->itemSize,
						(1- blackLength) * this->getWidth(), this->itemSize);
					g.setFont(labelFont);
					g.setColour(blackLabelColor);
					g.drawFittedText(keyName, keyLabelRect,
						juce::Justification::centred, 1, 0.75f);
				}
			}
		}
	}
}

void PianoComponent::setPos(double pos, double itemSize) {
	this->pos = pos;
	this->itemSize = itemSize;

	this->sizePerOctave = this->itemSize * this->keysPerOctave;
	this->sizePerWhiteKey = this->sizePerOctave / this->whiteKeys.size();

	this->updateKeysInternal();
}

void PianoComponent::mouseMove(const juce::MouseEvent& event) {
	float posX = event.position.getX(), posY = event.position.getY();

	/** Check Mouse Hovered Key */
	float blackLength = 0.7;
	float blackWidth = blackLength * this->getWidth();
	if (posX <= blackWidth) {
		int keyDistance = std::floor((this->startOctavePos - posY) / this->itemSize);
		int keyBase = this->startOctave * this->keysPerOctave;
		this->keyHovered = keyBase + keyDistance;
	}
	else {
		int whiteKeyDistance = std::floor((this->startOctavePos - posY) / this->sizePerWhiteKey);
		int keyBase = this->startOctave * this->keysPerOctave;
		int octaveDistance = whiteKeyDistance / this->whiteKeys.size();
		int keyInOctave = this->whiteKeys[whiteKeyDistance % this->whiteKeys.size()];
		int keyDistance = octaveDistance * this->keysPerOctave + keyInOctave;
		this->keyHovered = keyBase + keyDistance;
	}

	/** Repaint */
	this->repaint();
}

void PianoComponent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void PianoComponent::mouseExit(const juce::MouseEvent& event) {
	this->keyHovered = -1;
	this->repaint();
}

void PianoComponent::mouseWheelMove(
	const juce::MouseEvent& event,
	const juce::MouseWheelDetails& wheel) {
	if (event.mods.isAltDown()) {
		double thumbPer = event.position.getY() / (double)this->getHeight();
		double centerNum = (this->pos / this->itemSize) + (this->getHeight() / this->itemSize) * thumbPer;

		this->wheelAltFunc(centerNum, thumbPer, wheel.deltaY, wheel.isReversed);
	}
	else {
		this->wheelFunc(wheel.deltaY, wheel.isReversed);
	}
}

void PianoComponent::updateKeysInternal() {
	this->topKey = this->totalKeys - std::floor(this->pos / this->itemSize);
	this->bottomKey = this->totalKeys - std::ceil((this->pos + this->getHeight()) / this->itemSize);
	this->startOctave = std::floor(this->bottomKey / (double)this->keysPerOctave);
	this->endOctave = std::ceil(this->topKey / (double)this->keysPerOctave);
	this->octaveNum = this->endOctave - this->startOctave;

	this->startOctavePos = (-this->pos) + this->totalKeys * this->itemSize - this->startOctave * this->sizePerOctave;

	this->repaint();
}
