﻿#include "ActionRemove.h"

#include "../AudioCore.h"
#include "../misc/PlayPosition.h"
#include "../plugin/Plugin.h"
#include "../Utils.h"
#include <VSP4.h>
using namespace org::vocalsharp::vocalshaper;

ActionRemovePluginBlackList::ActionRemovePluginBlackList(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginBlackList::doAction() {
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin black list while searching plugin.");

	Plugin::getInstance()->removeFromPluginBlackList(this->path);
	
	this->output("Remove from plugin black list.");
	ACTION_RESULT(true);
}

ActionRemovePluginSearchPath::ActionRemovePluginSearchPath(
	const juce::String& path)
	: path(path) {}

bool ActionRemovePluginSearchPath::doAction() {
	ACTION_CHECK_PLUGIN_LOADING(
		"Don't do this while loading plugin.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change plugin search path while searching plugin.");

	Plugin::getInstance()->removeFromPluginSearchPath(this->path);
	
	this->output("Remove from plugin search path.");
	ACTION_RESULT(true);
}

ActionRemoveMixerTrack::ActionRemoveMixerTrack(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveMixerTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioT2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioT2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioSrc2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioI2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioI2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioTrk2T).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2T)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioTrk2O).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2O)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiI2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiI2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiTrk2O).size());
	for (auto [src, dst] : ACTION_DATA(midiTrk2O)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (ACTION_DATA(index) < 0 || ACTION_DATA(index) >= graph->getTrackNum()) { ACTION_RESULT(false); }

		/** Save Connections */
		ACTION_DATA(audioT2Trk) = graph->getTrackInputFromTrackConnections(ACTION_DATA(index));
		ACTION_DATA(audioSrc2Trk) = graph->getTrackInputFromSrcConnections(ACTION_DATA(index));
		ACTION_DATA(audioI2Trk) = graph->getTrackInputFromDeviceConnections(ACTION_DATA(index));
		ACTION_DATA(audioTrk2T) = graph->getTrackOutputToTrackConnections(ACTION_DATA(index));
		ACTION_DATA(audioTrk2O) = graph->getTrackOutputToDeviceConnections(ACTION_DATA(index));
		ACTION_DATA(midiSrc2Trk) = graph->getTrackMidiInputFromSrcConnections(ACTION_DATA(index));
		ACTION_DATA(midiI2Trk) = graph->getTrackMidiInputFromDeviceConnections(ACTION_DATA(index));
		ACTION_DATA(midiTrk2O) = graph->getTrackMidiOutputToDeviceConnections(ACTION_DATA(index));

		/** Save Track State */
		auto track = graph->getTrackProcessor(ACTION_DATA(index));
		if (!track) { ACTION_RESULT(false); }
		auto state = track->serialize(Serializable::createSerializeConfigQuickly());

		auto statePtr = dynamic_cast<vsp4::MixerTrack*>(state.get());
		if (!statePtr) { ACTION_RESULT(false); }
		statePtr->set_bypassed(graph->getTrackBypass(ACTION_DATA(index)));

		ACTION_DATA(data).setSize(state->ByteSizeLong());
		state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

		/** Remove Track */
		graph->removeTrack(ACTION_DATA(index));

		juce::String result;
		result += "Remove Mixer Track: " + juce::String(ACTION_DATA(index)) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
		this->output(result);
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrack::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioT2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioT2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioSrc2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioI2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioI2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioTrk2T).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2T)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(audioTrk2O).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2O)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiI2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiI2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(midiTrk2O).size());
	for (auto [src, dst] : ACTION_DATA(midiTrk2O)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::MixerTrack>();
		if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
			ACTION_RESULT(false);
		}

		/** Add Track */
		graph->insertTrack(ACTION_DATA(index),
			utils::getChannelSet(static_cast<utils::TrackType>(state->type())));
		
		/** Recover Track State */
		auto track = graph->getTrackProcessor(ACTION_DATA(index));
		graph->setTrackBypass(ACTION_DATA(index), state->bypassed());
		track->parse(state.get(), Serializable::createParseConfigQuickly());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioT2Trk)) {
			graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
			graph->setAudioSrc2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioI2Trk)) {
			graph->setAudioI2TrkConnection(dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2T)) {
			graph->setAudioTrk2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioTrk2O)) {
			graph->setAudioTrk2OConnection(src, srcc, dstc);
		}

		for (auto [src, dst] : ACTION_DATA(midiI2Trk)) {
			graph->setMIDII2TrkConnection(dst);
		}

		for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
			graph->setMIDISrc2TrkConnection(src, dst);
		}

		for (auto [src, dst] : ACTION_DATA(midiTrk2O)) {
			graph->setMIDITrk2OConnection(src);
		}

		juce::String result;
		result += "Undo Remove Mixer Track: " + juce::String(ACTION_DATA(index)) + "\n";
		result += "Total Mixer Track Num: " + juce::String(graph->getTrackNum()) + "\n";
		this->output(result);
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackSend::ActionRemoveMixerTrackSend(
	int src, int srcc, int dst, int dstc)
	: ACTION_DB{ src, srcc, dst, dstc } {}

