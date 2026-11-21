#pragma once
#include <JuceHeader.h>

class SynthVoice : public juce::SynthesiserVoice {
public:
    SynthVoice() {
        oscillator.initialise([](float x) { return std::sin(x) + 0.5f * std::sin(2.0f * x); }, 128);
    }

    void prepare(const juce::dsp::ProcessSpec& spec) override {
        oscillator.prepare(spec);
        envelope.setSampleRate(spec.sampleRate);
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override {
        return dynamic_cast<juce::SynthesiserSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override {
        oscillator.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber), true);
        envelope.noteOn();
        currentLevel = velocity;
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override {
        if (allowTailOff) {
            envelope.noteOff();
        } else {
            envelope.reset();
            clearCurrentNote();
        }
    }

    void pitchWheelMoved(int /*newValue*/) override {}
    void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
        for (int i = 0; i < numSamples; ++i) {
            float envVal = envelope.getNextSample();
            float oscVal = oscillator.processSample(0.0f) * currentLevel * envVal;
            
            for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch) {
                outputBuffer.addSample(ch, startSample + i, oscVal);
            }
        }
        
        if (!envelope.isActive()) {
            envelope.reset();
            clearCurrentNote();
        }
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
        for (int i = 0; i < 8; ++i) {
            synth.addVoice(new SynthVoice());
        }
        synth.setCurrentPlaybackSampleRate(spec.sampleRate);
    }

    void processMidi(const juce::MidiBuffer& midiMessages, juce::AudioBuffer<float>& buffer) {
        synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }

private:
    juce::Synthesiser synth;
};
