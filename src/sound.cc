#include "./sound.h"
Sound::Sound()
{
  printf("** create a sound\n");
  ptr = NULL;
}
Sound::~Sound()
{
  printf("** destroys a sound\n");
  if (ptr != NULL) free(ptr);
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
    printf("SF_INFO :\n-frames: %d\n-samplerate %d\n-channels %d\n-format %b\n-sections %d\n-seekable %d\n",
           sfinfo.frames,sfinfo.samplerate, sfinfo.channels, sfinfo.format, sfinfo.sections, sfinfo.seekable);

    int samples = sfinfo.frames * sfinfo.channels;
    ptr = (float *)malloc(samples * sizeof(float));

    read_count = sf_read_float(sndfile, ptr, samples);
    printf("asked sndfile for %d samples, read %d\n ", samples, read_count);

    sf_close(sndfile);
  }
};

bool Sound::is_loaded()
{
  return (sndfile != NULL) && (read_count > 0);
}
sf_count_t Sound::get_frame_count()
{
  return sfinfo.frames;
}
int Sound::get_samplerate()
{
  return sfinfo.samplerate;
}
int Sound::get_channels()
{
  return sfinfo.channels;
}
float *Sound::get_sound_data()
{
  return ptr;
}