﻿#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class ControllerLookAndFeel : public MainLookAndFeel {
public:
	ControllerLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerLookAndFeel)
};
