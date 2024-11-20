#include "MIDISourceEditor.h"
#include "../../misc/Tools.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"
#include <IconManager.h>

#define MIDI_TAIL_SEC 10;

MIDISourceEditor::MIDISourceEditor() {
	/** Icons */
	this->menuIcon = flowUI::IconManager::getSVG(
		utils::getIconFile("Arrows", "arrow-drop-down-fill").getFullPathName());
	this->menuIcon->replaceColour(juce::Colours::black,
		this->getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOffId));

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

	/** Button */
	this->menuButton = std::make_unique<juce::DrawableButton>(
		TRANS("Menu"), juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
	this->menuButton->setImages(this->menuIcon.get());
	this->menuButton->setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
	this->menuButton->setWantsKeyboardFocus(false);
	this->menuButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->menuButton->onClick = [this] { this->menuButtonClicked(); };
	this->addAndMakeVisible(this->menuButton.get());

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
		[comp = MIDISourceEditor::SafePointer(this)] {
			if (comp) {
				comp->processAreaDragStart();
			}
		},
		[comp = MIDISourceEditor::SafePointer(this)]
		(int distanceX, int distanceY, bool moveX, bool moveY) {
			if (comp) {
				comp->processAreaDragTo(
					distanceX, distanceY, moveX, moveY);
			}
		},
		[comp = MIDISourceEditor::SafePointer(this)] {
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
		(double delta) {
			if (comp) {
				comp->scroll(delta);
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
		},
		[comp = MIDISourceEditor::SafePointer(this)] {
			if (comp) {
				comp->processAreaDragStart();
			}
		},
		[comp = MIDISourceEditor::SafePointer(this)]
		(int distanceX, int distanceY, bool moveX, bool moveY) {
			if (comp) {
				comp->processAreaDragTo(
					distanceX, distanceY, moveX, moveY);
			}
		},
		[comp = MIDISourceEditor::SafePointer(this)] {
			if (comp) {
				comp->processAreaDragEnd();
			}
		});
	this->addAndMakeVisible(this->content.get());

	/** Init Image Temp */
	this->midiScrollerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB, 1, 1, false);

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

	int menuButtonHeight = screenSize.getHeight() * 0.025;
	int menuButtonPaddingWidth = screenSize.getWidth() * 0.005;
	int menuButtonPaddingHeight = screenSize.getHeight() * 0.005;

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

	/** Menu Button */
	juce::Rectangle<int> menuRect(
		pianoWidth - menuButtonPaddingWidth - menuButtonHeight,
		rulerHeight - menuButtonPaddingHeight - menuButtonHeight,
		menuButtonHeight, menuButtonHeight);
	this->menuButton->setBounds(menuRect);

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

	/** Image Temp */
	this->midiScrollerTemp = std::make_unique<juce::Image>(
		juce::Image::ARGB,
		std::max(hScrollerRect.getWidth(), 1),
		std::max(hScrollerRect.getHeight(), 1), false);
	this->updateMIDIScrollerImageTemp();

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

	/** Update Block Temp */
	this->updateBlocks();
}

void MIDISourceEditor::updateBlocks() {
	/** Total Length */
	this->totalLength = quickAPI::getTotalLength() + MIDI_TAIL_SEC;

	/** Update Block Temp */
	this->updateBlockTemp();

	/** Update Data */
	this->updateData();

	/** Update Content */
	this->content->updateBlocks();

	/** Update View Pos */
	this->vScroller->update();
	this->hScroller->update();
}

