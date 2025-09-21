#ifndef TRACK_H
#define TRACK_H

#include <vector>
#include <span>

class Track {
public:
    explicit Track(unsigned int channels) noexcept;

    void clear();
    void insert(std::span<const float> data, size_t pos = 0);
    void normalize();

    [[nodiscard]] auto getData() const { return m_buffer; }

private:
    std::vector<float> m_buffer;
    const unsigned int m_channels{2};
};

#endif //TRACK_H
