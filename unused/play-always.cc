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
    using namespace std::chrono_literals;
    for (;;)
    {
        if (m_terminate_the_play_thread.load() == true)
        {
            return;
        }
        if (m_play_started.load() == false)
        {
            std::this_thread::sleep_for(500ms);
            continue;
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
        //printf("sound samplerate : %d ; channels : %d, selection [%d,%d] ; pitch scale : %f ; position : %d, time position : %f\n", m_sound->sfinfo.samplerate, m_sound->sfinfo.channels, selection_left, selection_right, pitch_scale, position, time_position);

        if (position < selection_left)
            position = selection_left;

        if (position >= selection_right)
            position = selection_left;

        void *sound_pointer = m_sound->ptr + position;
        long block_size = 256 * 4; // 48000 * 4; // 256;
        int error;

        if ((position + block_size) >= selection_right)
        {
            block_size = selection_right - position;
        }

        pa_simple_write(pas, sound_pointer, block_size * sizeof(float), &error);

        if (error != 0)
        {
            printf("error while playing ? %d : %s\n", error, pa_strerror(error));
        }
        position += block_size;

        m_sound_position.store(position);

        float block_duration_ms = 1000.0 * ((float)block_size) / ((float)(m_sound->sfinfo.samplerate));
        printf("block duration %f ms\n", block_duration_ms);
        long bloc_duration_ms_long = (long)std::floor(block_duration_ms);
        if (block_size == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
}