void MIDISourceEditor::updateData() {
	/** Total Length */
	this->totalLength = quickAPI::getTotalLength() + MIDI_TAIL_SEC;

	/** Update Note Temp */
	this->updateNoteTemp();

	/** Update Content */
	this->content->updateData();

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
		auto [start, end, sourceStart] = this->blockItemTemp.getUnchecked(i);
		float startPos = start / totalNum * width;
		float endPos = end / totalNum * width;

		juce::Rectangle<float> blockRect(
			startPos, 0, endPos - startPos, blockRectHeight);
		g.fillRect(blockRect);
	}

	/** Notes */
	if (this->midiScrollerTemp) {
		g.drawImageAt(*(this->midiScrollerTemp.get()), 0, 0);
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

	/** Update Image Temp */
	this->updateMIDIScrollerImageTemp();
}

void MIDISourceEditor::updateNoteTemp() {
	/** Clear Temp */
	this->midiDataTemp.clear();

	/** Update Note Temp */
	if (this->index >= 0 && this->ref != 0) {
		int currentMIDITrack = quickAPI::getSeqTrackCurrentMIDITrack(this->index);
		auto midiNoteList = quickAPI::getMIDISourceNotes(this->ref, currentMIDITrack);

		/** Add Each Note */
		this->midiDataTemp.ensureStorageAllocated(midiNoteList.size());
		for (auto& note : midiNoteList) {
			this->midiDataTemp.add({ note.timeSec, note.endSec, note.pitch });
		}
	}

	/** Update Note Zone Temp */
	{
		uint8_t minNote = 127, maxNote = 0;
		for (auto& [start, end, num] : this->midiDataTemp) {
			minNote = std::min(minNote, num);
			maxNote = std::max(maxNote, num);
		}
		if (maxNote < minNote) { maxNote = minNote = 0; }
		this->midiMinNote = minNote;
		this->midiMaxNote = maxNote;
	}

	/** Update Image Temp */
	this->updateMIDIScrollerImageTemp();
}

void MIDISourceEditor::updateMIDIScrollerImageTemp() {
	/** Clear Temp */
	this->midiScrollerTemp->clear(this->midiScrollerTemp->getBounds());
	juce::Graphics g(*(this->midiScrollerTemp.get()));

	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float paddingHeight = screenSize.getHeight() * 0.003;
	float noteMaxHeight = screenSize.getHeight() * 0.015;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour noteColor = laf.findColour(
		juce::Label::ColourIds::textWhenEditingColourId);

	/** Actual Total Length */
	double totalLength = ScrollerBase::limitItemNum(
		this->hScroller->getItemNum(), this->getViewWidth(),
		std::get<0>(this->getTimeWidthLimit()));
	juce::Rectangle<float> paintableArea = this->midiScrollerTemp->getBounds()
		.toFloat().withTrimmedTop(paddingHeight).withTrimmedBottom(paddingHeight);

	/** Limit Note Height */
	double minNoteID = this->midiMinNote, maxNoteID = this->midiMaxNote;
	float noteHeight = paintableArea.getHeight() / (maxNoteID - minNoteID + 1);
	if (noteHeight > noteMaxHeight) {
		noteHeight = noteMaxHeight;

		double centerNoteID = minNoteID + (maxNoteID - minNoteID) / 2;
		minNoteID = centerNoteID - ((paintableArea.getHeight() / noteHeight + 1) / 2 - 1);
		maxNoteID = centerNoteID + ((paintableArea.getHeight() / noteHeight + 1) / 2 - 1);
	}

	/** Paint Each Block */
	g.setColour(noteColor);
	int startNoteIndexTemp = 0;
	for (int i = 0; i < this->blockItemTemp.size(); i++) {
		auto [blockStart, blockEnd, sourceStart] = this->blockItemTemp.getUnchecked(i);

		/** Skip Notes Before Block */
		for (; startNoteIndexTemp < this->midiDataTemp.size(); startNoteIndexTemp++) {
			auto [start, end, num] = this->midiDataTemp.getUnchecked(startNoteIndexTemp);
			if (start >= blockStart) { break; }
		}

		/** Paint Each Note */
		for (int j = startNoteIndexTemp; j < this->midiDataTemp.size(); j++) {
			auto [start, end, num] = this->midiDataTemp.getUnchecked(j);
			if (start >= blockEnd) { break; }

			/** Get Time */
			double startInSeq = blockStart + (start - sourceStart);
			double endInSeq = std::min(blockStart + (end - sourceStart), blockEnd);

			/** Note Rect */
			float startPosX = startInSeq / totalLength * paintableArea.getWidth();
			float endPosX = endInSeq / totalLength * paintableArea.getWidth();
			juce::Rectangle<float> noteRect(
				startPosX, paintableArea.getY() + (maxNoteID - num) * noteHeight,
				endPosX - startPosX, noteHeight);
			g.fillRect(noteRect);
		}
	}
}

