#include "MIDISourceEditor.h"
#include "../../misc/Tools.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

#define MIDI_TAIL_SEC 10;

MIDISourceEditor::MIDISourceEditor() {
	/** Scroller */
	this->hScroller = std::make_unique<Scroller>(false,
		[this] { return (double)(this->getViewWidth()); },
		[this] { return this->getTimeLength(); },
		[this] { return this->getTimeWidthLimit(); },
		[this](double pos, double itemSize) { this->updateHPos(pos, itemSize); },
		[this](juce::Graphics& g, int width, int height, bool vertical, double totalNum) {
			this->paintNotePreview(g, width, height, vertical, totalNum); },
			Scroller::PaintItemPreviewFunc{},
			[this] { return this->getPlayPos(); });
	this->hScroller->setShouldShowPlayPos(true);
	this->addAndMakeVisible(this->hScroller.get());

	this->vScroller = std::make_unique<Scroller>(true,
		[this] { return (double)(this->getViewHeight()); },
		[this] { return (double)(this->getKeyNum()); },
		[this] { return this->getKeyHeightLimit(); },
		[this](double pos, double itemSize) { this->updateVPos(pos, itemSize); },
		Scroller::PaintPreviewFunc{},
		Scroller::PaintItemPreviewFunc{});
	this->addAndMakeVisible(this->vScroller.get());

	/** Ruler */
	this->ruler = std::make_unique<SourceTimeRuler>(
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(double delta) {
			if (comp) {
				comp->scroll(delta);
			}
		},
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(double centerPer, double thumbPer, double delta) {
			if (comp) {
				comp->scale(centerPer, thumbPer, delta);
			}
		},
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(float deltaY, bool reversed) {
			if (comp) {
				comp->mouseWheelOutside(deltaY, reversed);
			}
		},
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(double centerNum, double thumbPer, float deltaY, bool reversed) {
			if (comp) {
				comp->mouseWheelOutsideWithAlt(centerNum, thumbPer, deltaY, reversed);
			}
		},
		[comp = ScrollerBase::SafePointer(this)] {
			if (comp) {
				comp->processAreaDragStart();
			}
		},
		[comp = ScrollerBase::SafePointer(this)]
		(int distanceX, int distanceY, bool moveX, bool moveY) {
			if (comp) {
				comp->processAreaDragTo(
					distanceX, distanceY, moveX, moveY);
			}
		},
		[comp = ScrollerBase::SafePointer(this)] {
			if (comp) {
				comp->processAreaDragEnd();
			}
		});
	this->addAndMakeVisible(this->ruler.get());

	/** Piano */
	this->piano = std::make_unique<PianoComponent>(
		[comp = ScrollerBase::SafePointer(this->vScroller.get())]
		(float deltaY, bool reversed) {
			if (comp) {
				comp->mouseWheelOutside(deltaY, reversed);
			}
		},
		[comp = ScrollerBase::SafePointer(this->vScroller.get())]
		(double centerNum, double thumbPer, float deltaY, bool reversed) {
			if (comp) {
				comp->mouseWheelOutsideWithAlt(centerNum, thumbPer, deltaY, reversed);
			}
		},
		[comp = MIDISourceEditor::SafePointer(this)]
		(int noteNum, bool isDown, float vel) {
			if (comp) {
				comp->sendKeyUpDown(noteNum, isDown, vel);
			}
		}
	);
	this->addAndMakeVisible(this->piano.get());

	/** Content */
	this->content = std::make_unique<MIDIContentViewer>(
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(float deltaY, bool reversed) {
			if (comp) {
				comp->mouseWheelOutside(deltaY, reversed);
			}
		},
		[comp = ScrollerBase::SafePointer(this->hScroller.get())]
		(double centerNum, double thumbPer, float deltaY, bool reversed) {
			if (comp) {
				comp->mouseWheelOutsideWithAlt(centerNum, thumbPer, deltaY, reversed);
			}
		},
		[comp = PianoComponent::SafePointer(this->piano.get())]
		(float posY) {
			if (comp) {
				comp->mouseYPosChangedOutside(posY);
			}
		},
		[comp = PianoComponent::SafePointer(this->piano.get())] {
			if (comp) {
				comp->mouseLeaveOutside();
			}
		});
	this->addAndMakeVisible(this->content.get());

	/** Set Default V Pos */
	juce::MessageManager::callAsync([scroller = Scroller::SafePointer{ this->vScroller.get() }] {
		if (scroller) {
			scroller->setPos((128 - 6 * 12) * scroller->getItemSize());
		}
		});
}

