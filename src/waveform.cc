#include "./waveform.h"
#include <cairo.h>

std::string Waveform::ScaleUnit::to_string() const
{
    return "period: " + std::to_string(m_period_s) + "s" + " " + "low bound: " + std::to_string(m_display_low_bound_px) + "px" + " " + "height: " + std::to_string(m_display_height_px) + "px" + " " + "name: " + m_name + " " + "name period: " + std::to_string(m_name_period_s);
}
Waveform::ScaleUnit::ScaleUnit(double period_s, double display_low_bound_px, double display_height_px, std::string name, double name_period_s)
{
    m_period_s = period_s;
    m_display_low_bound_px = display_low_bound_px;
    m_display_height_px = display_height_px;
    m_name = name;
    m_name_period_s = name_period_s;
}
std::string Waveform::ScaleUnit::duration_display_string(double seconds) const
{
    int d_hours = std::floor(seconds / 3600.0);
    seconds -= (double)d_hours * 3600;
    int d_minutes = std::floor(seconds / 60.0);
    seconds -= (double)d_minutes * 60;
    int d_seconds = std::floor(seconds);
    seconds -= (double)d_seconds;
    int d_milliseconds = std::floor(1000 * seconds);

    bool showing_round_minutes = (m_period_s >= 60.0);
    bool showing_round_seconds = (m_period_s >= 1.0);

    std::string s = "";
    if (d_hours > 0)
    {
        s += std::to_string(d_hours) + ":";
    }
    if (d_minutes > 0)
    {
        std::string unpadded = std::to_string(d_minutes);
        s += std::string(2 - unpadded.length(), '0') + unpadded + ((showing_round_minutes) ? "m" : ":");
    }
    if (!showing_round_minutes)
    {
        // if (d_seconds > 0)
        {
            std::string unpadded = std::to_string(d_seconds);
            s += std::string(2 - unpadded.length(), '0') + unpadded +
                 ((showing_round_seconds || (d_milliseconds == 0)) ? "" : ".");
        }
        if (!showing_round_seconds)
        {
            // if (d_milliseconds > 0)
            {
                std::string unpadded = std::to_string(d_milliseconds);
                s += std::string(4 - unpadded.length(), '0') + unpadded;
            }
        }
    }
    return s;

    //

    // seconds -= d_minutes * 60;
    // double d_seconds = seconds;
    // /*double d_seconds = std::floor(seconds);
    // seconds -= d_seconds;
    // double d_milliseconds = seconds * 1000.0*/
    //
    // std::string s_hours = (d_hours == 0) ? "" : (std::to_string((int)d_hours) + ":");
    // std::string s_minutes = (d_minutes == 0) ? "" : (std::to_string((int)d_minutes) + ":");
    // std::string s_seconds = std::to_string(d_seconds);
    // // string s_milliseconds = std::to_string(d_milliseconds);
    // return s_hours + s_minutes + s_seconds;
    //
    // return "super";
}
std::string Waveform::regular_timecode_display(double seconds) const
{
    int d_hours = std::floor(seconds / 3600.0);
    seconds -= (double)d_hours * 3600;
    int d_minutes = std::floor(seconds / 60.0);
    seconds -= (double)d_minutes * 60;
    int d_seconds = std::floor(seconds);
    seconds -= (double)d_seconds;
    int d_milliseconds = std::floor(1000 * seconds);

    std::string s = "";
    s += std::to_string(d_hours);
    s += ":";
    {
        std::string unpadded = std::to_string(d_minutes);
        s += std::string(2 - unpadded.length(), '0') + unpadded;
    }
    s += ":";
    {
        std::string unpadded = std::to_string(d_seconds);
        s += std::string(2 - unpadded.length(), '0') + unpadded;
    }
    s += ".";
    {
        std::string unpadded = std::to_string(d_milliseconds);
        s += std::string(4 - unpadded.length(), '0') + unpadded;
    }
    return s;
}
Waveform::Waveform()
{
    // hack
    hack_sound_start = NULL;
    hack_sound_end = NULL;
    hack_sound_position = NULL;

    m_sound = NULL;

    m_waveform_surface_dirty = true;
    m_scale_surface_dirty = true;
    m_selection_surface_dirty = true;
    m_position_surface_dirty = true;
    m_text_surface_dirty = true;
    
    selection_start = 0;
    selection_end = 0;

    create_draw_surface();

    // m_scale_units.push_back(Waveform::ScaleUnit(10, 10, 40.0));
    // m_scale_units.push_back(Waveform::ScaleUnit(600.0, 10, 21.0, "s", 1.0));
    // m_scale_units.push_back(Waveform::ScaleUnit(300.0, 10, 20.0, "s", 1.0));
    m_scale_units.push_back(Waveform::ScaleUnit(60.0, 10, 19.0, "s", 1.0));
    // m_scale_units.push_back(Waveform::ScaleUnit(30.0, 10, 17.0, "s", 1.0));
    // m_scale_units.push_back(Waveform::ScaleUnit(10.0, 10, 16.0, "s", 1.0));
    // m_scale_units.push_back(Waveform::ScaleUnit(5.0, 10, 15.0, "s", 1.0));
    m_scale_units.push_back(Waveform::ScaleUnit(1.0, 10, 14.0, "s", 1.0));
    // m_scale_units.push_back(Waveform::ScaleUnit(0.5, 10, 12.0, "ms", 0.001));
    m_scale_units.push_back(Waveform::ScaleUnit(0.1, 10, 11.0, "ms", 0.001));
    // m_scale_units.push_back(Waveform::ScaleUnit(0.05, 10, 9.0, "ms", 0.001));
    m_scale_units.push_back(Waveform::ScaleUnit(0.01, 10, 8.0, "ms", 0.001));
    // m_scale_units.push_back(Waveform::ScaleUnit(0.005, 10, 6.0, "ms", 0.001));
    m_scale_units.push_back(Waveform::ScaleUnit(0.001, 10, 5.0, "ms", 0.001));

    // resize event
    signal_resize().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_resize));

    // draw event
    set_draw_func(sigc::mem_fun(*this, &Waveform::on_drawingarea_draw));

    // selection (drag)
    m_Drag_selection = Gtk::GestureDrag::create();
    m_Drag_selection->set_button(GDK_BUTTON_PRIMARY);
    add_controller(m_Drag_selection);
    selection_hot_handle = SelectionHotHandle::NONE;
    proximity_hot_handle = SelectionHotHandle::NONE;
    m_Drag_selection->signal_drag_begin().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_begin));
    m_Drag_selection->signal_drag_update().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_update));
    m_Drag_selection->signal_drag_end().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_selection_end));

    // translation (drag)
    m_Drag_translation = Gtk::GestureDrag::create();
    m_Drag_translation->set_button(GDK_BUTTON_SECONDARY); // GDK_BUTTON_MIDDLE
    add_controller(m_Drag_translation);
    m_Drag_translation->signal_drag_begin().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_translation_begin));
    m_Drag_translation->signal_drag_update().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_translation_update));
    m_Drag_translation->signal_drag_end().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_drag_translation_end));

    // zoom (scroll wheel)
    m_Scroll = Gtk::EventControllerScroll::create();
    m_Scroll->set_flags(Gtk::EventControllerScroll::Flags::BOTH_AXES);
    add_controller(m_Scroll);
    m_Scroll->signal_scroll_begin().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_scroll_begin));
    m_Scroll->signal_scroll().connect(sigc::mem_fun(*this, &Waveform::on_drawingarea_scroll), true);

    // pointer position
    m_Mousemotion = Gtk::EventControllerMotion::create();
    add_controller(m_Mousemotion);
    m_mouse_hover = false;
    m_Mousemotion->signal_motion().connect(sigc::mem_fun(*this, &Waveform::on_mouse_motion));
    m_Mousemotion->signal_enter().connect(sigc::mem_fun(*this, &Waveform::on_mouse_enter));
    m_Mousemotion->signal_leave().connect(sigc::mem_fun(*this, &Waveform::on_mouse_leave));

    // fast periodic call for play position redraw
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Waveform::on_vbl_timeout), 1000 / 20);

    // key event are not passed
    // m_Keypressed = Gtk::EventControllerKey::create();
    // add_controller(m_Keypressed);
    // m_Keypressed->signal_key_pressed().connect(sigc::mem_fun(*this, &Waveform::on_key_pressed), false);
}

