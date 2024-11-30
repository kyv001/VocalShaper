﻿#pragma once

#include <JuceHeader.h>
#include "../graph/PluginDecorator.h"
#include "../graph/SeqSourceProcessor.h"
#include "../source/SourceMIDITemp.h"
#include "../Utils.h"

namespace quickAPI {
	juce::Component* getAudioDebugger();
	juce::Component* getMidiDebugger();

	const juce::File getProjectDir();

	double getCPUUsage();
	bool getReturnToStartOnStop();
	bool getAnonymousMode();
	std::unique_ptr<juce::Component> createAudioDeviceSelector();

	std::tuple<int64_t, double> getTimeInBeat();
	double getTimeInSecond();
	std::tuple<double, double> getLoopTimeSec();
	const juce::Array<float> getAudioOutputLevel();
	bool isPlaying();
	bool isRecording();
	double getTotalLength();
	int getTempoTempIndexBySec(double timeSec);
	/** timeInSec, timeInQuarter, timeInBar, secPerQuarter, numerator, denominator */
	using TempoData = std::tuple<double, double, double, double, int, int>;
	const TempoData getTempoData(int tempIndex);
	double limitTimeSec(double timeSec, double limitLevel);
	using TrackInfo = std::tuple<juce::String, juce::String>;
	const juce::Array<TrackInfo> getMixerTrackInfos();

	const juce::StringArray getFormatQualityOptionsForExtension(const juce::String& extension);
	const juce::Array<int> getFormatPossibleBitDepthsForExtension(const juce::String& extension);
	const juce::Array<int> getFormatPossibleSampleRatesForExtension(const juce::String& extension);
	const juce::StringArray getFormatPossibleMetaKeyForExtension(const juce::String& extension);

	const juce::StringArray getAudioFormatsSupported(bool isWrite);
	const juce::StringArray getMidiFormatsSupported(bool isWrite);
	const juce::StringArray getProjectFormatsSupported(bool isWrite);
	const juce::StringArray getPluginFormatsSupported(bool isWrite);

	int getFormatBitsPerSample(const juce::String& extension);
	const juce::StringPairArray getFormatMetaData(const juce::String& extension);
	int getFormatQualityOptionIndex(const juce::String& extension);

	using AudioFormatInfo = utils::AudioFormatInfo;
	const AudioFormatInfo getAudioFormatData(const juce::String& path);

	const juce::Array<double> getSampleRateSupported();
	const juce::Array<double> getSourceSampleRateSupported();
	double getCurrentSampleRate();
	double getCurrentBufferSize();

	const juce::StringArray getPluginBlackList();
	const juce::StringArray getPluginSearchPath();

	int getSIMDLevel();
	const juce::String getSIMDInsName();
	const juce::StringArray getAllSIMDInsName();

	const std::tuple<bool, juce::Array<juce::PluginDescription>>
		getPluginList(bool filter = false, bool instr = true);

	/** ID, Name */
	using TrackType = std::tuple<int, juce::String>;
	const juce::Array<TrackType> createAllTrackTypeWithName();
	const juce::Array<TrackType> getAllTrackTypeWithName();

	using PluginHolder = PluginDecorator::SafePointer;
	using EditorPointer = juce::Component::SafePointer<juce::AudioProcessorEditor>;

	/** Param Index，CC Channel  */
	using PluginParamLink = std::tuple<int, int>;

	/** Src Index, Dst Index */
	using MIDILink = std::tuple<int, int>;

	/** Src Index, Src Channel, Dst Index, Dst Channel */
	using AudioLink = std::tuple<int, int, int, int>;

	const juce::String getPluginIdentifier(PluginHolder pointer);

	PluginHolder getInstrPointer(int index);
	bool isInstrValid(int index);
	const juce::String getInstrName(int index);
	bool getInstrBypass(int index);
	bool getInstrOffline(int index);
	EditorPointer getInstrEditor(int index);
	const juce::String getInstrName(PluginHolder pointer);
	bool getInstrBypass(PluginHolder pointer);
	EditorPointer getInstrEditor(PluginHolder pointer);
	EditorPointer getInstrEditorExists(PluginHolder pointer);
	int getInstrMIDIChannel(PluginHolder pointer);
	bool getInstrMIDICCIntercept(PluginHolder pointer);
	bool getInstrMIDIOutput(PluginHolder pointer);
	const juce::Array<PluginParamLink> getInstrParamCCLink(PluginHolder pointer);
	const juce::String getInstrParamName(PluginHolder pointer, int paramIndex);
	const juce::StringArray getInstrParamList(PluginHolder pointer);

	int getEffectNum(int trackIndex);
	PluginHolder getEffectPointer(int trackIndex, int index);
	const juce::String getEffectName(int trackIndex, int index);
	bool getEffectBypass(int trackIndex, int index);
	const juce::String getEffectName(PluginHolder pointer);
	bool getEffectBypass(PluginHolder pointer);
	EditorPointer getEffectEditor(PluginHolder pointer);
	EditorPointer getEffectEditorExists(PluginHolder pointer);
	int getEffectMIDIChannel(PluginHolder pointer);
	bool getEffectMIDICCIntercept(PluginHolder pointer);
	bool getEffectMIDIOutput(PluginHolder pointer);
	const juce::Array<PluginParamLink> getEffectParamCCLink(PluginHolder pointer);
	const juce::String getEffectParamName(PluginHolder pointer, int paramIndex);
	const juce::StringArray getEffectParamList(PluginHolder pointer);
	const juce::AudioChannelSet getEffectChannelSet(int trackIndex, int index);
	int getEffectInputChannelNum(int trackIndex, int index);

