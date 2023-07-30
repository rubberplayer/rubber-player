#include "./waveform.h"
#include <cairo.h>
Waveform::Waveform()
{
    // hack
    hack_sound_start = NULL;
    hack_sound_end = NULL;

    signal_resize().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_resize));
    set_draw_func(sigc::mem_fun(*this, &Waveform::on_drawingarea_draw));

    m_Drag_selection = Gtk::GestureDrag::create();
    m_Drag_selection->set_button(GDK_BUTTON_PRIMARY);
    add_controller(m_Drag_selection);

    m_Drag_selection->signal_drag_begin().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_begin));
    m_Drag_selection->signal_drag_update().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_update));
    m_Drag_selection->signal_drag_end().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_end));

    // has_selection = false;
    m_waveform_surface_dirty = true;
    m_selection_surface_dirty = true;
    m_position_surface_dirty = true;
    m_text_surface_dirty = true;
    create_draw_surface();

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

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Waveform::on_vbl_timeout), 1000 / 20);
}

bool Waveform::on_vbl_timeout()
{
    //long position = hack_sound_position->load();
    //printf("position %d\n", position);
    m_position_surface_dirty = true;
    queue_draw();
    return true;
}
void Waveform::on_mouse_leave()
{
    mouse_inside = false;
}
void Waveform::on_mouse_enter(double x, double y)
{
    mouse_inside = true;
    mouse_x = x;
    mouse_y = y;
    m_text_surface_dirty = true;
    queue_draw();
}
void Waveform::on_mouse_motion(double x, double y)
{
    mouse_x = x;
    mouse_y = y;
    m_text_surface_dirty = true;
    queue_draw();
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

    m_waveform_surface_dirty = true;
    m_selection_surface_dirty = true;
    m_text_surface_dirty = true;

    queue_draw();
}
void Waveform::on_drawingarea_resize(int width, int height)
{
    create_draw_surface();
    m_waveform_surface_dirty = true;
    m_selection_surface_dirty = true;
    m_text_surface_dirty = true;
}
void Waveform::create_draw_surface()
{
    m_waveform_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
    m_text_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
    m_selection_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
    m_position_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
}
void Waveform::on_drawingarea_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height)
{
    cr->set_source_rgba(0, 0, 0, 1.0);
    cr->paint();

    if (m_waveform_surface_dirty)
        draw_sound();

    cr->set_source(m_waveform_surface, 0, 0);
    cr->paint();

    if (m_selection_surface_dirty)
        draw_selection();

    cr->set_source(m_selection_surface, 0, 0);
    cr->paint();

    if (m_position_surface_dirty)
        draw_position();

    cr->set_source(m_position_surface, 0, 0);
    cr->paint();

    if (m_text_surface_dirty)
        draw_text();

    cr->set_source(m_text_surface, 0, 0);
    cr->paint();
}
void Waveform::draw_all()
{
    //   create_draw_surface();
    //    draw_sound();
    //  draw_selection();
    //    draw_text();
}
void Waveform::draw_text()
{
    m_text_surface_dirty = false;

    auto cr = Cairo::Context::create(m_text_surface);
    cr->set_operator(Cairo::Context::Operator::CLEAR);
    cr->paint();
    cr->set_operator(Cairo::Context::Operator::OVER);
    cr->set_source_rgba(0, 0, 0, 0.0);
    cr->paint();

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
void Waveform::draw_position()
{
    m_position_surface_dirty = false;
    
    
    auto cr = Cairo::Context::create(m_position_surface);
    cr->set_operator(Cairo::Context::Operator::CLEAR);
    cr->paint();
    cr->set_operator(Cairo::Context::Operator::OVER);
    cr->set_source_rgba(0, 0, 0, 0.0);
    cr->paint();

    auto sw = m_waveform_surface->get_width();
    auto sh = m_waveform_surface->get_height();

    long position = hack_sound_position->load();
    double left = get_pixel_at(position);
    cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
    cr->rectangle(left, 0, 1, sh);
    cr->fill();

}
void Waveform::draw_selection()
{
    m_selection_surface_dirty = false;

    auto cr = Cairo::Context::create(m_selection_surface);
    cr->set_operator(Cairo::Context::Operator::CLEAR);
    cr->paint();
    cr->set_operator(Cairo::Context::Operator::OVER);
    cr->set_source_rgba(0, 0, 0, 0.0);
    cr->paint();
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
    m_waveform_surface_dirty = false;
    auto cr = Cairo::Context::create(m_waveform_surface);
    cr->set_source_rgb(0, 0, 0);
    cr->paint();

    auto sw = m_waveform_surface->get_width();
    auto sh = m_waveform_surface->get_height();

    cr->set_source_rgb(1.0, 0.5, 0.25);
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
void Waveform::on_drawingarea_drag_selection_begin(double start_x, double start_y)
{
    set_selection_bounds(get_frame_number_at(start_x), get_frame_number_at(start_x));
}

void Waveform::on_drawingarea_drag_selection_update(double offset_x, double offset_y)
{
    set_selection_bounds(selection_start, selection_start + get_frame_number_at(offset_x) - get_frame_number_at((long)0));
}

void Waveform::on_drawingarea_drag_selection_end(double offset_x, double offset_y)
{
    set_selection_bounds(selection_start, selection_start + get_frame_number_at(offset_x) - get_frame_number_at((long)0));
}

void Waveform::set_selection_bounds(int _selection_start, int _selection_end)
{
    selection_start = std::clamp(_selection_start, 0, (int)sound.read_count - 1);
    selection_end = std::clamp(_selection_end, 0, (int)sound.read_count - 1);

    m_selection_surface_dirty = true;
    queue_draw();

    if (hack_sound_start != NULL)
        hack_sound_start->store(selection_start);
    if (hack_sound_end != NULL)
        hack_sound_end->store(selection_end);
}

void Waveform::on_drawingarea_scroll_begin()
{
}
bool Waveform::on_drawingarea_scroll(double x, double y)
{
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
    double factor = zoom_out ? 2.0 : 0.5;
    double new_frames_at_left;
    double new_frames_at_right;
    if (zoom_out && ((frames_at_right - frames_at_left) == 0))
    {
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

    m_waveform_surface_dirty = true;
    m_selection_surface_dirty = true;
    m_text_surface_dirty = true;
    queue_draw();
}
// hack
void Waveform::set_hack_sound_start_sound_end_sound_position(std::atomic<long> *s, std::atomic<long> *e, std::atomic<long> *p)
{
    hack_sound_start = s;
    hack_sound_end = e;
    hack_sound_position = p;
}