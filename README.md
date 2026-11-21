# 🤘 BM Synth Engine

[![Build VST3 Plugin](https://github.com/Hunterice348/Synrh-And-FX-Module/actions/workflows/main.yml/badge.svg)](https://github.com/Hunterice348/Synrh-And-FX-Module/actions/workflows/main.yml)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)
![License](https://img.shields.io/badge/license-Proprietary%20%2F%20MIT-blue)

**BM Synth Engine** — это высококачественный цифровой VST3/AU синтезатор и драм-машина с гибридным звуковым движком, разработанный для создания максимально "сочного" и агрессивного звука. Вдохновлен эстетикой премиальных брендов (Arturia, U-he, Cymatics) с уникальным дизайном интерфейса в стиле "Black Metal 3D".

---

## 🎛 Особенности

- **🎹 Полифонический движок**: 8-голосная полифония с антиалиасинговыми волновыми таблицами (WaveTable) для чистого и мощного звука без цифровых артефактов.
- **🔥 Цепочка эффектов (FX Chain)** с тремя режимами обработки:
  - **Tube**: Насыщенный ламповый перегруз (Tanh waveshaper) и теплый, темный ревербератор.
  - **Analog**: Мягкая аналоговая сатурация и естественный спад частот.
  - **Digital**: Кристально чистая цифровая обработка с яркими хвостами.
- **🧠 AI-Ready архитектура**: Параметры синтезатора вынесены в `AudioProcessorValueTreeState`, что позволяет легко интегрировать внешние AI-скрипты (Python) для генерации JSON-пресетов.
- **💾 Гибкое сохранение**: Полная поддержка нативного сохранения пресетов DAW на любые диски (C:, D:, внешние накопители).
- **🖥 Кроссплатформенность**: Автоматическая сборка под Windows (x64), macOS (Universal: Intel + Apple Silicon) и Linux (x64) через GitHub Actions.

---

## 📥 Установка

### Готовые билды (Рекомендуется)
1. Перейдите в раздел [Releases](https://github.com/Hunterice348/Synrh-And-FX-Module/releases) этого репозитория.
2. Скачайте архив для вашей операционной системы (`Windows.zip`, `macOS.zip` или `Linux.tar.gz`).
3. Распакуйте файл `.vst3` в стандартную папку плагинов вашей DAW:
   - **Windows**: `C:\Program Files\Common Files\VST3\`
   - **macOS**: `/Library/Audio/Plug-Ins/VST3/`
   - **Linux**: `~/.vst3/` или `/usr/lib/vst3/`
4. Перезапустите DAW и найдите плагин **BM Synth**.

---

## 🛠 Сборка из исходного кода (Для разработчиков)

Проект использует **CMake** и **JUCE 8.0.4**. Для сборки убедитесь, что у вас установлены:
- CMake (≥ 3.15)
- C++17 компилятор (MSVC на Windows, Clang на macOS, GCC на Linux)
- Git

### Шаги сборки:

```bash
# 1. Клонируйте репозиторий с подмодулями
git clone --recursive https://github.com/Hunterice348/Synrh-And-FX-Module.git
cd Synrh-And-FX-Module

# 2. Создайте папку сборки
mkdir build && cd build

# 3. Настройте проект (пример для Windows с Ninja)
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..

# Для macOS используйте: cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Release ..
# Для Linux используйте: cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..

# 4. Соберите проект
cmake --build . --config Release --parallel
