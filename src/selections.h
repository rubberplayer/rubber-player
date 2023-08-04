#include <gtkmm.h>
#include "./waveform.h"
class IconContextLabel : public Gtk::Box
{
public:
    IconContextLabel(const Glib::ustring &left_timecode, const Glib::ustring &right_timecode, const Glib::ustring &duration_timecode, long start, long end);
    long m_selection_start_frame;
    long m_selection_end_frame;
    
private:
    Gtk::Label m_label;

};

class SelectionsListBox : public Gtk::ListBox
{
public:
    SelectionsListBox();
    ~SelectionsListBox() override;
    void add_context(const Glib::ustring &left_timecode, const Glib::ustring &right_timecode, const Glib::ustring &duration_timecode, long start, long end);
    void remove_selected();
    void set_waveform(Waveform *p_waveform);

protected:
    // Signal handlers:
    void on_context_list_selected_rows_changed();
    void on_mouse_enter(double x, double y);
    void on_mouse_leave();
    Waveform *m_p_Waveform;
};
