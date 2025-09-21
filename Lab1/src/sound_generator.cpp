#include "sound_generator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <bits/ostream.tcc>

namespace SoundGenerator {

    Generator::Generator(unsigned int sampleRate, unsigned int channels) noexcept
    : m_sampleRate(sampleRate)
    , m_channels(channels)
    {}

    float Generator::getSinValue(float frequency, float phase) {
        const auto value = static_cast<float>(std::sin(2 * M_PI * frequency * phase));
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

    std::vector<float> Generator::getSound(float amplitude, float frequency, int sampleIndex, float phase) const {
        std::vector<float> result(m_bufferSamples * m_channels);

        std::ranges::generate(result, [&, i=0]() mutable -> float {
            if (i++ % m_channels == 0)
                sampleIndex++;

            float currentPhase{ phase + static_cast<float>(sampleIndex) / static_cast<float>(m_sampleRate) };
            float multiplier{0.f};

            switch (m_type) {
                case WaveType::SINUSOID:
                    multiplier = getSinValue(frequency, currentPhase);
                    break;
                case WaveType::SAWTOOTH:
                    multiplier = getSawValue(frequency, currentPhase);
                    break;
                case WaveType::TRIANGLE:
                    multiplier = getTriangleValue(frequency, currentPhase);
                    break;
                case WaveType::IMPULSE:
                    multiplier = getSquareValue(frequency, currentPhase);
                    break;
                case WaveType::NOISE:
                    multiplier = getRandValue();
                    break;
                default:
                    break;
            }

            return amplitude * multiplier;
        });

        return result;
    }

    std::vector<float> Generator::getModulationSound(ModulationType modulation, float amplitude, float frequency, int sampleIndex, float phase) const {
        std::vector<float> result(m_bufferSamples * m_channels);
        float integratedPhase = phase;

        std::ranges::generate(result, [&, i=0]() mutable -> float {
            if (i++ % m_channels == 0)
                sampleIndex++;

            const float currentTime = phase + static_cast<float>(sampleIndex) / static_cast<float>(m_sampleRate);

            float multiplier{0.f};
            float currentAmplitude{ amplitude };

            switch (modulation) {
                case ModulationType::AMPLITUDE:
                    currentAmplitude = amplitude * getSawValue(1.f, currentTime) * 2.f;
                    break;
                case ModulationType::FREQUENCY:
                {
                    constexpr float modulationFrequency{ 2.f };
                    constexpr float b{ 2.f };

                    const float frequencyDiff = b * std::sin(2.f * M_PI * modulationFrequency * currentTime);
                    const float fmSignal = amplitude * std::sin(2.f * M_PI * currentTime * (frequency + frequencyDiff));
                    return fmSignal;
                }
                default:
                    break;
            }

            switch (m_type) {
                case WaveType::SINUSOID:
                    multiplier = getSinValue(frequency, currentTime);
                    break;
                case WaveType::SAWTOOTH:
                    multiplier = getSawValue(frequency, currentTime);
                    break;
                case WaveType::TRIANGLE:
                    multiplier = getTriangleValue(frequency, currentTime);
                    break;
                case WaveType::IMPULSE:
                    multiplier = getSquareValue(frequency, currentTime);
                    break;
                case WaveType::NOISE:
                    multiplier = getRandValue();
                    break;
                default:
                    break;
            }

            return currentAmplitude * multiplier;
        });

        return result;
    }

} // namespace SoundGenerator