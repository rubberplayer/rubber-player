#include <gtkmm.h>
#include "./waveform.h"
// #include "./db.h"

class IconContextLabel : public Gtk::Box
{
public:
    IconContextLabel(long start, long end, const Glib::ustring &label);
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
    std::string add_context(long start, long end, Sound *sound);
    std::string add_context(long start, long end, std::string label);
    std::string add_context(long start, long end, const Glib::ustring &left_timecode, const Glib::ustring &right_timecode);
    IconContextLabel* remove_selected();
    void reset();
    void set_waveform(Waveform *p_waveform);
    // void set_db(SelectionDB *p_selection_db);

protected:
    // Signal handlers:
    void on_context_list_selected_rows_changed();
    void on_mouse_enter(double x, double y);
    void on_mouse_leave();
    Waveform *m_p_Waveform;
    // SelectionDB* m_p_selection_db;
};