void MIDISourceEditor::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.065;
	int pianoWidth = screenSize.getWidth() * 0.065;

	int adsorbButtonHeight = screenSize.getHeight() * 0.025;
	int adsorbButtonPaddingWidth = screenSize.getWidth() * 0.005;
	int adsorbButtonPaddingHeight = screenSize.getHeight() * 0.005;

	/** Update Time */
	std::tie(this->secStart, this->secEnd) = this->getViewArea(this->pos, this->itemSize);

	/** Scroller */
	juce::Rectangle<int> hScrollerRect(
		pianoWidth, this->getHeight() - scrollerHeight,
		this->getWidth() - pianoWidth - scrollerWidth, scrollerHeight);
	this->hScroller->setBounds(hScrollerRect);
	juce::Rectangle<int> vScrollerRect(
		this->getWidth() - scrollerWidth, rulerHeight,
		scrollerWidth, this->getHeight() - rulerHeight - scrollerHeight);
	this->vScroller->setBounds(vScrollerRect);

	/** Adsorb Button */
	/*juce::Rectangle<int> adsorbRect(
		headWidth - adsorbButtonPaddingWidth - adsorbButtonHeight,
		rulerHeight - adsorbButtonPaddingHeight - adsorbButtonHeight,
		adsorbButtonHeight, adsorbButtonHeight);
	this->adsorbButton->setBounds(adsorbRect);*/

	/** Time Ruler */
	juce::Rectangle<int> rulerRect(
		pianoWidth, 0,
		hScrollerRect.getWidth(), rulerHeight);
	this->ruler->setBounds(rulerRect);

	/** Piano */
	juce::Rectangle<int> pianoRect(
		0, rulerRect.getBottom(),
		pianoWidth, hScrollerRect.getY() - rulerRect.getBottom());
	this->piano->setBounds(pianoRect);

	/** Content */
	juce::Rectangle<int> contentRect(
		hScrollerRect.getX(), vScrollerRect.getY(),
		hScrollerRect.getWidth(), vScrollerRect.getHeight());
	this->content->setBounds(contentRect);

	/** Update View Pos */
	this->hScroller->update();
	this->vScroller->update();
}

void MIDISourceEditor::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int scrollerHeight = screenSize.getHeight() * 0.0275;
	int scrollerWidth = screenSize.getWidth() * 0.015;
	int rulerHeight = screenSize.getHeight() * 0.065;
	int pianoWidth = screenSize.getWidth() * 0.065;

	float lineThickness = screenSize.getHeight() * 0.0025;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId);
	juce::Colour headBackgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundWhenEditingColourId);
	juce::Colour outlineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Background */
	g.fillAll(backgroundColor);

	/** Piano Bar */
	juce::Rectangle<int> headRect(
		0, 0, pianoWidth, this->getHeight());
	g.setColour(headBackgroundColor);
	g.fillRect(headRect);

	/** Top Head Split */
	juce::Rectangle<float> headTopLineRect(
		0, rulerHeight - lineThickness / 2, pianoWidth, lineThickness);
	g.setColour(outlineColor);
	g.fillRect(headTopLineRect);

	/** Bottom Head Split */
	juce::Rectangle<float> bottomTopLineRect(
		0, this->getHeight() - scrollerHeight - lineThickness / 2, pianoWidth, lineThickness);
	g.setColour(outlineColor);
	g.fillRect(bottomTopLineRect);

	/** Top Right Corner */
	juce::Rectangle<float> topRightRect(
		this->getWidth() - scrollerWidth, 0,
		scrollerWidth, rulerHeight);
	g.setColour(headBackgroundColor);
	g.fillRect(topRightRect);

	/** Bottom Right Corner */
	juce::Rectangle<float> bottomRightRect(
		this->getWidth() - scrollerWidth, this->getHeight() - scrollerHeight,
		scrollerWidth, scrollerHeight);
	g.setColour(headBackgroundColor);
	g.fillRect(bottomRightRect);
}

void MIDISourceEditor::update(int index, uint64_t ref) {
	this->index = index;
	this->ref = ref;

	/** Color */
	this->trackColor = quickAPI::getSeqTrackColor(index);

	/** Content */
	this->content->update(index, ref);

	/** Blocks */
	this->updateBlocks();
}

void MIDISourceEditor::updateTempo() {
	/** Update Time Ruler */
	this->ruler->updateTempoLabel();

	/** Update Content */
	this->content->updateTempoLabel();
}

