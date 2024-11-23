﻿#include "ActionDispatcher.h"
#include "ActionUndoableBase.h"
#include "../recovery/DataControl.hpp"

ActionDispatcher::ActionDispatcher() {
	/** Undo Manager */
	this->manager = std::make_unique<juce::UndoManager>();

	/** Recovery */
	initRecoveryMemoryBlock();
}

ActionDispatcher::~ActionDispatcher() {
	/** Recovery */
	destoryRecoveryMemoryBlock();
}

const juce::UndoManager& ActionDispatcher::getActionManager() const {
	return *(this->manager.get());
}

bool ActionDispatcher::dispatch(std::unique_ptr<ActionBase> action) {
	if (!action) { return false; }

	if (dynamic_cast<ActionUndoableBase*>(action.get())) {
    	juce::String name = action->getName();
		//this->manager->beginNewTransaction(action->getName());
		return this->manager->perform(
			dynamic_cast<ActionUndoableBase*>(action.release()), name);
	}
	return action->doAction();
}

void ActionDispatcher::clearUndoList() {
	this->manager->clearUndoHistory();
}

bool ActionDispatcher::performUndo() {
	return this->manager->undo();
}

bool ActionDispatcher::performRedo() {
	return this->manager->redo();
}

void ActionDispatcher::setOutput(
	const OutputCallback& output, const ErrorCallback& error) {
	this->output = output;
	this->error = error;
}

void ActionDispatcher::removeOutput() {
	this->output = [](const juce::String&) {};
	this->error = [](const juce::String&) {};
}

void ActionDispatcher::outputInternal(const juce::String& mes) {
	this->output(mes);
}

void ActionDispatcher::errorInternal(const juce::String& mes) {
	this->error(mes);
}

ActionDispatcher* ActionDispatcher::getInstance() {
	return ActionDispatcher::instance ? ActionDispatcher::instance : (ActionDispatcher::instance = new ActionDispatcher);
}

void ActionDispatcher::releaseInstance() {
	if (ActionDispatcher::instance) {
		delete ActionDispatcher::instance;
		ActionDispatcher::instance = nullptr;
	}
}

ActionDispatcher* ActionDispatcher::instance = nullptr;
