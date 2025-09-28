#ifndef WAVFILE_H
#define WAVFILE_H

#include <string>
#include <vector>
#include <span>
#include <cstdint>

class WavFile {
public:
    explicit WavFile(unsigned int sampleRate = 48000, unsigned int channels = 2) noexcept;
    virtual ~WavFile() = default;

    void clear();
    void append(const std::span<const float>& data);
    [[nodiscard]] std::vector<float> data() const;

    [[nodiscard]] bool save(const std::string& filename) const;
    [[nodiscard]] bool load(const std::string& filename);

private:
    std::vector<int16_t> m_buffer;
    const unsigned int m_channels;
    const unsigned int m_sampleRate;
};

#endif //WAVFILE_H
