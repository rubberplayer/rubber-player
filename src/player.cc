#include "./player.h"
#include "./name.h"
#include <cstdio>
#include <cstdlib>
#include <rubberband/RubberBandStretcher.h>
#include <cmath>

Player::~Player()
{
    m_terminate_the_play_thread.store(true);
    if (m_the_play_thread.joinable())
    {
        m_the_play_thread.join();
    }
    //  if (m_pa_simple != NULL)
    //  {
    //      pa_simple_free(m_pa_simple);
    //      m_pa_simple = NULL;
    //  }
    //  if (rubberBandStretcher != NULL)
    //  {
    //      delete rubberBandStretcher;
    //      rubberBandStretcher = NULL;
    //  }
}

Player::Player()
{
    set_pitch_scale(1);
    set_time_ratio(1);
    set_sound_start(0);
    set_sound_end(0);
    m_pa_simple = NULL;
    rubberBandStretcher = NULL;

    set_rubberband_flag_options(RubberBand::RubberBandStretcher::DefaultOptions | RubberBand::RubberBandStretcher::OptionEngineFiner);

    m_play_started.store(false);
    m_terminate_the_play_thread.store(false);
}
void Player::set_rubberband_flag_options(int flag_options)
{
    m_rubberband_flag_options.store(RubberBand::RubberBandStretcher::OptionProcessRealTime | flag_options);
}
int Player::get_rubberband_flag_options()
{
    return m_rubberband_flag_options.load();
}

