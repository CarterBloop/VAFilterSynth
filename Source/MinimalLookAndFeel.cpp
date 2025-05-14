/*
  ==============================================================================

    MinimalLookAndFeel.cpp
    Created: 29 Mar 2025 7:25:22pm
    Author:  Carter Lynch

  ==============================================================================
*/

#include "MinimalLookAndFeel.h"

MinimalLookAndFeel::MinimalLookAndFeel() {
    setDefaultSansSerifTypefaceName("Courier New");
}

MinimalLookAndFeel::~MinimalLookAndFeel() {}

void MinimalLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y,
                                          int width, int height,
                                          float sliderPos,
                                          const float rotaryStartAngle,
                                          const float rotaryEndAngle,
                                          juce::Slider &slider) {
    auto radius = (float)juce::jmin(width, height) / 2.0f - 4.0f;
    auto centreX = x + width * 0.5f;
    auto centreY = y + height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto angle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // background
    g.setColour(juce::Colours::black);
    g.fillEllipse(rx, ry, radius * 2, radius * 2);

    // white outline
    g.setColour(juce::Colours::white);
    g.drawEllipse(rx, ry, radius * 2, radius * 2, 1.0f);

    // indicator
    juce::Path pointer;
    pointer.addRectangle(-2, -radius, 4, radius * 0.8f);
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(
                            centreX, centreY));
}

void MinimalLookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y,
                                          int width, int height,
                                          float sliderPos, float minSliderPos,
                                          float maxSliderPos,
                                          const juce::Slider::SliderStyle style,
                                          juce::Slider &slider) {
    if (style == juce::Slider::LinearVertical) {
        g.setColour(juce::Colours::black);
        g.fillRect(x, y, width, height);

        int centreX = x + width / 2;

        g.setColour(juce::Colours::white);
        g.drawLine((float)centreX, (float)(y + height), (float)centreX,
                   sliderPos, 1.0f);

        float notchRad = 5.0f;
        g.drawLine(centreX - notchRad, sliderPos, centreX + notchRad, sliderPos,
                   3.0);
    } else {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos,
                                         minSliderPos, maxSliderPos, style,
                                         slider);
    }
}
