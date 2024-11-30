﻿#include "CoreCallbacks.h"
#include "../../audioCore/AC_API.h"

CoreCallbacks::CoreCallbacks() {
	UICallbackAPI<const juce::String&, const juce::String&>::set(UICallbackType::ErrorAlert,
		[](const juce::String& title, const juce::String& mes) {
			CoreCallbacks::getInstance()->invokeError(title, mes);
		});
	UICallbackAPI<bool>::set(UICallbackType::PlayStateChanged,
		[](bool status) {
			CoreCallbacks::getInstance()->invokePlayingStatus(status);
		});
	UICallbackAPI<bool>::set(UICallbackType::RecordStateChanged,
		[](bool status) {
			CoreCallbacks::getInstance()->invokeRecordingStatus(status);
		});
	UICallbackAPI<const juce::String&>::set(UICallbackType::ErrorMessage,
		[](const juce::String& mes) {
			CoreCallbacks::getInstance()->invokeErrorMes(mes);
		});
	UICallbackAPI<bool>::set(UICallbackType::PluginSearchStateChanged,
		[](bool state) {
			CoreCallbacks::getInstance()->invokeSearchPlugin(state);
		});
	UICallbackAPI<int>::set(UICallbackType::SourceChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeSourceChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::InstrChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeInstrChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeTrackChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackGainChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeTrackGainChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackPanChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeTrackPanChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackFaderChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeTrackFaderChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::TrackMuteChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeTrackMuteChanged(index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::EffectChanged,
		[](int track, int index) {
			CoreCallbacks::getInstance()->invokeEffectChanged(track, index);
		});
	UICallbackAPI<int>::set(UICallbackType::SeqChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeSeqChanged(index);
		});
	UICallbackAPI<int, int>::set(UICallbackType::SeqBlockChanged,
		[](int track, int index) {
			CoreCallbacks::getInstance()->invokeSeqBlockChanged(track, index);
		});
	UICallbackAPI<void>::set(UICallbackType::TempoChanged,
		[] {
			CoreCallbacks::getInstance()->invokeTempoChanged();
		});
	UICallbackAPI<int>::set(UICallbackType::SeqMuteChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeSeqMuteChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::SeqInputMonitoringChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeSeqInputMonitoringChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::SeqRecChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeSeqRecChanged(index);
		});
	UICallbackAPI<int>::set(UICallbackType::SeqDataRefChanged,
		[](int index) {
			CoreCallbacks::getInstance()->invokeSeqDataRefChanged(index);
		});
	UICallbackAPI<const juce::String&>::set(UICallbackType::PluginSearchMessage,
		[](const juce::String& mes) {
			CoreCallbacks::getInstance()->invokePluginSearchMes(mes);
		});
	UICallbackAPI<int, bool>::set(UICallbackType::SynthStateChanged,
		[](int index, bool status) {
			CoreCallbacks::getInstance()->invokeSynthStatus(index, status);
		});
	UICallbackAPI<const std::set<int>&>::set(UICallbackType::SourceRecord,
		[](const std::set<int>& trackList) {
			CoreCallbacks::getInstance()->invokeSourceRecord(trackList);
		});
}

void CoreCallbacks::addError(const ErrorCallback& callback) {
	this->error.add(callback);
}

void CoreCallbacks::addPlayingStatus(const PlayingStatusCallback& callback) {
	this->playingStatus.add(callback);
}

void CoreCallbacks::addRecordingStatus(const RecordingStatusCallback& callback) {
	this->recordingingStatus.add(callback);
}

void CoreCallbacks::addErrorMes(const ErrorMesCallback& callback) {
	this->errorMes.add(callback);
}

void CoreCallbacks::addSearchPlugin(const SearchPluginCallback& callback) {
	this->searchPlugin.add(callback);
}

void CoreCallbacks::addSourceChanged(const SourceChangedCallback& callback) {
	this->sourceChanged.add(callback);
}

void CoreCallbacks::addInstrChanged(const InstrChangedCallback& callback) {
	this->instrChanged.add(callback);
}

void CoreCallbacks::addTrackChanged(const TrackChangedCallback& callback) {
	this->trackChanged.add(callback);
}

void CoreCallbacks::addTrackGainChanged(const TrackChangedCallback& callback) {
	this->trackGainChanged.add(callback);
}

void CoreCallbacks::addTrackPanChanged(const TrackChangedCallback& callback) {
	this->trackPanChanged.add(callback);
}

void CoreCallbacks::addTrackFaderChanged(const TrackChangedCallback& callback) {
	this->trackFaderChanged.add(callback);
}

void CoreCallbacks::addTrackMuteChanged(const TrackChangedCallback& callback) {
	this->trackMuteChanged.add(callback);
}

void CoreCallbacks::addEffectChanged(const EffectChangedCallback& callback) {
	this->effectChanged.add(callback);
}

void CoreCallbacks::addSeqChanged(const SeqChangedCallback& callback) {
	this->seqChanged.add(callback);
}

