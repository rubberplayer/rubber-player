#include "./waveform.h"

Waveform::Waveform()
{
    signal_resize().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_scribble_resize));
    set_draw_func(sigc::mem_fun(*this, &Waveform::on_drawingarea_checkerboard_draw));

    m_Drag_selection = Gtk::GestureDrag::create();
    m_Drag_selection->set_button(GDK_BUTTON_PRIMARY);
    add_controller(m_Drag_selection);

    m_Drag_selection->signal_drag_begin().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_begin));
    m_Drag_selection->signal_drag_update().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_update));
    m_Drag_selection->signal_drag_end().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_end));

    // has_selection = false;
    set_selection_bounds(0, 0);

    m_Scroll = Gtk::EventControllerScroll::create();
    m_Scroll->set_flags(Gtk::EventControllerScroll::Flags::BOTH_AXES);
    add_controller(m_Scroll);
    m_Scroll->signal_scroll_begin().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_scroll_begin));
    m_Scroll->signal_scroll().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_scroll), true);

    m_Keypressed = Gtk::EventControllerKey::create();
    add_controller(m_Keypressed);
    m_Keypressed->signal_key_pressed().connect(sigc::mem_fun(*this, &Waveform::on_key_pressed), false);

    m_Mousemotion = Gtk::EventControllerMotion::create();
    add_controller(m_Mousemotion);
    m_Mousemotion->signal_motion().connect(sigc::mem_fun(*this, &Waveform::on_mouse_motion));

    mouse_inside = false;
    m_Mousemotion = Gtk::EventControllerMotion::create();
    add_controller(m_Mousemotion);
    m_Mousemotion->signal_motion().connect(sigc::mem_fun(*this, &Waveform::on_mouse_motion));
    m_Mousemotion->signal_enter().connect(sigc::mem_fun(*this, &Waveform::on_mouse_enter));
    m_Mousemotion->signal_leave().connect(sigc::mem_fun(*this, &Waveform::on_mouse_leave));
}
void Waveform::on_mouse_leave()
{
    //   printf("mouse leave\n");
    mouse_inside = false;
}
void Waveform::on_mouse_enter(double x, double y)
{
    mouse_inside = true;
    mouse_x = x;
    mouse_y = y;
    // printf("mouse enter, %f, %f\n");
}
void Waveform::on_mouse_motion(double x, double y)
{
    mouse_x = x;
    mouse_y = y;
    draw_all();
    queue_draw();
    // printf("mouse motion, %f, %f\n",x,y);
}

