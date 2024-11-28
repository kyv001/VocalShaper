#include "SeqTrackInputMonitoringComponent.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"
#include "../../misc/CoreActions.h"
#include "../../Utils.h"
#include "../../../audioCore/AC_API.h"

SeqTrackInputMonitoringComponent::SeqTrackInputMonitoringComponent() {
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::InputMonitoringButton));
}

void SeqTrackInputMonitoringComponent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;

	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;

	float textFontHeight = buttonWidth * 0.7;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(this->inputMonitoring
		? juce::TextButton::ColourIds::buttonOnColourId
		: juce::TextButton::ColourIds::buttonColourId);
	juce::Colour textColor = laf.findColour(this->inputMonitoring
		? juce::TextButton::ColourIds::textColourOnId
		: juce::TextButton::ColourIds::textColourOffId);

	/** Font */
	juce::Font textFont(juce::FontOptions{ textFontHeight });

	/** Button */
	juce::Rectangle<float> buttonRect(
		this->getWidth() / 2.f - buttonWidth / 2.f,
		this->getHeight() / 2.f - buttonHeight / 2.f,
		buttonWidth, buttonHeight);
	g.setColour(backgroundColor);
	g.fillRect(buttonRect);

	g.setColour(textColor);
	g.drawRect(buttonRect, lineThickness);

	g.setFont(textFont);
	g.drawFittedText("I", buttonRect.toNearestInt(),
		juce::Justification::centred, 1, 0.f);
}

void SeqTrackInputMonitoringComponent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void SeqTrackInputMonitoringComponent::mouseMove(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;
	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;
	juce::Rectangle<float> buttonRect(
		this->getWidth() / 2.f - buttonWidth / 2.f,
		this->getHeight() / 2.f - buttonHeight / 2.f,
		buttonWidth, buttonHeight);

	/** Cursor */
	this->setMouseCursor(buttonRect.contains(event.position)
		? juce::MouseCursor::PointingHandCursor
		: juce::MouseCursor::NormalCursor);
}

void SeqTrackInputMonitoringComponent::mouseUp(const juce::MouseEvent& event) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	float lineThickness = screenSize.getHeight() * 0.001;
	int buttonWidth = std::min(this->getWidth(), this->getHeight()) - lineThickness;
	int buttonHeight = buttonWidth;
	juce::Rectangle<float> buttonRect(
		this->getWidth() / 2.f - buttonWidth / 2.f,
		this->getHeight() / 2.f - buttonHeight / 2.f,
		buttonWidth, buttonHeight);

	if (buttonRect.contains(event.position)) {
		if (event.mods.isLeftButtonDown()) {
			this->changeInputMonitoring();
		}
		else if (event.mods.isRightButtonDown()) {
			this->changeInputMonitoring();
		}
	}
}

void SeqTrackInputMonitoringComponent::update(int index) {
	this->index = index;
	if (index > -1) {
		this->inputMonitoring = quickAPI::getSeqTrackInputMonitoring(index);

		this->repaint();
	}
}

void SeqTrackInputMonitoringComponent::changeInputMonitoring() {
	CoreActions::setSeqInputMonitoring(this->index, !(this->inputMonitoring));
}
