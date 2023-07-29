#include "./waveform.h"

Waveform::Waveform()
{
    signal_resize().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_scribble_resize));
    set_draw_func(sigc::mem_fun(*this, &Waveform::on_drawingarea_checkerboard_draw));

    m_drag = Gtk::GestureDrag::create();
    // m_drag->set_button(GDK_BUTTON_PRIMARY);
    m_drag->set_button(GDK_BUTTON_MIDDLE);
    add_controller(m_drag);

    m_drag->signal_drag_begin().connect(
        sigc::mem_fun(*this, &Waveform::on_drawingarea_scribble_drag_begin));
    m_drag->signal_drag_update().connect(
        sigc::mem_fun(*this, &Waveform::on_drawingarea_scribble_drag_update));
    m_drag->signal_drag_end().connect(
        sigc::mem_fun(*this, &Waveform::on_drawingarea_scribble_drag_end));

    // has_selection = false;
    set_selection_bounds(0, 0);
    scale = 1;

    /*
        m_GestureZoom = Gtk::GestureZoom::create();
        m_GestureZoom->set_propagation_phase(Gtk::PropagationPhase::BUBBLE);
        m_GestureZoom->signal_scale_changed().connect(sigc::mem_fun(*this, &Waveform::on_gesture_zoom_scale_changed));
        add_controller(m_GestureZoom);
        */
}
void Waveform::set_sound(Sound _sound)
{
    sound = _sound;
}
void Waveform::on_drawingarea_scribble_resize(int width, int height)
{
    draw_all();
}
void Waveform::scribble_create_surface()
{
    m_waveform_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32,
                                                     get_width(), get_height());

    auto cr = Cairo::Context::create(m_waveform_surface);
    cr->set_source_rgb(0, 0, 0);
    cr->paint();
    /*cr->set_source_rgb(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));
    cr->rectangle(10, 10, 30, 30);*/
    cr->fill();
}
void Waveform::on_drawingarea_checkerboard_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height)
{
    cr->set_source(m_waveform_surface, 0, 0);
    cr->paint();
}
void Waveform::draw_all()
{
    scribble_create_surface();
    draw_sound();
    draw_selection();
    draw_text();
}
void Waveform::draw_text()
{
    auto cr = Cairo::Context::create(m_waveform_surface);
    float font_size = 12.0;
    /*auto sw = m_waveform_surface->get_width();
    auto sh = m_waveform_surface->get_height();
    */
    /*    float left =  ((float)selection_start) / ((float)sound.get_frame_count()); //* (float)sw;
        float right = ((float)selection_end) / ((float)sound.get_frame_count());// * (float)sw;
      */
    float left_s = ((float)selection_start) / (float)sound.sfinfo.samplerate;
    float right_s = ((float)selection_end) / (float)sound.sfinfo.samplerate;

    cr->set_font_size(font_size);
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->move_to(5, font_size);
    cr->show_text(std::to_string(left_s) + " s");
    cr->move_to(5, font_size * 2);
    cr->show_text(std::to_string(right_s) + " s");
}
void Waveform::draw_selection()
{
    auto cr = Cairo::Context::create(m_waveform_surface);
    auto sw = m_waveform_surface->get_width();
    auto sh = m_waveform_surface->get_height();

    cr->set_source_rgba(0.3, 0.8, 0.7, 0.5);
    // cr->set_source_rgba(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX),0.5);

    int left = selection_start / ((float)sound.get_frame_count()) * (float)sw;
    int right = selection_end / ((float)sound.get_frame_count()) * (float)sw;

    cr->rectangle(left, 0, right - left, sh);
    cr->fill();
}
void Waveform::draw_sound()
{
    // scribble_create_surface();
    auto cr = Cairo::Context::create(m_waveform_surface);
    /*
    sf_count_t read_count;
    float *ptr;
    */
    // printf("MainWindow::draw_sound %d \n", sound.read_count);

    cr->set_source_rgb(0, 0, 0);
    auto sw = m_waveform_surface->get_width();
    auto sh = m_waveform_surface->get_height();
    // printf("dimensions %dx%d\n", sw, sh);

    cr->set_source_rgb(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));
    cr->set_source_rgb(1.0, 0.5, 0.25);

    int method = (sw > sound.get_frame_count()) ? (-1) : 1;

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
            //    printf("@%d interval idx [%d,%d] samples [%f,%f]\n", i, first_sample, last_sample, min, max);
            int x = i;
            int height = std::max(1.0, (((max - min)) / 2.0) * sh);
            int width = 1;
            // int height = 1;
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
int Waveform::get_frame_number_at(double offset_x)
{
    return ((float)sound.get_frame_count() * (offset_x) / get_width());
}
void Waveform::on_drawingarea_scribble_drag_begin(double start_x, double start_y)
{
    printf("Waveform::on_drawingarea_scribble_drag_begin %f,%f\n", start_x, start_y);

    set_selection_bounds(get_frame_number_at(start_x), get_frame_number_at(start_x));

    /*m_start_x = start_x;
    m_start_y = start_y;
    scribble_draw_brush(m_start_x, m_start_y);
    */
}

void Waveform::on_drawingarea_scribble_drag_update(double offset_x, double offset_y)
{
    // printf("Waveform::on_drawingarea_scribble_drag_update %f,%f -> frame %d of %d\n",offset_x,offset_y,get_frame_number_at(offset_x),sound.get_frame_count());
    // scribble_draw_brush(m_start_x + offset_x, m_start_y + offset_y);

    set_selection_bounds(selection_start, selection_start + get_frame_number_at(offset_x));

    draw_all();
    queue_draw();
}

void Waveform::on_drawingarea_scribble_drag_end(double offset_x, double offset_y)
{
    if (sound.read_count > 0)
    {
        // printf("Waveform::on_drawingarea_scribble_drag_end %f,%f\n", offset_x, offset_y);
        selection_end = selection_start + get_frame_number_at(offset_x);
        set_selection_bounds(selection_start, selection_start + get_frame_number_at(offset_x));
    }
    // printf("selection is now %d,%d\n", selection_start, selection_end);
    //  scribble_draw_brush(m_start_x + offset_x, m_start_y + offset_y);
}
/*
void Waveform::on_gesture_zoom_scale_changed(double scale)
{
    printf("here is scale %f\n", scale);
    // m_GestureZoom->get_scale_delta()
}
*/

void Waveform::set_selection_bounds(int _selection_start, int _selection_end)
{
    selection_start = std::clamp(_selection_start,0,(int)sound.read_count-1);
    selection_end = std::clamp(_selection_end,0,(int)sound.read_count-1);
    printf("selection is now %d,%d %d\n", selection_start, selection_end, sound.read_count);
}