void Player::connect_to_pulseaudio(int channels, int framerate)
{
    printf("[player] connect to pulseaudio channels %d; framerate %d\n", channels, framerate);
    m_pa_sample_spec.format = PA_SAMPLE_FLOAT32;
    m_pa_sample_spec.channels = channels;
    m_pa_sample_spec.rate = framerate;

    int pa_connect_error = 0;
    m_pa_simple = pa_simple_new(NULL,             // Use the default server.
                                APPLICATION_NAME, // Our application's name.
                                PA_STREAM_PLAYBACK,
                                NULL,              // Use the default device.
                                "Music",           // Description of our stream.
                                &m_pa_sample_spec, // Our sample format.
                                NULL,              // Use default channel map
                                NULL,              // Use default buffering attributes.
                                &pa_connect_error  // Ignore error code.
    );
    printf("[player] is pulseaudio object null ? %p , error? %d , error message : %s \n", m_pa_simple, pa_connect_error, pa_strerror(pa_connect_error));
};
int Player::initialize_RubberBand(int channels, int samplerate)
{
    /*
        RubberBand::RubberBandStretcher::Options rubberband_options =
            RubberBand::RubberBandStretcher::DefaultOptions | RubberBand::RubberBandStretcher::OptionProcessRealTime | RubberBand::RubberBandStretcher::OptionEngineFiner;
    */

    RubberBand::RubberBandStretcher::Options rubberband_options = m_rubberband_flag_options.load() | RubberBand::RubberBandStretcher::OptionProcessRealTime;
    rubberBandStretcher = new RubberBand::RubberBandStretcher(samplerate, channels, rubberband_options);
    printf("[player] RubberBand engine version : %d\n", rubberBandStretcher->getEngineVersion());
    printf("[player] RubberBand channel count : %ld\n", rubberBandStretcher->getChannelCount());
    return rubberband_options;
}
void Player::play_always()
{
    int channels = m_sound->get_channels();
    connect_to_pulseaudio(channels, m_sound->get_samplerate());
    int rubberband_flag_options = initialize_RubberBand(channels, m_sound->get_samplerate());

    long position = 0;

    long max_rubberband_input_block_size = 256 * 4; // max block size to be fed to rubberband
    float **rubberband_desinterleaved_input = new float *[channels];
    for (int i = 0; i < channels; ++i)
    {
        rubberband_desinterleaved_input[i] = new float[max_rubberband_input_block_size];
    }

    size_t rubberband_output_block_size = 2 * 3 * 4 * 5 * 6 * 7 * 256;
    float **rubberband_output = new float *[channels];
    for (int i = 0; i < channels; ++i)
    {
        rubberband_output[i] = new float[rubberband_output_block_size];
    }

    float *pulseaudio_interleaved_input = new float[rubberband_output_block_size * channels];

    long samples_sent_to_sink = 0;
    auto date_of_first_sample_sent_to_sink = std::chrono::high_resolution_clock::now();

    long previous_play_state = m_play_started.load();
    using namespace std::chrono_literals;
    for (;;)
    {
        if (m_terminate_the_play_thread.load() == true)
        {
            break;
        }
        bool l_play_started = m_play_started.load();
        if (l_play_started == false)
        {
            if (previous_play_state != l_play_started)
            {
                // just stopped
                auto begin = date_of_first_sample_sent_to_sink;
                auto end = std::chrono::high_resolution_clock::now();
                long previous_play_session_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                printf("[player] just stopped, with %ld samples sent\n", samples_sent_to_sink);
                long samples_sent_to_sink_ms = (1000 * samples_sent_to_sink) / ((long)(m_sound->sfinfo.samplerate));
                printf("[player]                    %ld ms of samples sent\n", samples_sent_to_sink_ms);
                printf("[player]      while %ld ms of time passed\n", previous_play_session_duration_ms);
                long samples_remaining_ms = samples_sent_to_sink_ms - previous_play_session_duration_ms;
                printf("[player] so %ld ms of time are still played by sink\n", samples_remaining_ms);
                samples_sent_to_sink = 0;
                previous_play_state = l_play_started;
                // i don't know why
                int pa_drain_error;
                if (pa_simple_drain(m_pa_simple, &pa_drain_error) < 0)
                {
                    printf( "[player] pa_simple_drain() failed: %s\n", pa_strerror(pa_drain_error));
                }
                // so sleep before doing anything;
                std::this_thread::sleep_for(std::chrono::milliseconds(samples_remaining_ms));
            }
            else
            {
                // sleep a little to not loop furiously
                std::this_thread::sleep_for(100ms);
            }
            continue;
        }
        else
        {
            if (previous_play_state != l_play_started)
            {
                printf("[player] just started, with %ld samples sent \n", samples_sent_to_sink);
            }
            previous_play_state = l_play_started;
        }
        int maybe_changed_rubberband_flag_options = m_rubberband_flag_options.load();
        if (maybe_changed_rubberband_flag_options != rubberband_flag_options)
        {
            printf("[player] saw changed option\n");
            rubberBandStretcher->setTransientsOption(maybe_changed_rubberband_flag_options);
            rubberBandStretcher->setDetectorOption(maybe_changed_rubberband_flag_options);
            rubberBandStretcher->setPhaseOption(maybe_changed_rubberband_flag_options);
            rubberBandStretcher->setFormantOption(maybe_changed_rubberband_flag_options);
            rubberBandStretcher->setPitchOption(maybe_changed_rubberband_flag_options);
            rubberband_flag_options = maybe_changed_rubberband_flag_options;
        }

        long selection_start = m_sound_start.load();
        long selection_end = m_sound_end.load();
        long selection_left = std::min(selection_start, selection_end);
        long selection_right = std::max(selection_start, selection_end);

        if (selection_right == selection_left)
        {
            // collapesed selection
            selection_right = m_sound->get_frame_count();
        }
        double pitch_scale = m_pitch_scale.load();
        double time_ratio = m_time_ratio.load();
        rubberBandStretcher->setTimeRatio(time_ratio);
        rubberBandStretcher->setPitchScale(pitch_scale);

        if (position < selection_left)
            position = selection_left;

        if (position >= selection_right)
            position = selection_left;

        size_t samples_requiered = rubberBandStretcher->getSamplesRequired();

        long block_size = std::min(max_rubberband_input_block_size, (long)samples_requiered);

        if ((position + block_size) >= selection_right)
        {
            block_size = selection_right - position;
        }
        float *ppp = m_sound->ptr + position * channels;
        {
            for (long c = 0; c < channels; c++)
            {
                for (long i = 0; i < block_size; i++)
                {
                    rubberband_desinterleaved_input[c][i] = *(ppp + (channels * i) + c);
                }
            }
        }

        bool last_process = false;
        // rubberBandStretcher->process(&ppp, block_size, last_process);
        rubberBandStretcher->process(rubberband_desinterleaved_input, block_size, last_process);

        int available = rubberBandStretcher->available();
        int retrieve_from_rubberband_size = std::min((int)rubberband_output_block_size, available);
        if (retrieve_from_rubberband_size > 0)
        {
            // printf("retrieve %d\n",retrieve_from_rubberband_size);
            rubberBandStretcher->retrieve(rubberband_output, retrieve_from_rubberband_size);
            {
                // copy to pulseaudio interleaved input
                for (long c = 0; c < channels; c++)
                {
                    for (long i = 0; i < retrieve_from_rubberband_size; i++)
                    {
                        pulseaudio_interleaved_input[i * channels + c] = rubberband_output[c][i];
                    }
                }
            }
            int pa_write_error;
            pa_simple_write(m_pa_simple, (void *)pulseaudio_interleaved_input, retrieve_from_rubberband_size * sizeof(float) * channels, &pa_write_error);

            if (samples_sent_to_sink == 0)
            {
                date_of_first_sample_sent_to_sink = std::chrono::high_resolution_clock::now();
            }
            samples_sent_to_sink += retrieve_from_rubberband_size;

            if (pa_write_error < 0)
            {
                printf("[player] pa_write_error\n");
                printf("[player] error while writing to pa sink (%d samples) ? %d : %s\n", retrieve_from_rubberband_size, pa_write_error, pa_strerror(pa_write_error));
            }
        }
        position += block_size;

        m_sound_position.store(position);

        if (block_size == 0)
        {
            printf("[player] Should not be here ?\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    printf("[player] end of thread\n");

    int pa_drain_error;
    if (pa_simple_drain(m_pa_simple, &pa_drain_error) < 0)
    {
        printf( "[player] pa_simple_drain() failed: %s\n", pa_strerror(pa_drain_error));
    }

    for (int i = 0; i < channels; ++i)
    {
        delete[] rubberband_desinterleaved_input[i];
    }
    delete[] rubberband_desinterleaved_input;

    for (int i = 0; i < channels; ++i)
    {
        delete[] rubberband_output[i];
    }
    delete[] rubberband_output;

    delete[] pulseaudio_interleaved_input;

    if (m_pa_simple != NULL)
    {
        pa_simple_free(m_pa_simple);
        m_pa_simple = NULL;
    }

    if (rubberBandStretcher != NULL)
    {
        delete rubberBandStretcher;
        rubberBandStretcher = NULL;
    }
}

void Player::start_playing()
{
    m_play_started.store(true);
}

void Player::stop_playing()
{
    m_play_started.store(false);
}
void Player::stop_playing_thread()
{
    // terminate potential previous thread
    m_terminate_the_play_thread.store(true);
    if (m_the_play_thread.joinable())
    {
        m_the_play_thread.join();
    }
    m_terminate_the_play_thread.store(false);
    m_play_started.store(false);
}
void Player::start_playing_thread()
{
    if (m_sound != NULL)
    {
        m_the_play_thread = std::thread([this]
                                        { play_always(); });
    }
}
void Player::set_sound(Sound *sound)
{
    stop_playing_thread();

    // set new sound
    m_sound = sound;
    set_sound_start(0);
    set_sound_end(m_sound->get_frame_count());
    m_sound_position.store(0);

    // start the thread
    start_playing_thread();
}
void Player::set_pitch_scale(double pitch_scale)
{
    m_pitch_scale.store(pitch_scale);
}
void Player::set_time_ratio(double time_ratio)
{
    m_time_ratio.store(time_ratio);
}
void Player::set_sound_start(long sound_start)
{
    m_sound_start.store(sound_start);
}
void Player::set_sound_end(long sound_end)
{
    m_sound_end.store(sound_end);
}
