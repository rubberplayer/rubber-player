#include <gtkmm.h>
#include <iostream>
/////////
#include <stdio.h>
// #include <sndfile.h>
#include <ctime>

#include <cstdlib>
#include <ctime>

#include "./player.h"
#include "./name.h"
#include "./sound.h"
#include "./waveform.h"
//////////////////////////////////

///
/// https://docs.gtk.org/gtk4/class.FileDialog.html since 4.10
///

class MainWindow : public Gtk::Window
{
public:
  MainWindow();

  Waveform m_Waveform;

  // void on_drawingarea_checkerboard_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);
  Gtk::Box m_VBox;
  // Gtk::Frame m_Frame_Checkerboard;
  Gtk::Frame m_Frame_Waveform;
  // Gtk::DrawingArea m_DrawingArea_Checkerboard;
  // Gtk::Label m_Label_Checkerboard;

  Gtk::Button m_Button_play;
  void on_button_play_clicked();

  Gtk::Button m_Button_stop;
  void on_button_stop_clicked();

  Gtk::Button m_Button_load;
  void on_button_load_clicked();

  //  Cairo::RefPtr<Cairo::ImageSurface> m_surface;
  // Glib::RefPtr<Gtk::GestureDrag> m_drag;

  Player player;
  Sound sound;

  Glib::RefPtr<Gtk::FileChooserNative> m_Dialog_open_audio_file;
  void on_folder_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative> &m_Dialog_open_audio_file);

  Cairo::RefPtr<Cairo::ImageSurface> m_surface;
  // void draw_sound(Sound some_sound);
  void on_drawingarea_scribble_resize(int width, int height);
  // void scribble_create_surface();

  Glib::RefPtr<Gtk::EventControllerKey> m_Keypressed;
  // void on_key_pressed(guint keyval, int keycode, Gdk::ModifierType state);
  // bool on_key_pressed(const Gdk::Event* event);
  bool on_key_pressed(const unsigned int a, const unsigned int b, const Gdk::ModifierType c);
};

MainWindow::MainWindow() : m_VBox(Gtk::Orientation::VERTICAL, 8),
                           m_Button_play("play", true),
                           m_Button_stop("stop", true),
                           m_Button_load("load", true)
{

  //  sound.load("/home/vivien/Bureau/redhouse-clip-mono-short.flac");
  sound.load("/home/vivien/Bureau/redhouse-clip-mono.flac");
  m_Waveform.set_sound(sound);

  set_title(APPLICATION_NAME);
  set_default_size(200, 200);

  m_VBox.set_margin(16);
  set_child(m_VBox);

  m_VBox.append(m_Frame_Waveform);
  m_Waveform.set_content_width(100);
  m_Waveform.set_content_height(100);
  m_Waveform.set_expand();

  m_Frame_Waveform.set_child(m_Waveform);

  player.connect_to_pulseaudio();

  m_VBox.append(m_Button_play);
  m_VBox.append(m_Button_stop);
  m_VBox.append(m_Button_load);

  m_Button_play.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_play_clicked));
  m_Button_stop.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_stop_clicked));
  m_Button_load.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_load_clicked));

  m_Dialog_open_audio_file = Gtk::FileChooserNative::create("Please choose a folder",
                                                            *this,
                                                            Gtk::FileChooser::Action::OPEN,
                                                            "Choose",
                                                            "Cancel");

  m_Dialog_open_audio_file->set_transient_for(*this);
  m_Dialog_open_audio_file->set_modal(true);
  m_Dialog_open_audio_file->signal_response().connect(sigc::bind(sigc::mem_fun(*this,
                                                                               &MainWindow::on_folder_dialog_response),
                                                                 m_Dialog_open_audio_file)); // <- error

  m_Keypressed = Gtk::EventControllerKey::create();
  add_controller(m_Keypressed);

  m_Keypressed->signal_key_pressed().connect(sigc::mem_fun(*this, &MainWindow::on_key_pressed), false);
}
void MainWindow::on_button_play_clicked()
{
  player.play_some();
}

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(); //"org.gtkmm.examples.base");
  return app->make_window_and_run<MainWindow>(argc, argv);
}
void MainWindow::on_button_load_clicked()
{
  sound.load("/home/vivien/Bureau/redhouse-clip-mono.flac");
  player.play_some(sound.ptr, sound.read_count); //* sizeof(float));
}
void MainWindow::on_button_stop_clicked()
{
  m_Dialog_open_audio_file->show();
  // dialog.hide();
}

void MainWindow::on_folder_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative> &dialog)
{
  printf("got a response\n");
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

bool MainWindow::on_key_pressed(const unsigned int a, const unsigned int b, const Gdk::ModifierType c)
{
  // void Waveform::on_key_pressed(guint keyval, int keycode, Gdk::ModifierType state){

  printf("has ctrl : %d ; has shift : %d \n", c & Gdk::ModifierType::CONTROL_MASK,c & Gdk::ModifierType::SHIFT_MASK);

  printf("I am here KEYRPESS %d %d  %d \n", a, b, GDK_KEY_plus);
  // KEY_KP_Add
  return true;
}