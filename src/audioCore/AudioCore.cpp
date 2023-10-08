﻿#include "AudioCore.h"

#include "debug/AudioDebugger.h"
#include "debug/MIDIDebugger.h"
#include "plugin/PluginLoader.h"
#include "misc/PlayPosition.h"
#include "misc/PlayWatcher.h"
#include "misc/Renderer.h"
#include "project/ProjectInfoData.h"
#include "Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

class AudioDeviceChangeListener : public juce::ChangeListener {
public:
	AudioDeviceChangeListener(AudioCore* parent) : parent(parent) {};
	void changeListenerCallback(juce::ChangeBroadcaster* /*source*/) override {
		if (parent) {
			/** Update Audio Buses */
			parent->updateAudioBuses();
		}
	};

private:
	AudioCore* const parent = nullptr;
};

AudioCore::AudioCore() {
	/** Main Audio Device Manager */
	this->audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();

	/** Main Audio Graph Of The Audio Core */
	this->mainAudioGraph = std::make_unique<MainGraph>();

	/** Main Graph Player */
	this->mainGraphPlayer = std::make_unique<juce::AudioProcessorPlayer>();

	/** Mackie Control Hub */
	this->mackieHub = std::make_unique<MackieControlHub>();

	/** Audio Debug */
	this->audioDebugger = std::make_unique<AudioDebugger>(this);

	/** MIDI Debug */
	this->midiDebugger = std::make_unique<MIDIDebugger>();
	this->mainAudioGraph->setMIDIMessageHook(
		[debugger = juce::Component::SafePointer(dynamic_cast<MIDIDebugger*>(this->midiDebugger.get()))] 
		(const juce::MidiMessage& mes, bool isInput) {
			if (debugger) {
				debugger->addMessage(mes, isInput);
			}
		});

	/** Audio Plugin Manager */
	this->audioPluginSearchThread = std::make_unique<AudioPluginSearchThread>();

	/** Audio Device Listener */
	this->audioDeviceListener = std::make_unique<AudioDeviceChangeListener>(this);
	this->audioDeviceManager->addChangeListener(this->audioDeviceListener.get());

	/** Init Audio Device */
	this->initAudioDevice();

	/** Init Project */
	ProjectInfoData::getInstance()->init();

	/** Start Play Watcher */
	PlayWatcher::getInstance()->startTimer(1000);
}

AudioCore::~AudioCore() {
	Renderer::releaseInstance();
	this->audioDeviceManager->removeAllChangeListeners();
	this->audioDeviceManager->removeAudioCallback(this->mainGraphPlayer.get());
	this->mainGraphPlayer->setProcessor(nullptr);
	PlayWatcher::releaseInstance();
	PlayPosition::releaseInstance();
	AudioIOList::releaseInstance();
	CloneableSourceManager::releaseInstance();
}

const juce::StringArray AudioCore::getAudioDeviceList(AudioCore::AudioDeviceType type, bool isInput) {
	std::unique_ptr<juce::AudioIODeviceType> ptrType = nullptr;
	switch (type) {
	case AudioCore::AudioDeviceType::CoreAudio:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_CoreAudio());
		break;
	case AudioCore::AudioDeviceType::IOSAudio:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_iOSAudio());
		break;
	case AudioCore::AudioDeviceType::WASAPIShared:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::shared));
		break;
	case AudioCore::AudioDeviceType::WASAPIExclusive:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::exclusive));
		break;
	case AudioCore::AudioDeviceType::WASAPISharedLowLatency:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::sharedLowLatency));
		break;
	case AudioCore::AudioDeviceType::DirectSound:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_DirectSound());
		break;
	case AudioCore::AudioDeviceType::ASIO:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_ASIO());
		break;
	case AudioCore::AudioDeviceType::ALSA:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_ALSA());
		break;
	case AudioCore::AudioDeviceType::JACK:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_JACK());
		break;
	case AudioCore::AudioDeviceType::Android:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Android());
		break;
	case AudioCore::AudioDeviceType::OpenSLES:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_OpenSLES());
		break;
	case AudioCore::AudioDeviceType::Oboe:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Oboe());
		break;
	case AudioCore::AudioDeviceType::Bela:
		ptrType.reset(juce::AudioIODeviceType::createAudioIODeviceType_Bela());
		break;
	default:
		break;
	}

	if (!ptrType) { return juce::StringArray{}; }
	ptrType->scanForDevices();
	return ptrType->getDeviceNames(isInput);
}

