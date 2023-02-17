﻿#pragma once

#include <JuceHeader.h>

class Track final : public juce::AudioProcessorGraph {
public:
	Track() = delete;
	Track(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());

	/**
	 * @brief	Add an audio input bus onto the track.
	 */
	void addAdditionalAudioBus();
	/**
	 * @brief	Remove the last audio input bus from the track.
	 */
	void removeAdditionalAudioBus();

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;
	juce::AudioChannelSet audioChannels;

	juce::AudioProcessorGraph::Node::Ptr pluginDockNode;

	juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::Panner<float>> gainAndPanner;
	juce::dsp::ProcessorChain<juce::dsp::Gain<float>> slider;
	bool isMute = false;

private:
	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};