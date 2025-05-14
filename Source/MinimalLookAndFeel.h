/*
  ==============================================================================

    MinimalLookAndFeel.h
    Created: 29 Mar 2025 7:25:22pm
    Author:  Carter Lynch

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MinimalLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    MinimalLookAndFeel();
    ~MinimalLookAndFeel() override;

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width,
                          int height, float sliderPos,
                          const float rotaryStartAngle,
                          const float rotaryEndAngle,
                          juce::Slider &slider) override;

    void drawLinearSlider(juce::Graphics &g, int x, int y, int width,
                          int height, float sliderPos, float minSliderPos,
                          float maxSliderPos,
                          const juce::Slider::SliderStyle style,
                          juce::Slider &slider) override;
    
};
