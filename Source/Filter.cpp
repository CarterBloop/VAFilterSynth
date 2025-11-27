/*
  ==============================================================================

    Filter.cpp
    Created: 18 Mar 2025 3:09:56pm
    Author:  Carter Lynch

  ==============================================================================
*/

#include "Filter.h"
#include <cmath>

using namespace juce;

Filter::Filter() {}

void Filter::prepareToPlay(double sampleRate, int oversamplingFactor) {
    Fs = sampleRate * (double)oversamplingFactor;
    reset();
}

void Filter::reset() { st = FilterState(); }

void Filter::selectFilterType(const int /*type*/) {
    // only lowpass implemented (type=0)
}

/* Derived from system of equations #22
    vc_1 = (I0/2C) * (
        tanh((VC2 - VC1)  / (2 * gamma)) +
        tanh((vin - vout) / (2 * gamma))
    )

    vc_2 = (I0/2C) * (
        tanh((VC2 - VC1) / (2 * gamma)) +
        tanh((VC3 - VC2) / (2 * gamma))
    )

    vc_3 = (I0/2C) * (
        tanh((VC3 - VC2) / (2 * gamma)) +
        tanh((VC4 - VC3) / (2 * gamma))
    )

    vc_4 = (I0/2C) * (
        tanh((VC4 - VC3) / (2 * gamma)) +
        tanh((-VC4)      / (6 * gamma))
    )
*/

float Filter::processNextSample(float input, float cutoffHz, float resonance) {
    cutoffHz = jlimit(20.0f, 20000.0f, cutoffHz);
    double vin = (double)input; // voltage in
    vin *= (double)0.4;
    double vout = st.Vout; // voltage out
    // constants
    double warped = std::tan(MathConstants<double>::pi * cutoffHz / Fs);
    double I0 = 8.0 * C * VT * 2.0 * Fs * warped;
    double dt = 1 / (2 * Fs);
    double G = (I0 / (2.0 * C)) * dt;
    double K = (double)resonance;

    for (int i = 0; i < maxIterations; i++) {
        st.VC1 = G * (fastTanh((st.VC2 - st.VC1) / (2 * gamma)) +
                      fastTanh((vin - st.Vout) / (2 * VT))) +
                 st.s1;
        st.VC2 = G * (fastTanh((st.VC3 - st.VC2) / (2 * gamma)) -
                      fastTanh((st.VC2 - st.VC1) / (2 * gamma))) +
                 st.s2;
        st.VC3 = G * (fastTanh((st.VC4 - st.VC3) / (2 * gamma)) -
                      fastTanh((st.VC3 - st.VC2) / (2 * gamma))) +
                 st.s3;
        st.VC4 = G * (-fastTanh(st.VC4 / (6.0f * gamma)) -
                      fastTanh((st.VC4 - st.VC3) / (2 * gamma))) +
                 st.s4;

        vout = (K + 0.5f) * st.VC4;

        if (abs(vout - st.Vout) >= Mp * abs(st.Vout)) {
            st.Vout = vout;
            break;
        }

        st.Vout = vout;
    }

    st.s1 = dt * fastTanh((vin - st.Vout) / (2 * VT)) + st.VC1;
    st.s2 = dt * fastTanh((st.VC2 - st.VC1) / (2 * gamma)) + st.VC2;
    st.s3 = dt * fastTanh((st.VC3 - st.VC2) / (2 * gamma)) + st.VC3;
    st.s4 = dt * fastTanh((st.VC4 - st.VC3) / (2 * gamma)) + st.VC4;

    return float(vout);
}

// tanh optimization
inline float Filter::fastTanh(float x) noexcept {
    constexpr float threshold = 5.0f;
    if (std::abs(x) < threshold) // approx only good from -5...5
        return juce::dsp::FastMathApproximations::tanh(x);

    return tanh(x);
}
