/*
  ==============================================================================

    OscComponent.cpp
    Created: 14 May 2025 2:13:09pm
    Author:  Carter Lynch

  ==============================================================================
*/

#include "OscComponent.h"
#include <JuceHeader.h>

OscComponent::OscComponent(juce::AudioProcessorValueTreeState &apvts,
                           juce::String gainId, juce::String pitchId,
                           juce::String detuneId)
    : gain("Gain", gainId, apvts, dialWidth, dialHeight),
      pitch("Pitch", pitchId, apvts, dialWidth, dialHeight),
      detune("Detune", detuneId, apvts, dialWidth, dialHeight) {
    addAndMakeVisible(gain);
    addAndMakeVisible(pitch);
    addAndMakeVisible(detune);
}

OscComponent::~OscComponent() {}

void OscComponent::resized() {
    const auto yStart = 50;
    const auto width = 70;
    const auto height = 88;

    gain.setBounds(20, yStart, width, height);
    pitch.setBounds(90, yStart, width, height);
    detune.setBounds(160, yStart, width, height);
}
