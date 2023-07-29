#include <pulse/simple.h>
#include <pulse/error.h>
#include <rubberband/RubberBandStretcher.h>

class Player
{
public:
  Player();
  ~Player();
  pa_simple *s;
  pa_sample_spec ss;
  void connect_to_pulseaudio();
  void play_some();
  void play_some(float* ptr, size_t bytes);

	RubberBand::RubberBandStretcher*  rubberBandStretcher;
};
