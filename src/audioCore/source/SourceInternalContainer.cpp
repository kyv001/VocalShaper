﻿#include "SourceInternalContainer.h"
#include "../misc/VMath.h"

SourceInternalContainer::SourceInternalContainer(
	const SourceType type, const juce::String& name)
	: type(type), name(name) {}

SourceInternalContainer::SourceInternalContainer(
	const SourceInternalContainer& other)
	: type(other.type), name(SourceInternalContainer::getForkName(other.name)), forked(true) {
	if (&other != this) {
		if (other.midiData) {
			this->midiData = std::make_unique<SourceMIDITemp>();
			this->midiData->setData(*(other.midiData->getSourceData()));
		}
		if (other.audioData) {
			this->audioData = std::make_unique<juce::AudioSampleBuffer>(*(other.audioData));
		}

		this->audioSampleRate = other.audioSampleRate;
		this->savedFlag = false;

		this->format = other.format;
		this->metaData = other.metaData;
		this->bitsPerSample = other.bitsPerSample;
		this->quality = other.quality;
	}
}

SourceInternalContainer::SourceType SourceInternalContainer::getType() const {
	return this->type;
}

const juce::String SourceInternalContainer::getName() const {
	return this->name;
}

juce::MidiFile* SourceInternalContainer::getMidiData() const {
	if (this->midiData) {
		return this->midiData->getSourceData();
	}
	return nullptr;
}

juce::AudioSampleBuffer* SourceInternalContainer::getAudioData() const {
	return this->audioData.get();
}

double SourceInternalContainer::getAudioSampleRate() const {
	return this->audioSampleRate;
}

void SourceInternalContainer::changed() {
	this->savedFlag = false;
}

void SourceInternalContainer::saved() {
	this->savedFlag = true;
}

bool SourceInternalContainer::isSaved() const {
	return this->savedFlag;
}

const juce::String SourceInternalContainer::getFormat() const {
	return this->format;
}

const juce::StringPairArray SourceInternalContainer::getMetaData() const {
	return this->metaData;
}

int SourceInternalContainer::getBitsPerSample() const {
	return this->bitsPerSample;
}

int SourceInternalContainer::getQuality() const {
	return this->quality;
}

void SourceInternalContainer::initMidiData() {
	if (this->type == SourceType::MIDI) {
		if (!this->midiData) {
			this->midiData = std::make_unique<SourceMIDITemp>();
		}

		this->midiData->addTrack(juce::MidiMessageSequence{});

		this->changed();
	}
}

void SourceInternalContainer::initAudioData(
	int channelNum, double sampleRate, double length) {
	if (this->type == SourceType::Audio) {
		this->audioData = std::make_unique<juce::AudioSampleBuffer>(
			channelNum, (int)std::ceil(length * sampleRate));
		vMath::zeroAllAudioData(*(this->audioData.get()));
		this->audioSampleRate = sampleRate;

		this->initAudioFormat();

		this->changed();
	}
}

void SourceInternalContainer::setMIDI(const juce::MidiFile& data) {
	if (this->type == SourceType::MIDI) {
		if (!this->midiData) {
			this->midiData = std::make_unique<SourceMIDITemp>();
		}
		this->midiData->setData(data);

		this->changed();
	}
}

void SourceInternalContainer::setAudio(
	double sampleRate, const juce::AudioSampleBuffer& data) {
	if (this->type == SourceType::Audio) {
		this->audioData = std::make_unique<juce::AudioSampleBuffer>(data);
		this->audioSampleRate = sampleRate;

		this->changed();
	}
}

void SourceInternalContainer::setAudioFormat(const AudioFormat& format) {
	if (this->type == SourceType::Audio) {
		std::tie(this->format, this->metaData, this->bitsPerSample, this->quality) = format;
	}
}

const SourceInternalContainer::AudioFormat SourceInternalContainer::getAudioFormat() const {
	if (this->type == SourceType::Audio) {
		return { this->format, this->metaData, this->bitsPerSample, this->quality };
	}
	return {};
}

bool SourceInternalContainer::isForked() const {
	return this->forked;
}

void SourceInternalContainer::initAudioFormat() {
	this->format.clear();
	this->metaData.clear();
	this->bitsPerSample = 0;
	this->quality = 0;
}

const juce::String SourceInternalContainer::getForkName(const juce::String& name) {
	auto file = juce::File::createFileWithoutCheckingPath(name);

	juce::String nameTemp = file.getFileNameWithoutExtension();
	int lastSplit = name.lastIndexOf("_");
	if (lastSplit > -1) {
		juce::String timeStr = name.substring(lastSplit + 1, name.length() - 1);
		if (juce::Time::fromISO8601(timeStr).toMilliseconds() != 0) {
			nameTemp = nameTemp.substring(0, lastSplit);
		}
	}
	nameTemp = nameTemp.trimCharactersAtEnd("_");

	return nameTemp + "_" + juce::Time::getCurrentTime().toISO8601(false) + file.getFileExtension();
}
