﻿#pragma once

#include <Defs.h>

class Sequencer final : public juce::AudioProcessorGraph {
public:
	Sequencer();

	void insertSource(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	void removeSource(int index);
	void insertInstrument(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	void removeInstrument(int index);

	int getSourceNum() const;
	juce::AudioProcessor* getSourceProcessor(int index) const;
	int getInstrumentNum() const;
	juce::AudioProcessor* getInstrumentProcessor(int index) const;

	void setMIDIInputConnection(int sourceIndex);
	void removeMIDIInputConnection(int sourceIndex);
	void setAudioInputConnection(int sourceIndex, int srcChannel, int dstChannel);
	void removeAudioInputConnection(int sourceIndex, int srcChannel, int dstChannel);
	void addMIDIInstrumentConnection(int sourceIndex, int instrIndex);
	void removeMIDIInstrumentConnection(int sourceIndex, int instrIndex);
	void setMIDISendConnection(int sourceIndex);
	void removeMIDISendConnection(int sourceIndex);
	void setAudioSendConnection(int sourceIndex, int srcChannel, int dstChannel);
	void removeAudioSendConnection(int sourceIndex, int srcChannel, int dstChannel);
	void setAudioOutputConnection(int instrIndex, int srcChannel, int dstChannel);
	void removeAudioOutputConnection(int instrIndex, int srcChannel, int dstChannel);

	bool isMIDIInputConnected(int sourceIndex) const;
	bool isAudioInputConnected(int sourceIndex, int srcChannel, int dstChannel) const;
	bool isMIDIInstrumentConnected(int sourceIndex, int instrIndex) const;
	bool isMIDISendConnected(int sourceIndex) const;
	bool isAudioSendConnected(int sourceIndex, int srcChannel, int dstChannel) const;
	bool isAudioOutputConnected(int instrIndex, int srcChannel, int dstChannel) const;

	void setAudioLayout(const juce::AudioProcessorGraph::BusesLayout& busLayout);

	void removeIllegalAudioInputConnections();
	void removeIllegalAudioSendConnections();
	void removeIllegalAudioOutputConnections();

	void setInputChannels(const juce::Array<juce::AudioChannelSet>& channels);

	void addOutputBus(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	void removeOutputBus();

private:
	juce::AudioProcessorGraph::Node::Ptr midiInputNode, midiOutputNode;
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> audioSourceNodeList;
	juce::Array<juce::AudioProcessorGraph::Node::Ptr> instrumentNodeList;

	juce::Array<juce::AudioProcessorGraph::Connection> midiInputConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioInputConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiInstrumentConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> midiSendConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioSendConnectionList;
	juce::Array<juce::AudioProcessorGraph::Connection> audioOutputConnectionList;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sequencer)
};
