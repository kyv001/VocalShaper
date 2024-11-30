﻿#include "MainGraph.h"

#include "../misc/PlayPosition.h"
#include "../misc/Renderer.h"
#include "../misc/AudioLock.h"
#include "../misc/VMath.h"
#include "../uiCallback/UICallback.h"
#include "../AudioCore.h"
#include "../Utils.h"
#include "SourceRecordProcessor.h"
#include <VSP4.h>

using namespace org::vocalsharp::vocalshaper;

MainGraph::MainGraph() {
	/** The Main Audio IO Node */
	this->audioInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
	this->audioOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

	/** The Main MIDI IO Node */
	this->midiInputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));
	this->midiOutputNode = this->addNode(
		std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
			juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode));

	/** The Source Recorder Node */
	this->recorder = std::make_unique<SourceRecordProcessor>(this);
}

MainGraph::~MainGraph() {
	this->clearGraph();
}

void MainGraph::setAudioLayout(int inputChannelNum, int outputChannelNum) {
	/** Create Buses Layout */
	juce::AudioProcessorGraph::BusesLayout busLayout;
	busLayout.inputBuses.add(juce::AudioChannelSet::discreteChannels(inputChannelNum));
	busLayout.outputBuses.add(juce::AudioChannelSet::discreteChannels(outputChannelNum));

	/** Set Layout Of Main Graph */
	this->setBusesLayout(busLayout);

	/** Set Layout Of Input Node */
	juce::AudioProcessorGraph::BusesLayout inputLayout = busLayout;
	inputLayout.outputBuses = inputLayout.inputBuses;
	this->audioInputNode->getProcessor()->setBusesLayout(inputLayout);

	/** Set Layout Of Output Node */
	juce::AudioProcessorGraph::BusesLayout outputLayout = busLayout;
	outputLayout.inputBuses = outputLayout.outputBuses;
	this->audioOutputNode->getProcessor()->setBusesLayout(outputLayout);

	/** Auto Remove Connections */
	this->removeIllegalAudioI2SrcConnections();
	this->removeIllegalAudioI2TrkConnections();
	this->removeIllegalAudioTrk2OConnections();

	/** Set Layout of Recorder */
	this->getRecorder()->setBusesLayout(inputLayout);

	/** Set Level Size */
	{
		juce::ScopedWriteLock locker(audioLock::getLevelMeterLock());
		this->outputLevels.resize(outputChannelNum);
	}
}

void MainGraph::setMIDIMessageHook(
	const std::function<void(const juce::MidiMessage&, bool)> hook) {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->midiHook = hook;
}

void MainGraph::setMIDICCListener(const MIDICCListener& listener) {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->ccListener = listener;
}

void MainGraph::clearMIDICCListener() {
	juce::ScopedWriteLock locker(audioLock::getAudioLock());
	this->ccListener = MIDICCListener{};
}

void MainGraph::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
	/** Play Head */
	if (auto position = dynamic_cast<PlayPosition*>(this->getPlayHead())) {
		position->setSampleRate(sampleRate);
	}

	/** Renderer */
	Renderer::getInstance()->updateSampleRateAndBufferSize(
		sampleRate, maximumExpectedSamplesPerBlock);

	/** Recorder */
	this->recorder->prepareToPlay(
		sampleRate, maximumExpectedSamplesPerBlock);

	/** Current Graph */
	this->juce::AudioProcessorGraph::prepareToPlay(
		sampleRate, maximumExpectedSamplesPerBlock);
}

void MainGraph::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessorGraph::setPlayHead(newPlayHead);

	/** Recorder */
	this->getRecorder()->setPlayHead(newPlayHead);

	/** Audio Source */
	for (auto& i : this->audioSourceNodeList) {
		auto src = i->getProcessor();
		src->setPlayHead(newPlayHead);
	}

	/** Track */
	for (auto& i : this->trackNodeList) {
		auto track = i->getProcessor();
		track->setPlayHead(newPlayHead);
	}
}

double MainGraph::getTailLengthSeconds() const {
	double result = 0;
	for (auto& t : this->audioSourceNodeList) {
		result = std::max(t->getProcessor()->getTailLengthSeconds(), result);
	}
	return this->totalLengthTemp = std::max(result, this->totalLengthTemp);
}

SourceRecordProcessor* MainGraph::getRecorder() const {
	return this->recorder.get();
}

