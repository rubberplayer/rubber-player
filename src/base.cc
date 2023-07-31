#include <gtkmm.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include "./name.h"
#include "./player.h"
#include "./sound.h"
#include "./waveform.h"
//////////////////////////////////

///
/// https://docs.gtk.org/gtk4/class.FileDialog.html since 4.10
///
// char** GLOBAL_ARGV;

class TimeRatioRange
{
public:
  double display_min = -2;
  double display_max = 4;
  double get_value_for_display(double display_value)
  {
    return std::pow(2, display_value);
  }
  std::string get_value_string_for_label_display(double display_value)
  {
    double value = get_value_for_display(display_value);
    if (value == 1)
      return "no change";
    else if (value > 1)
      return std::to_string((int)std::round(value));
    else
      return "1/" + std::to_string((int)std::round(1 / value));
  }
};

class PitchRange
{
public:
  double display_min = -4;
  double display_max = 2;
  double get_value_for_display(double display_value)
  {
    return std::pow(2, display_value);
  }
  std::string get_value_string_for_label_display(double display_value)
  {
    double value = get_value_for_display(display_value);
    if (value == 1)
      return "no change";
    else if (value > 1)
      return std::to_string((int)std::round(value));
    else
      return "1/" + std::to_string((int)std::round(1 / value));
  }
};

class MainWindow : public Gtk::Window
{
public:
  MainWindow();

  Waveform m_Waveform;

  Gtk::Box m_VBox;

  Gtk::Frame m_Frame_Waveform;

  Gtk::ToggleButton m_Button_play;
  void on_button_play_clicked();

  Gtk::Button m_Button_open;
  void on_button_open_clicked();

  Gtk::Box m_HBox_time_ratio;
  TimeRatioRange m_time_ratio_range;
  Gtk::Label m_Label_time_ratio;
  Gtk::Scale m_Scale_time_ratio;
  void on_time_ratio_value_changed();
  Gtk::Entry m_Entry_time_ratio;

  // Gtk::Scale::SlotFormatValue m_Scale_time_ratio_format_value;
  // char* m_Scale_time_ratio_format_value(double value);
  /*
    PitchRange m_pitch_range;
    Gtk::Label m_Label_pitch;
    Gtk::Scale m_Scale_pitch;
    void on_pitch_value_changed();
    Gtk::Entry m_Entry_pitch;
  */

  Player player;
  Sound sound;

  // Gtk::FileDialog m_FileDialog_open_audio_file;

  Glib::RefPtr<Gtk::FileChooserNative> m_Dialog_open_audio_file;
  void on_open_audio_file_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative> &m_Dialog_open_audio_file);

  Glib::RefPtr<Gtk::EventControllerKey> m_Keypressed;
  // void on_key_pressed(guint keyval, int keycode, Gdk::ModifierType state);
  // bool on_key_pressed(const Gdk::Event* event);
  bool on_key_pressed(const unsigned int a, const unsigned int b, const Gdk::ModifierType c);

  long m_selection_start;
  long m_selection_end;
  void set_selection_bounds(long selection_start, long selection_end);
};

void MainWindow::set_selection_bounds(long selection_start, long selection_end)
{
  m_selection_start = selection_start;
  m_selection_end = selection_end;
}

MainWindow::MainWindow() : m_VBox(Gtk::Orientation::VERTICAL, 8),
                           m_HBox_time_ratio(Gtk::Orientation::HORIZONTAL, 8),
                           m_Button_play("play", true),
                           m_Button_open("open", true)
