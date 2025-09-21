#include "wav_file.h"
#include "audio_player.h"
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
            {.name = "sinusoid.wav", .type = WaveType::SINUSOID},
            {.name = "impulse.wav",  .type = WaveType::IMPULSE},
            {.name = "triangle.wav", .type = WaveType::TRIANGLE},
            {.name = "sawtooth.wav", .type = WaveType::SAWTOOTH},
            {.name = "noise.wav",    .type = WaveType::NOISE}
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
        generator.setWaveType(waveType);

        const auto buffer = generator.getSound(kAmplitude, Notes::A<4>);
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
    constexpr auto kFileName{ "task2.wav" };

    Generator generator(kSampleRate, kChannels);
    generator.setBufferSize(kSampleCount);

    Track track(kChannels);
    generator.setWaveType(WaveType::SAWTOOTH);
    track.insert(generator.getSound(0.3f, Notes::B<3>));
    track.insert(generator.getSound(0.3f, Notes::D<3>));
    track.insert(generator.getSound(0.3f, Notes::Fs<3>));
    track.normalize();

    WavFile file(kSampleRate, kChannels);
    file.append(track.getData());
    if (!file.save(kFileName)) {
        std::cerr << "Failed to save file " << kFileName << std::endl;
    }
}

void task3() {
    constexpr auto kDuration{3s};
    constexpr auto kSampleCount{getSamplesCount(kDuration)};
    constexpr auto kFileName{ "task3.wav" };

    Generator generator(kSampleRate, kChannels);
    generator.setBufferSize(kSampleCount);

    Track track(kChannels);
    generator.setWaveType(WaveType::SINUSOID);
    track.insert(generator.getModulationSound(ModulationType::AMPLITUDE, 0.3f, Notes::A<4>));
    track.insert(generator.getModulationSound(ModulationType::FREQUENCY, 0.3f, Notes::A<4>), kSampleCount);

    WavFile file(kSampleRate, kChannels);
    file.append(track.getData());
    if (!file.save(kFileName)) {
        std::cerr << "Failed to save file " << kFileName << std::endl;
    }
}

void task4() {
    constexpr auto kFileName{ "task4.wav" };
    constexpr auto kBpm{ 360.f };

    Generator generator(kSampleRate, kChannels);
    generator.setWaveType(WaveType::IMPULSE);

    const auto& melody = Melody::createComplexMelody();

    Track track(kChannels);
    std::ranges::for_each(melody, [&](const auto& note) {
        const auto& [frequency, startBar, length] = note;

        const auto noteDurationMs = std::chrono::milliseconds(
            static_cast<int>((length * 4 * 60000.f) / kBpm)
        );

        const auto noteStartTimeMs = std::chrono::milliseconds(
            static_cast<int>((startBar * 4 * 60000.f) / kBpm)
        );

        const auto samplesCount = getSamplesCount(noteDurationMs);
        const auto startSampleIndex = getSamplesCount(noteStartTimeMs);

        generator.setBufferSize(samplesCount);
        track.insert(generator.getSound(0.2f, frequency), startSampleIndex);
    });
    track.normalize();

    WavFile file(kSampleRate, kChannels);
    file.append(track.getData());

    if (!file.save(kFileName)) {
        std::cerr << "Failed to save file " << kFileName << std::endl;
    }
}

int main(int argc, char* argv[]) {
    task1();
    task2();
    task3();
    task4();
    return 0;
}