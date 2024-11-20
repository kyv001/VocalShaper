#pragma once

#include <JuceHeader.h>

class SourceMIDITemp final {
public:
	SourceMIDITemp() = default;

	void setData(const juce::MidiFile& data);
	void addTrack(const juce::MidiMessageSequence& track);

	const juce::MidiFile makeMIDIFile() const;
	const juce::MidiMessageSequence makeMIDITrack(int index) const;

	struct MIDIStruct {
		virtual ~MIDIStruct() = default;

		uint8_t channel = 0;
		double timeSec = 0;

		int eventIndex = -1;
		int eventInListIndex = -1;
	};
	struct Note : public MIDIStruct {
		double endSec = 0;
		uint8_t pitch = 0;
		uint8_t vel = 0;
		juce::String lyrics;

		int eventOffIndex = -1;
	};
	struct NoteOffMarker : public MIDIStruct {
		int eventOnIndex = -1;
	};
	struct IntParam : public MIDIStruct {
		int value = 0;
	};
	struct AfterTouch : public MIDIStruct {
		uint8_t notePitch = 0;
		uint8_t value = 0;
	};
	struct Controller : public MIDIStruct {
		uint8_t number = 0;
		uint8_t value = 0;
	};
	struct Misc : public MIDIStruct {
		juce::MidiMessage message;
	};
	
	int getTrackNum() const;
	double getLength() const;

	int getNoteNum(int track) const;
	int getPitchWheelNum(int track) const;
	int getAfterTouchNum(int track) const;
	int getChannelPressureNum(int track) const;
	const std::set<uint8_t> getControllerNumbers(int track) const;
	int getControllerNum(int track, uint8_t number) const;
	int getMiscNum(int track) const;

	const Note getNote(int track, int index) const;
	const IntParam getPitchWheel(int track, int index) const;
	const AfterTouch getAfterTouch(int track, int index) const;
	const IntParam getChannelPressure(int track, int index) const;
	const Controller getController(int track, uint8_t number, int index) const;
	const Misc getMisc(int track, int index) const;

	void findMIDIMessages(
		int track, double startSec, double length,
		juce::MidiMessageSequence& list) const;
	void addMIDIMessages(
		int track, const juce::MidiMessageSequence& list);

private:
	juce::Array<juce::OwnedArray<MIDIStruct>> eventList;
	short timeFormat = 480;

	juce::Array<juce::Array<int>> noteList;

	juce::Array<juce::Array<int>> pitchWheelList;
	juce::Array<juce::Array<int>> afterTouchList;
	juce::Array<juce::Array<int>> channelPressureList;

	juce::Array<std::unordered_map<uint8_t, juce::Array<int>>> controllerList;

	juce::Array<juce::Array<int>> miscList;
};