const juce::StringArray AudioCore::getAllAudioDeviceList(bool isInput) {
	juce::StringArray result, resultTemp;

	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::CoreAudio, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[CoreAudio]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::IOSAudio, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[iOSAudio]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPIShared, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(shared)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPIExclusive, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(exclusive)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::WASAPISharedLowLatency, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[WASAPI(sharedLowLatency)]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::DirectSound, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[DirectSound]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::ASIO, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[ASIO]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::ALSA, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[ALSA]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::JACK, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[JACK]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Android, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Android]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::OpenSLES, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[OpenSLES]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Oboe, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Oboe]") + s);
	}
	resultTemp = AudioCore::getAudioDeviceList(AudioDeviceType::Bela, isInput);
	for (auto& s : resultTemp) {
		result.add(juce::String("[Bela]") + s);
	}

	return result;
}

const juce::StringArray AudioCore::getAllAudioInputDeviceList() {
	return AudioCore::getAllAudioDeviceList(true);
}

const juce::StringArray AudioCore::getAllAudioOutputDeviceList() {
	return AudioCore::getAllAudioDeviceList(false);
}

const juce::String  AudioCore::setAudioInputDevice(const juce::String& deviceName) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.inputDeviceName = deviceName;
	deviceSetup.useDefaultInputChannels = true;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String  AudioCore::setAudioOutputDevice(const juce::String& deviceName) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.outputDeviceName = deviceName;
	deviceSetup.useDefaultOutputChannels = true;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String AudioCore::getAudioInputDeviceName() const {
	auto audioSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return audioSetup.inputDeviceName;
}

const juce::String AudioCore::getAudioOutputDeviceName() const {
	auto audioSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return audioSetup.outputDeviceName;
}

void AudioCore::setCurrentAudioDeviceType(const juce::String& typeName) {
	this->audioDeviceManager->setCurrentAudioDeviceType(typeName, true);
}

const juce::String AudioCore::getCurrentAudioDeivceType() const {
	return this->audioDeviceManager->getCurrentAudioDeviceType();
}

const juce::String  AudioCore::setAudioSampleRate(double value) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.sampleRate = value;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

const juce::String  AudioCore::setAudioBufferSize(int value) {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	deviceSetup.bufferSize = value;
	return this->audioDeviceManager->setAudioDeviceSetup(deviceSetup, true);
}

double AudioCore::getAudioSampleRate() const {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return deviceSetup.sampleRate;
}

int AudioCore::getAudioBufferSize() const {
	auto deviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
	return deviceSetup.bufferSize;
}

void AudioCore::playTestSound() const {
	this->audioDeviceManager->playTestSound();
}

void AudioCore::setMIDIInputDeviceEnabled(const juce::String& deviceID, bool enabled) {
	this->audioDeviceManager->setMidiInputDeviceEnabled(deviceID, enabled);
}

void AudioCore::setMIDIOutputDevice(const juce::String& deviceID) {
	this->audioDeviceManager->setDefaultMidiOutputDevice(deviceID);
}

bool AudioCore::getMIDIInputDeviceEnabled(const juce::String& deviceID) const {
	return this->audioDeviceManager->isMidiInputDeviceEnabled(deviceID);
}

const juce::String AudioCore::getMIDIOutputDevice() const {
	return this->audioDeviceManager->getDefaultMidiOutputIdentifier();
}

