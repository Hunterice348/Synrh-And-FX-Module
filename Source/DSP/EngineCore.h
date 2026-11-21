#pragma once
#include <JuceHeader.h>

class Oscillator {
public:
    void prepare(const juce::dsp::ProcessSpec& spec) {
        phase = 0.0;
        sampleRate = spec.sampleRate;
    }

    float process(float frequency, float shapeParam) {
        // Простая реализация пилообразной волны с PWM для "жирности"
        phase += frequency / sampleRate;
        if (phase >= 1.0f) phase -= 1.0f;
        
        // ShapeParam меняет скважность или форму волны
        float output = std::sin(2.0f * juce::MathConstants<float>::pi * phase); 
        output += 0.5f * std::sin(4.0f * juce::MathConstants<float>::pi * phase * shapeParam);
        
        return output * 0.5f; // Gain staging
    }

private:
    double phase = 0.0;
    double sampleRate = 44100.0;
};

class EngineCore {
public:
    void prepare(const juce::dsp::ProcessSpec& spec) {
        osc1.prepare(spec);
        osc2.prepare(spec);
        gain.setValue(0.5f);
    }

    void processBlock(juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages) {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            auto* data = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                // Здесь должна быть логика Voice Management (Polyphony)
                // Для упрощения делаем монофонический тестовый сигнал
                float val = osc1.process(440.0f, 0.5f); 
                val += osc2.process(442.0f, 0.2f); // Detune
                data[sample] = val * gain.getCurrentValue();
            }
        }
    }

private:
    Oscillator osc1, osc2;
    juce::SmoothedValue<float> gain;
};
