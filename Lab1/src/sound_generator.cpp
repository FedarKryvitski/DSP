#include "sound_generator.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <numbers>

namespace {
    constexpr auto PI = std::numbers::pi;
}

namespace SoundGenerator {

    Generator::Generator(unsigned int sampleRate, unsigned int channels) noexcept
    : m_sampleRate(sampleRate)
    , m_channels(channels)
    {}

    float Generator::getSinValue(float frequency, float phase) {
        const auto value = static_cast<float>(std::sin(2 * PI * frequency * phase));
        return value;
    }

    float Generator::getSawValue(float frequency, float phase) {
        const auto value = frequency * phase - std::floor(frequency * phase);
        return (value - 0.5f) * 2.f;
    }

    float Generator::getTriangleValue(float frequency, float phase) {
        const auto value = std::abs(2.f * (frequency * phase - std::floor(frequency * phase + 0.5f)));
        return (value - 0.5f) * 2.f;
    }

    float Generator::getSquareValue(float frequency, float phase) {
        constexpr float width{ 0.5f };
        const auto pos = std::fmod(frequency * phase, 1.f);
        return pos < width ? 1.f : -1.f;
    }

    float Generator::getRandValue() {
        static std::random_device rd;
        static std::default_random_engine generator(rd());
        static std::uniform_real_distribution distribution(-1.f, 1.f);

        return distribution(generator);
    }

    float Generator::getSoundValue(WaveType waveType, float frequency, float phase) {
        switch (waveType) {
            case WaveType::SINUSOID:
                return getSinValue(frequency, phase);
            case WaveType::SAWTOOTH:
                return getSawValue(frequency, phase);
            case WaveType::TRIANGLE:
                return getTriangleValue(frequency, phase);
            case WaveType::IMPULSE:
                return getSquareValue(frequency, phase);
            case WaveType::NOISE:
                return getRandValue();
            default:
                return 0.f;
        }
    }

    std::vector<float> Generator::getSound(WaveType waveType, float amplitude, float frequency, int sampleIndex, float phase) const {
        std::vector<float> result(m_bufferSamples * m_channels);

        std::ranges::generate(result, [&, i=0]() mutable -> float {
            if (i++ % m_channels == 0)
                sampleIndex++;

            float currentPhase{ phase + static_cast<float>(sampleIndex) / static_cast<float>(m_sampleRate) };
            float soundValue = getSoundValue(waveType, frequency, currentPhase);

            return amplitude * soundValue;
        });

        return result;
    }

    std::vector<float> Generator::getModulationSound(ModulationType modulationType, WaveType waveType, float amplitude, float frequency, int sampleIndex, float phase) const {
        std::vector<float> result(m_bufferSamples * m_channels);
        std::ranges::generate(result, [&, i=0]() mutable -> float {
            if (i++ % m_channels == 0)
                sampleIndex++;

            const float currentPhase{ phase + static_cast<float>(sampleIndex) / static_cast<float>(m_sampleRate) };

            if (modulationType == ModulationType::AMPLITUDE) {
                constexpr float amplitudeModulationFrequency{ 1.f };

                const float amplitudeMultiplier = getSinValue(amplitudeModulationFrequency, currentPhase);
                const float currentAmplitude = amplitude * amplitudeMultiplier;
                const float soundValue = getSoundValue(waveType, frequency, currentPhase);

                return currentAmplitude * soundValue;
            }
            if (modulationType == ModulationType::FREQUENCY) {
                constexpr float modulationFrequency{ 1.f };
                constexpr float frequencyModulationAmplitude{ 30.f };

                const float integratedFrequency = static_cast<float>(frequencyModulationAmplitude * std::cos(2.f * PI * modulationFrequency * currentPhase));
                const float soundValue = static_cast<float>(std::sin(2.f * PI * currentPhase * frequency + integratedFrequency));

                return amplitude * soundValue;
            }
            return 0.f;
        });

        return result;
    }

} // namespace SoundGenerator