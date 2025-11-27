/*
  ==============================================================================

    SynthVoice.h
    Created: 27 Feb 2025 8:09:05pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once
#include "Adsr.h"
#include "Filter.h"
#include "Osc.h"
#include "SynthSound.h"
#include <JuceHeader.h>

class SynthVoice : public juce::SynthesiserVoice {
  public:
    bool canPlaySound(juce::SynthesiserSound *sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *sound,
                   int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void pitchWheelMoved(int newPitchWheelValue) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock,
                       int outputChannels);
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                         int startSample, int numSamples) override;

    void reset();

    std::array<Osc, 2> &getOscillator1() { return osc1; }
    std::array<Osc, 2> &getOscillator2() { return osc2; }

    Adsr &getAdsr() { return adsr; }
    Adsr &getFilterAdsr() { return filterAdsr; }
    void updateFilterParams(const float filterCutoff,
                            const float filterResonance, const float adsrDepth);

  private:
    static constexpr int numChannelsToProcess{2};
    std::array<Osc, numChannelsToProcess> osc1;
    std::array<Osc, numChannelsToProcess> osc2;
    std::array<Filter, numChannelsToProcess> filter;

    Adsr adsr;
    Adsr filterAdsr;
    std::vector<float> cutoffPerSample;

    float filterCutoffParam = 1000.0f;
    float filterResonanceParam = 0.1f;
    float filterEnvDepthParam = 10000.0f;

    std::array<std::unique_ptr<juce::dsp::Oversampling<float>>,
               numChannelsToProcess>
        oversamplers;
    static constexpr int oversamplingFactor = 4; // 2x, 4x, or 8x
    static constexpr int oversamplingStages = 2; // 1, 2, or 3

    juce::AudioBuffer<float> synthBuffer;
    juce::dsp::Gain<float> gain;

    bool isPrepared{false};
};
