#include <sndfile.h>
#include <string>
class Sound
{
public:
  SNDFILE *sndfile;
  SF_INFO sfinfo;
  std::string path;
  sf_count_t read_count;
  float *ptr;
  void load(std::string path);
};
