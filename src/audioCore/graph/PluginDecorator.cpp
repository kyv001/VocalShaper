#include "PluginDecorator.h"

PluginDecorator::PluginDecorator() {
	for (int i = 0; i < this->paramCCList.size(); i++) {
		this->paramCCList[i] = -1;
	}
}

PluginDecorator::PluginDecorator(std::unique_ptr<juce::AudioPluginInstance> plugin)
	: PluginDecorator() {
	this->setPlugin(std::move(plugin));
}

void PluginDecorator::setPlugin(std::unique_ptr<juce::AudioPluginInstance> plugin) {
	{
		juce::ScopedWriteLock locker(this->pluginLock);
		this->plugin = std::move(plugin);
	}

	this->initFlag = true;
	this->syncBusesFromPlugin();
	this->initFlag = false;

	this->plugin->setPlayHead(this->getPlayHead());
	this->plugin->setNonRealtime(this->isNonRealtime());
	this->plugin->prepareToPlay(this->getSampleRate(), this->getBlockSize());
}

bool PluginDecorator::canPluginAddBus(bool isInput) const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return false; }
	return this->plugin->canAddBus(isInput);
}

bool PluginDecorator::canPluginRemoveBus(bool isInput) const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return false; }
	return this->plugin->canRemoveBus(isInput);
}

void PluginDecorator::setMIDIChannel(int channel) {
	if (channel > 16 || channel < 0) {
		channel = 0;
	}
	this->midiChannel = channel;
}

int PluginDecorator::getMIDIChannel() const {
	return this->midiChannel;
}

const juce::Array<juce::AudioProcessorParameter*>& PluginDecorator::getPluginParamList() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { static juce::Array<juce::AudioProcessorParameter*> empty; return empty; }
	return this->plugin->getParameters();
}

const juce::String PluginDecorator::getParamName(int index) const {
	auto& paramList = this->getPluginParamList();
	if (index < 0 || index >= paramList.size()) { return juce::String{}; }

	auto param = paramList.getUnchecked(index);
	return param->getName(INT_MAX);
}

float PluginDecorator::getParamValue(int index) const {
	auto& paramList = this->getPluginParamList();
	if (index < 0 || index >= paramList.size()) { return 0; }

	auto param = paramList.getUnchecked(index);
	return param->getValue();
}

float PluginDecorator::getParamDefaultValue(int index) const {
	auto& paramList = this->getPluginParamList();
	if (index < 0 || index >= paramList.size()) { return 0; }

	auto param = paramList.getUnchecked(index);
	return param->getDefaultValue();
}

void PluginDecorator::setParamValue(int index, float value) {
	auto& paramList = this->getPluginParamList();
	if (index < 0 || index >= paramList.size()) { return; }

	auto param = paramList.getUnchecked(index);
	param->setValue(value);
}

void PluginDecorator::connectParamCC(int paramIndex, int CCIndex) {
	if (CCIndex < 0 || CCIndex >= this->paramCCList.size()) { return; }
	if (paramIndex < 0 || paramIndex >= this->getPluginParamList().size()) { return; }

	auto currentCC = this->getParamCCConnection(paramIndex);
	if (currentCC > -1) {
		this->paramCCList[currentCC] = -1;
	}

	this->paramCCList[CCIndex] = paramIndex;
}

int PluginDecorator::getCCParamConnection(int CCIndex) const {
	if (CCIndex < 0 || CCIndex >= this->paramCCList.size()) { return -1; }
	return this->paramCCList[CCIndex];
}

int PluginDecorator::getParamCCConnection(int paramIndex) const {
	auto it = std::find(this->paramCCList.begin(), this->paramCCList.end(), paramIndex);

	if (it != this->paramCCList.end()) {
		return it - this->paramCCList.begin();
	}

	return -1;
}

void PluginDecorator::removeCCParamConnection(int CCIndex) {
	if (CCIndex < 0 || CCIndex >= this->paramCCList.size()) { return; }
	this->paramCCList[CCIndex] = -1;
}

void PluginDecorator::setParamCCListenning(int paramIndex) {
	if (paramIndex < 0 || paramIndex >= this->getPluginParamList().size()) {
		this->paramListenningCC = -1;
		return;
	}

	auto currentCC = this->getParamCCConnection(paramIndex);
	if (currentCC > -1) {
		this->paramCCList[currentCC] = -1;
	}

	this->paramListenningCC = paramIndex;
}

void PluginDecorator::setMIDICCIntercept(bool midiCCShouldIntercept) {
	this->midiCCShouldIntercept = midiCCShouldIntercept;
}

