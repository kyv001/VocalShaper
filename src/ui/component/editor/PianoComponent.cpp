#include "PianoComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"

PianoComponent::PianoComponent(
	const WheelFunc& wheelFunc,
	const WheelAltFunc& wheelAltFunc,
	const KeyUpDownFunc& keyUpDownFunc)
	: wheelFunc(wheelFunc), wheelAltFunc(wheelAltFunc), keyUpDownFunc(keyUpDownFunc),
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
	juce::Colour pressedColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::keyDownOverlayColourId);

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
				if (keyNumber == this->keyPressed) {
					g.setColour(pressedColor);
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
				if (keyNumber == this->keyPressed) {
					g.setColour(pressedColor);
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

void PianoComponent::mouseDown(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		auto [key, vel] = this->findCurrentKey(event.position);
		if (this->keyPressed == -1 && key != -1) {
			this->keyPressed = key;
			this->keyUpDownFunc(key, true, vel);

			this->repaint();
		}
	}
}

void PianoComponent::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		if (this->keyPressed != -1) {
			this->keyUpDownFunc(this->keyPressed, false, 0);
			this->keyPressed = -1;

			this->repaint();
		}
	}
}

void PianoComponent::mouseMove(const juce::MouseEvent& event) {
	this->mouseYPosChangedOutside(
		event.position.getY(), event.position.getX());
}

void PianoComponent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);

	/** Check Key Pressed */
	auto [key, vel] = this->findCurrentKey(event.position);
	if (this->keyPressed != -1 && key != -1 && this->keyPressed != key) {
		this->keyUpDownFunc(this->keyPressed, false, 0);
		this->keyUpDownFunc(key, true, vel);
		this->keyPressed = key;

		this->repaint();
	}
}

void PianoComponent::mouseExit(const juce::MouseEvent& event) {
	if (this->keyPressed != -1) {
		this->keyUpDownFunc(this->keyPressed, false, 0);
		this->keyPressed = -1;
	}

	this->mouseLeaveOutside();
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

void PianoComponent::mouseYPosChangedOutside(float posY, float posX) {
	/** Check Mouse Hovered Key */
	std::tie(this->keyHovered, std::ignore) = this->findCurrentKey(juce::Point{ posX, posY });

	/** Repaint */
	this->repaint();
}

void PianoComponent::mouseLeaveOutside() {
	this->keyHovered = -1;
	this->repaint();
}

std::tuple<int, float> PianoComponent::findCurrentKey(const juce::Point<float>& pos) const {
	float posX = pos.getX(), posY = pos.getY();

	float blackLength = 0.7;
	float blackWidth = blackLength * this->getWidth();
	if (posX <= blackWidth) {
		int keyDistance = std::floor((this->startOctavePos - posY) / this->itemSize);
		int keyBase = this->startOctave * this->keysPerOctave;
		bool isBlack = std::find(this->blackKeys.begin(), this->blackKeys.end(), keyDistance % this->keysPerOctave) != this->blackKeys.end();
		float vel = isBlack ? (posX / blackWidth) : (posX / this->getWidth());
		return { keyBase + keyDistance, vel };
	}
	else {
		int whiteKeyDistance = std::floor((this->startOctavePos - posY) / this->sizePerWhiteKey);
		int keyBase = this->startOctave * this->keysPerOctave;
		int octaveDistance = whiteKeyDistance / this->whiteKeys.size();
		int keyInOctave = this->whiteKeys[whiteKeyDistance % this->whiteKeys.size()];
		int keyDistance = octaveDistance * this->keysPerOctave + keyInOctave;
		return { keyBase + keyDistance, posX / this->getWidth() };
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
