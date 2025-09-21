#include "../include/audio_player.h"

#include <iostream>

AudioPlayer::AudioPlayer(unsigned int rate, unsigned int channels) noexcept
: m_channels(channels), m_rate(rate)
{}

AudioPlayer::~AudioPlayer()
{
    if (!stop())
        std::cerr << "Failed to stop audio player." << std::endl;
}

void AudioPlayer::setDevice(const std::string& device)
{
    m_device = device;
}

bool AudioPlayer::start()
{
    if (m_isPlaying)
        return false;

    int err = snd_pcm_open (&m_handle, m_device.c_str(),
                           SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        std::cerr << "cannot open audio device " << m_device << snd_strerror(err);
        return false;
    }

    err = snd_pcm_set_params(m_handle, m_format, SND_PCM_ACCESS_RW_INTERLEAVED,
                             m_channels, m_rate, 1, 50000);
    if (err < 0) {
        std::cerr << "Playback open error: " << snd_strerror(err);
        return false;
    }

    snd_pcm_prepare(m_handle);
    m_isPlaying = true;

    return true;
}

bool AudioPlayer::stop()
{
    if (!m_isPlaying)
        return false;

    snd_pcm_close(m_handle);
    m_isPlaying = false;

    return true;
}

void AudioPlayer::playSound(const std::span<float> &data) const
{
    if (const snd_pcm_sframes_t err = snd_pcm_writei(m_handle, data.data(), data.size() / m_channels); err < 0) {
        if (err == -EPIPE) {
            perror("Buffer overfill");
            snd_pcm_prepare(m_handle);
        } else {
            perror("write to audio interface failed");
        }
    }
}