const juce::StringArray AudioCore::getAllMIDIDeviceList(bool isInput) {
	auto list = isInput
		? juce::MidiInput::getAvailableDevices()
		: juce::MidiOutput::getAvailableDevices();

	juce::StringArray result;
	for (auto& i : list) {
		result.add(i.name);
	}
	return result;
}

const juce::StringArray AudioCore::getAllMIDIInputDeviceList() {
	return AudioCore::getAllMIDIDeviceList(true);
}

const juce::StringArray AudioCore::getAllMIDIOutputDeviceList() {
	return AudioCore::getAllMIDIDeviceList(false);
}

juce::Component* AudioCore::getAudioDebugger() const {
	return this->audioDebugger.get();
}

juce::Component* AudioCore::getMIDIDebugger() const {
	return this->midiDebugger.get();
}

void AudioCore::setMIDIDebuggerMaxNum(int num) {
	if (auto dbg = dynamic_cast<MIDIDebugger*>(this->getMIDIDebugger())) {
		dbg->setMaxNum(num);
	}
}

int AudioCore::getMIDIDebuggerMaxNum() const {
	if (auto dbg = dynamic_cast<MIDIDebugger*>(this->getMIDIDebugger())) {
		return dbg->getMaxNum();
	}
	return -1;
}

const juce::StringArray AudioCore::getPluginTypeList() const {
	auto formatList = this->audioPluginSearchThread->getFormats();

	juce::StringArray result;
	for (auto i : formatList) {
		result.add(i->getName());
	}

	return result;
}

const std::tuple<bool, juce::KnownPluginList&> AudioCore::getPluginList() const {
	return this->audioPluginSearchThread->getPluginList();
}

void AudioCore::clearPluginList() {
	this->audioPluginSearchThread->clearList();
}

void AudioCore::clearPluginTemporary() {
	this->audioPluginSearchThread->clearTemporary();
}

bool AudioCore::pluginSearchThreadIsRunning() const {
	return this->audioPluginSearchThread->isThreadRunning();
}

const std::unique_ptr<juce::PluginDescription> AudioCore::findPlugin(const juce::String& identifier, bool isInstrument) const {
	auto& list = std::get<1>(this->getPluginList());

	auto ptr = list.getTypeForIdentifierString(identifier);
	if (ptr && ptr->isInstrument == isInstrument) {
		return std::move(ptr);
	}

	return nullptr;
}

bool AudioCore::addEffect(const juce::String& identifier, int trackIndex, int effectIndex) {
	if (auto des = this->findPlugin(identifier, false)) {
		if (auto graph = this->getGraph()) {
			if (auto track = graph->getTrackProcessor(trackIndex)) {
				if (auto pluginDock = track->getPluginDock()) {
					if (auto ptr = pluginDock->insertPlugin(effectIndex)) {
						PluginLoader::getInstance()->loadPlugin(*(des.get()), ptr);
						return true;
					}
				}
			}
		}
	}
	return false;
}

PluginDecorator* AudioCore::getEffect(int trackIndex, int effectIndex) const {
	if (auto graph = this->getGraph()) {
		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				return pluginDock->getPluginProcessor(effectIndex);
			}
		}
	}
	return nullptr;
}

bool AudioCore::removeEffect(int trackIndex, int effectIndex) {
	if (auto graph = this->getGraph()) {
		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->removePlugin(effectIndex);
				return true;
			}
		}
	}
	return false;
}

void AudioCore::bypassEffect(int trackIndex, int effectIndex, bool bypass) {
	if (auto graph = this->getGraph()) {
		if (auto track = graph->getTrackProcessor(trackIndex)) {
			if (auto pluginDock = track->getPluginDock()) {
				pluginDock->setPluginBypass(effectIndex, bypass);
			}
		}
	}
}

bool AudioCore::addInstrument(const juce::String& identifier,
	int instrIndex, const juce::AudioChannelSet& type) {
	if (auto des = this->findPlugin(identifier, true)) {
		if (auto graph = this->getGraph()) {
			if (auto ptr = graph->insertInstrument(instrIndex, type)) {
				PluginLoader::getInstance()->loadPlugin(*(des.get()), ptr);
				return true;
			}
		}
	}
	return false;
}