void MIDISourceEditor::updateBlocks() {
	/** Total Length */
	this->totalLength = quickAPI::getTotalLength() + MIDI_TAIL_SEC;

	/** Update Block Temp */
	this->updateBlockTemp();

	/** Update Content */
	this->content->updateBlocks();

	/** Update View Pos */
	this->vScroller->update();
	this->hScroller->update();
}

void MIDISourceEditor::updateLevelMeter() {
	/** Get Play Position */
	this->playPosSec = quickAPI::getTimeInSecond();

	/** Get Play State */
	bool isPlaying = quickAPI::isPlaying();

	/** Follow */
	if (isPlaying && Tools::getInstance()->getFollow()) {
		if ((this->playPosSec < this->secStart) || (this->playPosSec > this->secEnd)) {
			this->hScroller->setPos(this->playPosSec * this->itemSize);
		}
	}
}

int MIDISourceEditor::getViewWidth() const {
	return this->hScroller->getWidth();
}

double MIDISourceEditor::getTimeLength() const {
	return this->totalLength;
}

std::tuple<double, double> MIDISourceEditor::getTimeWidthLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getWidth() * 0.02, screenSize.getWidth() * 0.5 };
}

double MIDISourceEditor::getPlayPos() const {
	return quickAPI::getTimeInSecond();
}

void MIDISourceEditor::updateHPos(double pos, double itemSize) {
	/** Set Pos */
	this->pos = pos;
	this->itemSize = itemSize;
	std::tie(this->secStart, this->secEnd) = this->getViewArea(pos, itemSize);

	/** Update Comp */
	this->ruler->updateHPos(pos, itemSize);
	this->content->updateHPos(pos, itemSize);
}

void MIDISourceEditor::paintNotePreview(juce::Graphics& g,
	int width, int height, bool /*vertical*/, double totalNum) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float blockRectHeight = screenSize.getHeight() * 0.003;

	/** Blocks */
	g.setColour(this->trackColor);
	for (int i = 0; i < this->blockItemTemp.size(); i++) {
		auto [start, end] = this->blockItemTemp.getUnchecked(i);
		float startPos = start / totalNum * width;
		float endPos = end / totalNum * width;

		juce::Rectangle<float> blockRect(
			startPos, 0, endPos - startPos, blockRectHeight);
		g.fillRect(blockRect);
	}
}

int MIDISourceEditor::getViewHeight() const {
	return this->vScroller->getHeight();
}

int MIDISourceEditor::getKeyNum() const {
	return 128;
}

std::tuple<double, double> MIDISourceEditor::getKeyHeightLimit() const {
	auto screenSize = utils::getScreenSize(this);
	return { screenSize.getHeight() * 0.02, screenSize.getHeight() * 0.035 };
}

void MIDISourceEditor::updateVPos(double pos, double itemSize) {
	this->piano->setPos(pos, itemSize);
	this->content->updateVPos(pos, itemSize);
}

void MIDISourceEditor::processAreaDragStart() {
	this->viewMoving = true;
	this->moveStartPosX = this->hScroller->getViewPos();
	this->moveStartPosY = this->vScroller->getViewPos();
}

void MIDISourceEditor::processAreaDragTo(int distanceX, int distanceY, bool moveX, bool moveY) {
	if (this->viewMoving) {
		if (moveX) {
			this->hScroller->setPos(this->moveStartPosX - distanceX);
		}
		if (moveY) {
			this->vScroller->setPos(this->moveStartPosY - distanceY);
		}
	}
}

void MIDISourceEditor::processAreaDragEnd() {
	this->viewMoving = false;
	this->moveStartPosX = this->moveStartPosY = 0;
}

void MIDISourceEditor::sendKeyUpDown(int noteNum, bool isDown, float vel) {
	if (this->index > -1) {
		if (isDown) {
			quickAPI::sendDirectNoteOn(this->index, noteNum, vel * UINT8_MAX);
		}
		else {
			quickAPI::sendDirectNoteOff(this->index, noteNum);
		}
	}
}

void MIDISourceEditor::updateBlockTemp() {
	/** Clear Temp */
	this->blockItemTemp.clear();

	/** Update Block Temp */
	auto list = quickAPI::getBlockList(this->index);
	for (auto [startTime, endTime, offset] : list) {
		this->blockItemTemp.add({ startTime, endTime });
	}
}

std::tuple<double, double> MIDISourceEditor::getViewArea(double pos, double itemSize) const {
	double secStart = pos / itemSize;
	double secLength = this->getViewWidth() / itemSize;
	return { secStart, secStart + secLength };
}
