#include "RecordTemp.h"
#include "VMath.h"

#define AUDIO_BUFFER_MIN 48000 * 30

RecordTemp::RecordTemp() {
	/** Init Audio Buffer */
	this->clearAudio();
}

void RecordTemp::setInputSampleRate(double sampleRate) {
	juce::GenericScopedLock locker(this->lock);
	this->sampleRate = sampleRate;
	this->clearAll();
}

void RecordTemp::setInputChannelNum(int channels) {
	juce::GenericScopedLock locker(this->lock);
	this->audioBuffer.setSize(channels,
		this->audioBuffer.getNumSamples(), true, true, true);
}

void RecordTemp::recordData(double timeSec,
	const juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages) {
	/** Lock */
	juce::GenericScopedLock locker(this->lock);

	/** Set Start Time */
	if (timeSec < this->startTime) {
		this->clearAll();
	}
	if (this->startTime < 0) {
		this->startTime = timeSec;
	}

	/** Collect MIDI Message */
	if (this->recordMIDI) {
		for (auto i : midiMessages) {
			auto message = i.getMessage();
			message.setTimeStamp((timeSec - this->startTime) + message.getTimeStamp() / this->sampleRate);
			this->midiCollector.addMessageToQueue(message);
		}
	}
	
	/** Write Audio Buffer */
	if (this->recordAudio) {
		uint64_t startSample = (timeSec - this->startTime) * this->sampleRate;
		uint64_t endSample = startSample + buffer.getNumSamples();
		if (this->tryToEnsureAudioBufferSamplesAllocated(endSample)) {
			int channelNum = std::min(buffer.getNumChannels(), this->audioBuffer.getNumChannels());
			for (int i = 0; i < channelNum; i++) {
				vMath::copyAudioData(this->audioBuffer, buffer,
					(int)startSample, 0, i, i, buffer.getNumSamples());
			}
		}
	}
}

void RecordTemp::clearAll() {
	juce::GenericScopedLock locker(this->lock);
	this->clearAudio();
	this->clearMIDI();
	this->startTime = -1;
}

void RecordTemp::clearMIDI() {
	juce::GenericScopedLock locker(this->lock);
	this->midiCollector.reset(sampleRate);
}

void RecordTemp::clearAudio() {
	juce::GenericScopedLock locker(this->lock);
	this->audioBuffer.setSize(
		this->audioBuffer.getNumChannels(),
		AUDIO_BUFFER_MIN, false, true, true);
}

bool RecordTemp::tryToEnsureAudioBufferSamplesAllocated(uint64_t sampleNum) {
	while (this->audioBuffer.getNumSamples() < sampleNum) {
		uint16_t num = (uint64_t)(this->audioBuffer.getNumSamples()) * 2;
		if (num > INT_MAX) {
			this->audioBuffer.setSize(this->audioBuffer.getNumChannels(),
				INT_MAX, false, true, true);
			break;
		}

		this->audioBuffer.setSize(this->audioBuffer.getNumChannels(),
			(int)num, false, true, true);
	}

	return this->audioBuffer.getNumSamples() >= sampleNum;
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
