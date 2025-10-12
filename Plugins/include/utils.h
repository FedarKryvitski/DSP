#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <complex>
#include <numbers>

namespace utils {

using complex = std::complex<float>;

constexpr float pi = std::numbers::pi_v<float>;
constexpr float kDefaultSampleRate = 48000.f;

constexpr int kMinFrequency = 20;
constexpr int kMaxFrequency = 20'000;
constexpr int kBufferSize = kMaxFrequency - kMinFrequency + 1;

} // namespace utils

#endif //CONSTANTS_H
