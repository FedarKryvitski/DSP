#ifndef FOURIER_H
#define FOURIER_H

#include "utils.h"

#include <span>
#include <vector>

namespace fourier {

using complex = utils::complex;

std::vector<complex> dft(std::span<const float> input, float sampleRate = utils::kDefaultSampleRate);
std::vector<float> idft(std::span<const complex> input, size_t N, float sampleRate = utils::kDefaultSampleRate);
std::vector<float> dft_freqs();

std::vector<complex> fft(std::span<const float> inputs);
std::vector<float> ifft(std::span<const complex> inputs);
std::vector<float> fft_freqs(size_t N, float sampleRate = utils::kDefaultSampleRate);

}

#endif //FOURIER_H
