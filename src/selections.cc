#include "./selections.h"

Selection::Selection(long selection_start_frame, long selection_end_frame, std::string sound_path, std::string label){
    m_selection_start_frame = selection_start_frame;
    m_selection_end_frame = selection_end_frame;
    m_sound_path = sound_path;
    m_label = label;
}

IconContextLabel::IconContextLabel(long start, long end, const Glib::ustring &label)
{
    m_selection_start_frame = start;
    m_selection_end_frame = end;
    append(m_label);
    m_label.set_text(label);
    m_label.set_hexpand();
    m_label.set_halign(Gtk::Align::START);

    append(m_entry);
    m_entry.hide();

}
void IconContextLabel::edit(){
    // printf("edit me %s\n",m_label.get_text().c_str());
    // m_label.hide();
    // m_entry.get_buffer()->set_text(m_label.get_text());
    // m_entry.show();
}
// Main window.

// Definition of main window methods.
SelectionsListBox::SelectionsListBox()
{
    set_selection_mode(Gtk::SelectionMode::SINGLE);
    signal_selected_rows_changed().connect(sigc::mem_fun(*this, &SelectionsListBox::on_context_list_selected_rows_changed));
    m_p_Waveform = NULL;
    // m_p_selection_db = NULL;
}

SelectionsListBox::~SelectionsListBox()
{
}

void SelectionsListBox::on_context_list_selected_rows_changed()
{
    auto row = /*m_context_list.*/ get_selected_row();
    if (!row)
        return;

    auto rowchild = row->get_child();
    IconContextLabel *label = dynamic_cast<IconContextLabel *>(rowchild);

    if ((m_p_Waveform != NULL) && (label != NULL))
    {
        m_p_Waveform->set_selection_bounds(label->m_selection_start_frame, label->m_selection_end_frame);
    }
}
void SelectionsListBox::on_mouse_enter(double x, double y)
{
}
void SelectionsListBox::on_mouse_leave()
{
}
IconContextLabel *SelectionsListBox::remove_selected()
{
    auto row = get_selected_row();
    if (!row)
        return NULL;

    auto rowchild = row->get_child();
    IconContextLabel *label = dynamic_cast<IconContextLabel *>(rowchild);
    remove(*row);
    return label;
}
void SelectionsListBox::reset()
{
    while (get_row_at_index(0) != NULL)
    {
        auto first_row = get_row_at_index(0);
        remove(*first_row);
    }
}
std::string SelectionsListBox::add_context(long start, long end, std::string label)
{
    IconContextLabel *row = Gtk::make_managed<IconContextLabel>(start, end, label);
    row->set_margin(1);
    append(*row);

    // Set the tooltip on the list box row.
    // auto listboxrow = row->get_parent();
    //    listboxrow->set_tooltip_text(duration_timecode);

    // auto m_Mousemotion = Gtk::EventControllerMotion::create();
    // add_controller(m_Mousemotion);
    row->activate();
    return label;
}

std::string SelectionsListBox::add_context(long start, long end, const Glib::ustring &left_timecode, const Glib::ustring &right_timecode)
{
    std::string label = left_timecode + " to " + right_timecode;
    return add_context(start, end, label);
}
std::string SelectionsListBox::add_context(long start, long end, Sound *sound)
{
    long selection_left_frame = std::min(start, end);
    long selection_right_frame = std::max(start, end);

    double selection_left_seconds = sound->get_second_at_frame(selection_left_frame);
    double selection_right_seconds = sound->get_second_at_frame(selection_right_frame);

    auto left_timecode = Waveform::regular_timecode_display(selection_left_seconds);
    auto right_timecode = Waveform::regular_timecode_display(selection_right_seconds);

    return add_context(start, end, left_timecode, right_timecode);
}
void SelectionsListBox::set_waveform(Waveform *p_waveform)
{
    m_p_Waveform = p_waveform;
}
// void SelectionsListBox::set_db(SelectionDB *p_selection_db){
//     m_p_selection_db = p_selection_db;
// }

std::string SelectionsListBox::rename_selected()
{
    auto row = get_selected_row();
    if (!row)
        return NULL;

    auto rowchild = row->get_child();
    IconContextLabel *label = dynamic_cast<IconContextLabel *>(rowchild);
    label->edit();
    return "super";
}
