#include "PluginEditor.h"

class BlackMetalLookAndFeel : public juce::LookAndFeel_V4 {
public:
    BlackMetalLookAndFeel() {
        setColour(juce::Slider::thumbColourId, juce::Colours::gold);
        setColour(juce::Slider::trackColourId, juce::Colours::black);
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1a1a1a)); // Темный металл
        setColour(juce::TextButton::textColourOffId, juce::Colours::silver);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        auto bounds = button.getLocalBounds().toFloat();
        
        // 3D эффект металла
        juce::DropShadow shadow(juce::Colours::black, 10, {0, 4});
        shadow.drawForRectangle(g, bounds.toNearestInt());

        juce::Path path;
        path.addRoundedRectangle(bounds, 5.0f);
        
        // Градиент "черный металл"
        juce::ColourGradient grad(juce::Colours::darkgrey, bounds.getX(), bounds.getY(),
                                  juce::Colours::black, bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill(grad);
        g.fillPath(path);
        
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }
};

MySuperSynthAudioProcessorEditor::MySuperSynthAudioProcessorEditor(MySuperSynthAudioProcessor& p)
    : AudioProcessorEditor(&p)
{
    setLookAndFeel(&customLF);
    setSize(800, 600);
    setOpaque(true);
}

MySuperSynthAudioProcessorEditor::~MySuperSynthAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

void MySuperSynthAudioProcessorEditor::paint(juce::Graphics& g) {
    // Фон: Темный, текстурированный
    g.fillAll(juce::Colour(0xFF050505));
    
    // Логотип или декоративные элементы
    g.setColour(juce::Colours::red.withAlpha(0.2f));
    g.setFont(24.0f);
    g.drawText("BM SYNTH ENGINE", getLocalBounds(), juce::Justification::centredTop);
}

void MySuperSynthAudioProcessorEditor::resized() {
    // Расположение элементов управления
    // Здесь ты добавляешь слайдеры для Drive, Mode Switcher и т.д.
}
