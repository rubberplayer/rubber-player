#include "./sound.h"
Sound::Sound()
{
  ptr = NULL;
}
Sound::~Sound()
{
  if (ptr != NULL)
    free(ptr);
}
void Sound::load(std::string ppath)
{
  fprintf(stderr,"[libsndfile] version : %s\n", sf_version_string());
  path = ppath;
  sndfile = sf_open(path.c_str(), SFM_READ, &sfinfo);
  if (sndfile == NULL)
  {
    fprintf(stderr,"[libsndfile] error opening file %s : %s - %p\n", path.c_str(), sf_strerror(sndfile), sndfile);
  }
  else
  {
    fprintf(stderr,"[libsndfile] file opened ok %s : %p\n", path.c_str(), sndfile);
    fprintf(stderr,"[libsndfile] SF_INFO :\n-frames: %ld\n-samplerate %d\n-channels %d\n-format %b\n-sections %d\n-seekable %d\n",
           sfinfo.frames, sfinfo.samplerate, sfinfo.channels, sfinfo.format, sfinfo.sections, sfinfo.seekable);

    int samples = sfinfo.frames * sfinfo.channels;
    ptr = (float *)malloc(samples * sizeof(float));

    read_count = sf_read_float(sndfile, ptr, samples);

    sf_close(sndfile);
  }
};

bool Sound::is_loaded()
{
  return (sndfile != NULL) && (read_count > 0);
}
std::string Sound::get_error_string()
{
  if (sf_error(sndfile) != SF_ERR_NO_ERROR)
  {
    return std::string("Could not load file at ") + "\"" + path + "\"" + "... Libsndfile said : " + "<b>" + sf_strerror(sndfile) + "</b>";
  }
  return std::string(sf_strerror(sndfile));
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
double Sound::get_second_at_frame(long frame){
  return ((double)frame) / sfinfo.samplerate;
}