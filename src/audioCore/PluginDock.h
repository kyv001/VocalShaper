﻿#pragma once

#include <JuceHeader.h>

class PluginDock final : public juce::AudioProcessorGraph {
public:
	PluginDock() = delete;
	PluginDock(const juce::AudioChannelSet& type = juce::AudioChannelSet::stereo());
	~PluginDock() override;
	
	/**
	 * @brief	Insert a plugin onto the plugin dock.
	 */
	bool insertPlugin(std::unique_ptr<juce::AudioProcessor> processor, int index = -1);
	/**
	 * @brief	Remove a plugin from the plugin dock.
	 */
	void removePlugin(int index);

	int getPluginNum() const;
	juce::AudioPluginInstance* getPluginProcessor(int index) const;
	void setPluginBypass(int index, bool bypass);

	/**
	 * @brief	Add an audio input bus onto the plugin dock.
	 */
	bool addAdditionalAudioBus();
	/**
	 * @brief	Remove the last audio input bus from the plugin dock.
	 */
	bool removeAdditionalAudioBus();

	using PluginState = std::tuple<juce::String, bool>;
	using PluginStateList = juce::Array<PluginState>;
	PluginStateList getPluginList() const;

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;

private:
	juce::AudioProcessorGraph::Node::Ptr audioInputNode, audioOutputNode;
	juce::AudioProcessorGraph::Node::Ptr midiInputNode;
	juce::AudioChannelSet audioChannels;

	juce::Array<juce::AudioProcessorGraph::Node::Ptr> pluginNodeList;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginDock)
};
