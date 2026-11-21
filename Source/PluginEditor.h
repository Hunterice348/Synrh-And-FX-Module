#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class BlackMetalLookAndFeel : public juce::LookAndFeel_V4 {
public:
    BlackMetalLookAndFeel() {
        setColour(juce::Slider::thumbColourId, juce::Colours::gold);
        setColour(juce::Slider::trackColourId, juce::Colours::black);
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1a1a1a));
        setColour(juce::TextButton::textColourOffId, juce::Colours::silver);
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF0a0a0a));
        setColour(juce::ComboBox::textColourId, juce::Colours::gold);
        setColour(juce::ComboBox::arrowColourId, juce::Colours::gold);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        auto bounds = button.getLocalBounds().toFloat();
        juce::DropShadow shadow(juce::Colours::black, 8, {0, 4});
        shadow.drawForRectangle(g, bounds.toNearestInt());

        juce::Path path;
        path.addRoundedRectangle(bounds, 6.0f);
        
        juce::ColourGradient grad(juce::Colours::darkgrey, bounds.getX(), bounds.getY(),
                                  juce::Colours::black, bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill(grad);
        g.fillPath(path);
        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }
};

class MySuperSynthAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    MySuperSynthAudioProcessorEditor(MySuperSynthAudioProcessor&);
    ~MySuperSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MySuperSynthAudioProcessor& audioProcessor;
    BlackMetalLookAndFeel customLF;

    juce::Slider driveSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

    juce::ComboBox fxModeCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> fxModeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MySuperSynthAudioProcessorEditor)
};
