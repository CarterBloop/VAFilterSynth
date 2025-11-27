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

    void prepareToPlay(double sampleRate, int oversamplingFactor);

    float processNextSample(float input, float cutoffHz, float resonance);

    void reset();

  private:
    void selectFilterType(const int type);
    inline float fastTanh(float x) noexcept;

    float cutoffHz = 1000.0f;
    float resonance = 1.0f;

    // base sample rate
    double Fs = 44100.0;

    static constexpr double VT = 0.0260; // thermal voltage
    static constexpr double eta = 1.836; // diode emission coefficient (1N4148)
    static constexpr double gamma = eta * VT; // diode-law scale factor
    static constexpr double C = 1.0e-7;       // capacitance (100 nF)

    double Mp = 1.0e-4f;
    static constexpr int maxIterations = 10; // convergence iter limit

    struct FilterState {
        double VC1 = 0, VC2 = 0, VC3 = 0, VC4 = 0;
        double s1 = 0, s2 = 0, s3 = 0, s4 = 0;
        double Vout = 0;
    };

    FilterState st;
};
