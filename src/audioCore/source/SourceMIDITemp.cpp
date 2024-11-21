#include "SourceMIDITemp.h"
#include "../Utils.h"

#define MIDI_LYRICS_TYPE 0x05

void SourceMIDITemp::setData(const juce::MidiFile& data) {
	/** Get Time Format */
	this->timeFormat = data.getTimeFormat();
	
	/** Clear Lists */
	this->eventList.clear();

	this->noteList.clear();
	this->pitchWheelList.clear();
	this->afterTouchList.clear();
	this->channelPressureList.clear();
	this->controllerList.clear();
	this->miscList.clear();

	/** For Each Track */
	for (int i = 0; i < data.getNumTracks(); i++) {
		this->addTrack(*(data.getTrack(i)));
	}
}

void SourceMIDITemp::addTrack(const juce::MidiMessageSequence& track) {
	juce::MidiMessageSequence trackTemp{ track };
	trackTemp.updateMatchedPairs(utils::regardVel0NoteAsNoteOff());
	double endTime = trackTemp.getEndTime();

	/** Track Event Temp */
	/** Second, Lyrics */
	using LyricsItem = std::tuple<double, juce::String>;
	const LyricsItem initLyricsItem{ -1.0, "" };
	LyricsItem lastLyrics{ initLyricsItem };
	std::unordered_map<juce::MidiMessageSequence::MidiEventHolder*, int> noteOffObjectTemp;

	juce::OwnedArray<MIDIStruct> events;

	juce::Array<int> noteTrack;
	juce::Array<int> pitchWheel, channelPressure;
	juce::Array<int> afterTouch;
	std::unordered_map<uint8_t, juce::Array<int>> controllers;
	juce::Array<int> miscs;

	/** For Each Event */
	for (int i = 0; i < trackTemp.getNumEvents(); i++) {
		auto event = trackTemp.getEventPointer(i);

		/** Get Notes */
		if (event->message.isNoteOn(!utils::regardVel0NoteAsNoteOff())) {
			auto note = std::make_unique<Note>();
			note->channel = (uint8_t)event->message.getChannel();
			note->timeSec = event->message.getTimeStamp();
			note->endSec = event->noteOffObject ? event->noteOffObject->message.getTimeStamp() : endTime;
			note->pitch = (uint8_t)event->message.getNoteNumber();
			note->vel = event->message.getVelocity();

			if (juce::approximatelyEqual(std::get<0>(lastLyrics), note->timeSec)) {
				note->lyrics = std::get<1>(lastLyrics);
				lastLyrics = initLyricsItem;
			}

			note->eventOffIndex = -1;
			if (auto offObject = event->noteOffObject) {
				noteOffObjectTemp[event->noteOffObject] = i;
			}

			note->eventIndex = i;
			note->eventInListIndex = noteTrack.size();

			events.add(std::move(note));
			noteTrack.add(i);

			continue;
		}
		/** Get Lyrics */
		if (event->message.isMetaEvent() && event->message.getMetaEventType() == MIDI_LYRICS_TYPE) {
			lastLyrics = { event->message.getTimeStamp(), event->message.getTextFromTextMetaEvent() };
			continue;
		}
		/** Note Off Marker */
		if (event->message.isNoteOff(utils::regardVel0NoteAsNoteOff())) {
			auto noteOff = std::make_unique<NoteOffMarker>();
			noteOff->channel = (uint8_t)event->message.getChannel();
			noteOff->timeSec = event->message.getTimeStamp();

			noteOff->eventOnIndex = -1;
			{
				auto tempIt = noteOffObjectTemp.find(event);
				if (tempIt != noteOffObjectTemp.end()) {
					int noteIndex = tempIt->second;
					if (noteIndex >= 0 && noteIndex < i) {
						if (auto note = dynamic_cast<Note*>(events[noteIndex])) {
							if (juce::approximatelyEqual(note->endSec, noteOff->timeSec)) {
								noteOff->eventOnIndex = noteIndex;
							}
						}
					}

					noteOffObjectTemp.erase(tempIt);
				}
			}

			noteOff->eventIndex = i;
			noteOff->eventInListIndex = -1;

			events.add(std::move(noteOff));

			continue;
		}
		/** Pitch Wheel */
		if (event->message.isPitchWheel()) {
			auto param = std::make_unique<IntParam>();
			param->channel = (uint8_t)event->message.getChannel();
			param->timeSec = event->message.getTimeStamp();
			param->value = event->message.getPitchWheelValue();

			param->eventIndex = i;
			param->eventInListIndex = pitchWheel.size();

			events.add(std::move(param));
			pitchWheel.add(i);

			continue;
		}
		/** After Touch */
		if (event->message.isAftertouch()) {
			auto param = std::make_unique<AfterTouch>();
			param->channel = (uint8_t)event->message.getChannel();
			param->timeSec = event->message.getTimeStamp();
			param->notePitch = (uint8_t)event->message.getNoteNumber();
			param->value = (uint8_t)event->message.getAfterTouchValue();

			param->eventIndex = i;
			param->eventInListIndex = afterTouch.size();

			events.add(std::move(param));
			afterTouch.add(i);

			continue;
		}
		/** Channel Pressure */
		if (event->message.isChannelPressure()) {
			auto param = std::make_unique<IntParam>();
			param->channel = (uint8_t)event->message.getChannel();
			param->timeSec = event->message.getTimeStamp();
			param->value = event->message.getChannelPressureValue();

			param->eventIndex = i;
			param->eventInListIndex = channelPressure.size();

			events.add(std::move(param));
			channelPressure.add(i);

			continue;
		}
		/** MIDI CC */
		if (event->message.isController()) {
			auto controller = std::make_unique<Controller>();
			controller->channel = (uint8_t)event->message.getChannel();
			controller->timeSec = event->message.getTimeStamp();
			controller->number = (uint8_t)event->message.getControllerNumber();
			controller->value = (uint8_t)event->message.getControllerValue();

			auto& controllerList = controllers[controller->number];

			controller->eventIndex = i;
			controller->eventInListIndex = controllerList.size();

			events.add(std::move(controller));
			controllerList.add(i);

			continue;
		}
		/** Other exclude Lyrics */
		{
			auto misc = std::make_unique<Misc>();
			misc->channel = (event->message.isSysEx() || event->message.isMetaEvent())
				? 0 : (uint8_t)event->message.getChannel();
			misc->timeSec = event->message.getTimeStamp();
			misc->message = event->message;

			misc->eventIndex = i;
			misc->eventInListIndex = miscs.size();

			events.add(std::move(misc));
			miscs.add(i);

			continue;
		}
	}

	/** Add Track to List */
	this->eventList.add(std::move(events));

	this->noteList.add(noteTrack);
	this->pitchWheelList.add(pitchWheel);
	this->afterTouchList.add(afterTouch);
	this->channelPressureList.add(channelPressure);
	this->controllerList.add(controllers);
	this->miscList.add(miscs);
}

