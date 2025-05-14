/*
  ==============================================================================

    CustomComponent.cpp
    Created: 19 Mar 2025 5:18:15pm
    Author:  Carter Lynch

  ==============================================================================
*/

#include "CustomComponent.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using SliderStyle = juce::Slider::SliderStyle;

SliderWithLabel::SliderWithLabel(juce::String labelName, juce::String paramId,
                                 juce::AudioProcessorValueTreeState &apvts,
                                 const int width, const int height,
                                 SliderStyle style) {
    sliderWidth = width;
    sliderHeight = height;

    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, textBoxWidth,
                           textBoxHeight);

    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxBackgroundColourId,
                     juce::Colours::black);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId,
                     juce::Colours::white);
    addAndMakeVisible(slider);

    juce::FontOptions font{"Courier New", fontHeight, juce::Font::plain};
    label.setFont(font);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setText(labelName, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    attachment = std::make_unique<SliderAttachment>(apvts, paramId, slider);
}

void SliderWithLabel::resized() {
    const auto dialToLabelRatio = 15;
    const auto labelHeight = 18;

    jassert(sliderWidth > 0);
    jassert(sliderHeight > 0);

    label.setBounds(0, 0, sliderWidth, labelHeight);
    slider.setBounds(0, 0 + dialToLabelRatio, sliderWidth, sliderHeight);
}

CustomComponent::CustomComponent() {
    // Add any child components, and init special settings
}

CustomComponent::~CustomComponent() {}

void CustomComponent::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::black);
    auto bounds = getLocalBounds();
    g.setColour(boundsColour);
    g.drawRect(bounds, 2);

    g.setColour(juce::Colours::white);
    juce::FontOptions font{"Courier New", fontHeight, juce::Font::bold};
    g.setFont(font);
    jassert(name.isNotEmpty());
    g.drawText(name, 20, 15, 100, 25, juce::Justification::left);
}

void CustomComponent::resized() {
    // Set bounds of any child components
}