enum MIDIEditorMenuActionType {
	MIDIChannelBase = 0, AdsorbBase = 0x20
};

void MIDISourceEditor::menuButtonClicked() {
	auto menu = this->createMenu();
	int result = menu.showAt(this->menuButton.get());
	if (result == 0) { return; }

	if (result > MIDIEditorMenuActionType::MIDIChannelBase
		&& result < MIDIEditorMenuActionType::AdsorbBase) {
		Tools::getInstance()->setMIDIChannel((uint8_t)(result - MIDIEditorMenuActionType::MIDIChannelBase));
	}
	else if (result == MIDIEditorMenuActionType::AdsorbBase) {
		Tools::getInstance()->setAdsorb(0);
	}
	else if (result > MIDIEditorMenuActionType::AdsorbBase) {
		Tools::getInstance()->setAdsorb(1 / (double)(result - MIDIEditorMenuActionType::AdsorbBase));
	}
}

juce::PopupMenu MIDISourceEditor::createMenu() {
	juce::PopupMenu menu;

	menu.addSubMenu(TRANS("Adsorb"), this->createAdsorbMenu());
	menu.addSubMenu(TRANS("MIDI Channel"), this->createMIDIChannelMenu());

	return menu;
}

juce::PopupMenu MIDISourceEditor::createAdsorbMenu() {
	double currentAdsorb = Tools::getInstance()->getAdsorb();

	juce::PopupMenu menu;
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 1, "1", true,
		juce::approximatelyEqual(currentAdsorb, 1.0));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 2, "1/2",
		true, juce::approximatelyEqual(currentAdsorb, 1 / (double)2));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 4, "1/4",
		true, juce::approximatelyEqual(currentAdsorb, 1 / (double)4));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 6, "1/6",
		true, juce::approximatelyEqual(currentAdsorb, 1 / (double)6));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 8, "1/8",
		true, juce::approximatelyEqual(currentAdsorb, 1 / (double)8));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 12, "1/12",
		true, juce::approximatelyEqual(currentAdsorb, 1 / (double)12));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 16, "1/16",
		true, juce::approximatelyEqual(currentAdsorb, 1 / (double)16));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 24, "1/24",
		true, juce::approximatelyEqual(currentAdsorb, 1 / (double)24));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 32, "1/32",
		true, juce::approximatelyEqual(currentAdsorb, 1 / (double)32));
	menu.addItem(MIDIEditorMenuActionType::AdsorbBase + 0, "Off",
		true, juce::approximatelyEqual(currentAdsorb, 0.0));

	return menu;
}

juce::PopupMenu MIDISourceEditor::createMIDIChannelMenu() {
	uint8_t midiChannel = Tools::getInstance()->getMIDIChannel();

	juce::PopupMenu menu;
	for (int i = 1; i <= 16; i++) {
		menu.addItem(MIDIEditorMenuActionType::MIDIChannelBase + i,
			juce::String{ i }, true, midiChannel == i);
	}

	return menu;
}

std::tuple<double, double> MIDISourceEditor::getViewArea(double pos, double itemSize) const {
	double secStart = pos / itemSize;
	double secLength = this->getViewWidth() / itemSize;
	return { secStart, secStart + secLength };
}
