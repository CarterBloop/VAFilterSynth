/*
  ==============================================================================

    FilterComponent.cpp
    Created: 19 Mar 2025 5:17:22pm
    Author:  Carter Lynch

  ==============================================================================
*/

#include "FilterComponent.h"

#include <JuceHeader.h>

//==============================================================================
FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState &apvts,
                                 juce::String cutoffId,
                                 juce::String resonanceId,
                                 juce::String adsrDepth)
    : cutoff("Cutoff", cutoffId, apvts, dialWidth, dialHeight),
      resonance("Res", resonanceId, apvts, dialWidth, dialHeight),
      depth("Depth", adsrDepth, apvts, dialWidth, dialHeight) {
    addAndMakeVisible(cutoff);
    addAndMakeVisible(resonance);
    addAndMakeVisible(depth);
}

FilterComponent::~FilterComponent() {}

void FilterComponent::resized() {
    const auto startX = 40;
    const auto startY = 50;
    const auto width = 70;
    const auto height = 88;
    const auto gap = 10;

    cutoff.setBounds(startX, startY, width, height);
    resonance.setBounds(startX, startY + height + gap, width, height);
    depth.setBounds(startX, startY + 2 * (height + gap), width, height);
}
