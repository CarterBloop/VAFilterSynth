/*
  ==============================================================================

    FilterComponent.h
    Created: 19 Mar 2025 5:17:22pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once

#include "CustomComponent.h"
#include <JuceHeader.h>

class FilterComponent : public CustomComponent {
  public:
    FilterComponent(juce::AudioProcessorValueTreeState &apvts,
                    juce::String cutoffId, juce::String resonanceId,
                    juce::String depthId);
    ~FilterComponent() override;

    void resized() override;

  private:
    SliderWithLabel cutoff;
    SliderWithLabel resonance;
    SliderWithLabel depth;

    static constexpr int dialWidth = 70;
    static constexpr int dialHeight = 70;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterComponent)
};
