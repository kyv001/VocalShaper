#include "SourceRecordProcessor.h"
#include "MainGraph.h"
#include "../uiCallback/UICallback.h"

SourceRecordProcessor::SourceRecordProcessor(MainGraph* parent)
	: parent(parent) {}

SourceRecordProcessor::~SourceRecordProcessor() {}

void SourceRecordProcessor::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);
}

void SourceRecordProcessor::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	/** Check Play State */
	auto playHead = this->getPlayHead();
	if (!playHead) { return; }
	auto playPosition = playHead->getPosition();
	if (!playPosition->getIsPlaying() || !playPosition->getIsRecording()) { return; }
	int timeInSamples = playPosition->getTimeInSamples().orFallback(0);

	/** TODO Record Data Temp */

	/** TODO Callback */
	/*if (trackIndexList.size() > 0 && buffer.getNumSamples() > 0) {
		this->limitedCall.call([trackIndexList] {
			UICallbackAPI<const std::set<int>&>::invoke(
				UICallbackType::SourceRecord, trackIndexList);
			}, 500 / (1000 / (this->getSampleRate() / buffer.getNumSamples())), 500);
	}*/
}

double SourceRecordProcessor::getTailLengthSeconds() const {
	if (auto playHead = this->getPlayHead()) {
		return playHead->getPosition()->getTimeInSeconds().orFallback(0);
	}
	return 0;
}