bool Waveform::on_vbl_timeout()
{
    // long position = hack_sound_position->load();
    // printf("position %d\n", position);
    m_position_surface_dirty = true;
    queue_draw();
    return true;
}
void Waveform::on_mouse_leave()
{
    m_mouse_hover = false;
    m_selection_surface_dirty = true;
    m_text_surface_dirty = true;
    queue_draw();
}
void Waveform::on_mouse_enter(double x, double y)
{
    m_mouse_hover = true;
    mouse_x = x;
    mouse_y = y;
    m_selection_surface_dirty = true;
    m_text_surface_dirty = true;
    queue_draw();
}
void Waveform::on_mouse_motion(double x, double y)
{
    m_mouse_hover = true;
    mouse_x = x;
    mouse_y = y;

    proximity_hot_handle = closest_hot_handle(x);
    m_selection_surface_dirty = true;
    m_text_surface_dirty = true;
    queue_draw();
}

// bool Waveform::on_key_pressed(const unsigned int a, const unsigned int b, const Gdk::ModifierType c)
// {
//     return true;
// }
void Waveform::set_sound(Sound *sound)
{
    m_sound = sound;
    if (sound != NULL)
    {
        visible_start = 0;
        visible_end = m_sound->get_frame_count();
        selection_start = 0;
        selection_end = 0;
        selection_hot_handle = SelectionHotHandle::NONE;
        proximity_hot_handle = SelectionHotHandle::NONE;

        m_waveform_surface_dirty = true;
        m_selection_surface_dirty = true;
        m_text_surface_dirty = true;
        m_scale_surface_dirty = true;
    }
    queue_draw();
}
void Waveform::on_drawingarea_resize(int width, int height)
{
    create_draw_surface();
    m_waveform_surface_dirty = true;
    m_scale_surface_dirty = true;
    m_selection_surface_dirty = true;
    m_text_surface_dirty = true;
    m_scale_surface_dirty = true;
}
void Waveform::create_draw_surface()
{
    m_waveform_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
    m_scale_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
    m_selection_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
    m_position_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
    m_text_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, get_width(), get_height());
}
void Waveform::on_drawingarea_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height)
{

    cr->set_source_rgba(0, 0, 0, 1.0);
    cr->paint();

    if (m_sound == NULL)
        return;

    if (m_waveform_surface_dirty)
        draw_sound();

    cr->set_source(m_waveform_surface, 0, 0);
    cr->paint();

    if (m_scale_surface_dirty)
        draw_scale();

    cr->set_source(m_scale_surface, 0, 0);
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

void Waveform::draw_scale()
{
    m_scale_surface_dirty = false;

    auto cr = Cairo::Context::create(m_scale_surface);
    cr->set_operator(Cairo::Context::Operator::CLEAR);
    cr->paint();
    cr->set_operator(Cairo::Context::Operator::OVER);
    cr->set_source_rgba(0, 0, 0, 0.0);
    cr->paint();

    cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);

    double vw = (double)get_width();
    double vh = (double)get_height();

    double samplerate_d = (double)(m_sound->get_samplerate());
    double visible_start_s = ((double)visible_start) / samplerate_d;
    double visible_end_s = ((double)visible_end) / samplerate_d;
    double visible_length_s = visible_end_s - visible_start_s;

    std::set<int> used_positions;

    int previous_caption_shown_count = 0;
    const Waveform::ScaleUnit *caption_scale_unit = NULL;
    double unit_label_font_size = 12.0;

    for (auto const &scale_unit : m_scale_units)
    {

        double unit_length_s = scale_unit.m_period_s;                   // 0.1;
        double unit_display_height_px = scale_unit.m_display_height_px; // scale_unit.m_period_s 20;
        std::string unit_name = scale_unit.m_name;

        double pixel_per_second = vw / visible_length_s;
        double pixel_per_unit = unit_length_s * pixel_per_second;

        //        printf("p/s: %f ; unit : %f p/u : %f \n", pixel_per_second, unit_length_s, pixel_per_unit);

        double show_unit_low_bound_px = 20.0;
        int caption_shown_count = 0;
        if (pixel_per_unit <= show_unit_low_bound_px)
            continue;

        double left_s = std::floor(visible_start_s / unit_length_s) * unit_length_s;
        double right_s = std::ceil(visible_end_s / unit_length_s) * unit_length_s;
        for (double x_s = left_s; x_s <= right_s; x_s += unit_length_s)
        {
            double x = get_pixel_at((long)(x_s * ((double)(m_sound->get_samplerate()))));

            if (!((x_s >= visible_start_s) && (x_s < visible_end_s)))
                continue;

            if (auto search = used_positions.find((int)x); search != used_positions.end())
                continue;

            used_positions.insert((int)x);
            cr->set_source_rgba(1.0, 1.0, 1.0, 0.5);

            cr->rectangle(std::round(x), vh - unit_display_height_px, 1, vh);
            cr->fill();

            caption_shown_count++;
        }

        if ((caption_shown_count >= 2) && (previous_caption_shown_count == 0))
        {
            previous_caption_shown_count = caption_shown_count;
            caption_scale_unit = &scale_unit;
        }
    }

    if ((false) && (caption_scale_unit != NULL))
    {
        // printf("caption_scale_unit::::: %s\n", caption_scale_unit->to_string().c_str());
        double unit_length_s = caption_scale_unit->m_period_s; // 0.1;
        //        double unit_display_height_px = caption_scale_unit->m_display_height_px; // scale_unit.m_period_s 20;

        double left_s = std::floor(visible_start_s / unit_length_s) * unit_length_s;
        double right_s = std::ceil(visible_end_s / unit_length_s) * unit_length_s;
        for (double x_s = left_s; x_s <= right_s; x_s += unit_length_s)
        {
            double x = get_pixel_at((long)(x_s * ((double)(m_sound->get_samplerate()))));
            if (!((x_s >= visible_start_s) && (x_s < visible_end_s)))
                continue;

            cr->set_font_size(unit_label_font_size);
            cr->set_source_rgb(1.0, 1.0, 1.0);
            cr->move_to(x + 3, vh - 3);
            cr->show_text(caption_scale_unit->duration_display_string(x_s));
        }
    }
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

    float margin_top = font_size * 1.1;
    float line_height = font_size;
    float margin_left = 5;

    float pos_y = margin_top;

    cr->set_font_size(font_size);
    cr->set_source_rgb(1.0, 1.0, 1.0);
    if (m_mouse_hover)
    {
        float mouse_s = (float)get_frame_number_at(mouse_x) / ((float)(m_sound->get_samplerate()));
        cr->move_to(margin_left, pos_y);
        cr->show_text(regular_timecode_display(mouse_s));
        pos_y += line_height;
    }
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

    auto sh = m_waveform_surface->get_height();

    if (hack_sound_position != NULL)
    {
        long position = hack_sound_position->load();
        double left = get_pixel_at(position);
        cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        cr->rectangle(left, 0, 1, sh);
        cr->fill();
    }
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

    auto sh = m_waveform_surface->get_height();

    cr->set_source_rgba(0.3, 0.8, 0.7, 0.5);
    // cr->set_source_rgba(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX),0.5);
    double left = get_pixel_at(selection_start);
    double right = get_pixel_at(selection_end);

    cr->rectangle(left, 0, right - left, sh);
    cr->fill();

    cr->set_source_rgba(1.0, 1.0, 1.0, 0.9);
    if ((proximity_hot_handle != Waveform::NONE) && (left != right))
    {
        float half_radius = 6.0;
        float half_separation = 8.0;
        // float y = mouse_y;             // moves with cursor
        float y = sh / 2.0; // fixed in the v middle
        float x = (proximity_hot_handle == Waveform::START) ? left : right;
        // cr->rectangle(left - half_radius, mouse_y - half_radius, 2 * half_radius, 2 * half_radius);
        cr->move_to(x + half_separation, y - half_radius);
        cr->line_to(x + half_separation + half_radius, y);
        cr->line_to(x + half_separation, y + half_radius);
        cr->fill();
        cr->move_to(x - half_separation, y - half_radius);
        cr->line_to(x - half_separation - half_radius, y);
        cr->line_to(x - half_separation, y + half_radius);
        cr->fill();
    }
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
    float *sound_data = m_sound->get_sound_data();
    int channels = m_sound->get_channels();

    bool separated_channels = true;
    float channel_height = (separated_channels) ? (sh / (float)channels) : (sh);

    // do not analyse every sample if there are too many
    double max_samples_analyzed = 1500000;
    double max_samples_analyzed_by_channel_frame = max_samples_analyzed / sw / channels;
    double no_pass_samples_analyzed_by_channel_frame = visible_frames / sw;
    int analyze_step = std::max(1.0, std::ceil(no_pass_samples_analyzed_by_channel_frame / max_samples_analyzed_by_channel_frame));

    for (int channel = 0; channel < channels; channel++)
    {
        int channel_offset = separated_channels ? (channel * channel_height) : 0;
        cr->set_source_rgb(1.0, 0.5, 0.25);

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
            for (int j = p0; j <= p1; j += analyze_step)
            {
                float value = sound_data[j * channels + channel];
                max = std::max(max, value);
                min = std::min(min, value);
                // n_samples_analyzed++;
            }
            int x = i;
            int y = channel_offset + ((min + 1.0) / 2.0) * channel_height;
            int height = std::max(1.0, (((max - min)) / 2.0) * channel_height);
            int width = 1;
            cr->rectangle(x, y, width, height);
            cr->fill();
        }
    }
    // printf("%ld samples abalyzed\n",n_samples_analyzed);
}

