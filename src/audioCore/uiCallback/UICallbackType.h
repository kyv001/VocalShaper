#pragma once

enum class UICallbackType : int {
	ErrorAlert,
	PlayStateChanged,
	RecordStateChanged,
	ErrorMessage,
	PluginSearchStateChanged,
	SourceChanged,
	InstrChanged,
	TrackChanged,
	TrackGainChanged,
	TrackPanChanged,
	TrackFaderChanged,
	TrackMuteChanged,
	EffectChanged,
	SeqChanged,
	SeqBlockChanged,
	SeqMuteChanged,
	SeqInputMonitoringChanged,
	SeqRecChanged,
	TempoChanged,
	SeqDataRefChanged,
	PluginSearchMessage,
	SynthStateChanged,
	SourceRecord,

	TypeMaxNum
};
