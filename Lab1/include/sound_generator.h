#ifndef SOUNDGENERATOR_H
#define SOUNDGENERATOR_H

#include <vector>

namespace SoundGenerator {

    enum class WaveType {
        SINUSOID,
        IMPULSE,
        TRIANGLE,
        SAWTOOTH,
        NOISE,
    };

    enum class ModulationType {
        AMPLITUDE,
        FREQUENCY
    };

    class Generator {
    public:
        explicit Generator(unsigned int sampleRate = 44100, unsigned int channels = 2) noexcept;
        virtual ~Generator() = default;

        static float getSinValue(float frequency, float phase);
        static float getSawValue(float frequency, float phase);
        static float getTriangleValue(float frequency, float phase);
        static float getSquareValue(float frequency, float phase);
        static float getRandValue();

        void setBufferSize(size_t samples) noexcept { m_bufferSamples = samples; }
        void setWaveType(WaveType type) noexcept { m_type = type; }
        void setSampleRate(unsigned int sampleRate) noexcept { m_sampleRate = sampleRate; }
        void setChannels(unsigned int channels) noexcept { m_channels = channels; }

        [[nodiscard]] size_t getBufferSize() const noexcept { return m_bufferSamples; }
        [[nodiscard]] WaveType getWaveType() const noexcept { return m_type; }
        [[nodiscard]] unsigned int getSampleRate() const noexcept { return m_sampleRate; }
        [[nodiscard]] unsigned int getChannels() const noexcept { return m_channels; }

        [[nodiscard]] std::vector<float> getSound(float amplitude, float frequency, int sampleIndex = 0, float phase = 0.f) const;
        [[nodiscard]] std::vector<float> getModulationSound(ModulationType modulation, float amplitude, float frequency, int sampleIndex = 0, float phase = 0.f) const;

    private:
        WaveType m_type{WaveType::SINUSOID};
        unsigned int m_sampleRate{ 44100 };
        unsigned int m_channels{ 2 };
        size_t m_bufferSamples{ 1024 };
    };

} // namespace SoundGenerator

#endif //SOUNDGENERATOR_H