Waveform::SelectionHotHandle Waveform::closest_hot_handle(double x)
{

    double selection_start_pixel = get_pixel_at(selection_start);
    double distance_to_selection_start_pixel = std::abs(x - selection_start_pixel);

    double selection_end_pixel = get_pixel_at(selection_end);
    double distance_to_selection_end_pixel = std::abs(x - selection_end_pixel);

    double max_grab_distance = 10.0;

    if ((distance_to_selection_start_pixel < distance_to_selection_end_pixel) && (distance_to_selection_start_pixel < max_grab_distance))
        return SelectionHotHandle::START;
    else if ((distance_to_selection_end_pixel <= distance_to_selection_start_pixel) && (distance_to_selection_end_pixel < max_grab_distance))
        return SelectionHotHandle::END;
    else
        return SelectionHotHandle::NONE;
}

void Waveform::on_drawingarea_drag_selection_begin(double start_x, double start_y)
{
    if (m_sound == NULL)
        return;

    selection_hot_handle = closest_hot_handle(start_x);

    switch (selection_hot_handle)
    {
    case SelectionHotHandle::START:
        set_selection_bounds(get_frame_number_at(start_x), selection_end);
        hot_handle_initial_position = selection_start;
        break;

    case SelectionHotHandle::END:
        set_selection_bounds(selection_start, get_frame_number_at(start_x));
        hot_handle_initial_position = selection_end;
        break;

    case SelectionHotHandle::NONE:
        set_selection_bounds(get_frame_number_at(start_x), get_frame_number_at(start_x));
        break;
    }
}

