# Скрипт автоматического создания проекта VST3 (JUCE 8)
$root = "MySuperSynth"
New-Item -ItemType Directory -Force -Path "$root\.github\workflows" | Out-Null
New-Item -ItemType Directory -Force -Path "$root\Source\DSP" | Out-Null

function Write-File {
    param([string]$Path, [string]$Content)
    [System.IO.File]::WriteAllText($Path, $Content, [System.Text.UTF8Encoding]::new($false))
    Write-Host "Создан: $Path" -ForegroundColor Green
}

Write-File "$root\.github\workflows\main.yml" @'
name: Build VST3 Plugin

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  release:
    types: [ created ]

env:
  BUILD_TYPE: Release
  PLUGIN_NAME: "BM Synth"
  PROJECT_NAME: "MySuperSynth"

jobs:
  build:
    name: Build on ${{ matrix.os }}
    runs-on: ${{ matrix.os }}
    strategy:
      fail-fast: false
      matrix:
        include:
          - os: windows-latest
            cmake_generator: "Ninja"
            artifact_name: windows-x64
            vst3_path: "MySuperSynth_artefacts/Release/VST3/BM Synth.vst3"
          - os: macos-latest
            cmake_generator: "Xcode"
            artifact_name: macos-universal
            vst3_path: "MySuperSynth_artefacts/Release/VST3/BM Synth.vst3"
          - os: ubuntu-latest
            cmake_generator: "Ninja"
            artifact_name: linux-x64
            vst3_path: "MySuperSynth_artefacts/Release/VST3/BM Synth.vst3"

    steps:
    - name: Checkout repository
      uses: actions/checkout@v4
      with:
        submodules: recursive

    - name: Install Linux dependencies
      if: runner.os == 'Linux'
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake ninja-build pkg-config libasound2-dev libjack-jackd2-dev libfreetype6-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev libwebkit2gtk-4.1-dev libglu1-mesa-dev mesa-common-dev

    - name: Cache JUCE
      id: cache-juce
      uses: actions/cache@v4
      with:
        path: juce
        key: juce-${{ runner.os }}-${{ hashFiles('CMakeLists.txt') }}
        restore-keys: juce-${{ runner.os }}-

    - name: Clone JUCE framework
      if: steps.cache-juce.outputs.cache-hit != 'true'
      run: git clone --depth 1 --branch 8.0.4 https://github.com/juce-framework/JUCE.git juce

    - name: Setup MSVC for Ninja (Windows)
      if: runner.os == 'Windows'
      uses: ilammy/msvc-dev-cmd@v1

    - name: Configure CMake
      run: cmake -B build -G "${{ matrix.cmake_generator }}" -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }}

    - name: Build
      run: cmake --build build --config ${{ env.BUILD_TYPE }} --parallel

    - name: Upload VST3 artifact
      uses: actions/upload-artifact@v4
      with:
        name: ${{ env.PLUGIN_NAME }}-${{ matrix.artifact_name }}
        path: build/${{ matrix.vst3_path }}
        retention-days: 30

  release:
    name: Create Release
    needs: build
    if: github.event_name == 'release'
    runs-on: ubuntu-latest
    permissions:
      contents: write
    steps:
    - name: Download all artifacts
      uses: actions/download-artifact@v4
      with:
        path: artifacts
    - name: Package for Windows
      run: cd artifacts && zip -r ../${{ env.PLUGIN_NAME }}-Windows.zip *windows-x64* && cd ..
    - name: Package for macOS
      run: cd artifacts && zip -r ../${{ env.PLUGIN_NAME }}-macOS.zip *macos-universal* && cd ..
    - name: Package for Linux
      run: cd artifacts && tar -czf ../${{ env.PLUGIN_NAME }}-Linux.tar.gz *linux-x64* && cd ..
    - name: Upload to Release
      uses: softprops/action-gh-release@v2
      with:
        files: |
          ${{ env.PLUGIN_NAME }}-Windows.zip
          ${{ env.PLUGIN_NAME }}-macOS.zip
          ${{ env.PLUGIN_NAME }}-Linux.tar.gz
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
'@

Write-File "$root\CMakeLists.txt" @'
cmake_minimum_required(VERSION 3.15)
project(MySuperSynth VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(juce)

juce_add_plugin(MySuperSynth
    COMPANY_NAME "BlackMetalAudio"
    PLUGIN_NAME "BM Synth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "BM Synth")

target_sources(MySuperSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/DSP/EngineCore.cpp
    Source/DSP/EngineCore.h
    Source/DSP/FXChain.cpp
    Source/DSP/FXChain.h)

target_link_libraries(MySuperSynth PRIVATE 
    juce::juce_audio_utils 
    juce::juce_dsp
    juce::juce_gui_extra)
'@

Write-File "$root\Source\PluginProcessor.h" @'
#pragma once
#include <JuceHeader.h>
#include "DSP/EngineCore.h"
#include "DSP/FXChain.h"

class MySuperSynthAudioProcessor : public juce::AudioProcessor {
public:
    MySuperSynthAudioProcessor();
    ~MySuperSynthAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    EngineCore engine;
    FXChain fxChain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MySuperSynthAudioProcessor)
};
'@

Write-File "$root\Source\PluginProcessor.cpp" @'
#include "PluginProcessor.h"
#include "PluginEditor.h"

MySuperSynthAudioProcessor::MySuperSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : juce::AudioProcessor(juce::AudioProcessor::BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
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
    
    int modeIndex = parameters.getParameterAsValue("fxMode").getValue();
    fxChain.setMode(static_cast<FXMode>(modeIndex));
}

void MySuperSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    
    engine.processMidi(midiMessages, buffer);
    
    int modeIndex = parameters.getParameterAsValue("fxMode").getValue();
    fxChain.setMode(static_cast<FXMode>(modeIndex));

    fxChain.process(buffer);
}

juce::AudioProcessorEditor* MySuperSynthAudioProcessor::createEditor() {
    return new MySuperSynthAudioProcessorEditor(*this);
}

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
'@

Write-File "$root\Source\PluginEditor.h" @'
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
'@

Write-File "$root\Source\PluginEditor.cpp" @'
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
'@

Write-File "$root\Source\DSP\EngineCore.h" @'
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
'@

Write-File "$root\Source\DSP\EngineCore.cpp" @'
#include "EngineCore.h"
'@

Write-File "$root\Source\DSP\FXChain.h" @'
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
'@

Write-File "$root\Source\DSP\FXChain.cpp" @'
#include "FXChain.h"
'@

Write-Host "`n✅ ГОТОВО! Папка '$root' создана со всеми файлами." -ForegroundColor Cyan
Write-Host "Теперь ты можешь заархивировать эту папку или выполнить:" -ForegroundColor Yellow
Write-Host "  cd $root" -ForegroundColor White
Write-Host "  git init" -ForegroundColor White
Write-Host "  git add ." -ForegroundColor White
Write-Host "  git commit -m 'Initial commit'" -ForegroundColor White
Write-Host "  git remote add origin https://github.com/Hunterice348/Synrh-And-FX-Module.git" -ForegroundColor White
Write-Host "  git push -u origin main" -ForegroundColor White
