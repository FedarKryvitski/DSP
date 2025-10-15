#include <wav_file.h>
#include <filter.h>
#include <fourier.h>

#include <iostream>
#include <algorithm>
#include <fstream>

namespace {
    constexpr int kSampleRate{ 48000 };
    constexpr int kChannels{ 2 };
    constexpr int kOutputSize{ kSampleRate / 2 };

    constexpr int kLowerBoundHz{400};
    constexpr int kUpperBoundHz{1000};

    constexpr auto kInputFileName{"input.wav"};
    constexpr auto kOutputFileName{"output.wav"};
}

int main() {
    WavFile file(kSampleRate, kChannels);

    if (!file.load(kInputFileName))
        return 1;

    const auto data = file.data();

    std::vector<float> left;
    std::copy_if(data.begin(), data.end(), std::back_inserter(left), [i=0]([[maybe_unused]] const auto& elem) mutable {
        return (i++ % 2) == 0;
    });

    std::vector<float> right;
    std::copy_if(data.begin(), data.end(), std::back_inserter(right), [i=0]([[maybe_unused]] const auto& elem) mutable {
        return (i++ % 2) != 0;
    });

    auto complex_left = fourier::fft(left);
    auto complex_right = fourier::fft(right);

    const size_t N = complex_left.size();
    const auto freqs = fourier::fft_freqs(N);

    complex_left = filter::filter(complex_left, freqs, kLowerBoundHz, kUpperBoundHz);
    complex_right = filter::filter(complex_right, freqs, kLowerBoundHz, kUpperBoundHz);

    const auto output_left = fourier::ifft(complex_left);
    const auto output_right = fourier::ifft(complex_right);

    std::vector<float> output(2 * N, 0.0f);
    for (size_t i = 0; i < N; ++i){
        output[i * 2] = output_left[i];
        output[i * 2 + 1] = output_right[i];
    }

    file.clear();
    file.append(output);

    if (!file.save(kOutputFileName))
        return 1;

    return 0;
}