bool PluginDecorator::getMIDICCIntercept() const {
	return this->midiCCShouldIntercept;
}

const juce::String PluginDecorator::getName() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return ""; }
	return this->plugin->getName();
}

juce::StringArray PluginDecorator::getAlternateDisplayNames() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return {}; }
	return this->plugin->getAlternateDisplayNames();
}

void PluginDecorator::prepareToPlay(
	double sampleRate, int maximumExpectedSamplesPerBlock) {
	this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void PluginDecorator::releaseResources() {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->releaseResources();
}

void PluginDecorator::memoryWarningReceived() {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->memoryWarningReceived();
}

void PluginDecorator::processBlock(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	PluginDecorator::filterMIDIMessage(this->midiChannel, midiMessages);
	this->parseMIDICC(midiMessages);
	PluginDecorator::interceptMIDICCMessage(this->midiCCShouldIntercept, midiMessages);

	{
		juce::ScopedTryReadLock locker(this->pluginLock);
		if (locker.isLocked() && this->plugin) {
			this->plugin->processBlock(buffer, midiMessages);
		}
	}

	PluginDecorator::interceptMIDIMessage(this->midiShouldOutput, midiMessages);
}

void PluginDecorator::processBlock(
	juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
	PluginDecorator::filterMIDIMessage(this->midiChannel, midiMessages);
	this->parseMIDICC(midiMessages);
	PluginDecorator::interceptMIDICCMessage(this->midiCCShouldIntercept, midiMessages);

	{
		juce::ScopedTryReadLock locker(this->pluginLock);
		if (locker.isLocked() && this->plugin) {
			this->plugin->processBlock(buffer, midiMessages);
		}
	}

	PluginDecorator::interceptMIDIMessage(this->midiShouldOutput, midiMessages);
}

void PluginDecorator::processBlockBypassed(
	juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	juce::ScopedTryReadLock locker(this->pluginLock);
	if (locker.isLocked() && this->plugin) {
		this->plugin->processBlockBypassed(buffer, midiMessages);
	}
}

void PluginDecorator::processBlockBypassed(
	juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) {
	juce::ScopedTryReadLock locker(this->pluginLock);
	if (locker.isLocked() && this->plugin) {
		this->plugin->processBlockBypassed(buffer, midiMessages);
	}
}

bool PluginDecorator::canAddBus(bool isInput) const {
	return true;
}

bool PluginDecorator::canRemoveBus(bool isInput) const {
	return true;
}

bool PluginDecorator::supportsDoublePrecisionProcessing() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return false; }
	return this->plugin->supportsDoublePrecisionProcessing();
}

double PluginDecorator::getTailLengthSeconds() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return 0; }
	return this->plugin->getTailLengthSeconds();
}

bool PluginDecorator::acceptsMidi() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return false; }
	return this->plugin->acceptsMidi();
}

bool PluginDecorator::producesMidi() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return false; }
	return this->plugin->producesMidi();
}

bool PluginDecorator::supportsMPE() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return false; }
	return this->plugin->supportsMPE();
}

bool PluginDecorator::isMidiEffect() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return false; }
	return this->plugin->isMidiEffect();
}

void PluginDecorator::reset() {
	this->juce::AudioProcessor::reset();
	juce::ScopedReadLock locker(this->pluginLock);
	if (this->plugin) {
		this->plugin->reset();
	}
}

juce::AudioProcessorParameter* PluginDecorator::getBypassParameter() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return nullptr; }
	return this->plugin->getBypassParameter();
}

void PluginDecorator::setNonRealtime(bool isNonRealtime) noexcept {
	this->juce::AudioProcessor::setNonRealtime(isNonRealtime);
	juce::ScopedReadLock locker(this->pluginLock);
	if (this->plugin) {
		this->plugin->setNonRealtime(isNonRealtime);
	}
}

juce::AudioProcessorEditor* PluginDecorator::createEditor() {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return nullptr; }
	return this->plugin->createEditorIfNeeded();
}

bool PluginDecorator::hasEditor() const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return false; }
	return this->plugin->hasEditor();
}

void PluginDecorator::refreshParameterList() {
	this->juce::AudioProcessor::refreshParameterList();
	juce::ScopedReadLock locker(this->pluginLock);
	if (this->plugin) {
		this->plugin->refreshParameterList();
	}
}

int PluginDecorator::getNumPrograms() {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return 0; }
	return this->plugin->getNumPrograms();
}

