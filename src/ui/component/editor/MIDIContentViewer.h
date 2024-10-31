#pragma once

#include <JuceHeader.h>
#include "../../misc/LevelMeterHub.h"

class MIDIContentViewer final
	: public juce::Component,
	public LevelMeterHub::Target {
public:
	using WheelFunc = std::function<void(float, bool)>;
	using WheelAltFunc = std::function<void(double, double, float, bool)>;
	MIDIContentViewer(
		const WheelFunc& wheelFunc,
		const WheelAltFunc& wheelAltFunc);

	void update(int index, uint64_t ref);
	void updateTempoLabel();
	void updateBlocks();
	void updateLevelMeter() override;

	void updateHPos(double pos, double itemSize);
	void updateVPos(double pos, double itemSize);

	void resized() override;
	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;

	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel) override;

private:
	const WheelFunc wheelFunc;
	const WheelAltFunc wheelAltFunc;

	/** 0 for white key and 1 for black key */
	const std::array<int, 12> keyMasks{ 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
	const int totalKeys = 128;

	int index = -1;
	uint64_t ref = 0;

	double hPos = 0, hItemSize = 0;
	double secStart = 0, secEnd = 0;

	double vPos = 0, vItemSize = 0;
	double keyTop = 0, keyBottom = 0;

	double playPosSec = 0;
	double loopStartSec = 0, loopEndSec = 0;

	/** Place, IsBar, barId */
	using LineItem = std::tuple<double, bool, int>;
	juce::Array<LineItem> lineTemp;
	double minInterval = 0;

	/** Start, End */
	using BlockItem = std::tuple<double, double>;
	juce::Array<BlockItem> blockItemTemp;

	std::unique_ptr<juce::Image> rulerTemp = nullptr;
	std::unique_ptr<juce::Image> keyTemp = nullptr;
	std::unique_ptr<juce::Image> blockTemp = nullptr;

	void updateKeyImageTemp();
	void updateRulerImageTemp();
	void updateBlockImageTemp();
	void updateDataImageTemp();

	std::tuple<double, double> getHViewArea(double pos, double itemSize) const;
	std::tuple<double, double> getVViewArea(double pos, double itemSize) const;

	/** Line List, Min Interval */
	const std::tuple<juce::Array<LineItem>, double> createRulerLine(double pos, double itemSize) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIContentViewer)
};
