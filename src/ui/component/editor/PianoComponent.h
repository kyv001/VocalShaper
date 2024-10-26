#pragma once

#include <JuceHeader.h>

class PianoComponent final : public juce::Component {
public:
	using WheelFunc = std::function<void(float, bool)>;
	using WheelAltFunc = std::function<void(double, double, float, bool)>;
	PianoComponent(
		const WheelFunc& wheelFunc,
		const WheelAltFunc& wheelAltFunc);

	void resized() override;
	void paint(juce::Graphics& g) override;

	void setPos(double pos, double itemSize);

	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;
	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel) override;

private:
	const WheelFunc wheelFunc;
	const WheelAltFunc wheelAltFunc;

	const juce::StringArray keyNames{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

	const std::array<int, 5> blackKeys{ 1, 3, 6, 8, 10 };
	const std::array<int, 7> whiteKeys{ 0, 2, 4, 5, 7, 9, 11 };
	const int keysPerOctave = 0;
	const int totalKeys = 128;

	double pos = 0, itemSize = 0;
	float sizePerOctave = 0, sizePerWhiteKey = 0;
	int topKey = -1, bottomKey = -1;
	int startOctave = -1, endOctave = -1, octaveNum = 0;
	float startOctavePos = 0;
	int keyHovered = -1;

	bool showAllKeyNames = false;

	void updateKeysInternal();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoComponent)
};
