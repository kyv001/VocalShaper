#pragma once

#include <JuceHeader.h>
#include "../MainLookAndFeel.h"

class MidiContentLookAndFeel : public MainLookAndFeel {
public:
	MidiContentLookAndFeel();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiContentLookAndFeel)
};