	using RecordState = SeqSourceProcessor::RecordState;
	int getSeqTrackNum();
	const juce::String getSeqTrackName(int index);
	const juce::Colour getSeqTrackColor(int index);
	const juce::StringArray getSeqTrackNameList();
	const juce::AudioChannelSet getSeqTrackChannelSet(int index);
	int getSeqTrackInputChannelNum(int index);
	int getSeqTrackOutputChannelNum(int index);
	bool getSeqTrackMIDIInputFromDevice(int index);
	const juce::Array<AudioLink> getSeqTrackAudioInputFromDevice(int index);
	const juce::Array<MIDILink> getSeqTrackMIDIOutputToMixer(int index);
	const juce::Array<AudioLink> getSeqTrackAudioOutputToMixer(int index);
	bool getSeqTrackMute(int index);
	bool getSeqTrackInputMonitoring(int index);
	RecordState getSeqTrackRecording(int index);
	const juce::Array<float> getSeqTrackOutputLevel(int index);
	const juce::String getSeqTrackType(int index);
	bool isSeqTrackHasAudioData(int index);
	bool isSeqTrackHasMIDIData(int index);
	const juce::String getSeqTrackDataRefAudio(int index);
	const juce::String getSeqTrackDataRefMIDI(int index);
	const std::tuple<double, juce::AudioSampleBuffer> getSeqTrackAudioData(int index);
	//const juce::MidiMessageSequence getSeqTrackMIDIData(int index);
	int getSeqTrackMIDITrackNum(int index);
	int getSeqTrackCurrentMIDITrack(int index);
	bool isSeqTrackAudioRef(int index, uint64_t ref);
	bool isSeqTrackMIDIRef(int index, uint64_t ref);
	uint64_t getSeqTrackAudioRef(int index);
	uint64_t getSeqTrackMIDIRef(int index);

	int getMixerTrackNum();
	const juce::String getMixerTrackName(int index);
	const juce::StringArray getMixerTrackNameList();
	const juce::Colour getMixerTrackColor(int index);
	const juce::AudioChannelSet getMixerTrackChannelSet(int index);
	int getMixerTrackInputChannelNum(int index);
	int getMixerTrackOutputChannelNum(int index);
	int getMixerTrackSideChainBusNum(int index);
	bool getMixerTrackMIDIInputFromDevice(int index);
	const juce::Array<MIDILink> getMixerTrackMIDIInputFromSource(int index);
	const juce::Array<AudioLink> getMixerTrackAudioInputFromDevice(int index);
	const juce::Array<AudioLink> getMixerTrackAudioInputFromSource(int index);
	const juce::Array<AudioLink> getMixerTrackAudioInputFromSend(int index);
	bool getMixerTrackMIDIOutputToDevice(int index);
	const juce::Array<AudioLink> getMixerTrackAudioOutputToDevice(int index);
	const juce::Array<AudioLink> getMixerTrackAudioOutputToSend(int index);
	float getMixerTrackGain(int index);
	float getMixerTrackPan(int index);
	float getMixerTrackFader(int index);
	bool getMixerTrackMute(int index);
	bool isMixerTrackPanValid(int index);
	const juce::Array<float> getMixerTrackOutputLevel(int index);
	const juce::String getMixerTrackType(int index);

	int getLabelNum();
	bool isLabelTempo(int index);
	double getLabelTime(int index);
	double getLabelTempo(int index);
	std::tuple<int, int> getLabelBeat(int index);
	/** timeInSec, tempo, numerator, denominator, isTempo */
	using TempoLabelData = std::tuple<double, double, int, int, bool>;
	const juce::Array<TempoLabelData> getLabelDataList();

	/** StartTime, EndTime, Offset */
	using SeqBlock = std::tuple<double, double, double>;
	int getBlockNum(int trackIndex);
	const juce::Array<SeqBlock> getBlockList(int trackIndex);
	const SeqBlock getBlock(int trackIndex, int index);

	const juce::String getAudioDeviceName(bool isInput);
	int getAudioDeviceChannelNum(bool isInput);

	const juce::String getMIDICCChannelName(int channel);
	const juce::StringArray getMIDICCChannelNameList();

	using Note = SourceMIDITemp::Note;
	using NoteList = juce::Array<Note>;

	double getAudioSourceLength(uint64_t ref);
	double getMIDISourceLength(uint64_t ref);
	const juce::String getAudioSourceName(uint64_t ref);
	const juce::String getMIDISourceName(uint64_t ref);
	bool isAudioSourceValid(uint64_t ref);
	bool isMIDISourceValid(uint64_t ref);
	const NoteList getMIDISourceNotes(uint64_t ref, int track);
}