/*
  ==============================================================================

    AdsrComponent.cpp
    Created: 14 May 2025 2:10:21pm
    Author:  Carter Lynch

  ==============================================================================
*/

#include "AdsrComponent.h"

#include "AdsrComponent.h"
#include <JuceHeader.h>

//==============================================================================
AdsrComponent::AdsrComponent(juce::AudioProcessorValueTreeState &apvts,
                             juce::String attackId, juce::String decayId,
                             juce::String sustainId, juce::String releaseId)
    : attack("A", attackId, apvts, sliderWidth, sliderHeight,
             juce::Slider::SliderStyle::LinearVertical),
      decay("D", decayId, apvts, sliderWidth, sliderHeight,
            juce::Slider::SliderStyle::LinearVertical),
      sustain("S", sustainId, apvts, sliderWidth, sliderHeight,
              juce::Slider::SliderStyle::LinearVertical),
      release("R", releaseId, apvts, sliderWidth, sliderHeight,
              juce::Slider::SliderStyle::LinearVertical) {
    addAndMakeVisible(attack);
    addAndMakeVisible(decay);
    addAndMakeVisible(sustain);
    addAndMakeVisible(release);
}

AdsrComponent::~AdsrComponent() {}

void AdsrComponent::resized() {
    const auto startX = 15;
    const auto startY = 55;
    const auto width = sliderWidth;
    const auto height = sliderHeight + 20;

    attack.setBounds(startX, startY, width, height);
    decay.setBounds(attack.getRight(), startY, width, height);
    sustain.setBounds(decay.getRight(), startY, width, height);
    release.setBounds(sustain.getRight(), startY, width, height);
}
