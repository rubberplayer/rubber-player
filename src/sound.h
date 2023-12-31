#ifndef INCLUDE_SOUND
#define INCLUDE_SOUND 1

#include <sndfile.h>
#include <string>
class Sound
{
public:
  Sound();
  ~Sound();
  SNDFILE *sndfile;
  SF_INFO sfinfo;
  std::string path;
  sf_count_t read_count;
  float *ptr;
  void load(std::string path);
  bool is_loaded();
  std::string get_error_string();
  sf_count_t get_frame_count();
  int get_samplerate();
  int get_channels();
  float *get_sound_data();  
  double get_second_at_frame(long frame);
};
#endif