bool Waveform::on_key_pressed(const unsigned int a, const unsigned int b, const Gdk::ModifierType c)
{
    printf("I am here KEYRPESS %d %d \n", a, b);
    return true;
}
void Waveform::set_sound(Sound _sound)
{
    sound = _sound;
    visible_start = 0;
    visible_end = sound.get_frame_count();
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
    float margin_top = font_size * 1.1;
    float line_height = font_size;
    float margin_left = 5;

    float pos_y = margin_top;

    cr->set_font_size(font_size);
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->move_to(margin_left, pos_y);
    cr->show_text(std::to_string(left_s) + " s");
    pos_y += line_height;

    cr->move_to(margin_left, pos_y);
    cr->show_text(std::to_string(right_s) + " s");
    pos_y += line_height;

    float mouse_s = (float)get_frame_number_at(mouse_x) / (float)sound.sfinfo.samplerate;
    cr->move_to(margin_left, pos_y);
    cr->show_text(std::to_string(mouse_s) + " s");
    pos_y += line_height;
}
void Waveform::draw_selection()
{
    auto cr = Cairo::Context::create(m_waveform_surface);
    auto sw = m_waveform_surface->get_width();
    auto sh = m_waveform_surface->get_height();

    cr->set_source_rgba(0.3, 0.8, 0.7, 0.5);
    // cr->set_source_rgba(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX),0.5);
    double left = get_pixel_at(selection_start);
    double right = get_pixel_at(selection_end);

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

    // int method = (sw > sound.get_frame_count()) ? (-1) : 1;
    int method = -2;
    if (method == -2)
    {
        double visible_frames = (double)(visible_end - visible_start);
        for (int i = 0; i < sw; i++)
        {
            long p0 = visible_start + (long)(((double)i / (double)sw) * visible_frames);
            long p1;
            if (i == (sw - 1))
            {
                p1 = visible_end;
            }
            else
            {
                p1 = visible_start + (long)(((double)(i + 1) / (double)sw) * visible_frames);
            }
            float max = -1;
            float min = 1;
            for (int j = p0; j <= p1; j++)
            {
                float value = sound.ptr[j];
                max = std::max(max, value);
                min = std::min(min, value);
            }
             int x = i;
            int height = std::max(1.0, (((max - min)) / 2.0) * sh);
            int width = 1;
            cr->rectangle(
                x,
                ((min + 1.0) / 2.0) * sh,
                width,
                height);
            cr->fill();
        }
    }
    else if (method = 666)
    {
        double visible_frames = (double)(visible_end - visible_start);
        for (int i = 0; i < sw; i++)
        {
            long p = visible_start + (long)(((double)i / (double)sw) * visible_frames);
            float value = sound.ptr[p];
            int x = i;
            int y = (value + 1.0) / 2.0 * (float)sh;
            cr->rectangle(x, y, 1, 1);
            cr->fill();
        }
    }
    else if (method == 0)
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
void Waveform::on_drawingarea_drag_selection_begin(double start_x, double start_y)
{
    set_selection_bounds(get_frame_number_at(start_x), get_frame_number_at(start_x));
    draw_all();
    queue_draw();
}

void Waveform::on_drawingarea_drag_selection_update(double offset_x, double offset_y)
{
    set_selection_bounds(selection_start, selection_start + get_frame_number_at(offset_x));
    draw_all();
    queue_draw();
}

void Waveform::on_drawingarea_drag_selection_end(double offset_x, double offset_y)
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
    selection_start = std::clamp(_selection_start, 0, (int)sound.read_count - 1);
    selection_end = std::clamp(_selection_end, 0, (int)sound.read_count - 1);
    //  printf("selection is now %d,%d %d\n", selection_start, selection_end, sound.read_count);
}

void Waveform::on_drawingarea_scroll_begin()
{
    printf("scroll_begin\n");
}
bool Waveform::on_drawingarea_scroll(double x, double y)
{
    printf("scroll %f, %f\n", x, y);
    zoom_around(get_frame_number_at(mouse_x), (y > 0));
    return true;
}

long Waveform::get_frame_number_at(double offset_x)
{
    double wr = offset_x / get_width();
    double wf = ((double)(visible_end - visible_start)) * wr;
    return visible_start + (long)wf;
}
double Waveform::get_pixel_at(long frame)
{
    double r = ((double)(frame - visible_start)) / ((double)(visible_end - visible_start));
    double x = r * get_width();
    return x;
}
void Waveform::zoom_around(long frame, bool zoom_out)
{
    long frames_at_left = frame - visible_start;
    long frames_at_right = visible_end - frame;
    printf("zoom %s\n", zoom_out ? "out" : "in");
    double factor = zoom_out ? 2.0 : 0.5;
    double new_frames_at_left;
    double new_frames_at_right;
    if (zoom_out && ((frames_at_right - frames_at_left) == 0))
    {
        printf("special case %d,%d\n", frames_at_left, frames_at_right);
        double w = get_width();
        double wl = frames_at_left / std::max((long)1, (visible_end - visible_start));
        double wr = 1 - wl;
        new_frames_at_left = w * wl;
        new_frames_at_right = w * wr;
    }
    else
    {
        new_frames_at_left = factor * (double)frames_at_left;
        new_frames_at_right = factor * (double)frames_at_right;
    }
    long new_left = frame - (long)new_frames_at_left;
    long new_right = frame + (long)new_frames_at_right;

    visible_start = std::max((long)0, new_left);
    visible_end = std::min(new_right, (long)sound.get_frame_count());

    draw_all();
    queue_draw();

    printf("[%d,%d]\n", visible_start, visible_end);
}
