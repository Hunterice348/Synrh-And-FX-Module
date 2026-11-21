#pragma once
#include <JuceHeader.h>

enum class FXMode {
    Analog,
    Digital,
    Tube
};

class MultiModeEffect {
public:
    void prepare(const juce::dsp::ProcessSpec& spec) {
        drive.prepare(spec);
        delay.prepare(spec);
        reverb.prepare(spec);
        // Инициализация остальных эффектов...
    }

    void setMode(FXMode mode) {
        currentMode = mode;
        updateParameters();
    }

    void process(juce::AudioBuffer<float>& buffer) {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        // Пример логики Drive в зависимости от режима
        if (currentMode == FXMode::Tube) {
            drive.setGainDB(12.0f); // Сильный перегруз
            // Тут можно добавить waveshaper для имитации лампы
        } else if (currentMode == FXMode::Analog) {
            drive.setGainDB(6.0f); // Мягкий сатурейшн
        } else {
            drive.setGainDB(0.0f); // Чисто
        }

        drive.process(context);
        delay.process(context);
        reverb.process(context);
        // Обработать остальные эффекты аналогично
    }

private:
    FXMode currentMode = FXMode::Digital;
    
    // JUCE DSP модули
    juce::dsp::Gain<float> drive;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delay;
    juce::dsp::Reverb reverb;
    
    void updateParameters() {
        // Настройка параметров фильтров и эффектов под выбранный режим
        if (currentMode == FXMode::Analog) {
            reverb.setDamping(0.3f); // Более темный реверб
        } else {
            reverb.setDamping(0.8f); // Яркий цифровой
        }
    }
};
