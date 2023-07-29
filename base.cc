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

//////////////////////////////////

class MainWindow : public Gtk::Window
{
public:
  MainWindow();



  void on_drawingarea_checkerboard_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);
  Gtk::Box m_VBox;
  Gtk::Frame m_Frame_Checkerboard;
  Gtk::DrawingArea m_DrawingArea_Checkerboard;
  Gtk::Label m_Label_Checkerboard;

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
  void draw_sound(Sound some_sound);
  void on_drawingarea_scribble_resize(int width, int height);
  void scribble_create_surface();
};

MainWindow::MainWindow() : m_VBox(Gtk::Orientation::VERTICAL, 8),
                           m_Button_play("play", true),
                           m_Button_stop("stop", true),
                           m_Button_load("load", true)
{
  set_title(APPLICATION_NAME);
  set_default_size(200, 200);

  m_VBox.set_margin(16);
  set_child(m_VBox);

  m_VBox.append(m_Frame_Checkerboard);

  m_DrawingArea_Checkerboard.set_content_width(100);
  m_DrawingArea_Checkerboard.set_content_height(100);
  m_DrawingArea_Checkerboard.set_expand();
  m_Frame_Checkerboard.set_child(m_DrawingArea_Checkerboard);

  m_VBox.append(m_Label_Checkerboard);
  m_Label_Checkerboard.set_markup("<u>Checkerboard pattern</u>");
  m_DrawingArea_Checkerboard.set_draw_func(
      sigc::mem_fun(*this, &MainWindow::on_drawingarea_checkerboard_draw));

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

  m_DrawingArea_Checkerboard.signal_resize().connect(
      sigc::mem_fun(*this, &MainWindow::on_drawingarea_scribble_resize));
  sound.load("/home/vivien/Bureau/redhouse-clip-mono-short.flac");
  draw_sound(sound);
}
void MainWindow::draw_sound(Sound some_sound)
{
  scribble_create_surface();
  auto cr = Cairo::Context::create(m_surface);
  /*
  sf_count_t read_count;
  float *ptr;
  */
  printf("MainWindow::draw_sound %d \n", sound.read_count);

  cr->set_source_rgb(0, 0, 0);
  auto sw = m_surface->get_width();
  auto sh = m_surface->get_height();
  printf("dimensions %dx%d\n", sw, sh);

  cr->set_source_rgb(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));

  int method = -1;
  if (method == 0)
  {
    for (int i = 0; i < sound.read_count; i++)
    {
      // nearest value
      float value = sound.ptr[i];
      int x = ((float)i) / ((float)(sound.read_count)) * (float)sw;
      int y = (value + 1.0) / 2.0 * (float)sh;
      cr->rectangle(x, y, 2, 2);
      cr->fill();
    }
  }
  else if (method == -1)
  {
    for (int i = 0; i < sound.read_count; i++)
    {
      float value = sound.ptr[i];
      float r0 = ((float)i) / ((float)(sound.read_count)) * (float)sw;
      float r1 = ((float)(i + 1)) / ((float)(sound.read_count)) * (float)sw;
      int min_x = std::floor(r0);
      int max_x = std::ceil(r1);
      int y = (2.0 - (value + 1.0)) / 2.0 * (float)sh;
      cr->rectangle(min_x, y, max_x - min_x, 1);
      cr->fill();
    }
  }
  else if (method == 1)
  {
    // int width = std::max((float)1.0,((float)sw)/((float)(sound.read_count)));
    // printf("@WWWWWWWWWWWWWWWW %d\n",width);
    //  min max in interval
    int first_sample = 0;
    for (int i = 0; i < sw; i++)
    {
      float pos_to = ((float)(i + 1)) / ((float)sw) * (float)(sound.read_count);
      int last_sample = std::clamp((int)round(pos_to), first_sample, (int)sound.read_count - 1);

      float max = -1;
      float min = 1;
      for (int j = first_sample; j <= last_sample; j++)
      {
        float value = sound.ptr[j];
        max = std::max(max, value);
        min = std::min(min, value);
      }
      printf("@%d interval idx [%d,%d] samples [%f,%f]\n", i, first_sample, last_sample, min, max);
      int x = i;
      //  int height = std::max(1.0,(((max - min)) / 2.0) * sh);
      int width = 1;
      int height = 1;
      cr->rectangle(
          x,
          ((min + 1.0) / 2.0) * sh,
          width,
          height);
      cr->fill();
      first_sample = std::min(last_sample + 1, (int)sound.read_count - 1);
    }
  }
  else if (method == 2)
  {
    // same as 0 ?
    for (int i = 0; i < sw; i++)
    {
      int idx = std::floor(((float)(i) / ((float)sw) * (float)(sound.read_count)));
      float value = sound.ptr[idx];
      int x = i;
      int y = ((value + 1.0) / 2.0) * sh;
      cr->rectangle(x, y, 1, 1);
      cr->fill();
    }
  }
  else if (method == 3)
  {
    // a random in interval
    int first_sample = 0;
    for (int i = 0; i < sw; i++)
    {
      float pos_to = ((float)(i + 1)) / ((float)sw) * (float)(sound.read_count);
      int last_sample = std::clamp((int)round(pos_to), first_sample, (int)sound.read_count - 1);

      float max = -1;
      float min = 1;

      int j = min + (rand() % static_cast<int>(max - min + 1));
      float value = sound.ptr[j];
      int x = i;
      int y = ((value + 1.0) / 2.0) * sh;
      cr->rectangle(x, y, 1, 1);
      cr->fill();
      first_sample = std::min(last_sample + 1, (int)sound.read_count - 1);
    }
  }

  /*
    cr->set_source_rgb(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));
    cr->rectangle(20, 20, 50, 50);
    cr->fill();
    */
}
void MainWindow::on_button_play_clicked()
{
  player.play_some();
}

void MainWindow::on_drawingarea_scribble_resize(
    int /* width */, int /* height */)
{
  scribble_create_surface();
  draw_sound(sound);
}

void MainWindow::scribble_create_surface()
{
  m_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32,
                                          m_DrawingArea_Checkerboard.get_width(), m_DrawingArea_Checkerboard.get_height());

  // Initialize the surface to white.
  auto cr = Cairo::Context::create(m_surface);
  cr->set_source_rgb(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));
  cr->paint();
}

void MainWindow::on_drawingarea_checkerboard_draw(const Cairo::RefPtr<Cairo::Context> &cr,
                                                  int width, int height)

{

  cr->set_source(m_surface, 0, 0);
  cr->paint();
  /*
   std::srand(time(0));
  cr->set_source_rgb(rand() / double(RAND_MAX) , rand() / double(RAND_MAX), rand() / double(RAND_MAX) );
  cr->rectangle(20, 20, 50, 50);
  cr->fill();
  */
}

int main(int argc, char *argv[])
{
  printf("main\n");
  auto app = Gtk::Application::create(); //"org.gtkmm.examples.base");

  /*m_HBox.append(m_Button1);
  m_HBox.append(m_Button2);
*/
  // set_child(m_HBox);
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
  printf("stop!\n");
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
