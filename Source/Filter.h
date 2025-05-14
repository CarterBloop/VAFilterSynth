/*
  ==============================================================================

    Filter.h
    Created: 18 Mar 2025 3:09:56pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// Virtual analog filter based on:
// "A NONLINEAR DIGITAL MODEL OF THE EMS VCS3 VOLTAGE-CONTROLLED FILTER"
// by Marco Civolani and Federico Fontana
class Filter {
  public:
    Filter();

    void prepareToPlay(double sampleRate, int samplesPerBlock,
                       int outputChannels);

    void setParams(const int filterType, const float filterCutoff,
                   const float filterResonance);

    void processNextBlock(juce::AudioBuffer<float> &buffer);

    float processNextSample(int channel, float inputValue);

    void resetAll();

  private:
    void selectFilterType(const int type);
    inline float fastTanh(float x) noexcept;

    float cutoffHz = 1000.0f;
    float resonance = 1.0f;

    // base sample rate and oversampled rate
    double baseFs = 44100.0;
    double Fs = 44100.0 * 8.0;

    static constexpr double VT = 0.0260; // thermal voltage
    static constexpr double eta = 1.836; // diode emission coefficient (1N4148)
    static constexpr double gamma = eta * VT; // diode-law scale factor
    static constexpr double C = 1.0e-7;       // capacitance (100 nF)

    static constexpr int maxIterations = 3; // convergence iter limit

    struct FilterState {
        double VC1 = 0, VC2 = 0, VC3 = 0, VC4 = 0;
        double s1 = 0, s2 = 0, s3 = 0, s4 = 0;
        double Vout = 0;
    };

    std::vector<FilterState> states;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    juce::AudioBuffer<float> tempBuffer;
};
