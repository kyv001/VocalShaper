#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class PianoLookAndFeel : public MainLookAndFeel {
public:
	PianoLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoLookAndFeel)
};
