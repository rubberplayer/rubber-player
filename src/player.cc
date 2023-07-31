#include "./player.h"
#include "./name.h"
#include <cstdio>
#include <cstdlib>
#include <rubberband/RubberBandStretcher.h>
#include <cmath>

Player::~Player()
{
    m_terminate_the_play_thread.store(true);
    m_the_play_thread.join();
    pa_simple_free(this->s);
    delete rubberBandStretcher;
}

Player::Player()
{
    set_pitch_scale(1);
    set_time_ratio(1);
    set_sound_start(0);
    set_sound_end(0);
    m_play_started.store(false);
    m_terminate_the_play_thread.store(false);
    RubberBand::RubberBandStretcher::Options rubberband_options = RubberBand::RubberBandStretcher::DefaultOptions | RubberBand::RubberBandStretcher::OptionProcessRealTime;
    rubberBandStretcher = new RubberBand::RubberBandStretcher(48000, 1, rubberband_options);

    printf("RubberBand engine version : %d\n", rubberBandStretcher->getEngineVersion());
    printf("RubberBand engine channel count : %d\n", rubberBandStretcher->getChannelCount());
    connect_to_pulseaudio();

    m_the_play_thread = std::thread([this]
                                    { play_always(); });
}
void Player::connect_to_pulseaudio()
{
    /* https://freedesktop.org/software/pulseaudio/doxygen/simple.html */
    // this->ss.format = PA_SAMPLE_S16NE;
    // this->ss.format = PA_SAMPLE_S16NE;
    this->ss.format = PA_SAMPLE_FLOAT32;
    this->ss.channels = 1;
    this->ss.rate = 48000; // 44100;

    this->s = pa_simple_new(NULL,             // Use the default server.
                            APPLICATION_NAME, // Our application's name.
                            PA_STREAM_PLAYBACK,
                            NULL,    // Use the default device.
                            "Music", // Description of our stream.
                            &ss,     // Our sample format.
                            NULL,    // Use default channel map
                            NULL,    // Use default buffering attributes.
                            NULL     // Ignore error code.
    );
    printf("is pa object null ? %d \n", this->s);
};

