#include "wav_file.h"
#include "sound_generator.h"
#include "notes.h"
#include "track.h"
#include "melody.h"

#include <chrono>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

namespace {
    using WaveType = SoundGenerator::WaveType;
    using Generator = SoundGenerator::Generator;
    using ModulationType = SoundGenerator::ModulationType;

    constexpr unsigned int kChannels{2};
    constexpr unsigned int kSampleRate{48000};
    constexpr float kAmplitude{0.3f};
    constexpr auto kDefaultDuration{5s};

    struct FileData {
        std::string name;
        std::chrono::milliseconds duration{kDefaultDuration};
        WaveType type{WaveType::SINUSOID};
    };

    std::vector<FileData> createFiles() {
        return{
            {.name = "TASK_1_Sinusoid.wav", .type = WaveType::SINUSOID},
            {.name = "TASK_1_Impulse.wav",  .type = WaveType::IMPULSE},
            {.name = "TASK_1_Triangle.wav", .type = WaveType::TRIANGLE},
            {.name = "TASK_1_Sawtooth.wav", .type = WaveType::SAWTOOTH},
            {.name = "TASK_1_Noise.wav",    .type = WaveType::NOISE}
        };
    }

    constexpr size_t getSamplesCount(std::chrono::milliseconds duration) {
        const auto milliseconds = duration.count();
        return static_cast<size_t>(static_cast<float>(milliseconds) * kSampleRate / 1000);
    }
}

void task1() {
    WavFile wavFile(kSampleRate, kChannels);
    Generator generator(kSampleRate, kChannels);

    const auto files = createFiles();

    std::ranges::for_each(files, [&](const auto& file) {
        const auto& [name, duration, waveType] = file;
        const auto samplesCount = getSamplesCount(duration);
        generator.setBufferSize(samplesCount);

        const auto buffer = generator.getSound(waveType, kAmplitude, Notes::A<4>);
        wavFile.append(buffer);

        if (!wavFile.save(name)) {
            std::cerr << "Failed to save file " << name << std::endl;
        }
        wavFile.clear();
    });
}

void task2() {
    constexpr auto kDuration{3s};
    constexpr auto kSampleCount{getSamplesCount(kDuration)};
    constexpr auto kFileName{ "TASK_2.wav" };

    Generator generator(kSampleRate, kChannels);
    generator.setBufferSize(kSampleCount);

    Track track(kChannels);
    track.insert(generator.getSound(WaveType::SAWTOOTH, 0.3f, Notes::B<3>));
    track.insert(generator.getSound(WaveType::SAWTOOTH, 0.3f, Notes::D<3>));
    track.insert(generator.getSound(WaveType::SAWTOOTH, 0.3f, Notes::Fs<3>));
    track.normalize();

    WavFile file(kSampleRate, kChannels);
    file.append(track.getData());
    if (!file.save(kFileName)) {
        std::cerr << "Failed to save file " << kFileName << std::endl;
    }
}

void task3() {
    constexpr auto kDuration{5s};
    constexpr auto kSampleCount{getSamplesCount(kDuration)};
    constexpr auto kAmplitudeFileName{ "TASK_3_Amplitude.wav" };
    constexpr auto kFrequencyFileName{ "TASK_3_Frequency.wav" };

    Generator generator(kSampleRate, kChannels);
    generator.setBufferSize(kSampleCount);

    WavFile file(kSampleRate, kChannels);
    Track track(kChannels);
    track.insert(generator.getModulationSound(ModulationType::AMPLITUDE, WaveType::SINUSOID, 0.3f, Notes::A<4>));

    file.append(track.getData());
    if (!file.save(kAmplitudeFileName)) {
        std::cerr << "Failed to save file " << kAmplitudeFileName << std::endl;
    }

    track.clear();
    file.clear();

    track.insert(generator.getModulationSound(ModulationType::FREQUENCY, WaveType::SINUSOID, 0.3f, Notes::A<4>));
    file.append(track.getData());
    if (!file.save(kFrequencyFileName)) {
        std::cerr << "Failed to save file " << kFrequencyFileName << std::endl;
    }
}

void task4() {
    constexpr auto kFileName{ "TASK_4.wav" };
    constexpr auto kBpm{ 160.f };

    Generator generator(kSampleRate, kChannels);

    const auto& melody = Melody::createComplexMelody();

    Track track(kChannels);
    std::ranges::for_each(melody, [&](const auto& note) {
        const auto& [frequency, startBar, length] = note;
        const auto noteDurationMs = std::chrono::milliseconds(
            static_cast<int>(length * 4 * 60000.f / kBpm)
        );
        const auto noteStartTimeMs = std::chrono::milliseconds(
            static_cast<int>(startBar * 4 * 60000.f / kBpm)
        );
        const auto samplesCount = getSamplesCount(noteDurationMs);
        const auto startSampleIndex = getSamplesCount(noteStartTimeMs);

        generator.setBufferSize(samplesCount);
        track.insert(generator.getSound(WaveType::IMPULSE, 0.2f, frequency), startSampleIndex);
    });
    track.normalize();

    WavFile file(kSampleRate, kChannels);
    file.append(track.getData());

    if (!file.save(kFileName)) {
        std::cerr << "Failed to save file " << kFileName << std::endl;
    }
}

int main() {
    task1();
    task2();
    task3();
    task4();
    return 0;
}