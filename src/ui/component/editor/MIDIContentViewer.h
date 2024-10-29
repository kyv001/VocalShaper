#pragma once

#include <JuceHeader.h>

class MIDIContentViewer final : public juce::Component {
public:
	MIDIContentViewer();

	void updateTempoLabel();

	void updateHPos(double pos, double itemSize);
	void updateVPos(double pos, double itemSize);

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	/** 0 for white key and 1 for black key */
	const std::array<int, 12> keyMasks{ 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
	const int totalKeys = 128;

	double hPos = 0, hItemSize = 0;
	double secStart = 0, secEnd = 0;

	double vPos = 0, vItemSize = 0;
	double keyTop = 0, keyBottom = 0;

	/** Place, IsBar, barId */
	using LineItem = std::tuple<double, bool, int>;
	juce::Array<LineItem> lineTemp;
	double minInterval = 0;
	std::unique_ptr<juce::Image> rulerTemp = nullptr;

	std::unique_ptr<juce::Image> keyTemp = nullptr;

	void updateKeyImageTemp();
	void updateRulerImageTemp();
	void updateDataImageTemp();

	std::tuple<double, double> getHViewArea(double pos, double itemSize) const;
	std::tuple<double, double> getVViewArea(double pos, double itemSize) const;

	/** Line List, Min Interval */
	const std::tuple<juce::Array<LineItem>, double> createRulerLine(double pos, double itemSize) const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIContentViewer)
};
