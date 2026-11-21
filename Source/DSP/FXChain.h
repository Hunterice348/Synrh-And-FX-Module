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
        
        delay.setMaximumDelayInSamples(spec.sampleRate * 2.0);
        delay.setDelay(spec.sampleRate * 0.3);
        
        reverbParams.roomSize = 0.8f;
        reverbParams.damping = 0.5f;
        reverbParams.wetLevel = 0.3f;
        reverbParams.dryLevel = 0.7f;
        reverb.setParameters(reverbParams);
        
        setMode(FXMode::Digital);
    }

    void setMode(FXMode mode) {
        currentMode = mode;
        if (mode == FXMode::Tube) {
            drive.setGainLinear(2.5f);
            waveshaper.functionToUse = [](float x) { return std::tanh(x * 4.0f); };
            reverbParams.damping = 0.3f;
        } else if (mode == FXMode::Analog) {
            drive.setGainLinear(1.5f);
            waveshaper.functionToUse = [](float x) { return x * (1.5f - 0.5f * std::abs(x)); };
            reverbParams.damping = 0.5f;
        } else {
            drive.setGainLinear(1.0f);
            waveshaper.functionToUse = [](float x) { return x; };
            reverbParams.damping = 0.8f;
        }
        reverb.setParameters(reverbParams);
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
    juce::dsp::Reverb::Parameters reverbParams;
};
