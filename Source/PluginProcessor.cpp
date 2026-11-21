#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/EngineCore.h"
#include "DSP/FXChain.h"

MySuperSynthAudioProcessor::MySuperSynthAudioProcessor()
     : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void MySuperSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    engine.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    fxChain.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
}

void MySuperSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    
    // 1. Генерация звука
    engine.processBlock(buffer, midiMessages);
    
    // 2. Обработка эффектов
    fxChain.process(buffer);
}

// Важно: Сохранение состояния (Presets) на любой диск обрабатывается DAW автоматически.
// Ты просто сериализуешь параметры в XML/JSON.
juce::AudioProcessorValueTreeState MySuperSynthAudioProcessor::parameters { *this, nullptr };
