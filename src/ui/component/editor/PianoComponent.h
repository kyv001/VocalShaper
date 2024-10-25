#pragma once

#include <JuceHeader.h>

class PianoComponent final : public juce::Component {
public:
	PianoComponent();

	void paint(juce::Graphics& g) override;

	void setPos(double pos, double itemSize);

private:
	const juce::StringArray keyNames{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

	const std::array<int, 5> blackKeys{ 1, 3, 6, 8, 10 };
	const std::array<int, 7> whiteKeys{ 0, 2, 4, 5, 7, 9, 11 };
	const int totalKeys = 128;

	double pos = 0, itemSize = 0;
	int keyHovered = -1;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoComponent)
};
