﻿#include "Scroller.h"
#include "../../lookAndFeel/LookAndFeelFactory.h"

Scroller::Scroller(bool vertical,
	const ViewSizeFunc& viewSizeCallback,
	const ItemNumFunc& itemNumCallback,
	const ItemSizeLimitFunc& itemSizeLimitCallback,
	const UpdatePosFunc& updatePosCallback,
	const PaintPreviewFunc& paintPreviewCallback,
	const PaintItemPreviewFunc& paintItemPreviewCallback,
	const PlayPosFunc& playPosCallback)
	: ScrollerBase(vertical),
	viewSizeCallback(viewSizeCallback), itemNumCallback(itemNumCallback),
	itemSizeLimitCallback(itemSizeLimitCallback), updatePosCallback(updatePosCallback),
	paintPreviewCallback(paintPreviewCallback), paintItemPreviewCallback(paintItemPreviewCallback),
	playPosCallback(playPosCallback) {
	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->getLAFFor(LookAndFeelFactory::Scroller));
}

double Scroller::createViewSize() {
	return this->viewSizeCallback();
}

double Scroller::createItemNum() {
	return this->itemNumCallback();
}

std::tuple<double, double> Scroller::createItemSizeLimit() {
	return this->itemSizeLimitCallback();
}

void Scroller::updatePos(double pos, double itemSize) {
	this->updatePosCallback(pos, itemSize);
}

void Scroller::paintPreview(juce::Graphics& g,
	int width, int height, bool vertical, double totalNum) {
	if (this->paintPreviewCallback) {
		this->paintPreviewCallback(g, width, height, vertical, totalNum);
	}
}

void Scroller::paintItemPreview(juce::Graphics& g, int itemIndex,
	int width, int height, bool vertical) {
	if (this->paintItemPreviewCallback) {
		this->paintItemPreviewCallback(
			g, itemIndex, width, height, vertical);
	}
}

double Scroller::getPlayPos() {
	if (this->playPosCallback) {
		return this->playPosCallback();
	}
	return -1;
}
