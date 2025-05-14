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

void Filter::prepareToPlay(double sampleRate, int samplesPerBlock,
                           int outputChannels) {
    constexpr size_t oversamplingStages = 3; // 3 = 8× oversampling
    constexpr double oversamplingFactor = 8.0f;
    constexpr bool aliasingComp =
        false; // aliasing compensation (leave off due to latency issues)

    baseFs = sampleRate;
    Fs = sampleRate * oversamplingFactor;

    states.clear();
    states.resize((size_t)outputChannels);

    // oversampling
    dsp::ProcessSpec spec{baseFs, static_cast<uint32>(samplesPerBlock),
                          static_cast<uint32>(outputChannels)};
    oversampler.reset(new dsp::Oversampling<float>(
        spec.numChannels, oversamplingStages,
        dsp::Oversampling<float>::filterHalfBandFIREquiripple, aliasingComp));
    oversampler->initProcessing(static_cast<size_t>(samplesPerBlock));

    tempBuffer.setSize(outputChannels, 1);
}

void Filter::resetAll() {
    for (auto &st : states)
        st = FilterState();
    if (oversampler)
        oversampler->reset();
}

void Filter::setParams(const int filterType, const float filterCutoff,
                       const float filterResonance) {
    selectFilterType(filterType);
    cutoffHz = filterCutoff;
    resonance = filterResonance;
}

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

void Filter::processNextBlock(AudioBuffer<float> &buffer) {
    juce::ScopedNoDenormals _noDenormals;

    dsp::AudioBlock<float> block(buffer);

    // oversample to eliminate filter aliasing
    auto osBlock = oversampler->processSamplesUp(block);

    const int numSamples = static_cast<int>(osBlock.getNumSamples());
    const int numCh = static_cast<int>(osBlock.getNumChannels());

    float cutoffSmooth = cutoffHz;

    for (int ch = 0; ch < numCh; ++ch) {
        auto *data = osBlock.getChannelPointer(ch);
        auto &st = states[(size_t)ch];

        for (int n = 0; n < numSamples; ++n) {
            double vin = data[n];  // voltage in
            double vout = st.Vout; // voltage out

            const double alphaCut = std::exp(
                -2.0 * juce::MathConstants<double>::pi * 4000.0 / baseFs);
            cutoffSmooth += alphaCut * (cutoffHz - cutoffSmooth);

            // constants
            double warped =
                std::tan(MathConstants<double>::pi * cutoffSmooth / Fs);
            double I0 = 8.0 * C * VT * 2.0 * Fs * warped;
            double dt = 1 / (2 * Fs);
            double G = (I0 / (2.0 * C)) * dt;
            double K = resonance;

            std::vector<std::string> sv;
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
                st.Vout = vout;
            }

            st.s1 = dt * fastTanh((vin - st.Vout) / (2 * VT)) + st.VC1;
            st.s2 = dt * fastTanh((st.VC2 - st.VC1) / (2 * gamma)) + st.VC2;
            st.s3 = dt * fastTanh((st.VC3 - st.VC2) / (2 * gamma)) + st.VC3;
            st.s4 = dt * fastTanh((st.VC4 - st.VC3) / (2 * gamma)) + st.VC4;

            data[n] = float(vout);
        }
    }

    // downsample
    oversampler->processSamplesDown(block);
}

float Filter::processNextSample(int channel, float inputValue) {
    // route a single sample through the block processor
    tempBuffer.clear();
    tempBuffer.setSample(channel, 0, inputValue);
    processNextBlock(tempBuffer);
    return tempBuffer.getSample(channel, 0);
}

// tanh optimization
inline float Filter::fastTanh(float x) noexcept {
    constexpr float threshold = 5.0f;
    if (std::abs(x) < threshold) // approx only good from -5...5
        return juce::dsp::FastMathApproximations::tanh(x);

    return std::copysign(1.0f, x);
}
