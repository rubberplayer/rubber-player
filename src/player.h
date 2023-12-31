#ifndef PLAYER_H
#define PLAYER_H
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

  void play_always();
  std::thread m_the_play_thread;
  std::atomic<bool> m_terminate_the_play_thread;
  void set_rubberband_flag_options(int flag_options);
  int get_rubberband_flag_options();

  //std::atomic<RubberBand::RubberBandStretcher::Option> m_rubberband_flag_options;

  void stop_playing_thread();
  void start_playing_thread();

  std::atomic<bool> m_play_started;
  void start_playing();
  void stop_playing();

  std::atomic<double> m_pitch_scale;
  void set_pitch_scale(double pitch_scale);
  std::atomic<double> m_time_ratio;
  void set_time_ratio(double time_ratio);


  std::atomic<long> m_sound_start;
  void set_sound_start(long sound_start);
  std::atomic<long> m_sound_end;
  void set_sound_end(long sound_end);

  std::atomic<long> m_sound_position;

  void set_sound(Sound *sound);
  Sound *m_sound;

  bool is_playing();
  

private:
  void connect_to_pulseaudio(int channels, int framerate);
  pa_simple *m_pa_simple;
  pa_sample_spec m_pa_sample_spec;
  int initialize_RubberBand(int channels, int samplerate);
  RubberBand::RubberBandStretcher *rubberBandStretcher;
  std::atomic<int> m_rubberband_flag_options;

};
#endif