#include "./selections.h"

IconContextLabel::IconContextLabel(const Glib::ustring &left_timecode, const Glib::ustring &right_timecode, const Glib::ustring &duration_timecode, long start, long end)
{
    m_selection_start_frame = start;
    m_selection_end_frame = end;

    append(m_label);
    m_label.set_text(left_timecode + " to " + right_timecode);
    m_label.set_hexpand();
    m_label.set_halign(Gtk::Align::START);
}

// Main window.

// Definition of main window methods.
SelectionsListBox::SelectionsListBox()
{
    set_selection_mode(Gtk::SelectionMode::SINGLE);
    signal_selected_rows_changed().connect(sigc::mem_fun(*this, &SelectionsListBox::on_context_list_selected_rows_changed));
    m_p_Waveform = NULL;
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

    if ((m_p_Waveform != NULL) && (label != NULL)) {
        m_p_Waveform->set_selection_bounds(label->m_selection_start_frame,label->m_selection_end_frame);     
    }
// 
//     if (label)
//     {
//         // m_current_context_id = label->get_id();
//         //  filter_changed();
//     }
//     else
//         std::cout << "on_context_list_selected_rows_changed(): Unexpected child type." << std::endl;
}
void SelectionsListBox::on_mouse_enter(double x, double y)
{
    printf("enter : %f,%f\n", x, y);
}
void SelectionsListBox::on_mouse_leave()
{
    printf("leave \n");
}
void SelectionsListBox::remove_selected()
{
    auto row = get_selected_row();
    if (!row)
        return;

    remove(*row);
}
void SelectionsListBox::add_context(const Glib::ustring &left_timecode, const Glib::ustring &right_timecode, const Glib::ustring &duration_timecode, long start, long end)
{

    IconContextLabel *row = Gtk::make_managed<IconContextLabel>(left_timecode, right_timecode, duration_timecode, start, end);
    row->set_margin(1);
    append(*row);

    // Set the tooltip on the list box row.
    auto listboxrow = row->get_parent();
    listboxrow->set_tooltip_text(duration_timecode);

    //auto m_Mousemotion = Gtk::EventControllerMotion::create();
    //add_controller(m_Mousemotion);
    row->activate()        ;

    
    
    // m_Mousemotion->signal_enter().connect(sigc::mem_fun(*this, &SelectionsListBox::on_mouse_enter));
    // m_Mousemotion->signal_leave().connect(sigc::mem_fun(*this, &SelectionsListBox::on_mouse_leave));
}
void SelectionsListBox::set_waveform(Waveform * p_waveform){
    m_p_Waveform = p_waveform;
}