PluginDecorator* AudioCore::getInstrument(int instrIndex) const {
	if (auto graph = this->getGraph()) {
		return graph->getInstrumentProcessor(instrIndex);
	}
	return nullptr;
}

bool AudioCore::removeInstrument(int instrIndex) {
	if (auto graph = this->getGraph()) {
		graph->removeInstrument(instrIndex);
		return true;
	}
	return false;
}

void AudioCore::bypassInstrument(int instrIndex, bool bypass) {
	if (auto graph = this->getGraph()) {
		graph->setInstrumentBypass(instrIndex, bypass);
	}
}

bool AudioCore::addSequencerSourceInstance(int trackIndex, int srcIndex,
	double startTime, double endTime, double offset) {
	juce::ScopedTryReadLock srcLocker(CloneableSourceManager::getInstance()->getLock());
	if (srcLocker.isLocked()) {
		if (auto ptrSrc = CloneableSourceManager::getInstance()->getSource(srcIndex)) {
			if (auto seqTrack = this->mainAudioGraph->getSourceProcessor(trackIndex)) {
				return seqTrack->addSeq({ startTime, endTime, offset, ptrSrc, srcIndex });
			}
		}
	}
	return false;
}

void AudioCore::removeSequencerSourceInstance(int trackIndex, int index) {
	if (auto seqTrack = this->mainAudioGraph->getSourceProcessor(trackIndex)) {
		seqTrack->removeSeq(index);
	}
}

int AudioCore::getSequencerSourceInstanceNum(int trackIndex) const {
	if (auto seqTrack = this->mainAudioGraph->getSourceProcessor(trackIndex)) {
		return seqTrack->getSeqNum();
	}
	return -1;
}

bool AudioCore::addRecorderSourceInstance(int srcIndex, double offset) {
	juce::ScopedTryReadLock srcLocker(CloneableSourceManager::getInstance()->getLock());
	if (srcLocker.isLocked()) {
		if (auto ptrSrc = CloneableSourceManager::getInstance()->getSource(srcIndex)) {
			if (auto recorder = this->mainAudioGraph->getRecorder()) {
				recorder->addTask(ptrSrc, srcIndex, offset);
				return true;
			}
		}
	}
	return false;
}

void AudioCore::removeRecorderSourceInstance(int index) {
	if (auto recorder = this->mainAudioGraph->getRecorder()) {
		recorder->removeTask(index);
	}
}

int AudioCore::getRecorderSourceInstanceNum() const {
	if (auto recorder = this->mainAudioGraph->getRecorder()) {
		return recorder->getTaskNum();
	}
	return -1;
}

void AudioCore::play() {
	PlayPosition::getInstance()->transportPlay(true);
}

void AudioCore::pause() {
	PlayPosition::getInstance()->transportPlay(false);
	this->mainAudioGraph->closeAllNote();
}

void AudioCore::stop() {
	PlayPosition::getInstance()->transportPlay(false);
	this->mainAudioGraph->closeAllNote();
	if (this->returnToStart) {
		PlayPosition::getInstance()->setPositionInSeconds(this->playStartTime);
	}
}

void AudioCore::rewind() {
	this->playStartTime = 0;
	this->mainAudioGraph->closeAllNote();
	PlayPosition::getInstance()->transportRewind();
}

void AudioCore::record(bool start) {
	PlayPosition::getInstance()->transportRecord(start);
}

void AudioCore::setPositon(double pos) {
	this->playStartTime = pos;
	this->mainAudioGraph->closeAllNote();
	PlayPosition::getInstance()->setPositionInSeconds(pos);
}

void AudioCore::setReturnToPlayStartPosition(bool returnToStart) {
	this->returnToStart = returnToStart;
}

bool AudioCore::getReturnToPlayStartPosition() const {
	return this->returnToStart;
}

