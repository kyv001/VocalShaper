#include "SourceMIDITemp.h"
#include "../Utils.h"

#define MIDI_LYRICS_TYPE 0x05

void SourceMIDITemp::setData(const juce::MidiFile& data) {
	this->sourceData = data;

	this->trackNum = data.getNumTracks();
	this->timeFormat = data.getTimeFormat();
	this->update();
}

void SourceMIDITemp::addTrack(const juce::MidiMessageSequence& track) {
	this->sourceData.addTrack(track);

	this->trackNum++;
	this->update();
}

void SourceMIDITemp::update() {
	/** Clear Lists */
	this->noteList.clear();
	this->pitchWheelList.clear();
	this->afterTouchList.clear();
	this->channelPressureList.clear();
	this->controllerList.clear();
	this->miscList.clear();

	/** For Each Track */
	for (int i = 0; i < this->sourceData.getNumTracks(); i++) {
		juce::MidiMessageSequence track{ *(this->sourceData.getTrack(i)) };
		track.updateMatchedPairs(utils::regardVel0NoteAsNoteOff());
		double endTime = track.getEndTime();

		/** Track Event Temp */
		juce::Array<Note> noteTrack;
		/** Second, Lyrics */
		using LyricsItem = std::tuple<double, juce::String>;
		const LyricsItem initLyricsItem{ -1.0, "" };
		LyricsItem lastLyrics{ initLyricsItem };

		juce::Array<IntParam> pitchWheel, channelPressure;
		juce::Array<AfterTouch> afterTouch;
		std::unordered_map<uint8_t, juce::Array<Controller>> controllers;
		juce::Array<Misc> miscs;

		/** For Each Event */
		for (int i = 0; i < track.getNumEvents(); i++) {
			auto event = track.getEventPointer(i);

			/** Get Notes */
			if (event->message.isNoteOn(!utils::regardVel0NoteAsNoteOff())) {
				Note note{};
				note.channel = (uint8_t)event->message.getChannel();
				note.startSec = event->message.getTimeStamp();
				note.endSec = event->noteOffObject ? event->noteOffObject->message.getTimeStamp() : endTime;
				note.pitch = (uint8_t)event->message.getNoteNumber();
				note.vel = event->message.getVelocity();
				
				if (juce::approximatelyEqual(std::get<0>(lastLyrics), note.startSec)) {
					note.lyrics = std::get<1>(lastLyrics);
					lastLyrics = initLyricsItem;
				}

				note.noteOnEvent = i;

				noteTrack.add(note);
				continue;
			}
			/** Get Lyrics */
			if (event->message.isMetaEvent() && event->message.getMetaEventType() == MIDI_LYRICS_TYPE) {
				lastLyrics = { event->message.getTimeStamp(), event->message.getTextFromTextMetaEvent() };
				continue;
			}
			/** Pitch Wheel */
			if (event->message.isPitchWheel()) {
				IntParam param{};
				param.channel = (uint8_t)event->message.getChannel();
				param.timeSec = event->message.getTimeStamp();
				param.value = event->message.getPitchWheelValue();
				param.event = i;

				pitchWheel.add(param);
				continue;
			}
			/** After Touch */
			if (event->message.isAftertouch()) {
				AfterTouch param{};
				param.channel = (uint8_t)event->message.getChannel();
				param.timeSec = event->message.getTimeStamp();
				param.notePitch = (uint8_t)event->message.getNoteNumber();
				param.value = (uint8_t)event->message.getAfterTouchValue();
				param.event = i;

				afterTouch.add(param);
				continue;
			}
			/** Channel Pressure */
			if (event->message.isChannelPressure()) {
				IntParam param{};
				param.channel = (uint8_t)event->message.getChannel();
				param.timeSec = event->message.getTimeStamp();
				param.value = event->message.getChannelPressureValue();
				param.event = i;

				channelPressure.add(param);
				continue;
			}
			/** MIDI CC */
			if (event->message.isController()) {
				Controller controller{};
				controller.channel = (uint8_t)event->message.getChannel();
				controller.timeSec = event->message.getTimeStamp();
				controller.number = (uint8_t)event->message.getControllerNumber();
				controller.value = (uint8_t)event->message.getControllerValue();
				controller.event = i;

				controllers[controller.number].add(controller);
				continue;
			}
			/** Other exclude Lyrics */
			{
				Misc misc{};
				misc.channel = (event->message.isSysEx() || event->message.isMetaEvent())
					? 0 : (uint8_t)event->message.getChannel();
				misc.timeSec = event->message.getTimeStamp();
				misc.message = event->message;
				misc.event = i;

				miscs.add(misc);
				continue;
			}
		}

		/** Add Track to List */
		this->noteList.add(noteTrack);
		this->pitchWheelList.add(pitchWheel);
		this->afterTouchList.add(afterTouch);
		this->channelPressureList.add(channelPressure);
		this->controllerList.add(controllers);
		this->miscList.add(miscs);
	}
}

