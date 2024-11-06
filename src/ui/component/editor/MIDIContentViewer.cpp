#include "MIDIContentViewer.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

MIDIContentViewer::MIDIContentViewer(
	const ScrollFunc& scrollFunc,
	const WheelFunc& wheelFunc,
	const WheelAltFunc& wheelAltFunc,
	const MouseYPosFunc& mouseYPosFunc,
	const MouseLeaveFunc& mouseLeaveFunc,
	const DragStartFunc& dragStartFunc,
	const DragProcessFunc& dragProcessFunc,
	const DragEndFunc& dragEndFunc)
	: scrollFunc(scrollFunc), wheelFunc(wheelFunc), wheelAltFunc(wheelAltFunc),
	mouseYPosFunc(mouseYPosFunc), mouseLeaveFunc(mouseLeaveFunc),
	dragStartFunc(dragStartFunc), dragProcessFunc(dragProcessFunc), dragEndFunc(dragEndFunc) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::MidiContent));

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
	this->lineTemp = this->createRulerLine(this->hPos, this->hItemSize);
	this->updateRulerImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateBlocks() {
	/** Clear Temp */
	this->blockItemTemp.clear();

	/** Update Block Temp */
	auto list = quickAPI::getBlockList(this->index);
	for (auto [startTime, endTime, offset] : list) {
		this->blockItemTemp.add({ startTime, endTime, startTime - offset });
	}

	/** Sort by Source Start Time to Optimize Note Drawing Time */
	class BlockItemComparator {
	public:
		static int compareElements(const BlockItem& first, const BlockItem& second) {
			auto& firstSourceStartTime = std::get<2>(first);
			auto& secondSourceStartTime = std::get<2>(second);
			return (firstSourceStartTime < secondSourceStartTime) ? -1
				: ((secondSourceStartTime < firstSourceStartTime) ? 1 : 0);
		}
	} blockItemComp{};
	this->blockItemTemp.sort(blockItemComp);

	/** Update UI */
	this->updateBlockImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateData() {
	/** Clear Temp */
	this->midiDataTemp.clear();

	/** Update Note Temp */
	if (this->index >= 0 && this->ref != 0) {
		auto midiDataList = quickAPI::getSeqTrackMIDIData(this->index);

		/** Note Start Time Temp */
		constexpr int channalNum = 16;
		constexpr int noteMaxNum = 128;
		/** Start Sec, Velocity */
		using NoteTempItem = std::tuple<double, uint8_t>;
		const NoteTempItem initTempItem{ -1.0, 0 };
		std::array<NoteTempItem, noteMaxNum * channalNum> noteStartTime{};
		std::fill(noteStartTime.begin(), noteStartTime.end(), initTempItem);

		/** Match Each Note */
		for (auto event : midiDataList) {
			if (event->message.isNoteOn(true)) {
				int noteNumber = event->message.getNoteNumber();
				int channel = event->message.getChannel();
				if (noteNumber >= 0 && noteNumber < noteMaxNum
					&& channel > 0 && channel <= channalNum) {
					size_t tempIndex = noteMaxNum * ((size_t)channel - 1) + noteNumber;
					noteStartTime[tempIndex] = { event->message.getTimeStamp(), event->message.getVelocity() };
				}
			}
			else if (event->message.isNoteOff(false)) {
				int noteNumber = event->message.getNoteNumber();
				int channel = event->message.getChannel();
				if (noteNumber >= 0 && noteNumber < noteMaxNum
					&& channel > 0 && channel <= channalNum) {
					size_t tempIndex = noteMaxNum * ((size_t)channel - 1) + noteNumber;
					auto [noteStart, noteVel] = noteStartTime[tempIndex];
					noteStartTime[tempIndex] = initTempItem;
					if (noteStart >= 0) {
						double noteEnd = event->message.getTimeStamp();

						Note note{};
						note.startSec = noteStart;
						note.endSec = noteEnd;
						note.num = noteNumber;
						note.vel = noteVel;
						note.channel = channel;
						note.lyrics = "";/**< TODO Lyrics */
						this->midiDataTemp.add(note);
					}
				}
			}
		}

		/** Sort Note by Start Time */
		class NoteItemComparator {
		public:
			static int compareElements(const Note& first, const Note& second) {
				return (first.startSec < second.startSec) ? -1
					: ((second.startSec < first.startSec) ? 1 : 0);
			}
		} noteItemComp{};
		this->midiDataTemp.sort(noteItemComp);
	}

	/** Update Note Zone Temp */
	{
		uint8_t minNote = 127, maxNote = 0;
		for (auto& note : this->midiDataTemp) {
			minNote = std::min(minNote, note.num);
			maxNote = std::max(maxNote, note.num);
		}
		if (maxNote < minNote) { maxNote = minNote = 0; }
		this->midiMinNote = minNote;
		this->midiMaxNote = maxNote;
	}

	/** Update UI */
	this->updateNoteImageTemp();
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
	this->lineTemp = this->createRulerLine(pos, itemSize);

	this->updateRulerImageTemp();
	this->updateBlockImageTemp();
	this->updateNoteImageTemp();
	this->repaint();
}

void MIDIContentViewer::updateVPos(double pos, double itemSize) {
	if (this->getHeight() <= 0) { return; }

	this->vPos = pos;
	this->vItemSize = itemSize;

	std::tie(this->keyTop, this->keyBottom) = this->getVViewArea(pos, itemSize);

	this->updateKeyImageTemp();
	this->updateNoteImageTemp();
	this->repaint();
}

void MIDIContentViewer::resized() {
	/** Update Line Temp */
	std::tie(this->secStart, this->secEnd) = this->getHViewArea(this->hPos, this->hItemSize);
	std::tie(this->keyTop, this->keyBottom) = this->getVViewArea(this->vPos, this->vItemSize);
	this->lineTemp = this->createRulerLine(this->hPos, this->hItemSize);

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

void MIDIContentViewer::mouseDown(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown() && event.mods.isAltDown()) {
		/** Move View Area */
		this->viewMoving = true;
		this->setMouseCursor(juce::MouseCursor::DraggingHandCursor);
		this->dragStartFunc();
	}
}

void MIDIContentViewer::mouseUp(const juce::MouseEvent& event) {
	if (event.mods.isLeftButtonDown()) {
		/** Move View */
		if (this->viewMoving) {
			this->viewMoving = false;
			this->setMouseCursor(juce::MouseCursor::NormalCursor);
			this->dragEndFunc();
		}
	}
}

void MIDIContentViewer::mouseMove(const juce::MouseEvent& event) {
	/** Send Y Pos */
	this->mouseYPosFunc(event.position.getY());
}

void MIDIContentViewer::mouseDrag(const juce::MouseEvent& event) {
	/** Send Y Pos */
	this->mouseYPosFunc(event.position.getY());

	/** Auto Scroll */
	float xPos = event.position.getX();
	if (!this->viewMoving) {
		double delta = 0;
		if (xPos > this->getWidth()) {
			delta = xPos - this->getWidth();
		}
		else if (xPos < 0) {
			delta = xPos;
		}

		if (delta != 0) {
			this->scrollFunc(delta / 4);
		}
	}

	if (event.mods.isLeftButtonDown()) {
		/** Move View */
		if (this->viewMoving) {
			int distanceX = event.getDistanceFromDragStartX();
			int distanceY = event.getDistanceFromDragStartY();
			this->dragProcessFunc(distanceX, distanceY, true, true);
		}
	}
}

void MIDIContentViewer::mouseExit(const juce::MouseEvent& event) {
	/** Send Mouse Exit */
	this->mouseLeaveFunc();

	/** Move View */
	if (this->viewMoving) {
		this->viewMoving = false;
		this->setMouseCursor(juce::MouseCursor::NormalCursor);
		this->dragEndFunc();
	}
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

	float shortLineIntervalMin = screenSize.getWidth() * 0.01 / 8;
	float dashLineIntervalMin = screenSize.getWidth() * 0.01;

	float dashLineThickness = screenSize.getWidth() * 0.0005;
	float dashLineDashLength = screenSize.getHeight() * 0.005;
	float dashLineSkipLength = dashLineDashLength;
	float dashLineArray[2] = { dashLineDashLength, dashLineSkipLength };

	/** Colors */
	auto& laf = this->getLookAndFeel();
	juce::Colour lineColor = laf.findColour(
		juce::TableListBox::ColourIds::outlineColourId);

	/** Lines */
	for (int i = 0; i < this->lineTemp.size(); i++) {
		auto [xPos, type, barId] = this->lineTemp.getUnchecked(i);

		/** Check Interval */
		if (type == LineItemType::Dashed) {
			if (i > 0 && (xPos - std::get<0>(this->lineTemp.getUnchecked(i - 1))) < dashLineIntervalMin) {
				continue;
			}
			if (i < this->lineTemp.size() - 1 && (std::get<0>(this->lineTemp.getUnchecked(i + 1)) - xPos) < dashLineIntervalMin) {
				continue;
			}
		}
		if (type == LineItemType::Beat) {
			if (i > 0 && (xPos - std::get<0>(this->lineTemp.getUnchecked(i - 1))) < shortLineIntervalMin) {
				continue;
			}
			if (i < this->lineTemp.size() - 1 && (std::get<0>(this->lineTemp.getUnchecked(i + 1)) - xPos) < shortLineIntervalMin) {
				continue;
			}
		}

		/** Line */
		if (type != LineItemType::Dashed) {
			float lineThickness = type == LineItemType::Bar ? longLineThickness : shortLineThickness;
			juce::Rectangle<float> lineRect(
				xPos - lineThickness / 2, 0,
				lineThickness, this->rulerTemp->getHeight());

			g.setColour(lineColor);
			g.fillRect(lineRect);
		}
		else {
			juce::Line<float> line(
				xPos, 0, xPos, this->rulerTemp->getHeight());

			g.setColour(lineColor);
			g.drawDashedLine(line, dashLineArray,
				sizeof(dashLineArray) / sizeof(float), dashLineThickness, 0);
		}
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

void MIDIContentViewer::updateNoteImageTemp() {
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

const MIDIContentViewer::LineItemList
MIDIContentViewer::createRulerLine(double pos, double itemSize) const {
	/** Get View Area */
	auto [secStart, secEnd] = this->getHViewArea(pos, itemSize);
	double width = this->getWidth();

	/** Get Each Line */
	LineItemList result;

	/** Get Temp */
	int tempStartIndex = quickAPI::getTempoTempIndexBySec(secStart);
	int tempEndIndex = quickAPI::getTempoTempIndexBySec(secEnd);
	juce::Array<quickAPI::TempoData> tempoTempList;
	for (int i = tempStartIndex; i <= tempEndIndex; i++) {
		tempoTempList.add(quickAPI::getTempoData(i));
	}
	if (tempoTempList.size() <= 0) { return result; }

	/** Line Start */
	constexpr int dashLineNum = 8;
	double realSecStart = secStart;
	int tempIndex = 0;
	auto [timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator] = tempoTempList.getUnchecked(tempIndex);
	{
		/** Get Real Quarter */
		double quarterStart = timeInQuarter + (secStart - timeInSec) / secPerQuarter;
		double realQuarterStart = std::floor(quarterStart * (denominator / 4.0) * dashLineNum) / ((denominator / 4.0) * dashLineNum);
		if (!juce::approximatelyEqual(quarterStart, realQuarterStart)) {
			realQuarterStart += ((4.0 / denominator) / dashLineNum);
		}

		/** Next Temp */
		while ((tempoTempList.size() > (tempIndex + 1)) &&
			(realQuarterStart > std::get<1>(tempoTempList.getUnchecked(tempIndex + 1)))) {
			realQuarterStart -= (4.0 / denominator) / dashLineNum;
			std::tie(timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator) = tempoTempList.getUnchecked(++tempIndex);
			realQuarterStart += (4.0 / denominator) / dashLineNum;
		}

		/** Get Real Sec */
		realSecStart = timeInSec + (realQuarterStart - timeInQuarter) * secPerQuarter;
	}

	/** Build Line Temp */
	for (double currentSec = realSecStart; currentSec < secEnd;) {
		/** Check Current Is Bar */
		double currentQuarter = timeInQuarter + (currentSec - timeInSec) / secPerQuarter;
		double currentBar = timeInBar + (currentQuarter - timeInQuarter) * (denominator / 4.0) / numerator;
		constexpr double epsilon = 1.0 / 480.0;
		//bool isBar = juce::approximatelyEqual(currentBar, std::round(currentBar));
		LineItemType type = LineItemType::Dashed;
		if (std::abs(std::round(currentQuarter * (denominator / 4.0)) - (currentQuarter * (denominator / 4.0))) < epsilon) {
			type = LineItemType::Beat;
		}
		if (std::abs(std::round(currentBar) - currentBar) < epsilon) {
			type = LineItemType::Bar;
		}

		/** Get Current X Pos */
		double XPos = (currentSec - secStart) / (secEnd - secStart) * width;

		/** Add Into Result */
		result.add({ XPos, type, (int)std::round(currentBar) });

		/** Next Line */
		double nextQuarter = currentQuarter + (4.0 / denominator) / dashLineNum;
		while ((tempoTempList.size() > (tempIndex + 1)) &&
			(nextQuarter > std::get<1>(tempoTempList.getUnchecked(tempIndex + 1)))) {
			nextQuarter -= (4.0 / denominator) / dashLineNum;
			std::tie(timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator) = tempoTempList.getUnchecked(++tempIndex);
			nextQuarter += (4.0 / denominator)/ dashLineNum;
		}

		/** Update Current Sec */
		currentSec = timeInSec + (nextQuarter - timeInQuarter) * secPerQuarter;
	}

	/** Result */
	return result;
}
