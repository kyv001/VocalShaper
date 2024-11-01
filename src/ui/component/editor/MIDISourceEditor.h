#pragma once

#include <JuceHeader.h>
#include "PianoComponent.h"
#include "SourceTimeRuler.h"
#include "MIDIContentViewer.h"
#include "../base/Scroller.h"
#include "../../misc/LevelMeterHub.h"

class MIDISourceEditor final
	: public juce::Component,
	public LevelMeterHub::Target {
public:
	MIDISourceEditor();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(int index, uint64_t ref);
	void updateTempo();
	void updateBlocks();
	void updateLevelMeter() override;

private:
	int index = -1;
	uint64_t ref = 0;
	double totalLength = 0;

	std::unique_ptr<Scroller> hScroller = nullptr;
	std::unique_ptr<Scroller> vScroller = nullptr;

	std::unique_ptr<SourceTimeRuler> ruler = nullptr;
	std::unique_ptr<PianoComponent> piano = nullptr;
	std::unique_ptr<MIDIContentViewer> content = nullptr;

	bool viewMoving = false;
	double moveStartPosX = 0, moveStartPosY = 0;

	double pos = 0, itemSize = 0;
	double secStart = 0, secEnd = 0;
	double playPosSec = 0;

	/** Start, End */
	using BlockItem = std::tuple<double, double>;
	juce::Array<BlockItem> blockItemTemp;

	juce::Colour trackColor;

	int getViewWidth() const;
	double getTimeLength() const;
	std::tuple<double, double> getTimeWidthLimit() const;
	double getPlayPos() const;

	void updateHPos(double pos, double itemSize);
	void paintNotePreview(juce::Graphics& g,
		int width, int height, bool vertical, double totalNum);

	int getViewHeight() const;
	int getKeyNum() const;
	std::tuple<double, double> getKeyHeightLimit() const;

	void updateVPos(double pos, double itemSize);

	void processAreaDragStart();
	void processAreaDragTo(int distanceX, int distanceY, bool moveX = true, bool moveY = true);
	void processAreaDragEnd();

	void sendKeyUpDown(int noteNum, bool isDown, float vel);

	void updateBlockTemp();

	std::tuple<double, double> getViewArea(double pos, double itemSize) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDISourceEditor)
};
