#include "filter.h"

#include <algorithm>

namespace filter {

std::vector<complex> filter(std::span<const complex> input, std::span<const float> freqs, int lowerBound, int upperBound)
{
    std::vector<complex> result(input.begin(), input.end());
    const size_t N = input.size();

    for (size_t i = 0; i < N; ++i) {
        const float freq = std::abs(freqs[i]);
        if (freq < lowerBound || freq > upperBound) {
            result[i] = complex{};
        }
    }

    return result;
}

}