#pragma once
#include <JuceHeader.h>

enum class FXMode { Analog, Digital, Tube };

class FXChain {
public:
    void prepare(const juce::dsp::ProcessSpec& spec) {
        drive.prepare(spec);
        waveshaper.prepare(spec);
        delay.prepare(spec);
        reverb.prepare(spec);
        
        delay.setMaximumDelayInSamples(44100 * 2); // 2 секунды
        delay.setDelay(44100 * 0.5); // 500ms
        
        setMode(FXMode::Digital);
    }

    void setMode(FXMode mode) {
        currentMode = mode;
        if (mode == FXMode::Tube) {
            drive.setGainLinear(2.0f);
            waveshaper.functionToUse = [](float x) { return std::tanh(x * 3.0f); }; // Ламповый перегруз
            reverbParameters.damping = 0.3f; // Темный, теплый хвост
        } else if (mode == FXMode::Analog) {
            drive.setGainLinear(1.2f);
            waveshaper.functionToUse = [](float x) { return x * (1.5f - 0.5f * std::abs(x)); }; // Мягкая сатурация
            reverbParameters.damping = 0.5f;
        } else {
            drive.setGainLinear(1.0f);
            waveshaper.functionToUse = [](float x) { return x; }; // Чистый цифровой
            reverbParameters.damping = 0.8f; // Яркий
        }
        reverb.setParameters(reverbParameters);
    }

    void process(juce::AudioBuffer<float>& buffer) {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        drive.process(context);
        waveshaper.process(context);
        delay.process(context);
        reverb.process(context);
    }

private:
    FXMode currentMode = FXMode::Digital;
    juce::dsp::Gain<float> drive;
    juce::dsp::WaveShaper<float> waveshaper;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delay;
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParameters;
};
