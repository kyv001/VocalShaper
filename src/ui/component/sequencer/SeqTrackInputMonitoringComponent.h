#pragma once

#include <JuceHeader.h>

class SeqTrackInputMonitoringComponent final
	: public juce::Component,
	public juce::SettableTooltipClient {
public:
	SeqTrackInputMonitoringComponent();

	void paint(juce::Graphics& g) override;

	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;

	void update(int index);

private:
	int index = -1;
	bool inputMonitoring = false;

	using MIDILink = std::tuple<int, int>;
	using AudioLink = std::tuple<int, int, int, int>;

	bool midiInput = false;
	juce::Array<AudioLink> audioInput;

	void changeInputMonitoring();
	void showMenu();

	void changeMIDIInput();
	void changeAudioInput();

	juce::PopupMenu createMenu();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeqTrackInputMonitoringComponent)
};