juce::Optional<juce::AudioPlayHead::PositionInfo> AudioCore::getPosition() const {
	return PlayPosition::getInstance()->getPosition();
}

const juce::StringArray AudioCore::getPluginBlackList() const {
	return this->audioPluginSearchThread->getBlackList();
}

void AudioCore::addToPluginBlackList(const juce::String& plugin) const {
	this->audioPluginSearchThread->addToBlackList(plugin);
}

void AudioCore::removeFromPluginBlackList(const juce::String& plugin) const {
	this->audioPluginSearchThread->removeFromBlackList(plugin);
}

const juce::StringArray AudioCore::getPluginSearchPath() const {
	return this->audioPluginSearchThread->getSearchPath();
}

void AudioCore::addToPluginSearchPath(const juce::String& path) const {
	this->audioPluginSearchThread->addToSearchPath(path);
}

void AudioCore::removeFromPluginSearchPath(const juce::String& path) const {
	this->audioPluginSearchThread->removeFromSearchPath(path);
}

void AudioCore::setIsolation(bool isolation) {
	this->mainGraphPlayer->setProcessor(
		isolation ? nullptr : this->mainAudioGraph.get());
}

bool AudioCore::renderNow(const juce::Array<int>& tracks, const juce::String& path,
	const juce::String& name, const juce::String& extension) {
	return Renderer::getInstance()->start(tracks, path, name, extension);
}

bool AudioCore::isRendering() const {
	return Renderer::getInstance()->getRendering();
}

MainGraph* AudioCore::getGraph() const {
	return this->mainAudioGraph.get();
}

MackieControlHub* AudioCore::getMackie() const {
	return this->mackieHub.get();
}

bool AudioCore::save(const juce::String& path) {
	/** Get Save Dir */
	juce::File projFile = juce::File::getCurrentWorkingDirectory().getChildFile(path);
	juce::File projDir = projFile.getParentDirectory();
	projDir.createDirectory();

	/** Update Project Info */
	ProjectInfoData::getInstance()->push();
	ProjectInfoData::getInstance()->update();

	/** Lock Sources */
	juce::ScopedReadLock locker(CloneableSourceManager::getInstance()->getLock());

	/** Get Project Data */
	auto mes = this->serialize();
	if (!dynamic_cast<vsp4::Project*>(mes.get())) { ProjectInfoData::getInstance()->pop(); return false; };
	auto proj = std::unique_ptr<vsp4::Project>(dynamic_cast<vsp4::Project*>(mes.release()));

	juce::MemoryBlock projData;
	projData.setSize(proj->ByteSizeLong());
	if (!proj->SerializeToArray(projData.getData(), projData.getSize())) { ProjectInfoData::getInstance()->pop(); return false; }

	/** Save Sources */
	auto& srcList = proj->sources().sources();
	for (int i = 0; i < srcList.size(); i++) {
		juce::String srcPath = srcList.Get(i).path();
		juce::File srcFile = projDir.getChildFile(srcPath);
		srcFile.getParentDirectory().createDirectory();

		CloneableSourceManager::getInstance()->saveSourceAsync(i, srcFile.getFullPathName());
	}
	
	/** Write Project File */
	juce::FileOutputStream projStream(projFile);
	if (!projStream.openedOk()) { ProjectInfoData::getInstance()->pop(); return false; }
	projStream.setPosition(0);
	projStream.truncate();
	if (!projStream.write(projData.getData(), projData.getSize())) { ProjectInfoData::getInstance()->pop(); return false; }

	/** Set Working Directory */
	projDir.setAsCurrentWorkingDirectory();
	ProjectInfoData::getInstance()->release();
	return true;
}

bool AudioCore::load(const juce::String& path) {
	/** TODO */
	return false;
}

bool AudioCore::parse(const google::protobuf::Message* data) {
	/** TODO */
	return false;
}

