#include <gtkmm.h>
#include "./sound.h"

class Waveform : public Gtk::DrawingArea
{
public:
    Waveform();
    int id;
    Cairo::RefPtr<Cairo::ImageSurface> m_waveform_surface;

    void on_drawingarea_scribble_resize(int width, int height);
    void scribble_create_surface();
    void on_drawingarea_checkerboard_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);

    Sound sound;

    void draw_sound();
    void set_sound(Sound sound);

    void draw_selection();
    void draw_text();
    void draw_all();

    int scale;

    Glib::RefPtr<Gtk::GestureDrag> m_drag;

    void on_drawingarea_scribble_drag_begin(double start_x, double start_y);
    void on_drawingarea_scribble_drag_update(double offset_x, double offset_y);
    void on_drawingarea_scribble_drag_end(double offset_x, double offset_y);

    // bool has_selection;
    int selection_start;
    int selection_end;
    void set_selection_bounds(int _selection_start, int _selection_end);
    int get_frame_number_at(double offset_x);

    Glib::RefPtr<Gtk::GestureZoom> m_GestureZoom;
    // void on_gesture_zoom_scale_changed(double scale);
};