int PluginDecorator::getCurrentProgram() {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return -1; }
	return this->plugin->getCurrentProgram();
}

void PluginDecorator::setCurrentProgram(int index) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->setCurrentProgram(index);
}

const juce::String PluginDecorator::getProgramName(int index) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return ""; }
	return this->plugin->getProgramName(index);
}

void PluginDecorator::changeProgramName(int index, const juce::String& newName) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->changeProgramName(index, newName);
}

void PluginDecorator::getStateInformation(juce::MemoryBlock& destData) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->getStateInformation(destData);
}

void PluginDecorator::setStateInformation(const void* data, int sizeInBytes) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->setStateInformation(data, sizeInBytes);
}

void PluginDecorator::setCurrentProgramStateInformation(const void* data, int sizeInBytes) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->setCurrentProgramStateInformation(data, sizeInBytes);
}

void PluginDecorator::processorLayoutsChanged() {
	this->updatePluginBuses();
}

void PluginDecorator::addListener(juce::AudioProcessorListener* newListener) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->addListener(newListener);
}

void PluginDecorator::removeListener(juce::AudioProcessorListener* listenerToRemove) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->removeListener(listenerToRemove);
}

void PluginDecorator::setPlayHead(juce::AudioPlayHead* newPlayHead) {
	this->juce::AudioProcessor::setPlayHead(newPlayHead);
	juce::ScopedReadLock locker(this->pluginLock);
	if (this->plugin) {
		this->plugin->setPlayHead(newPlayHead);
	}
}

juce::int32 PluginDecorator::getAAXPluginIDForMainBusConfig(
	const juce::AudioChannelSet& mainInputLayout,
	const juce::AudioChannelSet& mainOutputLayout,
	bool idForAudioSuite) const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return 0; }
	return this->plugin->getAAXPluginIDForMainBusConfig(
		mainInputLayout, mainOutputLayout, idForAudioSuite);
}

juce::AudioProcessor::CurveData PluginDecorator::getResponseCurve(
	juce::AudioProcessor::CurveData::Type type) const {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return juce::AudioProcessor::CurveData{}; }
	return this->plugin->getResponseCurve(type);
}

void PluginDecorator::updateTrackProperties(
	const juce::AudioProcessor::TrackProperties& properties) {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	this->plugin->updateTrackProperties(properties);
}

bool PluginDecorator::parse(const google::protobuf::Message* data) {
	/** TODO */
	return true;
}

std::unique_ptr<const google::protobuf::Message> PluginDecorator::serialize() const {
	/** TODO */
	return nullptr;
}

void PluginDecorator::numChannelsChanged() {
	//this->updatePluginBuses();
}

void PluginDecorator::numBusesChanged() {
	this->syncBusesNumFromPlugin();
}

void PluginDecorator::updatePluginBuses() {
	juce::ScopedReadLock locker(this->pluginLock);
	if (this->initFlag) { return; }
	if (!this->plugin) { return; }

	auto currentBusesLayout = this->getBusesLayout();
	auto pluginBusesLayout = this->plugin->getBusesLayout();

	if (pluginBusesLayout == currentBusesLayout) {
		return;
	}

	int inputBusNumDefer = 
		currentBusesLayout.inputBuses.size() - pluginBusesLayout.inputBuses.size();
	int outputBusNumDefer =
		currentBusesLayout.outputBuses.size() - pluginBusesLayout.outputBuses.size();

	if (inputBusNumDefer > 0) {
		for (int i = 0; i < inputBusNumDefer; i++) {
			this->plugin->addBus(true);
		}
	}
	else if (inputBusNumDefer < 0) {
		for (int i = 0; i < -inputBusNumDefer; i++) {
			this->plugin->removeBus(true);
		}
	}
	if (outputBusNumDefer > 0) {
		for (int i = 0; i < outputBusNumDefer; i++) {
			this->plugin->addBus(false);
		}
	}
	else if (outputBusNumDefer < 0) {
		for (int i = 0; i < -outputBusNumDefer; i++) {
			this->plugin->removeBus(false);
		}
	}

	this->plugin->setBusesLayout(currentBusesLayout);
}