void MainGraph::clearGraph() {
	/** Clear Temp */
	this->totalLengthTemp = 0;

	/** Lock */
	juce::ScopedWriteLock locker(audioLock::getSourceLock());

	for (auto& i : this->midiI2SrcConnectionList) {
		this->removeConnection(i);
	}
	this->midiI2SrcConnectionList.clear();

	for (auto& i : this->audioI2SrcConnectionList) {
		this->removeConnection(i);
	}
	this->audioI2SrcConnectionList.clear();

	for (auto& i : this->midiSrc2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->midiSrc2TrkConnectionList.clear();

	for (auto& i : this->audioSrc2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->audioSrc2TrkConnectionList.clear();

	for (auto& i : this->midiI2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->midiI2TrkConnectionList.clear();

	for (auto& i : this->audioI2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->audioI2TrkConnectionList.clear();

	for (auto& i : this->audioTrk2TrkConnectionList) {
		this->removeConnection(i);
	}
	this->audioTrk2TrkConnectionList.clear();

	for (auto& i : this->audioTrk2OConnectionList) {
		this->removeConnection(i);
	}
	this->audioTrk2OConnectionList.clear();

	for (auto& i : this->midiTrk2OConnectionList) {
		this->removeConnection(i);
	}
	this->midiTrk2OConnectionList.clear();

	for (auto& i : this->trackNodeList) {
		this->removeNode(i->nodeID);
	}
	this->trackNodeList.clear();

	for (auto& i : this->audioSourceNodeList) {
		this->removeNode(i->nodeID);
	}
	this->audioSourceNodeList.clear();

	/** Callback */
	UICallbackAPI<int>::invoke(UICallbackType::InstrChanged, -1);
	UICallbackAPI<int>::invoke(UICallbackType::TrackChanged, -1);
	UICallbackAPI<int>::invoke(UICallbackType::SeqChanged, -1);
}

const juce::Array<float> MainGraph::getOutputLevels() const {
	juce::ScopedReadLock locker(audioLock::getLevelMeterLock());
	return this->outputLevels;
}

bool MainGraph::parse(
	const google::protobuf::Message* data,
	const ParseConfig& config) {
	auto mes = dynamic_cast<const vsp4::MainGraph*>(data);
	if (!mes) { return false; }

	this->clearGraph();

	auto& seqTracks = mes->seqtracks();
	for (auto& i : seqTracks) {
		this->insertSource(-1, utils::getChannelSet(static_cast<utils::TrackType>(i.type())));
		if (auto sourceNode = this->audioSourceNodeList.getLast()) {
			sourceNode->setBypassed(i.bypassed());
			if (auto source = dynamic_cast<SeqSourceProcessor*>(sourceNode->getProcessor())) {
				if (!source->parse(&i, config)) { return false; }
			}
		}
	}

	auto& mixTracks = mes->mixertracks();
	for (auto& i : mixTracks) {
		this->insertTrack(-1, utils::getChannelSet(static_cast<utils::TrackType>(i.type())));
		if (auto trackNode = this->trackNodeList.getLast()) {
			trackNode->setBypassed(i.bypassed());
			if (auto track = dynamic_cast<Track*>(trackNode->getProcessor())) {
				if (!track->parse(&i, config)) { return false; }
			}
		}
	}

	auto& connections = mes->connections();

	auto& midiI2Src = connections.midii2src();
	for (auto& i : midiI2Src) {
		this->setMIDII2SrcConnection(i.dst());
	}

	auto& audioI2Src = connections.audioi2src();
	for (auto& i : audioI2Src) {
		this->setAudioI2SrcConnection(i.dst(), i.srcchannel(), i.dstchannel());
	}

	auto& midiSrc2Track = connections.midisrc2track();
	for (auto& i : midiSrc2Track) {
		this->setMIDISrc2TrkConnection(i.src(), i.dst());
	}

	auto& audioSrc2Track = connections.audiosrc2track();
	for (auto& i : audioSrc2Track) {
		this->setAudioSrc2TrkConnection(i.src(), i.dst(), i.srcchannel(), i.dstchannel());
	}

	auto& midiI2Track = connections.midii2track();
	for (auto& i : midiI2Track) {
		this->setMIDII2TrkConnection(i.dst());
	}

	auto& audioI2Track = connections.audioi2track();
	for (auto& i : audioI2Track) {
		this->setAudioI2TrkConnection(i.dst(), i.srcchannel(), i.dstchannel());
	}

	auto& audioTrack2O = connections.audiotrack2o();
	for (auto& i : audioTrack2O) {
		this->setAudioTrk2OConnection(i.src(), i.srcchannel(), i.dstchannel());
	}

	auto& audioTrack2Track = connections.audiotrack2track();
	for (auto& i : audioTrack2Track) {
		this->setAudioTrk2TrkConnection(i.src(), i.dst(), i.srcchannel(), i.dstchannel());
	}

	auto& midiTrack2O = connections.miditrack2o();
	for (auto& i : midiTrack2O) {
		this->setMIDITrk2OConnection(i.src());
	}

	return true;
}

std::unique_ptr<google::protobuf::Message> MainGraph::serialize(
	const SerializeConfig& config) const {
	auto mes = std::make_unique<vsp4::MainGraph>();

	auto seqTracks = mes->mutable_seqtracks();
	for (auto& i : this->audioSourceNodeList) {
		if (auto track = dynamic_cast<SeqSourceProcessor*>(i->getProcessor())) {
			auto tmes = track->serialize(config);
			if (!dynamic_cast<vsp4::SeqTrack*>(tmes.get())) { return nullptr; }
			dynamic_cast<vsp4::SeqTrack*>(tmes.get())->set_bypassed(i->isBypassed());
			seqTracks->AddAllocated(dynamic_cast<vsp4::SeqTrack*>(tmes.release()));
		}
	}

	auto mixTracks = mes->mutable_mixertracks();
	for (auto& i : this->trackNodeList) {
		if (auto track = dynamic_cast<Track*>(i->getProcessor())) {
			auto tmes = track->serialize(config);
			if (!dynamic_cast<vsp4::MixerTrack*>(tmes.get())) { return nullptr; }
			dynamic_cast<vsp4::MixerTrack*>(tmes.get())->set_bypassed(i->isBypassed());
			mixTracks->AddAllocated(dynamic_cast<vsp4::MixerTrack*>(tmes.release()));
		}
	}

	auto connections = mes->mutable_connections();

	auto midiI2Src = connections->mutable_midii2src();
	for (auto& i : this->midiI2SrcConnectionList) {
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		if (!dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDIInputConnection>();
		cmes->set_dst(this->findSource(dynamic_cast<SeqSourceProcessor*>(dstNode->getProcessor())));

		midiI2Src->AddAllocated(cmes.release());
	}

	auto audioI2Src = connections->mutable_audioi2src();
	for (auto& i : this->audioI2SrcConnectionList) {
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioInputConnection>();
		cmes->set_dst(this->findSource(dynamic_cast<SeqSourceProcessor*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioI2Src->AddAllocated(cmes.release());
	}

	auto midiSrc2Track = connections->mutable_midisrc2track();
	for (auto& i : this->midiSrc2TrkConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		if (!srcNode || !dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDISendConnection>();
		cmes->set_src(this->findSource(dynamic_cast<SeqSourceProcessor*>(srcNode->getProcessor())));
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));

		midiSrc2Track->AddAllocated(cmes.release());
	}

	auto audioSrc2Track = connections->mutable_audiosrc2track();
	for (auto& i : this->audioSrc2TrkConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!srcNode || !dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioSendConnection>();
		cmes->set_src(this->findSource(dynamic_cast<SeqSourceProcessor*>(srcNode->getProcessor())));
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioSrc2Track->AddAllocated(cmes.release());
	}

	auto midiI2Track = connections->mutable_midii2track();
	for (auto& i : this->midiI2TrkConnectionList) {
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		if (!dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDIInputConnection>();
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));

		midiI2Track->AddAllocated(cmes.release());
	}

	auto audioI2Track = connections->mutable_audioi2track();
	for (auto& i : this->audioI2TrkConnectionList) {
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioInputConnection>();
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioI2Track->AddAllocated(cmes.release());
	}

	auto audioTrack2O = connections->mutable_audiotrack2o();
	for (auto& i : this->audioTrk2OConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!srcNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioOutputConnection>();
		cmes->set_src(this->findTrack(dynamic_cast<Track*>(srcNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioTrack2O->AddAllocated(cmes.release());
	}

	auto audioTrack2Track = connections->mutable_audiotrack2track();
	for (auto& i : this->audioTrk2TrkConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		auto dstNode = this->getNodeForId(i.destination.nodeID);
		int srcChannel = i.source.channelIndex;
		int dstChannel = i.destination.channelIndex;
		if (!srcNode || !dstNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::AudioSendConnection>();
		cmes->set_src(this->findTrack(dynamic_cast<Track*>(srcNode->getProcessor())));
		cmes->set_dst(this->findTrack(dynamic_cast<Track*>(dstNode->getProcessor())));
		cmes->set_srcchannel(srcChannel);
		cmes->set_dstchannel(dstChannel);

		audioTrack2Track->AddAllocated(cmes.release());
	}

	auto midiTrack2O = connections->mutable_miditrack2o();
	for (auto& i : this->midiTrk2OConnectionList) {
		auto srcNode = this->getNodeForId(i.source.nodeID);
		if (!srcNode) { return nullptr; }

		auto cmes = std::make_unique<vsp4::MIDIOutputConnection>();
		cmes->set_src(this->findTrack(dynamic_cast<Track*>(srcNode->getProcessor())));

		midiTrack2O->AddAllocated(cmes.release());
	}

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

void MainGraph::processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) {
	/** Lock */
	juce::ScopedWriteLock levelLocker(audioLock::getLevelMeterLock());
	juce::ScopedTryReadLock audioLocker(audioLock::getAudioLock());
	juce::ScopedTryWriteLock sourceLocker(audioLock::getSourceLock());
	juce::ScopedTryReadLock pluginLocker(audioLock::getPluginLock());
	juce::ScopedTryWriteLock positionLocker(audioLock::getPositionLock());
	juce::ScopedTryReadLock controlLocker(audioLock::getAudioControlLock());
	if (!(audioLocker.isLocked() && pluginLocker.isLocked() 
		&& sourceLocker.isLocked()  && positionLocker.isLocked() && controlLocker.isLocked())) {
		vMath::zeroAllAudioData(audio);
		midi.clear();
		return;
	}

	/** Render State */
	bool isRendering = Renderer::getInstance()->getRendering();

	/** Call MIDI Hook */
	if(!isRendering) {
		if (this->midiHook) {
			for (auto m : midi) {
				juce::MessageManager::callAsync(
					[mes = m.getMessage(), hook = this->midiHook] {
						hook(mes, true);
					});
			}
		}
	}

	/** Send MIDI CC */
	if (!isRendering) {
		/** Get Last CC Channel */
		int lastCCChannel = -1;
		for (auto m : midi) {
			/** Get Message */
			auto message = m.getMessage();
			if (!message.isController()) { continue; }

			/** Auto Link Param */
			lastCCChannel = message.getControllerNumber();
		}

		/** Send Auto Connect */
		if ((lastCCChannel > -1) && this->ccListener) {
			juce::MessageManager::callAsync(
				std::bind(this->ccListener, lastCCChannel));
		}
	}

	/** Transport MMC */
	if (!isRendering) {
		for (auto m : midi) {
			auto mes = m.getMessage();
			if (mes.isMidiMachineControlMessage()) {
				switch (mes.getMidiMachineControlCommand())
				{
				case juce::MidiMessage::MidiMachineControlCommand::mmc_play:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->play(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_pause:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->pause(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_stop:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->stop(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_rewind:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->rewind(); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_recordStart:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->record(true); });
					continue;
				case juce::MidiMessage::MidiMachineControlCommand::mmc_recordStop:
					juce::MessageManager::callAsync([] { AudioCore::getInstance()->record(false); });
					continue;
				default:
					break;
				}
			}
		}
	}

	/** Truncate Input */
	if (isRendering) {
		vMath::zeroAllAudioData(audio);
		midi.clear();
	}

	/** Process Audio Block */
	{
		this->recorder->processBlock(audio, midi);
		this->juce::AudioProcessorGraph::processBlock(audio, midi);
	}

	/** Truncate Output */
	if (isRendering) {
		vMath::zeroAllAudioData(audio);
		midi.clear();
	}

	/** Get Level */
	for (int i = 0; i < audio.getNumChannels() && i < this->outputLevels.size(); i++) {
		this->outputLevels.getReference(i) =
			audio.getRMSLevel(i, 0, audio.getNumSamples());
	}

	/** MIDI Output */
	if (!isRendering) {
		if (this->midiHook) {
			for (auto m : midi) {
				juce::MessageManager::callAsync(
					[mes = m.getMessage(), hook = this->midiHook] {
						hook(mes, false);
					});
			}
		}
	}

	/** Add Position */
	if (auto position = dynamic_cast<PlayPosition*>(this->getPlayHead())) {
		/** Current Time */
		int currentPos = position->getPosition()->getTimeInSamples().orFallback(0);
		int clipSize = audio.getNumSamples();

		/** Check Loop */
		if (position->getLooping()) {
			auto [loopStart, loopEnd] = position->getLoopingTimeSec();
			double sampleRate = position->getSampleRate();

			/** Overflow */
			int next = currentPos + clipSize;
			if (next < (loopStart * sampleRate) || next > (loopEnd * sampleRate)) {
				position->setPositionInSamples(loopStart* sampleRate);
				return;
			}
		}
		
		/** Next Clip */
		if (INT_MAX - clipSize > currentPos) {
			position->next(clipSize);
		}
		else {
			/** Time Overflow */
			position->setOverflow();
		}
	}
}
