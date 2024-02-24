﻿#pragma once

enum class ActionType : unsigned int {
	ActionAddMixerTrack = 0x0001,
	ActionAddMixerTrackSend,
	ActionAddMixerTrackInputFromDevice,
	ActionAddMixerTrackOutput,
	ActionAddEffect,
	ActionAddInstr,
	ActionAddMixerTrackMidiInput,
	ActionAddMixerTrackMidiOutput,
	ActionAddAudioSourceThenLoad,
	ActionAddAudioSourceThenInit,
	ActionAddMidiSourceThenLoad,
	ActionAddMidiSourceThenInit,
	ActionAddSequencerTrack,
	ActionAddSequencerTrackMidiOutputToMixer,
	ActionAddSequencerTrackOutput,
	ActionAddSequencerSourceInstance,
	ActionAddRecorderSourceInstance,
	ActionAddMixerTrackSideChainBus,

	ActionCloneSource = 0x0101,
	ActionSave,
	ActionReloadSource,

	ActionRemoveMixerTrack = 0x0201,
	ActionRemoveMixerTrackSend,
	ActionRemoveMixerTrackInputFromDevice,
	ActionRemoveMixerTrackOutput,
	ActionRemoveEffect,
	ActionRemoveInstr,
	ActionRemoveInstrParamCCConnection,
	ActionRemoveEffectParamCCConnection,
	ActionRemoveMixerTrackMidiInput,
	ActionRemoveMixerTrackMidiOutput,
	ActionRemoveSource,
	ActionRemoveSequencerTrack,
	ActionRemoveSequencerTrackMidiOutputToMixer,
	ActionRemoveSequencerTrackOutput,
	ActionRemoveSequencerSourceInstance,
	ActionRemoveRecorderSourceInstance,
	ActionRemoveMixerTrackSideChainBus,

	ActionSetMixerTrackGain = 0x0301,
	ActionSetMixerTrackPan,
	ActionSetMixerTrackSlider,
	ActionSetEffectBypass,
	ActionSetInstrBypass,
	ActionSetInstrMidiChannel,
	ActionSetEffectMidiChannel,
	ActionSetInstrParamValue,
	ActionSetEffectParamValue,
	ActionSetInstrParamConnectToCC,
	ActionSetEffectParamConnectToCC,
	ActionSetInstrMidiCCIntercept,
	ActionSetEffectMidiCCIntercept,
	ActionSetSequencerTrackBypass,
	ActionSetSourceSynthesizer,
	ActionSetSourceName,
	ActionSetSourceSynthDst,
	ActionSetEffectBypassByPtr,
	ActionSetInstrBypassByPtr,
	ActionSetInstrMidiChannelByPtr,
	ActionSetEffectMidiChannelByPtr,
	ActionSetInstrMidiCCInterceptByPtr,
	ActionSetEffectMidiCCInterceptByPtr,
	ActionSetInstrMidiOutputByPtr,
	ActionSetEffectMidiOutputByPtr,
	ActionSetInstrParamConnectToCCByPtr,
	ActionSetEffectParamConnectToCCByPtr,
	ActionSetMixerTrackName,
	ActionSetMixerTrackColor,
	ActionSetMixerTrackMute,
	ActionSetEffectIndex
};
