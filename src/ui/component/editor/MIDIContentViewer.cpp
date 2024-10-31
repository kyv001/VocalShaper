#include "MIDIContentViewer.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

MIDIContentViewer::MIDIContentViewer(
	const WheelFunc& wheelFunc,
	const WheelAltFunc& wheelAltFunc)
	: wheelFunc(wheelFunc), wheelAltFunc(wheelAltFunc) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forMidiContent());

	/** Init Temp */
	this->rulerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
	this->keyTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
	this->blockTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);
}

void MIDIContentViewer::update(int index, uint64_t ref) {
	this->index = index;
	this->ref = ref;

	this->updateBlocks();
}

void MIDIContentViewer::updateTempoLabel() {
	/** Update Ruler Temp */
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(this->hPos, this->hItemSize);
	this->updateRulerImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateBlocks() {
	/** Clear Temp */
	this->blockItemTemp.clear();

	/** Update Block Temp */
	auto list = quickAPI::getBlockList(this->index);
	for (auto [startTime, endTime, offset] : list) {
		this->blockItemTemp.add({ startTime, endTime });
	}

	/** Update UI */
	this->updateBlockImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateLevelMeter() {
	/** Get Play Position */
	this->playPosSec = quickAPI::getTimeInSecond();

	/** Get Loop Time */
	std::tie(this->loopStartSec, this->loopEndSec) = quickAPI::getLoopTimeSec();

	/** Repaint */
	this->repaint();
}

void MIDIContentViewer::updateHPos(double pos, double itemSize) {
	if (this->getWidth() <= 0) { return; }

	this->hPos = pos;
	this->hItemSize = itemSize;

	std::tie(this->secStart, this->secEnd) = this->getHViewArea(pos, itemSize);
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(pos, itemSize);

	this->updateRulerImageTemp();
	this->updateBlockImageTemp();
	this->updateDataImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateVPos(double pos, double itemSize) {
	if (this->getHeight() <= 0) { return; }

	this->vPos = pos;
	this->vItemSize = itemSize;

	std::tie(this->keyTop, this->keyBottom) = this->getVViewArea(pos, itemSize);

	this->updateKeyImageTemp();
	this->updateDataImageTemp();
	this->repaint();
}

void MIDIContentViewer::resized() {
	/** Update Line Temp */
	std::tie(this->secStart, this->secEnd) = this->getHViewArea(this->hPos, this->hItemSize);
	std::tie(this->keyTop, this->keyBottom) = this->getVViewArea(this->vPos, this->vItemSize);
	std::tie(this->lineTemp, this->minInterval) = this->createRulerLine(this->hPos, this->hItemSize);

	/** Update Ruler Temp */
	int width = this->getWidth(), height = this->getHeight();
	width = std::max(width, 1);
	height = std::max(height, 1);
	this->rulerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateRulerImageTemp();

	/** Update Key Temp */
	this->keyTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateKeyImageTemp();

	/** Update Block Temp */
	this->blockTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, width, height, false);
	this->updateBlockImageTemp();
}

void MIDIContentViewer::paint(juce::Graphics& g) {
	/** Key Temp */
	if (this->keyTemp) {
		g.drawImageAt(*(this->keyTemp.get()), 0, 0);
	}

	/** Ruler Temp */
	if (this->rulerTemp) {
		g.drawImageAt(*(this->rulerTemp.get()), 0, 0);
	}

	/** Block Temp */
	if (this->blockTemp) {
		g.drawImageAt(*(this->blockTemp.get()), 0, 0);
	}
}

void MIDIContentViewer::paintOverChildren(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float cursorThickness = screenSize.getWidth() * 0.00075;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour cursorColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	/*juce::Colour offColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);*/

	/** Cursor */
	int width = this->getWidth(), height = this->getHeight();
	float cursorPosX = ((this->playPosSec - this->secStart) / (this->secEnd - this->secStart)) * width;
	juce::Rectangle<float> cursorRect(
		cursorPosX - cursorThickness / 2, 0,
		cursorThickness, height);

	if (cursorPosX >= 0 && cursorPosX <= width) {
		g.setColour(cursorColor);
		g.fillRect(cursorRect);
	}

	/** Time Off */
	//if (this->loopEndSec > this->loopStartSec) {
	//	/** Left */
	//	if (this->loopStartSec > this->secStart) {
	//		float xPos = (this->loopStartSec - this->secStart) / (this->secEnd - this->secStart) * width;
	//		juce::Rectangle<float> offRect(0, 0, xPos, height);

	//		g.setColour(offColor);
	//		g.fillRect(offRect);
	//	}

	//	/** Right */
	//	if (this->loopEndSec < this->secEnd) {
	//		float xPos = (this->loopEndSec - this->secStart) / (this->secEnd - this->secStart) * width;
	//		juce::Rectangle<float> offRect(xPos, 0, width - xPos, height);

	//		g.setColour(offColor);
	//		g.fillRect(offRect);
	//	}
	//}
}

void MIDIContentViewer::mouseWheelMove(
	const juce::MouseEvent& event,
	const juce::MouseWheelDetails& wheel) {
	if (event.mods.isAltDown()) {
		double thumbPer = event.position.getX() / (double)this->getWidth();
		double centerNum = this->secStart + (this->secEnd - this->secStart) * thumbPer;

		this->wheelAltFunc(centerNum, thumbPer, wheel.deltaY, wheel.isReversed);
	}
	else {
		this->wheelFunc(wheel.deltaY, wheel.isReversed);
	}
}

void MIDIContentViewer::updateKeyImageTemp() {
	/** Clear Temp */
	juce::Graphics g(*(this->keyTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.0005;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour whiteKeyColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::whiteNoteColourId);
	juce::Colour blackKeyColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::blackNoteColourId);
	juce::Colour lineColor = laf.findColour(
		juce::MidiKeyboardComponent::ColourIds::keySeparatorLineColourId);

	/** Background */
	g.fillAll(backgroundColor);

	/** Keys */
	int keyStart = std::floor(this->keyBottom);
	int keyEnd = std::floor(this->keyTop);

	float keyStartPos = this->getHeight() + (this->keyBottom - keyStart) * this->vItemSize;
	for (int i = keyStart; i >= 0 && i <= keyEnd; i++) {
		bool isBlackKey = this->keyMasks[i % 12];
		float keyPos = keyStartPos - (i - keyStart) * this->vItemSize;

		juce::Rectangle<float> keyRect(
			0, keyPos - this->vItemSize,
			this->keyTemp->getWidth(), this->vItemSize);
		g.setColour(isBlackKey ? blackKeyColor : whiteKeyColor);
		g.fillRect(keyRect);

		juce::Rectangle<float> lineRect(
			0, keyPos - lineThickness / 2,
			keyRect.getWidth(), lineThickness);
		g.setColour(lineColor);
		g.fillRect(lineRect);
	}
}

void MIDIContentViewer::updateRulerImageTemp() {
	/** Clear Temp */
	this->rulerTemp->clear(this->rulerTemp->getBounds());
	juce::Graphics g(*(this->rulerTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float longLineThickness = screenSize.getWidth() * 0.00075;
	float shortLineThickness = screenSize.getWidth() * 0.0005;

	float shortLineIntervalMin = screenSize.getWidth() * 0.01;

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour lineColor = laf.findColour(
		juce::TableListBox::ColourIds::outlineColourId);

	/** Lines */
	for (auto& [xPos, isLong, barId] : this->lineTemp) {
		/** Check Interval */
		if (!isLong) {
			if (this->minInterval < shortLineIntervalMin) {
				continue;
			}
		}

		/** Line */
		float lineThickness = isLong ? longLineThickness : shortLineThickness;
		juce::Rectangle<float> lineRect(
			xPos - lineThickness / 2, 0,
			lineThickness, this->rulerTemp->getHeight());

		g.setColour(lineColor);
		g.fillRect(lineRect);
	}
}

void MIDIContentViewer::updateBlockImageTemp() {
	/** Clear Temp */
	this->blockTemp->clear(this->blockTemp->getBounds());
	juce::Graphics g(*(this->blockTemp.get()));

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour offColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);

	/** Paint Each Area */
	{
		double startSec = 0;
		for (int i = 0; i < this->blockItemTemp.size() + 1; i++) {
			double endSec = (i < this->blockItemTemp.size())
				? std::get<0>(this->blockItemTemp.getUnchecked(i))
				: std::max(startSec, this->secEnd);

			if (startSec < this->secEnd && endSec > this->secStart) {
				float startPos = (startSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();
				float endPos = (endSec - this->secStart) / (this->secEnd - this->secStart) * this->getWidth();

				juce::Rectangle<float> areaRect(
					startPos, 0, endPos - startPos, this->blockTemp->getHeight());
				g.setColour(offColor);
				g.fillRect(areaRect);
			}

			if (i < this->blockItemTemp.size()) {
				startSec = std::get<1>(this->blockItemTemp.getUnchecked(i));
			}
		}
	}
}

void MIDIContentViewer::updateDataImageTemp() {
	/** TODO */
}

std::tuple<double, double> MIDIContentViewer::getHViewArea(double pos, double itemSize) const {
	double secStart = pos / itemSize;
	double secLength = this->getWidth() / itemSize;
	return { secStart, secStart + secLength };
}

std::tuple<double, double> MIDIContentViewer::getVViewArea(double pos, double itemSize) const {
	double keyTop = this->totalKeys - pos / itemSize;
	double keyNum = this->getHeight() / itemSize;
	return { keyTop, keyTop - keyNum };
}

const std::tuple<juce::Array<MIDIContentViewer::LineItem>, double>
MIDIContentViewer::createRulerLine(double pos, double itemSize) const {
	/** Get View Area */
	auto [secStart, secEnd] = this->getHViewArea(pos, itemSize);
	double width = this->getWidth();

	/** Get Each Line */
	juce::Array<LineItem> result;
	double minInterval = DBL_MAX;

	/** Get Temp */
	int tempStartIndex = quickAPI::getTempoTempIndexBySec(secStart);
	int tempEndIndex = quickAPI::getTempoTempIndexBySec(secEnd);
	juce::Array<quickAPI::TempoData> tempoTempList;
	for (int i = tempStartIndex; i <= tempEndIndex; i++) {
		tempoTempList.add(quickAPI::getTempoData(i));
	}
	if (tempoTempList.size() <= 0) { return { result, minInterval }; }

	/** Line Start */
	double realSecStart = secStart;
	int tempIndex = 0;
	auto& [timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator] = tempoTempList.getReference(tempIndex);
	{
		/** Get Real Quarter */
		double quarterStart = timeInQuarter + (secStart - timeInSec) / secPerQuarter;
		double realQuarterStart = std::floor(quarterStart * (denominator / 4.0)) / (denominator / 4.0);
		if (!juce::approximatelyEqual(quarterStart, realQuarterStart)) {
			realQuarterStart += (4.0 / denominator);
		}

		/** Next Temp */
		while ((tempoTempList.size() > (tempIndex + 1)) &&
			(realQuarterStart > std::get<1>(tempoTempList.getReference(tempIndex + 1)))) {
			realQuarterStart -= (4.0 / denominator);
			std::tie(timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator) = tempoTempList.getReference(++tempIndex);
			realQuarterStart += (4.0 / denominator);
		}

		/** Get Real Sec */
		realSecStart = timeInSec + (realQuarterStart - timeInQuarter) * secPerQuarter;
	}

	/** Build Line Temp */
	for (double currentSec = realSecStart; currentSec < secEnd;) {
		/** Check Current Is Bar */
		double currentQuarter = timeInQuarter + (currentSec - timeInSec) / secPerQuarter;
		double currentBar = timeInBar + (currentQuarter - timeInQuarter) * (denominator / 4.0) / numerator;
		//bool isBar = juce::approximatelyEqual(currentBar, std::round(currentBar));
		bool isBar = std::abs(std::round(currentBar) - currentBar) < (1.0 / 480.0);

		/** Get Current X Pos */
		double XPos = (currentSec - secStart) / (secEnd - secStart) * width;

		/** Add Into Result */
		result.add({ XPos, isBar, (int)std::round(currentBar) });

		/** Next Line */
		double nextQuarter = currentQuarter + (4.0 / denominator);
		while ((tempoTempList.size() > (tempIndex + 1)) &&
			(nextQuarter > std::get<1>(tempoTempList.getReference(tempIndex + 1)))) {
			nextQuarter -= (4.0 / denominator);
			std::tie(timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator) = tempoTempList.getReference(++tempIndex);
			nextQuarter += (4.0 / denominator);
		}

		/** Update Current Sec */
		currentSec = timeInSec + (nextQuarter - timeInQuarter) * secPerQuarter;
	}

	/** Get Min Interval */
	for (auto& item : tempoTempList) {
		double lineDeltaSec = (4.0 / std::get<5>(item)) * std::get<3>(item);
		double interval = lineDeltaSec / (secEnd - secStart) * width;
		minInterval = std::min(minInterval, interval);
	}

	/** Result */
	return { result, minInterval };
}
