#include "./player.h"
#include "./name.h"
#include <cstdio>
#include <cstdlib>
#include <rubberband/RubberBandStretcher.h>

Player::~Player()
{
    terminate_the_play_thread.store(true);
    the_play_thread.join();
    pa_simple_free(this->s);
    delete rubberBandStretcher;
}

Player::Player()
{
    play_started.store(false);
    terminate_the_play_thread.store(false);

    rubberBandStretcher = new RubberBand::RubberBandStretcher(48000, 1,
                                                              RubberBand::RubberBandStretcher::DefaultOptions | RubberBand::RubberBandStretcher::OptionProcessOffline);
    printf("RubberBand engine version : %d\n", rubberBandStretcher->getEngineVersion());
    printf("RubberBand engine channel count : %d\n", rubberBandStretcher->getChannelCount());
    connect_to_pulseaudio();

    the_play_thread = std::thread([this]
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

/*
void RubberBand::RubberBandStretcher::study 	( 	const float *const *  	input,
        size_t  	samples,
        bool  	final
    )
void RubberBand::RubberBandStretcher::process 	( 	const float *const *  	input,
        size_t  	samples,
        bool  	final
    )

    size_t RubberBand::RubberBandStretcher::retrieve 	( 	float *const *  	output,
        size_t  	samples
    ) 		const

*/

void Player::play_some(float *ptr, size_t bytes)
{
    // study
    double pitch_scale = 1.0;
    double time_ratio = 0.5;
    rubberBandStretcher->setPitchScale(pitch_scale);
    rubberBandStretcher->setTimeRatio(time_ratio);
    rubberBandStretcher->study(&ptr, bytes, true); // all in one pass

    // convert and play
    size_t samples_requiered = rubberBandStretcher->getSamplesRequired();
    int position = 0;
    int i = 0;
    bool last_process = false;
    size_t OUTPUT_SAMPLE_COUNT = 2 * 3 * 4 * 5 * 6 * 7 * 256;
    float *output = (float *)malloc(OUTPUT_SAMPLE_COUNT * sizeof(float));
    while (samples_requiered > 0)
    {
        printf("[%d] samples requiered : %d\n", i, rubberBandStretcher->getSamplesRequired());
        if ((position + samples_requiered) >= bytes)
        {
            last_process = true;
            samples_requiered = bytes - position;
        }
        printf("[%d] samples given : %d\n", i, samples_requiered);
        printf("[%d] now process : last? %b\n", i, last_process);

        float *ppp = ptr + position;
        rubberBandStretcher->process(&ppp, samples_requiered, last_process);

        printf("[%d] now process : last? %b\n", i, last_process);

        printf("[%d] now processed\n", i);
        samples_requiered = rubberBandStretcher->getSamplesRequired();

        int available = rubberBandStretcher->available();
        int ii = 0;
        int error;
        while (available > 0)
        {
            printf("[%d][%d] available : %d\n", i, ii, available);
            int retrieve_count = (available > OUTPUT_SAMPLE_COUNT) ? OUTPUT_SAMPLE_COUNT : available;
            printf("[%d][%d] retrieve : %d\n", i, ii, retrieve_count);
            rubberBandStretcher->retrieve(&output, retrieve_count);
            if (retrieve_count > 0)
            {
                pa_simple_write(this->s, (void *)output, (retrieve_count * sizeof(float)), &error);
                if (error)
                {
                    printf("error while playing ? %d : %s\n", error, pa_strerror(error));
                }
            }
            available = rubberBandStretcher->available();
            ii++;
        }
        position += samples_requiered;
        i++;
    }

    printf("i play %d %d\n", ptr, bytes);
    int error;
    //  pa_simple_write(this->s, (void *)ptr, (size_t)(bytes * sizeof(float)), &error);
    printf("error while playing ? %d : %s\n", error, pa_strerror(error));
}
void Player::play_some()
{
    printf("Player::play_some()\n");
    printf("is pa object null ? %d \n", this->s);
    if (this->s != NULL)
    {
        int nbytes = 48000;
        uint8_t *data = (uint8_t *)malloc(sizeof(int16_t) * nbytes);
        for (int i = 0; i < nbytes; i++)
        {
            data[i] = (i / 2) % 256;
        }
        int error;
        pa_simple_write(this->s, (void *)data, (size_t)nbytes, &error);
        printf("error while playing ? %d : %s\n", error, pa_strerror(error));
    }
};

void Player::play_always()
{
    using namespace std::chrono_literals;
    for (;;)
    {
        if (terminate_the_play_thread.load() == true)
        {
            return;
        }
        if (play_started.load() == true)
        {
            printf("yoyoyo\n");
        }
        std::this_thread::sleep_for(500ms);
    }
}

/*


    if (pa_simple_drain(s, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        goto finish;
    }


    if (s)

*/

void Player::start_playing()
{
    printf("Player::start_playing\n");
    play_started.store(true);
}

void Player::stop_playing()
{
    printf("Player::stop_playing\n");
    play_started.store(false);
}

