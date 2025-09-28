#include "../include/wav_file.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

namespace {

#pragma pack(push, 1)
    struct WavHeader {
        char riff[4];
        uint32_t chunk_size;
        char wave[4];
        char fmt[4];
        uint32_t subchunk1_size;
        uint16_t audio_format;
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
        char data[4];
        uint32_t data_size;
    };
#pragma pack(pop)

}

WavFile::WavFile(unsigned int sampleRate, unsigned int channels) noexcept
: m_channels(channels), m_sampleRate(sampleRate)
{}

void WavFile::clear() {
    m_buffer.clear();
}

void WavFile::append(const std::span<const float>& data) {
    constexpr float multiplier{ 32767.f };
    std::ranges::transform(data, std::back_inserter(m_buffer), [](const auto& sample) {
        return static_cast<int16_t>(sample * multiplier);
    });
}

std::vector<float> WavFile::data() const {
    constexpr float divider{ 32767.f };

    std::vector<float> data(m_buffer.size());
    std::ranges::transform(m_buffer, data.begin(), [](const auto& sample) {
        return static_cast<float>(sample) / divider;
    });

    return data;
}

bool WavFile::save(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << std::endl;
        return false;
    }

    constexpr uint16_t BITS_PER_SAMPLE = 16;
    constexpr uint16_t PCM_FORMAT = 1;

    const auto frame_count = static_cast<uint32_t>(m_buffer.size());
    const uint32_t data_length = frame_count * sizeof(int16_t);

    WavHeader header{};
    std::memcpy(header.riff, "RIFF", 4);
    header.chunk_size = data_length + sizeof(WavHeader) - 8;
    std::memcpy(header.wave, "WAVE", 4);
    std::memcpy(header.fmt, "fmt ", 4);
    header.subchunk1_size = 16;
    header.audio_format = PCM_FORMAT;
    header.num_channels = static_cast<uint16_t>(m_channels);
    header.sample_rate = m_sampleRate;
    header.byte_rate = m_sampleRate * m_channels * sizeof(int16_t);
    header.block_align = static_cast<uint16_t>(m_channels * sizeof(int16_t));
    header.bits_per_sample = BITS_PER_SAMPLE;
    std::memcpy(header.data, "data", 4);
    header.data_size = data_length;

    file.write(reinterpret_cast<const char *>(&header), sizeof(decltype(header)));
    file.write(reinterpret_cast<const char *>(m_buffer.data()), data_length);

    return true;
}

bool WavFile::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << std::endl;
        return false;
    }

    WavHeader header{};
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    m_buffer.resize(header.data_size / sizeof(int16_t));
    file.read(reinterpret_cast<char*>(m_buffer.data()), header.data_size);

    return true;
}
