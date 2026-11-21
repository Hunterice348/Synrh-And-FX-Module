cmake_minimum_required(VERSION 3.15)
project(MySuperSynth VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем JUCE (предполагается, что он скачан в папку juce)
add_subdirectory(juce)

juce_add_plugin(MySuperSynth
    COMPANY_NAME "BlackMetalAudio"
    PLUGIN_NAME "BM Synth Engine"
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

target_link_libraries(MySuperSynth PRIVATE juce::juce_audio_utils juce::juce_dsp)
