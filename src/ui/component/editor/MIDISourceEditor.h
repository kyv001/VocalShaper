#pragma once

#include <JuceHeader.h>
#include "PianoComponent.h"
#include "SourceTimeRuler.h"
#include "../base/Scroller.h"

class MIDISourceEditor final : public juce::Component {
public:
	MIDISourceEditor();

	void resized() override;
	void paint(juce::Graphics& g) override;

	void update(uint64_t ref);
	void updateTempo();

private:
	uint64_t ref = 0;
	double totalLength = 0;

	std::unique_ptr<Scroller> hScroller = nullptr;
	std::unique_ptr<Scroller> vScroller = nullptr;

	std::unique_ptr<SourceTimeRuler> ruler = nullptr;
	std::unique_ptr<PianoComponent> piano = nullptr;

	bool viewMoving = false;
	double moveStartPosX = 0, moveStartPosY = 0;

	int getViewWidth() const;
	double getTimeLength() const;
	std::tuple<double, double> getTimeWidthLimit() const;
	double getPlayPos() const;

	void updateHPos(double pos, double itemSize);
	void paintNotePreview(juce::Graphics& g,
		int width, int height, bool vertical);

	int getViewHeight() const;
	int getKeyNum() const;
	std::tuple<double, double> getKeyHeightLimit() const;

	void updateVPos(double pos, double itemSize);

	void processAreaDragStart();
	void processAreaDragTo(int distanceX, int distanceY, bool moveX = true, bool moveY = true);
	void processAreaDragEnd();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDISourceEditor)
};
