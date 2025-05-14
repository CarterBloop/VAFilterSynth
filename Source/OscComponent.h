/*
  ==============================================================================

    OscComponent.h
    Created: 14 May 2025 2:13:09pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once

#include "CustomComponent.h"
#include <JuceHeader.h>

class OscComponent : public CustomComponent {
  public:
    OscComponent(juce::AudioProcessorValueTreeState &apvts, juce::String gainId,
                 juce::String pitchId, juce::String detuneId);
    ~OscComponent() override;

    void resized() override;

  private:
    SliderWithLabel gain;
    SliderWithLabel pitch;
    SliderWithLabel detune;

    static constexpr int dialWidth = 70;
    static constexpr int dialHeight = 70;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscComponent)
};