bool ActionRemoveMixerTrackSend::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioTrk2TrkConnected(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioTrk2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackSend::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackSend);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackInputFromDevice::ActionRemoveMixerTrackInputFromDevice(
	int srcc, int dst, int dstc)
	: ACTION_DB{ srcc, dst, dstc } {}

bool ActionRemoveMixerTrackInputFromDevice::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackInputFromDevice);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioI2TrkConnected(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioI2TrkConnection(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("[Device] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackInputFromDevice::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackInputFromDevice);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioI2TrkConnection(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo [Device] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrackInputFromDevice::ActionRemoveSequencerTrackInputFromDevice(
	int srcc, int dst, int dstc)
	: ACTION_DB{ srcc, dst, dstc } {
}

bool ActionRemoveSequencerTrackInputFromDevice::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrackInputFromDevice);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioI2SrcConnected(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioI2SrcConnection(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("[Device] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrackInputFromDevice::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrackInputFromDevice);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioI2SrcConnection(
			ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo [Device] " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackOutput::ActionRemoveMixerTrackOutput(
	int src, int srcc, int dstc)
	: ACTION_DB{ src, srcc, dstc } {}

bool ActionRemoveMixerTrackOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioTrk2OConnected(
			ACTION_DATA(src), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioTrk2OConnection(
			ACTION_DATA(src), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + "[Device] " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioTrk2OConnection(
			ACTION_DATA(src), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + "[Device] " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackSideChainBus::ActionRemoveMixerTrackSideChainBus(
	int track) : ACTION_DB{ track } {}

bool ActionRemoveMixerTrackSideChainBus::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackSideChainBus);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			/** Remove Bus */
			if (track->removeAdditionalAudioBus()) {
				this->output("Remove mixer track side chain bus: " + juce::String(ACTION_DATA(track)));
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackSideChainBus::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackSideChainBus);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			/** Add Bus */
			if (track->addAdditionalAudioBus()) {
				this->output("Undo remove mixer track side chain bus: " + juce::String(ACTION_DATA(track)));
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveEffect::ActionRemoveEffect(
	int track, int effect)
	: ACTION_DB{ track, effect } {}

bool ActionRemoveEffect::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveEffect);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto dock = track->getPluginDock()) {
				/** Check Effect */
				if (ACTION_DATA(effect) < 0 || ACTION_DATA(effect) >= dock->getPluginNum()) { ACTION_RESULT(false); }

				/** Save Effect State */
				auto effect = dock->getPluginProcessor(ACTION_DATA(effect));
				if (!effect) { ACTION_RESULT(false); }
				auto state = effect->serialize(Serializable::createSerializeConfigQuickly());

				auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
				if (!statePtr) { ACTION_RESULT(false); }
				statePtr->set_bypassed(dock->getPluginBypass(ACTION_DATA(effect)));

				ACTION_DATA(data).setSize(state->ByteSizeLong());
				state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

				/** Remove Effect */
				dock->removePlugin(ACTION_DATA(effect));

				this->output("Remove Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "]" + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveEffect::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change effect while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveEffect);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto dock = track->getPluginDock()) {
				/** Prepare Effect State */
				auto state = std::make_unique<vsp4::Plugin>();
				if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
					ACTION_RESULT(false);
				}

				/** Add Effect */
				dock->insertPlugin(ACTION_DATA(effect));

				/** Recover Effect State */
				auto effect = dock->getPluginProcessor(ACTION_DATA(effect));
				dock->setPluginBypass(ACTION_DATA(effect), state->bypassed());
				effect->parse(state.get(), Serializable::createParseConfigQuickly());

				this->output("Undo Remove Plugin: [" + juce::String(ACTION_DATA(track)) + ", " + juce::String(ACTION_DATA(effect)) + "]" + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveInstr::ActionRemoveInstr(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveInstr::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveInstr);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(index))) {
			/** Save Instr State */
			auto instr = track->getInstrProcessor();
			if (!instr) { ACTION_RESULT(false); }
			auto state = instr->serialize(Serializable::createSerializeConfigQuickly());

			auto statePtr = dynamic_cast<vsp4::Plugin*>(state.get());
			if (!statePtr) { ACTION_RESULT(false); }
			statePtr->set_bypassed(track->getInstrumentBypass());

			ACTION_DATA(data).setSize(state->ByteSizeLong());
			state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

			/** Remove Instr */
			track->removeInstr();

			this->output("Remove Instrument: [" + juce::String(ACTION_DATA(index)) + "]" + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveInstr::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");
	ACTION_CHECK_PLUGIN_SEARCHING(
		"Don't change instrument while searching plugin.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveInstr);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(index))) {
			/** Prepare Instr State */
			auto state = std::make_unique<vsp4::Plugin>();
			if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
				ACTION_RESULT(false);
			}

			/** Add Instr */
			track->prepareInstr();

			/** Recover Instr State */
			auto instr = track->getInstrProcessor();
			track->setInstrumentBypass(state->bypassed());
			instr->parse(state.get(), Serializable::createParseConfigQuickly());

			this->output("Undo Remove Instrument: [" + juce::String(ACTION_DATA(index)) + "]" + "\n");
			ACTION_RESULT(true);
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveInstrParamCCConnection::ActionRemoveInstrParamCCConnection(
	int instr, int cc)
	: ACTION_DB{ instr, cc } {}

bool ActionRemoveInstrParamCCConnection::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveInstrParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				ACTION_DATA(param) = instr->getCCParamConnection(ACTION_DATA(cc));

				instr->removeCCParamConnection(ACTION_DATA(cc));

				this->output("Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveInstrParamCCConnection::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveInstrParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(instr))) {
			if (auto instr = track->getInstrProcessor()) {
				instr->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

				this->output("Undo Remove Instr Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
				ACTION_RESULT(true);
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveEffectParamCCConnection::ActionRemoveEffectParamCCConnection(
	int track, int effect, int cc)
	: ACTION_DB{ track, effect, cc } {}

bool ActionRemoveEffectParamCCConnection::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveEffectParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					ACTION_DATA(param) = effect->getCCParamConnection(ACTION_DATA(cc));

					effect->removeCCParamConnection(ACTION_DATA(cc));

					this->output("Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

bool ActionRemoveEffectParamCCConnection::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveEffectParamCCConnection);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getTrackProcessor(ACTION_DATA(track))) {
			if (auto pluginDock = track->getPluginDock()) {
				if (auto effect = pluginDock->getPluginProcessor(ACTION_DATA(effect))) {
					effect->connectParamCC(ACTION_DATA(param), ACTION_DATA(cc));

					this->output("Undo Remove Effect Param MIDI CC Connection: " "MIDI CC " + juce::String(ACTION_DATA(cc)) + "\n");
					ACTION_RESULT(true);
				}
			}
		}
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackMidiInput::ActionRemoveMixerTrackMidiInput(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveMixerTrackMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDII2TrkConnected(ACTION_DATA(index))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDII2TrkConnection(ACTION_DATA(index));

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(ACTION_DATA(index)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackMidiInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2TrkConnection(ACTION_DATA(index));

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(ACTION_DATA(index)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrackMidiInput::ActionRemoveSequencerTrackMidiInput(int index)
	: ACTION_DB{ index } {
}

bool ActionRemoveSequencerTrackMidiInput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrackMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDII2SrcConnected(ACTION_DATA(index))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDII2SrcConnection(ACTION_DATA(index));

		this->output(juce::String("[MIDI Input]") + " - " + juce::String(ACTION_DATA(index)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrackMidiInput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrackMidiInput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDII2SrcConnection(ACTION_DATA(index));

		this->output(juce::String("Undo [MIDI Input]") + " - " + juce::String(ACTION_DATA(index)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveMixerTrackMidiOutput::ActionRemoveMixerTrackMidiOutput(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveMixerTrackMidiOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveMixerTrackMidiOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDITrk2OConnected(ACTION_DATA(index))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDITrk2OConnection(ACTION_DATA(index));

		this->output(juce::String(ACTION_DATA(index)) + " - " + "[MIDI Output]" + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveMixerTrackMidiOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveMixerTrackMidiOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDITrk2OConnection(ACTION_DATA(index));

		this->output("Undo " + juce::String(ACTION_DATA(index)) + " - " + "[MIDI Output]" + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrack::ActionRemoveSequencerTrack(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveSequencerTrack::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioSrc2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Check Track */
		if (ACTION_DATA(index) < 0 || ACTION_DATA(index) >= graph->getSourceNum()) { ACTION_RESULT(false); }

		/** Save Connections */
		ACTION_DATA(audioSrc2Trk) = graph->getSourceOutputToTrackConnections(ACTION_DATA(index));
		ACTION_DATA(midiSrc2Trk) = graph->getSourceMidiOutputToTrackConnections(ACTION_DATA(index));

		/** Save Track State */
		auto track = graph->getSourceProcessor(ACTION_DATA(index));
		if (!track) { ACTION_RESULT(false); }
		auto state = track->serialize(Serializable::createSerializeConfigQuickly());

		auto statePtr = dynamic_cast<vsp4::SeqTrack*>(state.get());
		if (!statePtr) { ACTION_RESULT(false); }
		statePtr->set_bypassed(graph->getSourceBypass(ACTION_DATA(index)));

		ACTION_DATA(data).setSize(state->ByteSizeLong());
		state->SerializeToArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize());

		/** Remove Track */
		graph->removeSource(ACTION_DATA(index));

		juce::String result;
		result += "Remove Sequencer Track: " + juce::String(ACTION_DATA(index)) + "\n";
		result += "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n";
		ACTION_RESULT(true);
	}

	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrack::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrack);
	ACTION_WRITE_DB();

	writeRecoverySizeValue(ACTION_DATA(audioSrc2Trk).size());
	for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(srcc);
		writeRecoveryIntValue(dst);
		writeRecoveryIntValue(dstc);
	}

	writeRecoverySizeValue(ACTION_DATA(midiSrc2Trk).size());
	for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
		writeRecoveryIntValue(src);
		writeRecoveryIntValue(dst);
	}

	writeRecoverySizeValue(ACTION_DATA(data).getSize());
	writeRecoveryDataBlockValue((const char*)(ACTION_DATA(data).getData()), ACTION_DATA(data).getSize());

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		/** Prepare Track State */
		auto state = std::make_unique<vsp4::SeqTrack>();
		if (!state->ParseFromArray(ACTION_DATA(data).getData(), ACTION_DATA(data).getSize())) {
			ACTION_RESULT(false);
		}

		/** Add Track */
		graph->insertSource(ACTION_DATA(index),
			utils::getChannelSet(static_cast<utils::TrackType>(state->type())));

		/** Recover Track State */
		auto track = graph->getSourceProcessor(ACTION_DATA(index));
		graph->setSourceBypass(ACTION_DATA(index), state->bypassed());
		track->parse(state.get(), Serializable::createParseConfigQuickly());

		/** Recover Connections */
		for (auto [src, srcc, dst, dstc] : ACTION_DATA(audioSrc2Trk)) {
			graph->setAudioSrc2TrkConnection(src, dst, srcc, dstc);
		}

		for (auto [src, dst] : ACTION_DATA(midiSrc2Trk)) {
			graph->setMIDISrc2TrkConnection(src, dst);
		}

		juce::String result;
		result += "Undo Remove Sequencer Track: " + juce::String(ACTION_DATA(index)) + "\n";
		result += "Total Sequencer Track Num: " + juce::String(graph->getSourceNum()) + "\n";
		this->output(result);
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrackMidiOutputToMixer::ActionRemoveSequencerTrackMidiOutputToMixer(
	int src, int dst)
	: ACTION_DB{ src, dst } {}

bool ActionRemoveSequencerTrackMidiOutputToMixer::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrackMidiOutputToMixer);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isMIDISrc2TrkConnected(ACTION_DATA(src), ACTION_DATA(dst))) {
			ACTION_RESULT(false);
		}

		graph->removeMIDISrc2TrkConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output(juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrackMidiOutputToMixer::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrackMidiOutputToMixer);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setMIDISrc2TrkConnection(ACTION_DATA(src), ACTION_DATA(dst));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + " - " + juce::String(ACTION_DATA(dst)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerTrackOutput::ActionRemoveSequencerTrackOutput(
	int src, int srcc, int dst, int dstc)
	: ACTION_DB{ src, srcc, dst, dstc } {}

bool ActionRemoveSequencerTrackOutput::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (!graph->isAudioSrc2TrkConnected(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc))) {
			ACTION_RESULT(false);
		}

		graph->removeAudioSrc2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output(juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerTrackOutput::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerTrackOutput);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		graph->setAudioSrc2TrkConnection(
			ACTION_DATA(src), ACTION_DATA(dst), ACTION_DATA(srcc), ACTION_DATA(dstc));

		this->output("Undo " + juce::String(ACTION_DATA(src)) + ", " + juce::String(ACTION_DATA(srcc)) + " - " + juce::String(ACTION_DATA(dst)) + ", " + juce::String(ACTION_DATA(dstc)) + " (Removed)\n");
		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

ActionRemoveSequencerBlock::ActionRemoveSequencerBlock(
	int seqIndex, int index)
	: ACTION_DB{ seqIndex, index } {}

bool ActionRemoveSequencerBlock::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(seqIndex))) {
			std::tie(ACTION_DATA(startTime), ACTION_DATA(endTime), ACTION_DATA(offset))
				= track->getSeq(ACTION_DATA(index));
			track->removeSeq(ACTION_DATA(index));

			this->output("Remove sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n"
				+ "Total sequencer blocks: " + juce::String(track->getSeqNum()) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't remove sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n");
	ACTION_RESULT(false);
}

bool ActionRemoveSequencerBlock::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveSequencerBlock);
	ACTION_WRITE_DB();

	if (auto graph = AudioCore::getInstance()->getGraph()) {
		if (auto track = graph->getSourceProcessor(ACTION_DATA(seqIndex))) {
			track->addSeq(
				{ ACTION_DATA(startTime), ACTION_DATA(endTime), ACTION_DATA(offset) });

			this->output("Undo remove sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n"
				+ "Total sequencer blocks: " + juce::String(track->getSeqNum()) + "\n");
			ACTION_RESULT(true);
		}
	}
	this->output("Can't undo remove sequencer block [" + juce::String(ACTION_DATA(seqIndex)) + "]\n");
	ACTION_RESULT(false);
}

ActionRemoveTempo::ActionRemoveTempo(int index)
	: ACTION_DB{ index } {}

bool ActionRemoveTempo::doAction() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE(ActionRemoveTempo);
	ACTION_WRITE_DB();

	if (ACTION_DATA(index) >= 0 && ACTION_DATA(index) < PlayPosition::getInstance()->getTempoLabelNum()) {
		ACTION_DATA(isTempo) = PlayPosition::getInstance()->isTempoLabelTempoEvent(ACTION_DATA(index));
		ACTION_DATA(time) = PlayPosition::getInstance()->getTempoLabelTime(ACTION_DATA(index));

		if (ACTION_DATA(isTempo)) {
			ACTION_DATA(tempo) = PlayPosition::getInstance()->getTempoLabelTempo(ACTION_DATA(index));
		}
		else {
			std::tie(ACTION_DATA(numerator), ACTION_DATA(denominator)) =
				PlayPosition::getInstance()->getTempoLabelBeat(ACTION_DATA(index));
		}

		PlayPosition::getInstance()->removeTempoLabel(ACTION_DATA(index));

		ACTION_RESULT(true);
	}
	ACTION_RESULT(false);
}

bool ActionRemoveTempo::undo() {
	ACTION_CHECK_RENDERING(
		"Don't do this while rendering.");

	ACTION_UNSAVE_PROJECT();

	ACTION_WRITE_TYPE_UNDO(ActionRemoveTempo);
	ACTION_WRITE_DB();

	if (ACTION_DATA(isTempo)) {
		PlayPosition::getInstance()->addTempoLabelTempo(
			ACTION_DATA(time), ACTION_DATA(tempo), ACTION_DATA(index));
	}
	else {
		PlayPosition::getInstance()->addTempoLabelBeat(
			ACTION_DATA(time), ACTION_DATA(numerator), ACTION_DATA(denominator), ACTION_DATA(index));
	}
	ACTION_RESULT(true);
}
