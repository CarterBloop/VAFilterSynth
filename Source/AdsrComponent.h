/*
  ==============================================================================

    AdsrComponent.h
    Created: 14 May 2025 2:10:21pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once

#include "CustomComponent.h"
#include <JuceHeader.h>

class AdsrComponent : public CustomComponent {
  public:
    AdsrComponent(juce::AudioProcessorValueTreeState &apvts,
                  juce::String attackId, juce::String decayId,
                  juce::String sustainId, juce::String releaseId);
    ~AdsrComponent() override;

    void resized() override;

  private:
    SliderWithLabel attack;
    SliderWithLabel decay;
    SliderWithLabel sustain;
    SliderWithLabel release;

    static constexpr int sliderWidth = 50;
    static constexpr int sliderHeight = 260;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdsrComponent)
};
