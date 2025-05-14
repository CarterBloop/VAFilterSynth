/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
VAFilterSynthAudioProcessorEditor::VAFilterSynthAudioProcessorEditor(
    VAFilterSynthAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      osc1(audioProcessor.apvts, "OSC1GAIN", "OSC1PITCH", "OSC1DETUNE"),
      osc2(audioProcessor.apvts, "OSC2GAIN", "OSC2PITCH", "OSC2DETUNE"),
      filter(audioProcessor.apvts, "FILTERCUTOFF", "FILTERRESONANCE",
             "FILTERADSRDEPTH"),
      adsr(audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE"),
      filterAdsr(audioProcessor.apvts, "FILTERATTACK", "FILTERDECAY",
                 "FILTERSUSTAIN", "FILTERRELEASE")

{
    setLookAndFeel(&minimalLookAndFeel);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(osc1);
    addAndMakeVisible(osc2);
    addAndMakeVisible(filter);
    addAndMakeVisible(adsr);
    addAndMakeVisible(filterAdsr);

    osc1.setName("Oscillator 1");
    osc2.setName("Oscillator 2");
    filter.setName("Filter");
    filterAdsr.setName("Filter ADSR");
    adsr.setName("ADSR");

    osc1.setBoundsColour(juce::Colours::white);
    osc2.setBoundsColour(juce::Colours::white);
    filter.setBoundsColour(juce::Colours::white);
    filterAdsr.setBoundsColour(juce::Colours::white);
    adsr.setBoundsColour(juce::Colours::white);

    setSize(910, 390);
}

VAFilterSynthAudioProcessorEditor::~VAFilterSynthAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

//==============================================================================
void VAFilterSynthAudioProcessorEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.fillAll(juce::Colours::black);
}

void VAFilterSynthAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    const int margin = 10;

    int oscWidth = 250;
    int oscHeight = 180;

    osc1.setBounds(margin, margin, oscWidth, oscHeight);
    osc2.setBounds(margin, osc1.getBottom() + margin, oscWidth, oscHeight);

    int filterWidth = 150;
    int filterHeight = 370;
    filter.setBounds(osc1.getRight() + margin, margin, filterWidth,
                     filterHeight);

    int adsrWidth = 230;
    int adsrHeight = 370;
    filterAdsr.setBounds(filter.getRight() + margin, margin, adsrWidth,
                         adsrHeight);
    adsr.setBounds(filterAdsr.getRight() + margin, margin, adsrWidth,
                   adsrHeight);
}
