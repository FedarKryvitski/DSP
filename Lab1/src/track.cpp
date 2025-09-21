#include "track.h"

#include <algorithm>

Track::Track(unsigned int channels) noexcept
: m_channels(channels)
{}


void Track::insert(std::span<const float> data, const size_t pos) {

    if (const size_t newSize = pos * m_channels + data.size(); newSize > m_buffer.size()) {
        m_buffer.resize(newSize);
    }

    const auto beginIt = std::next(m_buffer.begin(), pos * m_channels);
    const auto endIt = std::next(m_buffer.begin(), pos * m_channels + data.size());

    std::transform(beginIt, endIt, beginIt, [&, i=0](const auto& elem) mutable -> float {
        return elem + data[i++];
    });
}

void Track::normalize() {
    const float maxValue = *std::ranges::max_element(m_buffer);
    if (maxValue > 1.f) {
        std::ranges::transform(m_buffer, m_buffer.begin(), [maxValue](const auto& elem) -> float {
            return elem / maxValue;
        });
    }
}
