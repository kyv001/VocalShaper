﻿#include "GUICommandTarget.h"
#include "CommandTypes.h"
#include "../../audioCore/AC_API.h"
#include <FlowUI.h>

juce::ApplicationCommandTarget* GUICommandTarget::getNextCommandTarget() {
	return nullptr;
}

void GUICommandTarget::getAllCommands(
	juce::Array<juce::CommandID>& commands) {
	commands = {
		(juce::CommandID)(GUICommandType::CloseEditor),

		(juce::CommandID)(GUICommandType::Copy),
		(juce::CommandID)(GUICommandType::Cut),
		(juce::CommandID)(GUICommandType::Paste),
		(juce::CommandID)(GUICommandType::Clipboard),
		(juce::CommandID)(GUICommandType::SelectAll),
		(juce::CommandID)(GUICommandType::Delete)
	};
}

void GUICommandTarget::getCommandInfo(
	juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
	switch ((GUICommandType)(commandID)) {
	case GUICommandType::CloseEditor:
		result.setInfo(TRANS("Close Editor"), TRANS("Close and exit VocalShaper."), TRANS("File"), 0);
		result.setActive(!quickAPI::checkRendering());
		break;

	case GUICommandType::Copy:
		result.setInfo(TRANS("Copy"), TRANS("Copy selected items in the actived editor to clipboard."), TRANS("Edit"), 0);
		result.addDefaultKeypress('c', juce::ModifierKeys::ctrlModifier);
		result.setActive(false);
		break;
	case GUICommandType::Cut:
		result.setInfo(TRANS("Cut"), TRANS("Cut selected items in the actived editor to clipboard."), TRANS("Edit"), 0);
		result.addDefaultKeypress('x', juce::ModifierKeys::ctrlModifier);
		result.setActive(false);
		break;
	case GUICommandType::Paste:
		result.setInfo(TRANS("Paste"), TRANS("Paste items to actived editor from clipboard."), TRANS("Edit"), 0);
		result.addDefaultKeypress('v', juce::ModifierKeys::ctrlModifier);
		result.setActive(false);
		break;
	case GUICommandType::Clipboard:
		result.setInfo(TRANS("Clipboard"), TRANS("Show items in clipboard."), TRANS("Edit"), 0);
		result.setActive(false);
		break;
	case GUICommandType::SelectAll:
		result.setInfo(TRANS("Select All"), TRANS("Select all items in the actived editor."), TRANS("Edit"), 0);
		result.addDefaultKeypress('a', juce::ModifierKeys::ctrlModifier);
		result.setActive(false);
		break;
	case GUICommandType::Delete:
		result.setInfo(TRANS("Delete"), TRANS("Delete selected item in the activer editor."), TRANS("Edit"), 0);
		result.addDefaultKeypress(juce::KeyPress::deleteKey, juce::ModifierKeys::noModifiers);
		result.addDefaultKeypress(juce::KeyPress::backspaceKey, juce::ModifierKeys::noModifiers);
		result.setActive(false);
		break;
	}
}

bool GUICommandTarget::perform(
	const juce::ApplicationCommandTarget::InvocationInfo& info) {
	switch ((GUICommandType)(info.commandID)) {
	case GUICommandType::CloseEditor:
		if (flowUI::FlowWindowHub::getAppExitHook()()) {
			juce::JUCEApplication::getInstance()->systemRequestedQuit();
		}
		return true;

	case GUICommandType::Copy:
		/** TODO */
		return true;
	case GUICommandType::Cut:
		/** TODO */
		return true;
	case GUICommandType::Paste:
		/** TODO */
		return true;
	case GUICommandType::Clipboard:
		/** TODO */
		return true;
	case GUICommandType::SelectAll:
		/** TODO */
		return true;
	case GUICommandType::Delete:
		/** TODO */
		return true;
	}

	return false;
}

GUICommandTarget* GUICommandTarget::getInstance() {
	return GUICommandTarget::instance ? GUICommandTarget::instance
		: (GUICommandTarget::instance = new GUICommandTarget{});
}

void GUICommandTarget::releaseInstance() {
	if (GUICommandTarget::instance) {
		delete GUICommandTarget::instance;
		GUICommandTarget::instance = nullptr;
	}
}

GUICommandTarget* GUICommandTarget::instance = nullptr;
