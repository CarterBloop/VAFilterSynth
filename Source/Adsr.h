/*
  ==============================================================================

    Adsr.h
    Created: 18 Mar 2025 1:56:35pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Adsr : public juce::ADSR {
  public:
    void update(const float attack, const float decay, const float sustain,
                const float release);

  private:
    juce::ADSR::Parameters adsr;
};
