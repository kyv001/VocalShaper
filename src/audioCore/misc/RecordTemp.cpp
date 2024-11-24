#include "RecordTemp.h"

void RecordTemp::recordData(double timeSec,
	const juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages) {
	/** TODO */
}

RecordTemp* RecordTemp::getInstance() {
	return RecordTemp::instance
		? RecordTemp::instance : (RecordTemp::instance = new RecordTemp());
}

void RecordTemp::releaseInstance() {
	if (RecordTemp::instance) {
		delete RecordTemp::instance;
		RecordTemp::instance = nullptr;
	}
}

RecordTemp* RecordTemp::instance = nullptr;
