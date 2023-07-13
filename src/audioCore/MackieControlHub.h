#pragma once
#include <JuceHeader.h>

class MackieInputCallback;

class MackieControlHub final : public juce::MidiInputCallback {
public:
	MackieControlHub() = default;
	~MackieControlHub();

	juce::MidiInput* openInputDevice(const juce::String& deviceIdentifier, int index = 0);
	juce::MidiOutput* openOutputDevice(const juce::String& deviceIdentifier, int index = 0);
	bool closeInputDevice(const juce::String& deviceIdentifier);
	bool closeOutputDevice(const juce::String& deviceIdentifier);

	int findInputDevice(const juce::String& deviceIdentifier) const;
	int findOutputDevice(const juce::String& deviceIdentifier) const;
	juce::MidiInput* getInputDevice(int index) const;
	juce::MidiOutput* getOutputDevice(int index) const;
	int getInputDeviceIndex(int index) const;
	int getOutputDeviceIndex(int index) const;
	bool setInputDeviceIndex(int index, int deviceIndex);
	bool setOutputDeviceIndex(int index, int deviceIndex);

	void removeUnavailableDevices(
		const juce::Array<juce::MidiDeviceInfo>& inputDevices,
		const juce::Array<juce::MidiDeviceInfo>& outputDeivces);

private:
	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;

private:
	juce::Array<std::tuple<std::unique_ptr<juce::MidiInput>, int>> inputDevices;
	juce::Array<std::tuple<std::unique_ptr<juce::MidiOutput>, int>> outputDevices;
	juce::ReadWriteLock deviceListLock;

	void processInputMessage(juce::MidiInput* source, const juce::MidiMessage& message);
	friend class MackieInputCallback;

	JUCE_DECLARE_WEAK_REFERENCEABLE(MackieControlHub)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MackieControlHub)
};