/*m_Button_load("play", true)*/
{
  set_title(APPLICATION_NAME);
  set_default_size(600, 250);

  // sound.load("/home/vivien/Bureau/redhouse-clip-mono-short.flac");
  //  sound.load("/home/vivien/Bureau/redhouse-mono.wav");
  //  hack

  sound.load("/home/vivien/Bureau/redhouse-clip-mono.flac");
  m_Waveform.set_sound(sound);
  player.set_sound(&sound);

  m_Waveform.set_hack_sound_start_sound_end_sound_position(&player.m_sound_start, &player.m_sound_end, &player.m_sound_position);
  
  // vertical box container
  m_VBox.set_margin(16);
  set_child(m_VBox);

  // waveform frame and Waveform (DrawingArea)
  m_VBox.append(m_Frame_Waveform);
  m_Waveform.set_content_width(100);
  m_Waveform.set_content_height(100);
  m_Waveform.set_expand();
  m_Frame_Waveform.set_child(m_Waveform);

  // time ratio label
  m_Label_time_ratio.set_text("Time Ratio 🐢");
  m_Label_time_ratio.set_valign(Gtk::Align::END);
  m_Label_time_ratio.set_margin_bottom(8);
  m_Label_time_ratio.set_margin_start(4);
  m_HBox_time_ratio.append(m_Label_time_ratio);

  // time ratio scale
  m_Scale_time_ratio.set_draw_value(false);
  m_Scale_time_ratio.set_range(m_time_ratio_range.display_min, m_time_ratio_range.display_max);
  m_Scale_time_ratio.set_value(0);
  on_time_ratio_value_changed();
  m_Scale_time_ratio.set_hexpand();
  m_Scale_time_ratio.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_time_ratio_value_changed));
  double marks[] = {1.0, -1.0, 2.0, 3.0, 0.0, 666.0};
  int i_mark = 0;
  for (;;)
  {
    if (marks[i_mark] == 666.0)
      break;
    m_Scale_time_ratio.add_mark(marks[i_mark], Gtk::PositionType::TOP, m_time_ratio_range.get_value_string_for_label_display(marks[i_mark]));
    i_mark++;
  }
  m_HBox_time_ratio.append(m_Scale_time_ratio);

  // time ratio entry
  m_Entry_time_ratio.set_valign(Gtk::Align::END);
  m_HBox_time_ratio.append(m_Entry_time_ratio);
  m_VBox.append(m_HBox_time_ratio);

  // play button
  m_VBox.append(m_Button_play);
  m_Button_play.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_play_clicked));

  // open file button
  m_VBox.append(m_Button_open);
  m_Button_open.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_open_clicked));
  // m_Button_load.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_load_clicked));

  // open file dialog
  m_Dialog_open_audio_file = Gtk::FileChooserNative::create("Please choose an audio file",
                                                            *this,
                                                            Gtk::FileChooser::Action::OPEN,
                                                            "Choose",
                                                            "Cancel");
  m_Dialog_open_audio_file->set_transient_for(*this);
  m_Dialog_open_audio_file->set_modal(true);
  m_Dialog_open_audio_file->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::on_open_audio_file_dialog_response), m_Dialog_open_audio_file));
  
}

void MainWindow::on_time_ratio_value_changed()
{
  double value = m_time_ratio_range.get_value_for_display(m_Scale_time_ratio.get_value());
  m_Entry_time_ratio.get_buffer()->set_text(std::to_string(value));
  player.set_time_ratio(value);
}
void MainWindow::on_button_play_clicked()
{
  bool active = m_Button_play.get_active();
  printf("on_button_play_clicked clicked : active : %b\n", active);
  if (active)
  {
    player.start_playing();
  }
  else
  {
    player.stop_playing();
  }
}

void MainWindow::on_button_open_clicked()
{
  m_Dialog_open_audio_file->show();
}

void MainWindow::on_open_audio_file_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative> &dialog)
{  
  Glib::RefPtr<Gio::File> filename = m_Dialog_open_audio_file->get_file();
  if (filename == NULL)
  {
    printf("nothing selected\n");
  }
  else
  {
    std::string path = filename->get_path();
    printf("got a filename path %s\n", path.c_str());
  }
  m_Dialog_open_audio_file->hide();
}

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(); //"org.gtkmm.examples.base");
  return app->make_window_and_run<MainWindow>(argc, argv);
}
