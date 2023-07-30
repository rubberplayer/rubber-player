#include <gtkmm.h>
#include "./sound.h"

class Waveform : public Gtk::DrawingArea
{
public:
    Waveform();
    int id;
    Cairo::RefPtr<Cairo::ImageSurface> m_waveform_surface;

    void on_drawingarea_resize(int width, int height);
    void create_draw_surface();
    void on_drawingarea_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);

    long visible_start;
    long visible_end;

    Sound sound;
    void set_sound(Sound sound);

    void draw_sound();
    void draw_selection();
    void draw_text();
    void draw_all();

    Glib::RefPtr<Gtk::GestureDrag> m_Drag_selection;

    void on_drawingarea_drag_selection_begin(double start_x, double start_y);
    void on_drawingarea_drag_selection_update(double offset_x, double offset_y);
    void on_drawingarea_drag_selection_end(double offset_x, double offset_y);

    // bool has_selection;
    long selection_start;
    long selection_end;
    void set_selection_bounds(int _selection_start, int _selection_end);
    long get_frame_number_at(double offset_x);
    double get_pixel_at(long frame);
    // Glib::RefPtr<Gtk::GestureZoom> m_GestureZoom;
    //
    // Glib::RefPtr<Gtk::GestureDrag> m_Drag_zoom;
    // void on_drawingarea_drag_zoom_begin(double start_x, double start_y);
    // void on_drawingarea_drag_zoom_update(double offset_x, double offset_y);
    // void on_drawingarea_drag_zoom_end(double offset_x, double offset_y);
    //
    // int zoom_center_x;
    // int zoom_center_y;
    //
    // void set_zoom_center(int x, int y);
    // float zoom;
    // float zoom_at_start;

    Glib::RefPtr<Gtk::EventControllerKey> m_Keypressed;
    bool on_key_pressed(const unsigned int a, const unsigned int b, const Gdk::ModifierType c);

    Glib::RefPtr<Gtk::EventControllerMotion> m_Mousemotion;
    void on_mouse_motion(double x, double y);
    void on_mouse_leave();
    void on_mouse_enter(double x, double y);
    bool mouse_inside;
    double mouse_x, mouse_y;

    Glib::RefPtr<Gtk::EventControllerScroll> m_Scroll;
    void on_drawingarea_scroll_begin();
    bool on_drawingarea_scroll(double x, double y);
    void zoom_around(long frame, bool zoom_out);

    // hack
    std::atomic<long> *hack_sound_start;
    std::atomic<long> *hack_sound_end;
    void set_hack_sound_start_sound_end(std::atomic<long> *s, std::atomic<long> *e);

    //
    bool on_vbl_timeout();
};