juce::MidiFile* SourceMIDITemp::getSourceData() {
	return &(this->sourceData);
}

const juce::MidiFile SourceMIDITemp::makeMIDIFile() const {
	juce::MidiFile file;
	utils::setMIDITimeFormat(file, this->timeFormat);
	
	for (int i = 0; i < this->trackNum; i++) {
		auto track = this->makeMIDITrack(i);
		file.addTrack(track);
	}

	return file;
}

const juce::MidiMessageSequence SourceMIDITemp::makeMIDITrack(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->trackNum) { return juce::MidiMessageSequence{}; }

	/** Temp */
	juce::MidiMessageSequence track;
	auto controllerNumbers = this->getControllerNumbers(index);

	/** Notes */
	if (index < this->noteList.size()) {
		auto& notes = this->noteList.getReference(index);
		for (auto& note : notes) {
			/** Lyrics */
			if (note.lyrics.isNotEmpty()) {
				auto lyricsEvent = juce::MidiMessage::textMetaEvent(
					MIDI_LYRICS_TYPE, note.lyrics);
				lyricsEvent.setTimeStamp(note.startSec);

				track.addEvent(lyricsEvent);
			}

			/** Note On Off */
			auto onEvent = juce::MidiMessage::noteOn(
				note.channel, note.pitch, note.vel);
			auto offEvent = juce::MidiMessage::noteOff(
				note.channel, note.pitch);
			onEvent.setTimeStamp(note.startSec);
			offEvent.setTimeStamp(note.endSec);

			track.addEvent(onEvent);
			track.addEvent(offEvent);
		}
	}

	/** Pitch Wheel */
	if (index < this->pitchWheelList.size()) {
		auto& pitchs = this->pitchWheelList.getReference(index);
		for (auto& pitch : pitchs) {
			auto event = juce::MidiMessage::pitchWheel(
				pitch.channel, pitch.value);
			event.setTimeStamp(pitch.timeSec);

			track.addEvent(event);
		}
	}

	/** After Touch */
	if (index < this->afterTouchList.size()) {
		auto& afterTouches = this->afterTouchList.getReference(index);
		for (auto& afterTouch : afterTouches) {
			auto event = juce::MidiMessage::aftertouchChange(
				afterTouch.channel, afterTouch.notePitch, afterTouch.value);
			event.setTimeStamp(afterTouch.timeSec);

			track.addEvent(event);
		}
	}

	/** Channel Pressure */
	if (index < this->channelPressureList.size()) {
		auto& channelPressures = this->channelPressureList.getReference(index);
		for (auto& channelPressure : channelPressures) {
			auto event = juce::MidiMessage::channelPressureChange(
				channelPressure.channel, channelPressure.value);
			event.setTimeStamp(channelPressure.timeSec);

			track.addEvent(event);
		}
	}

	/** Controller */
	if (index < this->controllerList.size()) {
		auto& controllers = this->controllerList.getReference(index);
		for (auto& controller : controllers) {
			for (auto& controllerPoint : controller.second) {
				auto event = juce::MidiMessage::controllerEvent(
					controllerPoint.channel, controllerPoint.number, controllerPoint.value);
				event.setTimeStamp(controllerPoint.timeSec);

				track.addEvent(event);
			}
		}
	}

	/** Misc */
	if (index < this->miscList.size()) {
		auto& miscs = this->miscList.getReference(index);
		for (auto& misc : miscs) {
			auto event = misc.message;
			event.setTimeStamp(misc.timeSec);

			track.addEvent(event);
		}
	}

	/** Match Note On Off */
	track.updateMatchedPairs();

	return track;
}

