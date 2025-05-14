/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VAFilterSynthAudioProcessor::VAFilterSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      apvts(*this, nullptr, "Parameters", createParams())
#endif
{
    synth.addSound(new SynthSound());
    for (int i = 0; i < 1; i++) {
        synth.addVoice(new SynthVoice());
    }
}

VAFilterSynthAudioProcessor::~VAFilterSynthAudioProcessor() {}

//==============================================================================
const juce::String VAFilterSynthAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool VAFilterSynthAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool VAFilterSynthAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool VAFilterSynthAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double VAFilterSynthAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int VAFilterSynthAudioProcessor::getNumPrograms() {
    return 1; // NB: some hosts don't cope very well if you tell them there are
              // 0 programs, so this should be at least 1, even if you're not
              // really implementing programs.
}

int VAFilterSynthAudioProcessor::getCurrentProgram() { return 0; }

void VAFilterSynthAudioProcessor::setCurrentProgram(int index) {}

const juce::String VAFilterSynthAudioProcessor::getProgramName(int index) {
    return {};
}

void VAFilterSynthAudioProcessor::changeProgramName(int index,
                                                 const juce::String &newName) {}

//==============================================================================
void VAFilterSynthAudioProcessor::prepareToPlay(double sampleRate,
                                             int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice *>(synth.getVoice(i))) {
            voice->prepareToPlay(sampleRate, samplesPerBlock,
                                 getTotalNumOutputChannels());
        }
    }

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
}

void VAFilterSynthAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VAFilterSynthAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void VAFilterSynthAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    setParams();

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block{buffer};
}

//==============================================================================
bool VAFilterSynthAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *VAFilterSynthAudioProcessor::createEditor() {
    return new VAFilterSynthAudioProcessorEditor(*this);
}

//==============================================================================
void VAFilterSynthAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();

    if (auto xml = state.createXml()) {
        copyXmlToBinary(*xml, destData);
    }
}

void VAFilterSynthAudioProcessor::setStateInformation(const void *data,
                                                   int sizeInBytes) {
    // You should use this method to restore your parameters from this memory
    // block, whose contents will have been created by the getStateInformation()
    // call.
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes)) {
        auto tree = juce::ValueTree::fromXml(*xmlState);

        apvts.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new VAFilterSynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout
VAFilterSynthAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"OSC1GAIN", 1}, "Oscillator 1 Gain",
        juce::NormalisableRange<float>{-100.0f, 0.2f, 0.1f}, 0.1f, "dB"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"OSC2GAIN", 1}, "Oscillator 2 Gain",
        juce::NormalisableRange<float>{-100.0f, 0.2f, 0.1f}, 0.1f, "dB"));

    // pitch
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{"OSC1PITCH", 1}, "Oscillator 1 Pitch", -48, 48, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{"OSC2PITCH", 1}, "Oscillator 2 Pitch", -48, 48, 0));

    // detune
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"OSC1DETUNE", 1}, "Oscillator 1 Detune",
        juce::NormalisableRange<float>{-50.0f, +50.0f, 0.01f}, 0.0f, "cents"));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"OSC2DETUNE", 1}, "Oscillator 2 Detune",
        juce::NormalisableRange<float>{-50.0f, +50.0f, 0.01f}, 0.0f, "cents"));

    const float cutoffMin = 0.0f;
    const float cutoffMax = 20000.0f;
    const float midFreq = 1000.0f;

    // skew so that 0.5 normalized = midFreq real:
    const float cutoffSkew = std::log(0.5f) / std::log((midFreq - cutoffMin) /
                                                       (cutoffMax - cutoffMin));

    // filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FILTERCUTOFF", 1}, "Filter Cutoff",
        juce::NormalisableRange<float>{0.0f, 20000.0f, 0.1f, cutoffSkew},
        midFreq, "Hz"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FILTERRESONANCE", 1}, "Filter Resonance",
        juce::NormalisableRange<float>{0.01f, 10.0f, 0.01f}, 0.1f, ""));

    // ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"ATTACK", 1}, "Attack",
        juce::NormalisableRange<float>{0.0f, 3.0f, 0.001f, 0.4f}, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"DECAY", 1}, "Decay",
        juce::NormalisableRange<float>{0.0f, 3.0f, 0.001f, 0.4f}, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"SUSTAIN", 1}, "Sustain",
        juce::NormalisableRange<float>{0.0f, 1.0f, 0.001f, 0.4f}, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"RELEASE", 1}, "Release",
        juce::NormalisableRange<float>{0.0f, 5.0f, 0.001f, 0.4f}, 0.5f));

    // filter ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FILTERADSRDEPTH", 1}, "Filter ADSR Depth",
        juce::NormalisableRange<float>{0.0f, 20000.0f, 0.1f, 1.0f}, 10000.0f,
        ""));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FILTERATTACK", 1}, "Filter Attack",
        juce::NormalisableRange<float>{0.0f, 3.0f, 0.001f, 0.4f}, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FILTERDECAY", 1}, "Filter Decay",
        juce::NormalisableRange<float>{0.0f, 3.0f, 0.001f, 0.4f}, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FILTERSUSTAIN", 1}, "Filter Sustain",
        juce::NormalisableRange<float>{0.0f, 1.0f, 0.001f, 0.4f}, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FILTERRELEASE", 1}, "Filter Release",
        juce::NormalisableRange<float>{0.0f, 5.0f, 0.001f, 0.4f}, 0.5f));

    return {params.begin(), params.end()};
}

