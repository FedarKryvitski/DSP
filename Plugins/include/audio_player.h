#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <alsa/asoundlib.h>
#include <span>
#include <string>

class AudioPlayer
{
public:
    explicit AudioPlayer(unsigned int rate = 44100, unsigned int channels = 2) noexcept;
    virtual ~AudioPlayer();

    [[nodiscard]] bool start();
    [[nodiscard]] bool stop();

    void playSound(const std::span<float>& data) const;
    void setDevice(const std::string& device);

private:
    snd_pcm_format_t m_format{SND_PCM_FORMAT_FLOAT};
    snd_pcm_t *m_handle{nullptr};
    std::string m_device{"default"};

    unsigned int m_channels{2};
    unsigned int m_rate{44100};
    bool m_isPlaying{false};
};

#endif //AUDIOPLAYER_H
