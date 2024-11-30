#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class InputMonitoringButtonLookAndFeel : public MainLookAndFeel {
public:
	InputMonitoringButtonLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InputMonitoringButtonLookAndFeel)
};
