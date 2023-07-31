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
