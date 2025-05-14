/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "AdsrComponent.h"
#include "FilterComponent.h"
#include "MinimalLookAndFeel.h"
#include "OscComponent.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class VAFilterSynthAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    VAFilterSynthAudioProcessorEditor(VAFilterSynthAudioProcessor &);
    ~VAFilterSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VAFilterSynthAudioProcessor &audioProcessor;
    OscComponent osc1;
    OscComponent osc2;
    FilterComponent filter;
    AdsrComponent adsr;
    AdsrComponent filterAdsr;

    MinimalLookAndFeel minimalLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VAFilterSynthAudioProcessorEditor)
};
