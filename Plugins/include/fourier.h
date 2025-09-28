#ifndef FOURIER_H
#define FOURIER_H

#include <complex>
#include <span>
#include <vector>

namespace fourier {

using complex = std::complex<float>;

std::vector<complex> dft(std::span<const float> input);

std::vector<float> idft(std::span<const complex> input, size_t N);

}

#endif //FOURIER_H