void VAFilterSynthAudioProcessor::setParams() {
    setVoiceParams();
    setFilterParams();
}

void VAFilterSynthAudioProcessor::setVoiceParams() {
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<SynthVoice *>(synth.getVoice(i))) {

            auto &attack = *apvts.getRawParameterValue("ATTACK");
            auto &decay = *apvts.getRawParameterValue("DECAY");
            auto &sustain = *apvts.getRawParameterValue("SUSTAIN");
            auto &release = *apvts.getRawParameterValue("RELEASE");

            auto &osc1Gain = *apvts.getRawParameterValue("OSC1GAIN");
            auto &osc2Gain = *apvts.getRawParameterValue("OSC2GAIN");
            auto &osc1Pitch = *apvts.getRawParameterValue("OSC1PITCH");
            auto &osc2Pitch = *apvts.getRawParameterValue("OSC2PITCH");
            auto &osc1Detune = *apvts.getRawParameterValue("OSC1DETUNE");
            auto &osc2Detune = *apvts.getRawParameterValue("OSC2DETUNE");

            auto &filterAttack = *apvts.getRawParameterValue("FILTERATTACK");
            auto &filterDecay = *apvts.getRawParameterValue("FILTERDECAY");
            auto &filterSustain = *apvts.getRawParameterValue("FILTERSUSTAIN");
            auto &filterRelease = *apvts.getRawParameterValue("FILTERRELEASE");

            auto &osc1 = voice->getOscillator1();
            auto &osc2 = voice->getOscillator2();

            auto &adsr = voice->getAdsr();
            auto &filterAdsr = voice->getFilterAdsr();

            for (int i = 0; i < getTotalNumOutputChannels(); i++) {
                osc1[i].setParams(osc1Gain, osc1Pitch, osc1Detune);
                osc2[i].setParams(osc2Gain, osc2Pitch, osc2Detune);
            }

            adsr.update(attack.load(), decay.load(), sustain.load(),
                        release.load());
            filterAdsr.update(filterAttack, filterDecay, filterSustain,
                              filterRelease);
        }
    }
}

void VAFilterSynthAudioProcessor::setFilterParams() {
    auto &filterCutoff = *apvts.getRawParameterValue("FILTERCUTOFF");
    auto &filterResonance = *apvts.getRawParameterValue("FILTERRESONANCE");
    auto &adsrDepth = *apvts.getRawParameterValue("FILTERADSRDEPTH");

    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<SynthVoice *>(synth.getVoice(i))) {
            voice->updateFilterParams(filterCutoff, filterResonance, adsrDepth);
        }
    }
}
