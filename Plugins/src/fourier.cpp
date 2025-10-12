#include "fourier.h"

#include <cmath>
#include <numbers>
#include <algorithm>
#include <numeric>

namespace fourier {

namespace {

using namespace utils;

int upper_log2(const int x) {
    constexpr int upperBound{ 30 };
    for (int i = 0; i < upperBound; ++i) {
        const int temp = 1 << i;
        if (temp >= x)
            return i;
    }
    return upperBound;
}

const std::vector<complex> phase_vec(const int len)
{
    constexpr float radius{ 1.f };
    std::vector<complex> res(len);
    std::generate(res.begin(), res.end(), [&, i=0]() mutable{
        const float phase = -2.f * pi * i++ / len;
        return std::polar(radius, phase);
    });

    return res;
}

void forward(std::vector<complex> &prev, std::vector<complex> &temp,
             const std::vector<complex> &phases, const int turn, const int n_bits)
{
    if (turn == n_bits)
        return;

    const int group_size = 1 << (turn + 1);
    const int num_groups = prev.size() / group_size;
    const int phase_angular_freq = num_groups;
    for (int i_group = 0; i_group < num_groups; ++i_group)
    {
        const int base_index = i_group * group_size;
        for (int j = 0; j < group_size / 2; ++j)
        {
            const int x0_index = base_index + j;
            const int x1_index = base_index + group_size / 2 + j;
            prev[x1_index] *= phases[j * phase_angular_freq];
            temp[x0_index] = prev[x0_index] + prev[x1_index];
            temp[x1_index] = prev[x0_index] - prev[x1_index];
        }
    }
}

void bit_reversal_permutation(std::vector<complex> &vec, const int n_bits)
{
    if (vec.size() <= 2)
        return;

    if (vec.size() == 4)
    {
        std::swap(vec[1], vec[3]);
        return;
    }

    std::vector<int> bit_rerversal(vec.size());

    bit_rerversal[0] = 0;
    bit_rerversal[1] = 1 << (n_bits - 1);
    bit_rerversal[2] = 1 << (n_bits - 2);
    bit_rerversal[3] = bit_rerversal[1] + bit_rerversal[2];

    for (int k = 3; k <= n_bits; ++k)
    {
        const int nk = (1 << k) - 1;
        const int n_km1 = (1 << (k - 1)) - 1;
        bit_rerversal[nk] = bit_rerversal[n_km1] + (1 << (n_bits - k));
        for (int i = 1; i <= n_km1; ++i)
        {
            bit_rerversal[nk - i] = bit_rerversal[nk] - bit_rerversal[i];
        }
    }

    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (bit_rerversal[i] > i)
            std::swap(vec[i], vec[bit_rerversal[i]]);
    }
}

std::vector<complex> fft_impl(std::span<const complex> inputs)
{
    if (inputs.empty())
    {
        return {};
    }
    const int n_bits = upper_log2(inputs.size());
    const int len = 1 << n_bits;
    const std::vector<complex> phases = phase_vec(len);
    std::vector<complex> prev(len);
    std::vector<complex> temp(len);
    std::copy(inputs.begin(), inputs.end(), prev.begin());
    bit_reversal_permutation(prev, n_bits);
    for (int turn = 0; turn < n_bits; ++turn){
        forward(prev, temp, phases, turn, n_bits);
        std::swap(prev, temp);
    }
    return prev;
}

std::vector<complex> ifft_impl(std::span<const complex> inputs)
{
    std::vector<complex> reverse_freq_spectrum(inputs.begin(), inputs.end());
    std::reverse(std::next(reverse_freq_spectrum.begin()), reverse_freq_spectrum.end());
    return fft_impl(reverse_freq_spectrum);
}

} // namespace

std::vector<complex> dft(std::span<const float> input, float sampleRate) {
    const size_t N = input.size();
    std::vector<complex> output(kBufferSize, complex{});

    for (int k = kMinFrequency; k <= kMaxFrequency; ++k) {
        complex sum(0, 0);
        for (size_t n = 0; n < N; ++n) {
            const float angle = -2.f * pi * k * n / sampleRate;
            sum += input[n] * complex(std::cos(angle), std::sin(angle));
        }
        output[k - kMinFrequency] = sum;
    }

    return output;
}

std::vector<float> idft(std::span<const complex> input, const size_t N, float sampleRate) {
    std::vector<float> output(N);
    for (size_t n = 0; n < N; ++n) {
        complex sum{};
        for (int k = kMinFrequency; k < kMaxFrequency; ++k) {
            const float angle = 2.f * pi * k * n / sampleRate;
            sum += input[k - kMinFrequency] * complex(std::cos(angle), std::sin(angle));
        }
        output[n] = sum.real() / N;
    }
    return output;
}

std::vector<float> dft_freqs() {
    std::vector<float> output(kBufferSize);
    std::iota(output.begin(), output.end(), kMinFrequency);
    return output;
}

std::vector<complex> fft(std::span<const float> inputs) {
    std::vector<complex> data;
    std::ranges::transform(inputs, std::back_inserter(data), [](const auto& num) {
        return std::complex(num, 0.f);
    });

    return fft_impl(data);
}

std::vector<float> ifft(std::span<const complex> inputs) {
    const int N = inputs.size();
	const auto data = ifft_impl(inputs);
    std::vector<float> result;
    std::ranges::transform(data, std::back_inserter(result), [N](const auto& num) {
        return num.real() / N;
    });

    return result;
}

std::vector<float> fft_freqs(size_t N, float sampleRate) {
	std::vector<float> freqs(N);
    for (size_t i = 0; i < N; ++i) {
        if (i <= N / 2) {
            freqs[i] = static_cast<float>(i) * sampleRate / N;
        } else {
            freqs[i] = static_cast<float>(i - N) * sampleRate / N;
        }
    }
    return freqs;
}


} // namespace fourier