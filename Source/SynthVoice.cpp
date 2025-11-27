/*
  ==============================================================================

    SynthVoice.cpp
    Created: 27 Feb 2025 8:09:38pm
    Author:  Carter Lynch

  ==============================================================================
*/

#include "SynthVoice.h"

bool SynthVoice::canPlaySound(juce::SynthesiserSound *sound) {
    return dynamic_cast<juce::SynthesiserSound *>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound *sound,
                           int currentPitchWheelPosition) {
    for (int i = 0; i < 2; i++) {
        osc1[i].setFreq(midiNoteNumber);
        osc2[i].setFreq(midiNoteNumber);
    }
    adsr.noteOn();
    filterAdsr.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff) {
    adsr.noteOff();
    filterAdsr.noteOff();

    if (!allowTailOff || !adsr.isActive()) {
        clearCurrentNote();
    }
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue) {
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue) {}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock,
                               int outputChannels) {
    reset();

    adsr.setSampleRate(sampleRate);
    filterAdsr.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    for (int ch = 0; ch < numChannelsToProcess; ch++) {
        osc1[ch].prepareToPlay(sampleRate, samplesPerBlock, outputChannels);
        osc2[ch].prepareToPlay(sampleRate, samplesPerBlock, outputChannels);
        filter[ch].prepareToPlay(sampleRate, oversamplingFactor);

        oversamplers[ch] = std::make_unique<juce::dsp::Oversampling<float>>(
            1, oversamplingStages,
            juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple);
        oversamplers[ch]->initProcessing((size_t)samplesPerBlock);
        oversamplers[ch]->reset();
    }

    gain.prepare(spec);
    gain.setGainLinear(0.1f);

    isPrepared = true;
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                                 int startSample, int numSamples) {
    jassert(isPrepared);

    if (!isVoiceActive())
        return;

    if (numSamples <= 0 || outputBuffer.getNumChannels() <= 0) {
        jassertfalse;
        return;
    }

    const int numChannels = outputBuffer.getNumChannels();

    // resize and clear the per-voice working buffer
    synthBuffer.setSize(numChannels, numSamples, false, false, true);
    synthBuffer.clear();

    // filter adsr
    cutoffPerSample.resize((size_t)numSamples);
    for (int s = 0; s < numSamples; ++s) {
        float env = filterAdsr.getNextSample();
        float cutoff = filterCutoffParam + env * filterEnvDepthParam;
        cutoff = std::clamp(cutoff, 0.0f, 20000.0f);
        cutoffPerSample[(size_t)s] = cutoff;
    }

    // channel processing
    for (int ch = 0; ch < numChannels; ++ch) {
        if (!oversamplers[ch]) {
            jassertfalse;
            continue;
        }

        juce::AudioBuffer<float> oscBuffer(1, numSamples);
        float *oscData = oscBuffer.getWritePointer(0);
        if (oscData == nullptr) {
            jassertfalse;
            continue;
        }
        // generate oscillator at base rate
        for (int s = 0; s < numSamples; ++s) {
            float oscSample = osc1[ch].processNextSample(0.0f) +
                              osc2[ch].processNextSample(0.0f);

            oscData[s] = gain.processSample(oscSample);
        }

        // oversample
        juce::dsp::AudioBlock<float> oscBlock(oscBuffer);
        if (oscBlock.getNumChannels() == 0) {
            jassertfalse;
            continue;
        }
        auto upBlock = oversamplers[ch]->processSamplesUp(oscBlock);
        if (upBlock.getNumChannels() == 0) {
            jassertfalse;
            continue;
        }
        float *upData = upBlock.getChannelPointer(0);
        if (upData == nullptr) {
            jassertfalse;
            continue;
        }
        const int upSamples = (int)upBlock.getNumSamples();
        const int factor = oversamplingFactor;

        // filter runs at oversampled rate
        for (int i = 0; i < upSamples; ++i) {
            // map oversampled index back to base-rate envelope index
            int baseIdx = i / factor;
            if (baseIdx < 0)
                baseIdx = 0;
            else if (baseIdx >= numSamples)
                baseIdx = numSamples - 1;
            float cutoff = cutoffPerSample[(size_t)baseIdx];

            upData[i] = filter[ch].processNextSample(upData[i], cutoff,
                                                     filterResonanceParam);
        }

        // downsample to base rate
        oversamplers[ch]->processSamplesDown(oscBlock);

        // copy to synth buffer
        float *dst = synthBuffer.getWritePointer(ch);
        float *src = oscBlock.getChannelPointer(0);
        if (dst == nullptr || src == nullptr) {
            jassertfalse;
            continue;
        }
        std::memcpy(dst, src, sizeof(float) * numSamples);
    }

    // apply amplitude envelope
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, numSamples);

    // mix per-voice result into the output buffer
    for (int ch = 0; ch < numChannels; ++ch) {
        outputBuffer.addFrom(ch, startSample, synthBuffer, ch, 0, numSamples);
    }

    if (!adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::reset() {
    gain.reset();
    adsr.reset();
    filterAdsr.reset();

    for (int ch = 0; ch < numChannelsToProcess; ++ch)
        filter[ch].reset();
}

void SynthVoice::updateFilterParams(const float filterCutoff,
                                    const float filterResonance,
                                    const float adsrDepth) {
    filterCutoffParam = filterCutoff;
    filterResonanceParam = filterResonance;
    filterEnvDepthParam = adsrDepth;
}
