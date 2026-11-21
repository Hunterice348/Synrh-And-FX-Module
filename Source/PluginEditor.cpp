#include "PluginEditor.h"

MySuperSynthAudioProcessorEditor::MySuperSynthAudioProcessorEditor(MySuperSynthAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&customLF);
    setSize(600, 400);
    setOpaque(true);

    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(driveSlider);
    driveAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "drive", driveSlider));

    fxModeCombo.addItem("Analog", 1);
    fxModeCombo.addItem("Digital", 2);
    fxModeCombo.addItem("Tube", 3);
    fxModeCombo.setSelectedId(2);
    addAndMakeVisible(fxModeCombo);
    fxModeAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "fxMode", fxModeCombo));
}

MySuperSynthAudioProcessorEditor::~MySuperSynthAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

void MySuperSynthAudioProcessorEditor::paint(juce::Graphics& g) {
    juce::ColourGradient grad(juce::Colour(0xFF111111), 0, 0, juce::Colour(0xFF000000), 0, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillAll();

    g.setColour(juce::Colours::darkgrey);
    g.drawRect(getLocalBounds(), 2);

    g.setColour(juce::Colours::gold);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("BM SYNTH ENGINE", getLocalBounds().removeFromTop(60), juce::Justification::centred);
}

void MySuperSynthAudioProcessorEditor::resized() {
    auto area = getLocalBounds().reduced(40);
    area.removeFromTop(80);

    auto controlsArea = area.removeFromTop(150);
    driveSlider.setBounds(controlsArea.removeFromLeft(120));
    
    auto comboArea = controlsArea.removeFromLeft(150).withHeight(40).withY(60);
    fxModeCombo.setBounds(comboArea);
}
