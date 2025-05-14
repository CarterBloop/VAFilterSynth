/*
  ==============================================================================

    SynthSound.h
    Created: 27 Feb 2025 8:08:05pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound {
  public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};
