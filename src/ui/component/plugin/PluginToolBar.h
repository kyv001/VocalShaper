﻿#pragma once

#include <JuceHeader.h>
#include "../../misc/PluginType.h"
#include "../../../audioCore/AC_API.h"

class PluginEditorContent;

class PluginToolBar final : public juce::Component {
public:
	PluginToolBar(PluginEditorContent* parent,
		quickAPI::PluginHolder plugin, PluginType type);

	void resized() override;

	void update();

private:
	PluginEditorContent* const parent = nullptr;
	const quickAPI::PluginHolder plugin;
	const PluginType type;

	std::unique_ptr<juce::Drawable> bypassIcon = nullptr;
	std::unique_ptr<juce::Drawable> bypassIconOn = nullptr;
	std::unique_ptr<juce::Drawable> configIcon = nullptr;
	std::unique_ptr<juce::Drawable> configIconOn = nullptr;
	std::unique_ptr<juce::Drawable> pinIcon = nullptr;
	std::unique_ptr<juce::Drawable> pinIconOn = nullptr;
	std::unique_ptr<juce::Drawable> loadIcon = nullptr;
	std::unique_ptr<juce::Drawable> saveIcon = nullptr;
	std::unique_ptr<juce::Drawable> moreIcon = nullptr;
	std::unique_ptr<juce::DrawableButton> bypassButton = nullptr;
	std::unique_ptr<juce::DrawableButton> configButton = nullptr;
	std::unique_ptr<juce::DrawableButton> pinButton = nullptr;
	std::unique_ptr<juce::DrawableButton> loadButton = nullptr;
	std::unique_ptr<juce::DrawableButton> saveButton = nullptr;
	std::unique_ptr<juce::TextButton> scaleButton = nullptr;
	std::unique_ptr<juce::DrawableButton> moreButton = nullptr;

	void bypass();
	void config();
	void pin();
	void load();
	void save();
	void changeScale();
	void more();

	void judgeScaleResult(int result);

	float scale = 1.0;
	void setEditorScale(float scale);
	float getEditorScale() const;

	juce::PopupMenu createMoreMenu() const;
	juce::PopupMenu createScaleMenu() const;
};
