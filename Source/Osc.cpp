/*
  ==============================================================================

    Osc.cpp
    Created: 6 Mar 2025 11:40:33am
    Author:  Carter Lynch

  ==============================================================================
*/

#include "Osc.h"
#include <string>

void Osc::prepareToPlay(double sampleRate, int samplesPerBlock,
                        int outputChannels) {
    resetAll();

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    prepare(spec);
    gain.prepare(spec);

    setOscWaveform();
}

void Osc::setGain(const float levelInDecibels) {
    gain.setGainDecibels(levelInDecibels);
}

void Osc::setOscPitch(const int pitch) {
    lastPitch = pitch;
    updateFreq();
}

void Osc::setOscDetune(const float detune) {
    lastDetuneCents = detune;
    updateFreq();
}

void Osc::setFreq(const int midiNoteNumber) {
    lastMidiNote = midiNoteNumber;
    updateFreq();
}

void Osc::updateFreq() {
    double fractionalMidi = lastMidiNote + lastPitch + (lastDetuneCents * 0.01);
    double frequencyOfA = 440.0;
    double f = // adapted from juce::MidiMessage::getMidiNoteInHertz
        frequencyOfA * std::pow(2.0, (fractionalMidi - 69.0) / 12.0);

    setFrequency(f, true);
    currentFreq = (float)f;
}

void Osc::renderNextBlock(juce::dsp::AudioBlock<float> &audioBlock) {
    jassert(audioBlock.getNumSamples() > 0);
    process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}

float Osc::processNextSample(float input) {
    return gain.processSample(processSample(input));
}

void Osc::setParams(const float oscGain, const int oscPitch,
                    const float oscDetune) {
    setGain(oscGain);
    setOscPitch(oscPitch);
    setOscDetune(oscDetune);
}

void Osc::setOscWaveform() {
    initialise([this](float x) {
        // convert phase x to normalized t
        float t = (x + juce::MathConstants<float>::pi) /
                  (2.0f * juce::MathConstants<float>::pi);

        // correct phase
        t += 0.5;
        if (t >= 1.)
            t -= 1.;

        float naiveSaw = 2.0f * t - 1.0f;

        double dt = currentFreq / sampleRate;

        // apply polyBlep to reduce aliasing
        return naiveSaw - polyBlep(t, dt);
    });
}

void Osc::resetAll() {
    reset();
    gain.reset();
}