std::unique_ptr<google::protobuf::Message> AudioCore::serialize() const {
	auto mes = std::make_unique<vsp4::Project>();

	auto info = ProjectInfoData::getInstance()->serialize();
	if (!dynamic_cast<vsp4::ProjectInfo*>(info.get())) { return nullptr; }
	mes->set_allocated_info(dynamic_cast<vsp4::ProjectInfo*>(info.release()));

	auto [majorVer, minorVer, patchVer] = utils::getAudioPlatformVersion();
	auto version = mes->mutable_version();
	version->set_major(majorVer);
	version->set_minor(minorVer);
	version->set_patch(patchVer);

	auto sources = CloneableSourceManager::getInstance()->serialize();
	if (!dynamic_cast<vsp4::SourceList*>(sources.get())) { return nullptr; }
	mes->set_allocated_sources(dynamic_cast<vsp4::SourceList*>(sources.release()));

	auto graph = this->mainAudioGraph->serialize();
	if (!dynamic_cast<vsp4::MainGraph*>(graph.get())) { return nullptr; }
	mes->set_allocated_graph(dynamic_cast<vsp4::MainGraph*>(graph.release()));

	return std::unique_ptr<google::protobuf::Message>(mes.release());
}

void AudioCore::initAudioDevice() {
	/** Init With Default Device */
	this->audioDeviceManager->initialise(1, 2, nullptr, true);

	/** Add Main Graph To Main Player */
	this->mainGraphPlayer->setProcessor(this->mainAudioGraph.get());

	/** Add Player To Default Device */
	this->audioDeviceManager->addAudioCallback(this->mainGraphPlayer.get());

	/** Update Audio Buses */
	this->updateAudioBuses();
}

void AudioCore::updateAudioBuses() {
	/** Link Audio Bus To Sequencer And Mixer */
	if (auto mainGraph = this->mainAudioGraph.get()) {
		/** Get Input Channel Num */
		auto audioDeviceSetup = this->audioDeviceManager->getAudioDeviceSetup();
		auto inputChannelNum = audioDeviceSetup.inputChannels.countNumberOfSetBits();
		auto outputChannelNum = audioDeviceSetup.outputChannels.countNumberOfSetBits();

		/** Set Buses Layout Of Main Graph */
		mainGraph->setAudioLayout(inputChannelNum, outputChannelNum);

		/** Change Main Graph SampleRate And Set Play Head */
		mainGraph->setPlayHead(PlayPosition::getInstance());
		mainGraph->prepareToPlay(audioDeviceSetup.sampleRate, audioDeviceSetup.bufferSize);
	}

	/** Add MIDI Callback */
	{
		auto midiInputDevice = juce::MidiInput::getAvailableDevices();
		for (auto& i : midiInputDevice) {
			if (this->audioDeviceManager->isMidiInputDeviceEnabled(i.identifier)) {
				this->audioDeviceManager->addMidiInputDeviceCallback(
					i.identifier, this->mainGraphPlayer.get());
			}
			else {
				this->audioDeviceManager->removeMidiInputDeviceCallback(
					i.identifier, this->mainGraphPlayer.get());
			}
		}
	}

	/** Set MIDI Output */
	{
		auto midiOutput = this->audioDeviceManager->getDefaultMidiOutput();
		this->mainGraphPlayer->setMidiOutput(midiOutput);
	}

	/** Update Mackie Control Devices */
	{
		auto input = juce::MidiInput::getAvailableDevices();
		auto output = juce::MidiOutput::getAvailableDevices();
		this->mackieHub->removeUnavailableDevices(input, output);
	}
}

AudioCore* AudioCore::getInstance() {
	return AudioCore::instance ? AudioCore::instance : (AudioCore::instance = new AudioCore());
}

AudioCore* AudioCore::getInstanceWithoutCreate() {
	return AudioCore::instance;
}

void AudioCore::releaseInstance() {
	if (AudioCore::instance) {
		delete AudioCore::instance;
		AudioCore::instance = nullptr;
	}
}

AudioCore* AudioCore::instance = nullptr;
