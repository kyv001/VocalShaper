#pragma once

#include <JuceHeader.h>

class RecordTemp final : private juce::DeletedAtShutdown {
public:
	RecordTemp() = default;

	void recordData(double timeSec,
		const juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages);

public:
	static RecordTemp* getInstance();
	static void releaseInstance();

private:
	static RecordTemp* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordTemp)
};
