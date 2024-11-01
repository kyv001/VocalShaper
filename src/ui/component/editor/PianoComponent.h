#pragma once

#include <JuceHeader.h>

class PianoComponent final : public juce::Component {
public:
	using WheelFunc = std::function<void(float, bool)>;
	using WheelAltFunc = std::function<void(double, double, float, bool)>;
	using KeyUpDownFunc = std::function<void(int, bool, float)>;
	PianoComponent(
		const WheelFunc& wheelFunc,
		const WheelAltFunc& wheelAltFunc,
		const KeyUpDownFunc& keyUpDownFunc);

	void resized() override;
	void paint(juce::Graphics& g) override;

	void setPos(double pos, double itemSize);

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;
	void mouseWheelMove(const juce::MouseEvent& event,
		const juce::MouseWheelDetails& wheel) override;

	void mouseYPosChangedOutside(float posY, float posX = 0);
	void mouseLeaveOutside();

private:
	const WheelFunc wheelFunc;
	const WheelAltFunc wheelAltFunc;
	const KeyUpDownFunc keyUpDownFunc;

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
	int keyHovered = -1, keyPressed = -1;

	bool showAllKeyNames = false;

	std::tuple<int, float> findCurrentKey(const juce::Point<float>& pos) const;

	void updateKeysInternal();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoComponent)
};