void Waveform::on_drawingarea_drag_selection_update(double offset_x, double offset_y)
{
    if (m_sound == NULL)
        return;

    long d_position = get_frame_number_at(offset_x) - get_frame_number_at((long)0);
    switch (selection_hot_handle)
    {
    case SelectionHotHandle::START:
        set_selection_bounds(hot_handle_initial_position + d_position, selection_end);
        break;

    case SelectionHotHandle::END:
        set_selection_bounds(selection_start, hot_handle_initial_position + d_position);
        break;

    case SelectionHotHandle::NONE:
        set_selection_bounds(selection_start, selection_start + d_position);
        break;
    }
}

void Waveform::on_drawingarea_drag_selection_end(double offset_x, double offset_y)
{
    if (m_sound == NULL)
        return;
    selection_hot_handle = SelectionHotHandle::NONE;
}

void Waveform::on_drawingarea_drag_translation_begin(double start_x, double start_y)
{
    if (m_sound == NULL)
        return;
    translation_initial_visible_start = visible_start;
    translation_initial_visible_end = visible_end;
}
void Waveform::on_drawingarea_drag_translation_update(double offset_x, double offset_y)
{
    if (m_sound == NULL)
        return;
    long d_translation = get_frame_number_at(offset_x) - get_frame_number_at((long)0);

    if ((translation_initial_visible_start - d_translation) < 0)
    {
        d_translation = translation_initial_visible_start;
    }

    if ((translation_initial_visible_end - d_translation) >= (m_sound->get_frame_count()))
    {
        d_translation = translation_initial_visible_end - (m_sound->get_frame_count());
    }

    visible_start = translation_initial_visible_start - d_translation;
    visible_end = translation_initial_visible_end - d_translation;

    m_waveform_surface_dirty = true;
    m_scale_surface_dirty = true;
    m_selection_surface_dirty = true;
    m_text_surface_dirty = true;
    queue_draw();
}
void Waveform::on_drawingarea_drag_translation_end(double offset_x, double offset_y)
{
    if (m_sound == NULL)
        return;
}

void Waveform::set_selection_bounds(int _selection_start, int _selection_end)
{

    selection_start = std::clamp(_selection_start, 0, (int)(m_sound->get_frame_count()) - 1);
    selection_end = std::clamp(_selection_end, 0, (int)(m_sound->get_frame_count()) - 1);

    m_selection_surface_dirty = true;

    queue_draw();

    if (hack_sound_start != NULL)
        hack_sound_start->store(selection_start);
    if (hack_sound_end != NULL)
        hack_sound_end->store(selection_end);
}

void Waveform::on_drawingarea_scroll_begin()
{
    if (m_sound == NULL)
        return;
}
bool Waveform::on_drawingarea_scroll(double x, double y)
{
    if (m_sound == NULL)
        return true;
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
    visible_end = std::min(new_right, (long)(m_sound->get_frame_count()));

    m_waveform_surface_dirty = true;
    m_scale_surface_dirty = true;
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
long Waveform::get_selection_start_frame()
{
    return selection_start;
}
long Waveform::get_selection_end_frame()
{
    return selection_end;
}