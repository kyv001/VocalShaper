﻿#pragma once

#include <JuceHeader.h>
#include <FlowUI.h>
#include "../dataModel/MainMenuModel.h"
#include "MenuBarComponent.h"
#include "SysStatusComponent.h"

class ToolBar final : public flowUI::FlowComponent {
public:
	ToolBar();
	~ToolBar();

	void resized() override;
	void paint(juce::Graphics& g) override;

private:
	std::unique_ptr<MainMenuModel> mainMenuModel = nullptr;
	std::unique_ptr<MenuBarComponent> mainMenuBar = nullptr;
	std::unique_ptr<SysStatusComponent> sysStatus = nullptr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolBar)
};
