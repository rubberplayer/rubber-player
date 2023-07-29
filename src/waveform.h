#include <gtkmm.h>
#include "./sound.h"

class Waveform : public Gtk::DrawingArea
{
    public:
    Waveform();
    int id;
    Cairo::RefPtr<Cairo::ImageSurface> m_surface;

    void on_drawingarea_scribble_resize(int width, int height);
    void scribble_create_surface();
    void on_drawingarea_checkerboard_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);

    Sound sound;
        
    void draw_sound(Sound sound);
    void set_sound(Sound sound);

};