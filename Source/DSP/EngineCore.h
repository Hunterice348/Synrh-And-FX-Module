#pragma once
#include <JuceHeader.h>

class SynthVoice : public juce::MPESynthesiserVoice {
public:
    SynthVoice() {
        // Создаем "сочную" пилообразную волну с антиалиасингом
        auto wave = juce::dsp::WaveTable::generateSawWave(512);
        oscillator.initialise([](float x) { return std::sin(x); }, 512); // Заглушка, заменим на wave в prepare
    }

    void prepare(const juce::dsp::ProcessSpec& spec) {
        oscillator.prepare(spec);
        envelope.setSampleRate(spec.sampleRate);
        
        auto wave = juce::dsp::WaveTable::generateSawWave(512);
        oscillator.initialise(wave);
    }

    bool isActive() const override { return envelope.isActive(); }

    void noteStarted() override {
        auto velocity = getCurrentlyPlayingNote().noteOnVelocity.asUnsignedFloat();
        auto freq = getCurrentlyPlayingNote().getFrequencyInHertz();
        
        oscillator.setFrequency(freq, true);
        envelope.noteOn();
        currentLevel = velocity;
    }

    void noteStopped(bool allowTailOff) override {
        envelope.noteOff();
        if (!allowTailOff) clearCurrentNote();
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
        juce::dsp::AudioBlock<float> block(outputBuffer);
        juce::dsp::ProcessContextReplacing<float> context(block.getSubBlock(startSample, numSamples));
        
        for (int i = 0; i < numSamples; ++i) {
            float envVal = envelope.getNextSample();
            float oscVal = oscillator.processSample(0.0f) * currentLevel * envVal;
            
            for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch) {
                outputBuffer.addSample(ch, startSample + i, oscVal);
            }
        }
        
        if (!envelope.isActive()) clearCurrentNote();
    }

private:
    juce::dsp::Oscillator<float> oscillator;
    juce::ADSR envelope;
    float currentLevel = 0.5f;
};

class EngineCore {
public:
    void prepare(const juce::dsp::ProcessSpec& spec) {
        synth.clearVoices();
        for (int i = 0; i < 4; ++i) { // 4-голосная полифония
            synth.addVoice(new SynthVoice());
        }
        synth.setCurrentPlaybackSampleRate(spec.sampleRate);
    }

    void processMidi(const juce::MidiBuffer& midiMessages, juce::AudioBuffer<float>& buffer) {
        synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }

private:
    juce::MPESynthesiser synth;
};