int SourceMIDITemp::getTrackNum() const {
	return this->trackNum;
}

int SourceMIDITemp::getNoteNum(int track) const {
	if (track < 0 || track >= this->noteList.size()) {
		return 0;
	}
	return this->noteList.getReference(track).size();
}

int SourceMIDITemp::getPitchWheelNum(int track) const {
	if (track < 0 || track >= this->pitchWheelList.size()) {
		return 0;
	}
	return this->pitchWheelList.getReference(track).size();
}

int SourceMIDITemp::getAfterTouchNum(int track) const {
	if (track < 0 || track >= this->afterTouchList.size()) {
		return 0;
	}
	return this->afterTouchList.getReference(track).size();
}

int SourceMIDITemp::getChannelPressureNum(int track) const {
	if (track < 0 || track >= this->channelPressureList.size()) {
		return 0;
	}
	return this->channelPressureList.getReference(track).size();
}

const std::set<uint8_t> SourceMIDITemp::getControllerNumbers(int track) const {
	if (track < 0 || track >= this->controllerList.size()) {
		return {};
	}

	std::set<uint8_t> result;
	auto& map = this->controllerList.getReference(track);
	for (auto& i : map) {
		result.insert(i.first);
	}

	return result;
}

int SourceMIDITemp::getControllerNum(int track, uint8_t number) const {
	if (track < 0 || track >= this->controllerList.size()) {
		return 0;
	}

	auto& map = this->controllerList.getReference(track);
	auto it = map.find(number);
	if (it == map.end()) {
		return 0;
	}

	return it->second.size();
}

int SourceMIDITemp::getMiscNum(int track) const {
	if (track < 0 || track >= this->miscList.size()) {
		return 0;
	}
	return this->miscList.getReference(track).size();
}

const SourceMIDITemp::Note SourceMIDITemp::getNote(int track, int index) const {
	if (track < 0 || track >= this->noteList.size()) {
		return {};
	}

	auto& trackRef = this->noteList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::IntParam SourceMIDITemp::getPitchWheel(int track, int index) const {
	if (track < 0 || track >= this->pitchWheelList.size()) {
		return {};
	}

	auto& trackRef = this->pitchWheelList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::AfterTouch SourceMIDITemp::getAfterTouch(int track, int index) const {
	if (track < 0 || track >= this->afterTouchList.size()) {
		return {};
	}

	auto& trackRef = this->afterTouchList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::IntParam SourceMIDITemp::getChannelPressure(int track, int index) const {
	if (track < 0 || track >= this->channelPressureList.size()) {
		return {};
	}

	auto& trackRef = this->channelPressureList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}

const SourceMIDITemp::Controller SourceMIDITemp::getController(int track, uint8_t number, int index) const {
	if (track < 0 || track >= this->controllerList.size()) {
		return {};
	}

	auto& trackRef = this->controllerList.getReference(track);
	auto it = trackRef.find(number);
	if (it == trackRef.end()) {
		return {};
	}

	if (track < 0 || track >= it->second.size()) {
		return {};
	}

	return it->second.getUnchecked(index);
}

const SourceMIDITemp::Misc SourceMIDITemp::getMisc(int track, int index) const {
	if (track < 0 || track >= this->miscList.size()) {
		return {};
	}

	auto& trackRef = this->miscList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	return trackRef.getUnchecked(index);
}