const juce::MidiFile SourceMIDITemp::makeMIDIFile() const {
	juce::MidiFile file;
	utils::setMIDITimeFormat(file, this->timeFormat);
	
	for (int i = 0; i < this->eventList.size(); i++) {
		auto track = this->makeMIDITrack(i);
		file.addTrack(track);
	}

	return file;
}

const juce::MidiMessageSequence SourceMIDITemp::makeMIDITrack(int index) const {
	/** Check Index */
	if (index < 0 || index >= this->eventList.size()) { return juce::MidiMessageSequence{}; }

	/** Temp */
	juce::MidiMessageSequence track;

	/** Get Events */
	int indexTemp = 0;
	this->findMIDIMessages(index, 0, DBL_MAX, track, indexTemp);
	
	/** Match Note On Off */
	track.updateMatchedPairs();

	return track;
}

int SourceMIDITemp::getTrackNum() const {
	return this->eventList.size();
}

double SourceMIDITemp::getLength() const {
	double result = 0;

	for (auto& track : this->eventList) {
		if (auto ptr = track.getLast()) {
			result = std::max(result, ptr->timeSec);
		}
	}

	return result;
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
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->noteList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::Note*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::IntParam SourceMIDITemp::getPitchWheel(int track, int index) const {
	if (track < 0 || track >= this->pitchWheelList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->pitchWheelList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::IntParam*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::AfterTouch SourceMIDITemp::getAfterTouch(int track, int index) const {
	if (track < 0 || track >= this->afterTouchList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->afterTouchList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::AfterTouch*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::IntParam SourceMIDITemp::getChannelPressure(int track, int index) const {
	if (track < 0 || track >= this->channelPressureList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->channelPressureList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::IntParam*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::Controller SourceMIDITemp::getController(int track, uint8_t number, int index) const {
	if (track < 0 || track >= this->controllerList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
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

	int structIndex = it->second[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::Controller*>(
		eventTrackRef.getUnchecked(structIndex)));
}

const SourceMIDITemp::Misc SourceMIDITemp::getMisc(int track, int index) const {
	if (track < 0 || track >= this->miscList.size()) {
		return {};
	}
	if (track < 0 || track >= this->eventList.size()) {
		return {};
	}

	auto& trackRef = this->miscList.getReference(track);
	if (track < 0 || track >= trackRef.size()) {
		return {};
	}

	int structIndex = trackRef[index];
	auto& eventTrackRef = this->eventList.getReference(track);
	if (structIndex < 0 || structIndex >= eventTrackRef.size()) {
		return {};
	}

	return *(dynamic_cast<SourceMIDITemp::Misc*>(
		eventTrackRef.getUnchecked(structIndex)));
}

void SourceMIDITemp::findMIDIMessages(
	int track, double startSec, double endSec,
	juce::MidiMessageSequence& list, int& indexTemp) const {
	/** Check Track */
	if (track < 0 || track >= this->eventList.size()) { return; }
	auto& trackSeq = this->eventList.getReference(track);

	/** Check Start Index */
	if (indexTemp < 0 || indexTemp >= trackSeq.size()) {
		indexTemp = this->binarySearchStart(track, 0, trackSeq.size() - 1, startSec);
	}
	else {
		auto current = trackSeq.getUnchecked(indexTemp);
		if (current->timeSec >= startSec) {
			if (auto last = trackSeq[indexTemp - 1]) {
				if (last->timeSec >= startSec) {
					indexTemp = this->binarySearchStart(track, 0, indexTemp, startSec);
				}
			}
		}
		else {
			indexTemp = this->binarySearchStart(track, indexTemp, trackSeq.size() - 1, startSec);
		}
	}
	if (indexTemp < 0) { return; }

	/** Events */
	for (; indexTemp < trackSeq.size(); indexTemp++) {
		auto eventPtr = trackSeq.getUnchecked(indexTemp);

		/** End */
		if (eventPtr->timeSec >= endSec) { break; }

		/** Note On */
		if (auto note = dynamic_cast<Note*>(eventPtr)) {
			/** Lyrics */
			if (note->lyrics.isNotEmpty()) {
				auto lyricsEvent = juce::MidiMessage::textMetaEvent(
					MIDI_LYRICS_TYPE, note->lyrics);
				lyricsEvent.setTimeStamp(note->timeSec);

				list.addEvent(lyricsEvent);
			}

			/** Event */
			auto onEvent = juce::MidiMessage::noteOn(
				note->channel, note->pitch, note->vel);
			onEvent.setTimeStamp(note->timeSec);

			list.addEvent(onEvent);
		}
		/** Note Off */
		else if (auto noteOff = dynamic_cast<NoteOffMarker*>(eventPtr)) {
			if (auto note = dynamic_cast<Note*>(trackSeq[noteOff->eventOnIndex])) {
				/** Event */
				auto offEvent = juce::MidiMessage::noteOff(
					note->channel, note->pitch, note->vel);
				offEvent.setTimeStamp(noteOff->timeSec);

				list.addEvent(offEvent);
			}
		}
		/** Pitch Wheel */
		else if (auto pitch = dynamic_cast<IntParam*>(eventPtr)) {
			/** Event */
			auto event = juce::MidiMessage::pitchWheel(
				pitch->channel, pitch->value);
			event.setTimeStamp(pitch->timeSec);

			list.addEvent(event);
		}
		/** After Touch */
		else if (auto afterTouch = dynamic_cast<AfterTouch*>(eventPtr)) {
			/** Event */
			auto event = juce::MidiMessage::aftertouchChange(
				afterTouch->channel, afterTouch->notePitch, afterTouch->value);
			event.setTimeStamp(afterTouch->timeSec);

			list.addEvent(event);
		}
		/** Channel Pressure */
		else if (auto channelPressure = dynamic_cast<IntParam*>(eventPtr)) {
			/** Event */
			auto event = juce::MidiMessage::channelPressureChange(
				channelPressure->channel, channelPressure->value);
			event.setTimeStamp(channelPressure->timeSec);

			list.addEvent(event);
		}
		/** Controller */
		else if (auto controller = dynamic_cast<Controller*>(eventPtr)) {
			/** Event */
			auto event = juce::MidiMessage::controllerEvent(
				controller->channel, controller->number, controller->value);
			event.setTimeStamp(controller->timeSec);

			list.addEvent(event);
		}
		/** Misc */
		else if (auto misc = dynamic_cast<Misc*>(eventPtr)) {
			/** Event */
			auto event = misc->message;
			event.setTimeStamp(misc->timeSec);

			list.addEvent(event);
		}
	}
}

void SourceMIDITemp::addMIDIMessages(
	int track, const juce::MidiMessageSequence& list) {
	/** TODO */
}

int SourceMIDITemp::binarySearchStart(
	int track, int low, int high, double time) const {
	auto& trackSeq = this->eventList.getReference(track);

	while (low <= high) {
		int mid = low + (high - low) / 2;

		auto current = trackSeq[mid];
		auto next = trackSeq[mid + 1];
		if (!current) { return -1; }

		if (mid == low) {
			if (time <= current->timeSec) {
				return mid;
			}
		}
		if (mid == high) {
			if (time > current->timeSec) {
				if (next && time <= next->timeSec) {
					return mid;
				}
				return -1;
			}
		}

		if (time > current->timeSec && next && time <= next->timeSec) {
			return mid + 1;
		}
		else if (time <= current->timeSec) {
			high = mid - 1;
			continue;
		}
		else {
			low = mid + 1;
			continue;
		}
	}

	return -1;
}