void PluginDecorator::syncBusesFromPlugin() {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }

	auto currentBusesLayout = this->getBusesLayout();
	auto pluginBusesLayout = this->plugin->getBusesLayout();

	if (pluginBusesLayout == currentBusesLayout) {
		return;
	}

	int inputBusNumDefer =
		currentBusesLayout.inputBuses.size() - pluginBusesLayout.inputBuses.size();
	int outputBusNumDefer =
		currentBusesLayout.outputBuses.size() - pluginBusesLayout.outputBuses.size();

	if (inputBusNumDefer > 0) {
		for (int i = 0; i < inputBusNumDefer; i++) {
			this->removeBus(true);
		}
	}
	else if (inputBusNumDefer < 0) {
		for (int i = 0; i < -inputBusNumDefer; i++) {
			this->addBus(true);
		}
	}
	if (outputBusNumDefer > 0) {
		for (int i = 0; i < outputBusNumDefer; i++) {
			this->removeBus(false);
		}
	}
	else if (outputBusNumDefer < 0) {
		for (int i = 0; i < -outputBusNumDefer; i++) {
			this->addBus(false);
		}
	}

	this->setBusesLayout(pluginBusesLayout);
}

void PluginDecorator::syncBusesNumFromPlugin() {
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	if (this->initFlag) { return; }

	auto currentBusesLayout = this->getBusesLayout();
	auto pluginBusesLayout = this->plugin->getBusesLayout();

	if (pluginBusesLayout == currentBusesLayout) {
		return;
	}

	int inputBusNumDefer =
		currentBusesLayout.inputBuses.size() - pluginBusesLayout.inputBuses.size();
	int outputBusNumDefer =
		currentBusesLayout.outputBuses.size() - pluginBusesLayout.outputBuses.size();

	bool flagChanged = false;

	if ((inputBusNumDefer > 0 && (!this->canPluginAddBus(true)))
		|| (inputBusNumDefer < 0 && (!this->canPluginRemoveBus(true)))) {
		currentBusesLayout.inputBuses.resize(pluginBusesLayout.inputBuses.size());
		flagChanged = true;
	}

	if ((outputBusNumDefer > 0 && (!this->canPluginAddBus(false)))
		|| (outputBusNumDefer < 0 && (!this->canPluginRemoveBus(false)))) {
		currentBusesLayout.outputBuses.resize(pluginBusesLayout.outputBuses.size());
		flagChanged = true;
	}

	if (flagChanged) {
		this->setBusesLayout(currentBusesLayout);
	}
}

void PluginDecorator::filterMIDIMessage(int channel, juce::MidiBuffer& midiMessages) {
	/** Filter MIDI Channel */
	if (channel >= 1 && channel <= 16) {
		juce::MidiBuffer bufferTemp;
		for (auto it = midiMessages.cbegin(); it != midiMessages.cend(); it++) {
			auto message = (*it).getMessage();
			if (message.getChannel() == channel) {
				bufferTemp.addEvent(message, (*it).samplePosition);
			}
		}
		midiMessages = bufferTemp;
	}
}

void PluginDecorator::interceptMIDIMessage(bool shouldMIDIOutput, juce::MidiBuffer& midiMessages) {
	if (!shouldMIDIOutput) {
		midiMessages = juce::MidiBuffer{};
	}
}

void PluginDecorator::interceptMIDICCMessage(bool shouldMIDICCIntercept, juce::MidiBuffer& midiMessages) {
	if (shouldMIDICCIntercept) {
		juce::MidiBuffer bufferTemp;
		for (auto i : midiMessages) {
			auto message = i.getMessage();
			if (!message.isController()) {
				bufferTemp.addEvent(message, i.samplePosition);
			}
		}
		midiMessages = bufferTemp;
	}
}

void PluginDecorator::parseMIDICC(juce::MidiBuffer& midiMessages) {
	/** Param Changed Temp */
	std::map<int, float> paramTemp;

	/** Parse Message */
	for (auto i : midiMessages) {
		/** Get Message */
		auto message = i.getMessage();
		if (!message.isController()) { continue; }

		/** Auto Link Param */
		int paramListenningCC = this->paramListenningCC;
		this->paramListenningCC = -1;
		if (paramListenningCC > -1) {
			this->paramCCList[message.getControllerNumber()] = paramListenningCC;
		}

		/** Get Param Changed */
		int paramIndex = this->paramCCList[message.getControllerNumber()];
		if (paramIndex > -1) {
			paramTemp[paramIndex] = message.getControllerValue() / 127.f;
		}
	}

	/** Set Param Value */
	juce::ScopedReadLock locker(this->pluginLock);
	if (!this->plugin) { return; }
	auto& paramList = this->plugin->getParameters();
	for (auto& i : paramTemp) {
		auto param = paramList[i.first];
		if (param) {
			param->beginChangeGesture();
			param->setValueNotifyingHost(i.second);
			param->endChangeGesture();
		}
	}
}
