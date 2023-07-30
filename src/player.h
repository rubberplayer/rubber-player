#include <pulse/simple.h>
#include <pulse/error.h>
#include <rubberband/RubberBandStretcher.h>
#include <atomic>
#include <thread>
class Player
{
public:
  Player();
  ~Player();
  
  pa_simple *s;
  pa_sample_spec ss;
  
  void connect_to_pulseaudio();
  void play_some();
  void play_always();
  void play_some(float *ptr, size_t bytes);
  std::thread the_play_thread;
  std::atomic<bool> terminate_the_play_thread;
  
  void start_playing();
  void stop_playing();
  std::atomic<bool> play_started;
  
  RubberBand::RubberBandStretcher *rubberBandStretcher;
};