void CoreCallbacks::addSeqBlockChanged(const SeqBlockChangedCallback& callback) {
	this->seqBlockChanged.add(callback);
}

void CoreCallbacks::addTempoChanged(const TempoChangedCallback& callback) {
	this->tempoChanged.add(callback);
}

void CoreCallbacks::addSeqMuteChanged(const SeqMuteChangedCallback& callback) {
	this->seqMuteChanged.add(callback);
}

void CoreCallbacks::addSeqRecChanged(const SeqRecChangedCallback& callback) {
	this->seqRecChanged.add(callback);
}

void CoreCallbacks::addSeqInputMonitoringChanged(const SeqInputMonitoringChangedCallback& callback) {
	this->seqInputMonitoringChanged.add(callback);
}

void CoreCallbacks::addSeqDataRefChanged(const SeqDataRefChangedCallback& callback) {
	this->seqDataRefChanged.add(callback);
}

void CoreCallbacks::addPluginSearchMes(const PluginSearchMesCallback& callback) {
	this->pluginSearchMesChanged.add(callback);
}

void CoreCallbacks::addSynthStatus(const SynthStatusCallback& callback) {
	this->synthStatus.add(callback);
}

void CoreCallbacks::addSourceRecord(const SourceRecordCallback& callback) {
	this->sourceRecord.add(callback);
}

void CoreCallbacks::addEditingSeqChanged(const EditingSeqChangedCallback& callback) {
	this->editingSeqChanged.add(callback);
}

void CoreCallbacks::invokeError(
	const juce::String& title, const juce::String& mes) const {
	for (auto& i : this->error) {
		i(title, mes);
	}
}

void CoreCallbacks::invokePlayingStatus(bool status) const {
	for (auto& i : this->playingStatus) {
		i(status);
	}
}

void CoreCallbacks::invokeRecordingStatus(bool status) const {
	for (auto& i : this->recordingingStatus) {
		i(status);
	}
}

void CoreCallbacks::invokeErrorMes(const juce::String& mes) const {
	for (auto& i : this->errorMes) {
		i(mes);
	}
}

void CoreCallbacks::invokeSearchPlugin(bool status) const {
	for (auto& i : this->searchPlugin) {
		i(status);
	}
}

void CoreCallbacks::invokeSourceChanged(int index) const {
	for (auto& i : this->sourceChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeInstrChanged(int index) const {
	for (auto& i : this->instrChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeTrackChanged(int index) const {
	for (auto& i : this->trackChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeTrackGainChanged(int index) const {
	for (auto& i : this->trackGainChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeTrackPanChanged(int index) const {
	for (auto& i : this->trackPanChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeTrackFaderChanged(int index) const {
	for (auto& i : this->trackFaderChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeTrackMuteChanged(int index) const {
	for (auto& i : this->trackMuteChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeEffectChanged(int track, int index) const {
	for (auto& i : this->effectChanged) {
		i(track, index);
	}
}

void CoreCallbacks::invokeSeqChanged(int index) const {
	for (auto& i : this->seqChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeSeqBlockChanged(int track, int index) const {
	for (auto& i : this->seqBlockChanged) {
		i(track, index);
	}
}

void CoreCallbacks::invokeTempoChanged() const {
	for (auto& i : this->tempoChanged) {
		i();
	}
}

void CoreCallbacks::invokeSeqMuteChanged(int index) const {
	for (auto& i : this->seqMuteChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeSeqRecChanged(int index) const {
	for (auto& i : this->seqRecChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeSeqInputMonitoringChanged(int index) const {
	for (auto& i : this->seqInputMonitoringChanged) {
		i(index);
	}
}

void CoreCallbacks::invokeSeqDataRefChanged(int index) const {
	for (auto& i : this->seqDataRefChanged) {
		i(index);
	}
}

void CoreCallbacks::invokePluginSearchMes(const juce::String& mes) const {
	for (auto& i : this->pluginSearchMesChanged) {
		i(mes);
	}
}

void CoreCallbacks::invokeSynthStatus(int index, bool status) const {
	for (auto& i : this->synthStatus) {
		i(index, status);
	}
}

void CoreCallbacks::invokeSourceRecord(const std::set<int>& trackList) const {
	for (auto& i : this->sourceRecord) {
		i(trackList);
	}
}

void CoreCallbacks::invokeEditingSeqChanged(int index) const {
	for (auto& i : this->editingSeqChanged) {
		i(index);
	}
}

CoreCallbacks* CoreCallbacks::getInstance() {
	return CoreCallbacks::instance ? CoreCallbacks::instance
		: (CoreCallbacks::instance = new CoreCallbacks{});
}

void CoreCallbacks::releaseInstance() {
	if (CoreCallbacks::instance) {
		delete CoreCallbacks::instance;
		CoreCallbacks::instance = nullptr;
	}
}

CoreCallbacks* CoreCallbacks::instance = nullptr;
