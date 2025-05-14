/*
  ==============================================================================

    Osc.h
    Created: 6 Mar 2025 11:40:33am
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once

#include "PolyBlep.h"
#include <JuceHeader.h>

class Osc : public juce::dsp::Oscillator<float> {
  public:
    void prepareToPlay(double sampleRate, int samplesPerBlock,
                       int outputChannels);
    void setGain(const float levelInDecibels);
    void setOscPitch(const int pitch);
    void setOscDetune(const float detune);
    void setFreq(const int midiNoteNumber);
    void renderNextBlock(juce::dsp::AudioBlock<float> &audioBlock);
    float processNextSample(float input);
    void setParams(const float oscGain, const int oscPitch,
                   const float oscDetune);
    void setOscWaveform();
    void resetAll();

  private:
    void updateFreq();

    juce::dsp::Gain<float> gain;
    int lastPitch{0};
    float lastDetuneCents{0.0f};
    int lastMidiNote{0};

    double sampleRate = 44100.0;
    float currentFreq = 440.0f;
};