void Player::play_always()
{

    pa_sample_spec pass;
    pass.format = PA_SAMPLE_FLOAT32;
    pass.channels = 1;    
    pass.rate = 48000; // 44100;

    pa_simple *pas = pa_simple_new(NULL,             // Use the default server.
                                   APPLICATION_NAME, // Our application's name.
                                   PA_STREAM_PLAYBACK,
                                   NULL,    // Use the default device.
                                   "Music", // Description of our stream.
                                   &pass,   // Our sample format.
                                   NULL,    // Use default channel map
                                   NULL,    // Use default buffering attributes.
                                   NULL     // Ignore error code.
    );
    printf("is pas object null ? %d \n", pas);

    long position = 0;
    size_t rubberband_output_block_size = 2 * 3 * 4 * 5 * 6 * 7 * 256;
    float *rubberband_output = (float *)malloc(rubberband_output_block_size * sizeof(float));

    long samples_sent_to_sink = 0;
    auto date_of_first_sample_sent_to_sink = std::chrono::high_resolution_clock::now();

    long previous_play_state = m_play_started.load();
    using namespace std::chrono_literals;
    for (;;)
    {
        if (m_terminate_the_play_thread.load() == true)
        {
            return;
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
                printf("[player] just stopped, with %d samples sent\n", samples_sent_to_sink);
                long samples_sent_to_sink_ms = (1000 * samples_sent_to_sink) / ((long)(m_sound->sfinfo.samplerate));
                printf("[player]                    %d ms of samples sent\n", samples_sent_to_sink_ms);
                printf("[player]      while %d ms of time passed\n", previous_play_session_duration_ms);
                long samples_remaining_ms = samples_sent_to_sink_ms - previous_play_session_duration_ms;
                printf("[player] so %d ms of time are still played by sink\n", samples_remaining_ms);
                samples_sent_to_sink = 0;
                previous_play_state = l_play_started;
                // i don't know why
                int pa_drain_error;
                if (pa_simple_drain(s, &pa_drain_error) < 0)
                {
                    fprintf(stderr, __FILE__ ": pa_simple_drain() failed: %s\n", pa_strerror(pa_drain_error));
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
                printf("[player] just started, with %d samples sent \n", samples_sent_to_sink);
            }
            previous_play_state = l_play_started;
        }

        double time_position = position / (double)m_sound->sfinfo.samplerate;

        long selection_start = m_sound_start.load();
        long selection_end = m_sound_end.load();
        long selection_left = std::min(selection_start, selection_end);
        long selection_right = std::max(selection_start, selection_end);

        if (selection_right == selection_left)
        {
            // collapesed selection
            selection_right = m_sound->read_count;
        }
        double pitch_scale = m_pitch_scale.load();
        double time_ratio = m_time_ratio.load();
        rubberBandStretcher->setTimeRatio(time_ratio);
        rubberBandStretcher->setPitchScale(pitch_scale);

        // printf("sound samplerate : %d ; channels : %d, selection [%d,%d] ; pitch scale : %f ; time_ratio : %f, position : %d, time position : %f\n", m_sound->sfinfo.samplerate, m_sound->sfinfo.channels, selection_left, selection_right, pitch_scale, time_ratio, position, time_position);

        if (position < selection_left)
            position = selection_left;

        if (position >= selection_right)
            position = selection_left;

        void *sound_pointer = m_sound->ptr + position;

        size_t samples_requiered = rubberBandStretcher->getSamplesRequired();

        long max_block_size = 256 * 4;
        long block_size = std::min(max_block_size, (long)samples_requiered);
        // printf("samples_required %d, block_size %d\n",samples_requiered,block_size);
        if ((position + block_size) >= selection_right)
        {
            block_size = selection_right - position;
        }
        float *ppp = m_sound->ptr + position;
        bool last_process = false;
        rubberBandStretcher->process(&ppp, block_size, last_process);

        int available = rubberBandStretcher->available();
        int retrieve_from_rubberband_size = std::min((int)rubberband_output_block_size, available);
        if (retrieve_from_rubberband_size > 0)
        {
            // printf("retrieve %d\n",retrieve_from_rubberband_size);
            rubberBandStretcher->retrieve(&rubberband_output, retrieve_from_rubberband_size);

            int pa_write_error;
            pa_simple_write(pas, (void *)rubberband_output, retrieve_from_rubberband_size * sizeof(float), &pa_write_error);

            if (samples_sent_to_sink == 0)
            {
                printf("=========\n");
                date_of_first_sample_sent_to_sink = std::chrono::high_resolution_clock::now();
            }
            samples_sent_to_sink += retrieve_from_rubberband_size;

            if (pa_write_error != 0)
            {
                printf("error while playing ? %d : %s\n", pa_write_error, pa_strerror(pa_write_error));
            }
        }
        position += block_size;

        m_sound_position.store(position);

        // float block_duration_ms = 1000.0 * ((float)block_size) / ((float)(m_sound->sfinfo.samplerate));
        // printf("block duration %f ms\n", block_duration_ms);
        // long bloc_duration_ms_long = (long)std::floor(block_duration_ms);
        if (block_size == 0)
        {
            printf("ooooooooooooooooooodddddddddddddddddddiity\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
}

void Player::start_playing()
{
    printf("Player::start_playing\n");
    m_play_started.store(true);
}

void Player::stop_playing()
{
    printf("Player::stop_playing\n");
    m_play_started.store(false);
}

void Player::set_sound(Sound *_sound)
{
    stop_playing();
    m_sound = _sound;
    set_sound_start(0);
    set_sound_end(m_sound->get_frame_count());
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
