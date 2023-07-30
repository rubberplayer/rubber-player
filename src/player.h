#include <pulse/simple.h>
#include <pulse/error.h>
#include <rubberband/RubberBandStretcher.h>
#include <atomic>
#include <thread>
#include "./sound.h"
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
  std::thread m_the_play_thread;
  std::atomic<bool> m_terminate_the_play_thread;
  
  void start_playing();
  void stop_playing();
  std::atomic<bool> m_play_started;
  
  void set_sound(Sound* sound);
  
  std::atomic<double> m_pitch_scale;
  void set_pitch_scale(double pitch_scale);
  
  std::atomic<long> m_sound_start;
  std::atomic<long> m_sound_end;
  void set_sound_start(long sound_start );
  void set_sound_end(long sound_end );
  
  Sound* m_sound;

  RubberBand::RubberBandStretcher *rubberBandStretcher;
};
