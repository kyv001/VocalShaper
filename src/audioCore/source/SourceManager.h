﻿#pragma once

#include <JuceHeader.h>
#include "SourceItem.h"

class SourceManager final : private juce::DeletedAtShutdown {
public:
	SourceManager() = default;

	using SourceType = SourceItem::SourceType;
	uint64_t applySource(SourceType type);
	void releaseSource(uint64_t ref);

	const juce::String getFileName(uint64_t ref, SourceType type) const;
	void changed(uint64_t ref, SourceType type);
	void saved(uint64_t ref, SourceType type);
	bool isSaved(uint64_t ref, SourceType type) const;
	bool isValid(uint64_t ref, SourceType type) const;

	int getMIDITrackNum(uint64_t ref) const;
	double getLength(uint64_t ref, SourceType type) const;

	void initAudio(uint64_t ref, const juce::String& name, int channelNum, double sampleRate, double length);
	void initMIDI(uint64_t ref, const juce::String& name);
	void setAudio(uint64_t ref, double sampleRate, const juce::AudioSampleBuffer& data, const juce::String& name);
	void setMIDI(uint64_t ref, const juce::MidiFile& data, const juce::String& name);
	void setAudio(uint64_t ref, const juce::String& name);
	void setMIDI(uint64_t ref, const juce::String& name);
	const std::tuple<double, juce::AudioSampleBuffer> getAudio(uint64_t ref) const;
	const juce::MidiMessageSequence makeMIDITrack(uint64_t ref, int trackIndex) const;
	const juce::MidiFile makeMIDIFile(uint64_t ref) const;

	using AudioWriteType = SourceItem::AudioWriteType;
	using MIDIWriteType = SourceItem::MIDIWriteType;
	void writeAudio(uint64_t ref, AudioWriteType type, const juce::AudioSampleBuffer& buffer,
		double startTime, double length, double sampleRate);
	void writeMIDI(uint64_t ref, MIDIWriteType type, const juce::MidiMessageSequence& sequence,
		double startTime, double length);

	void prepareAudioPlay(uint64_t ref);
	void prepareMIDIPlay(uint64_t ref);

	using ChangedCallback = SourceItem::ChangedCallback;
	void setCallback(uint64_t ref, SourceType type,
		const ChangedCallback& callback);

	using AudioFormat = SourceItem::AudioFormat;
	void setAudioFormat(uint64_t ref, const AudioFormat& format);
	const AudioFormat getAudioFormat(uint64_t ref) const;
	double getAudioSampleRate(uint64_t ref) const;

public:
	void readAudioData(uint64_t ref, juce::AudioBuffer<float>& buffer, int bufferOffset,
		int dataOffset, int length) const;
	void readMIDIData(uint64_t ref, juce::MidiBuffer& buffer, double baseTime,
		double startTime, double endTime, int trackIndex) const;

public:
	int getMIDINoteNum(uint64_t ref, int track) const;
	int getMIDIPitchWheelNum(uint64_t ref, int track) const;
	int getMIDIAfterTouchNum(uint64_t ref, int track) const;
	int getMIDIChannelPressureNum(uint64_t ref, int track) const;
	const std::set<uint8_t> getMIDIControllerNumbers(uint64_t ref, int track) const;
	int getMIDIControllerNum(uint64_t ref, int track, uint8_t number) const;
	int getMIDIMiscNum(uint64_t ref, int track) const;

	const SourceMIDITemp::Note getMIDINote(uint64_t ref, int track, int index) const;
	const SourceMIDITemp::IntParam getMIDIPitchWheel(uint64_t ref, int track, int index) const;
	const SourceMIDITemp::AfterTouch getMIDIAfterTouch(uint64_t ref, int track, int index) const;
	const SourceMIDITemp::IntParam getMIDIChannelPressure(uint64_t ref, int track, int index) const;
	const SourceMIDITemp::Controller getMIDIController(uint64_t ref, int track, uint8_t number, int index) const;
	const SourceMIDITemp::Misc getMIDIMisc(uint64_t ref, int track, int index) const;

	const juce::Array<SourceMIDITemp::Note> getMIDINoteList(uint64_t ref, int track) const;
	const juce::Array<SourceMIDITemp::IntParam> getMIDIPitchWheelList(uint64_t ref, int track) const;
	const juce::Array<SourceMIDITemp::AfterTouch> getMIDIAfterTouchList(uint64_t ref, int track) const;
	const juce::Array<SourceMIDITemp::IntParam> getMIDIChannelPressureList(uint64_t ref, int track) const;
	const juce::Array<SourceMIDITemp::Controller> getMIDIControllerList(uint64_t ref, int track, uint8_t number) const;
	const juce::Array<SourceMIDITemp::Misc> getMIDIMiscList(uint64_t ref, int track) const;

public:
	void sampleRateChanged(double sampleRate, int blockSize);

private:
	std::map<uint64_t, std::shared_ptr<SourceItem>> sources;

	double sampleRate = 0;
	int blockSize = 0;

	SourceItem* getSource(uint64_t ref, SourceType type) const;
	SourceItem* getSourceFast(uint64_t ref, SourceType type) const;

public:
	static SourceManager* getInstance();
	static void releaseInstance();

private:
	static SourceManager* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceManager)
};
