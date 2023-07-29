#include "./waveform.h"

Waveform::Waveform()
{
    signal_resize().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_scribble_resize));
    set_draw_func(sigc::mem_fun(*this, &Waveform::on_drawingarea_checkerboard_draw));
}
void Waveform::set_sound(Sound _sound){
    sound = _sound;    

}
void Waveform::on_drawingarea_scribble_resize(int width, int height)
{
    scribble_create_surface();    
    draw_sound(sound);
}
void Waveform::scribble_create_surface()
{
      m_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32,
                                            get_width(), get_height());

    auto cr = Cairo::Context::create(m_surface);
    cr->set_source_rgb(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));    
    cr->paint();
    cr->set_source_rgb(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));    
    cr->rectangle(10,10,30,30);
    cr->fill();
}
void Waveform::on_drawingarea_checkerboard_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height)
{
    cr->set_source(m_surface, 0, 0);
    cr->paint();
}

void Waveform::draw_sound(Sound sound)
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
