#include "./sound.h"
Sound::Sound()
{
  ptr = NULL;
}
Sound::~Sound()
{
}
void Sound::load(std::string ppath)
{
  printf("libsndfile version : %s\n", sf_version_string());
  path = ppath;
  sndfile = sf_open(path.c_str(), SFM_READ, &sfinfo);
  if (sndfile == NULL)
  {
    printf("error reading file %s : %s\n", &path, sf_strerror(sndfile));
  }
  else
  {
    printf("file opened ok %s : %d\n", path.c_str(), sndfile);
    printf("SF_INFO :\n-samplerate %d\n-channels %d\n-format %b\n-sections %d\n-seekable %d\n",
           sfinfo.samplerate, sfinfo.channels, sfinfo.format, sfinfo.sections, sfinfo.seekable);

    // TODO remove file size limit (10 minutes);
    int samples = sfinfo.samplerate * sfinfo.channels * 10 * 60;
    ptr = (float *)malloc(samples * sizeof(float));

    read_count = sf_read_float(sndfile, ptr, samples);
    printf("sndfile asked for %d samples, read %d\n ", samples, read_count);

    // player.play_some(ptr, read_count * sizeof(float));
    sf_close(sndfile);
  }
};

bool Sound::is_loaded()
{
  return (sndfile != NULL) && (read_count > 0);
}
sf_count_t Sound::get_frame_count()
{
  return read_count;
}
int Sound::get_samplerate()
{
  return sfinfo.samplerate;
}
float *Sound::get_sound_data()
{
  return ptr;
}