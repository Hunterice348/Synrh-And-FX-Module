#include "PluginProcessor.h"
#include "PluginEditor.h"

MySuperSynthAudioProcessor::MySuperSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
       parameters(*this, nullptr, "Parameters", {
           std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.0f, 10.0f, 0.0f),
           std::make_unique<juce::AudioParameterChoice>("fxMode", "FX Mode", juce::StringArray{"Analog", "Digital", "Tube"}, 1)
       })
{
}

void MySuperSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    engine.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    fxChain.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    
    // Применяем начальный режим эффектов
    int modeIndex = parameters.getParameterAsValue("fxMode").getValue();
    fxChain.setMode(static_cast<FXMode>(modeIndex));
}

void MySuperSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    
    // 1. Синтез звука
    engine.processMidi(midiMessages, buffer);
    
    // 2. Обновление параметров эффектов в реальном времени
    float driveVal = parameters.getParameterAsValue("drive").getValue();
    // Здесь можно привязать driveVal к fxChain, если добавить сеттер
    
    int modeIndex = parameters.getParameterAsValue("fxMode").getValue();
    fxChain.setMode(static_cast<FXMode>(modeIndex));

    // 3. Обработка эффектов
    fxChain.process(buffer);
}

juce::AudioProcessorEditor* MySuperSynthAudioProcessor::createEditor() {
    return new MySuperSynthAudioProcessorEditor(*this);
}

// АВТОМАТИЧЕСКОЕ СОХРАНЕНИЕ ПРЕСЕТОВ НА ЛЮБОЙ ДИСК (C: или D:)
// DAW сама вызывает эти методы при сохранении пресета пользователем.
void MySuperSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MySuperSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters.state.getType())) {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MySuperSynthAudioProcessor();
}
