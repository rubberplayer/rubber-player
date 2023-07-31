
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