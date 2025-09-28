#include "fourier.h"

#include <cmath>
#include <numbers>
#include <algorithm>

namespace fourier {

constexpr float pi = std::numbers::pi_v<float>;
constexpr int kMinFrequency = 20;
constexpr int kMaxFrequency = 20'000;
constexpr int kBufferSize = kMaxFrequency - kMinFrequency + 1;

std::vector<complex> dft(std::span<const float> input) {
    const size_t N = input.size();
    std::vector<complex> output(kBufferSize, complex{});

    for (int k = kMinFrequency; k <= kMaxFrequency; ++k) {
        complex sum(0, 0);
        for (size_t n = 0; n < N; ++n) {
            const float angle = -2.f * pi * k * n / N;
            sum += input[n] * complex(std::cos(angle), std::sin(angle));
        }
        output[k - kMinFrequency] = sum;
    }

    return output;
}

std::vector<float> idft(std::span<const complex> input, size_t N) {
    std::vector<float> output(N, 0.0f);

    for (size_t n = 0; n < N; ++n) {
        complex sum(0, 0);
        for (int k = kMinFrequency; k <= kMaxFrequency; ++k) {
            const float angle = 2.f * pi * k * n / N;
            sum += input[k - kMinFrequency] * complex(std::cos(angle), std::sin(angle));
        }
        output[n] = sum.real() / N;
    }

    return output;
}

} // namespace fourier