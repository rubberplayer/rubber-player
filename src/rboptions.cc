#include "./rboptions.h"

// https://stackoverflow.com/questions/15441157/gtkmm-multiple-windows-popup-window
// https://rafaelmardojai.pages.gitlab.gnome.org/pygobject-guide/gtk4/controls/dropdown.html

RubberBandOptionsWindow::RubberBandOptionValue::RubberBandOptionValue(std::string name, RubberBand::RubberBandStretcher::Option rubberband_option_flag)
{
    m_name = name;
    m_rubberband_option_flag = rubberband_option_flag;
}
RubberBandOptionsWindow::RubberBandOption::RubberBandOption(std::string name, std::vector<RubberBandOptionValue> values, std::vector<std::string> engines, std::vector<std::string> needs_restart)
{
    m_name = name;
    m_values = values;
    m_engines = engines;
    m_needs_restart = needs_restart;
}
void RubberBandOptionsWindow::set_from_rubberband_option_bits(RubberBand::RubberBandStretcher::Options bits)
{
    for (auto p_option : options)
    {
        // build mask from this option values
        RubberBand::RubberBandStretcher::Options mask = 0;
        for (auto value : p_option->m_values)
        {
            mask |= value.m_rubberband_option_flag;
        }
        RubberBand::RubberBandStretcher::Options masked = bits & mask;

        // find and set value
        int i = 0;
        for (auto value : p_option->m_values)
        {
            if (value.m_rubberband_option_flag == masked)
            {
                p_option->widget->set_selected(i);
            }
            i++;
        }
    }
}
RubberBand::RubberBandStretcher::Options RubberBandOptionsWindow::get_rubberband_option_bits()
{
    RubberBand::RubberBandStretcher::Options bits = 0;
    for (auto p_option : options)
    {
        int selected = p_option->widget->get_selected();
        bits |= p_option->m_values[selected].m_rubberband_option_flag;
    }
    return bits;
}
void RubberBandOptionsWindow::set_sensitive_from_revision()
{
    //  get current_engine_revision
    std::string current_engine_revision;
    for (auto p_option : options)
    {
        if (p_option->m_name.compare("Engine") == 0)
        {
            std::string current_engine = p_option->m_values[p_option->widget->get_selected()].m_name;
            current_engine_revision = (current_engine.compare("Faster") == 0) ? "R2" : "R3";
            break;
        }
    }
    for (auto p_option : options)
    {
        bool match_current_engine_revision = false;
        for (auto e : p_option->m_engines)
        {
            if (e.compare(current_engine_revision) == 0)
            {
                match_current_engine_revision = true;
            }
        }
        p_option->widget->set_sensitive(match_current_engine_revision);
    }
}

void RubberBandOptionsWindow::selected_item_changed(const Gtk::DropDown *dropdown, RubberBandOption *rubber_band_option)
{
    auto name = rubber_band_option->m_name;
    int selected = dropdown->get_selected();
    auto selected_value = rubber_band_option->m_values[selected];
    printf("changed %s to %s\n", name.c_str(), selected_value.m_name.c_str());

    if (rubber_band_option->m_name.compare("Engine") == 0)
    {
        set_sensitive_from_revision();
    }
}
RubberBandOptionsWindow::RubberBandOptionsWindow()
    : m_vertical_box(Gtk::Orientation::VERTICAL, 8)
{
    {
        using RubberBand::RubberBandStretcher;
        options = {
            new RubberBandOption("Engine",
                                 {RubberBandOptionValue("Faster", RubberBandStretcher::OptionEngineFaster), RubberBandOptionValue("Finer", RubberBandStretcher::OptionEngineFiner)},
                                 {"R2", "R3"}, {"R2", "R3"}),
            new RubberBandOption("Transients",
                                 {RubberBandOptionValue("Crisp", RubberBandStretcher::OptionTransientsCrisp), RubberBandOptionValue("Mixed", RubberBandStretcher::OptionTransientsMixed), RubberBandOptionValue("Smooth", RubberBandStretcher::OptionTransientsSmooth)},
                                 {"R2"}, {}),
            new RubberBandOption("Detector",
                                 {RubberBandOptionValue("Compound", RubberBandStretcher::OptionDetectorCompound), RubberBandOptionValue("Percussive", RubberBandStretcher::OptionDetectorPercussive), RubberBandOptionValue("Soft", RubberBandStretcher::OptionDetectorSoft)},
                                 {"R2"}, {}),
            new RubberBandOption("Phase",
                                 {RubberBandOptionValue("Laminar", RubberBandStretcher::OptionPhaseLaminar), RubberBandOptionValue("Independent", RubberBandStretcher::OptionPhaseIndependent)},
                                 {"R2"}, {}),
            new RubberBandOption("Threading",
                                 {RubberBandOptionValue("Auto", RubberBandStretcher::OptionThreadingAuto), RubberBandOptionValue("Never", RubberBandStretcher::OptionThreadingNever), RubberBandOptionValue("Always", RubberBandStretcher::OptionThreadingAlways)},
                                 {"R2", "R3"}, {"R2", "R3"}), // maybe available later for R3
            new RubberBandOption("Window",
                                 {RubberBandOptionValue("Standard", RubberBandStretcher::OptionWindowStandard), RubberBandOptionValue("Short", RubberBandStretcher::OptionWindowShort), RubberBandOptionValue("Long", RubberBandStretcher::OptionWindowLong)},
                                 {"R2", "R3"}, {"R2", "R3"}),
            new RubberBandOption("Smoothing",
                                 {RubberBandOptionValue("Off", RubberBandStretcher::OptionSmoothingOff), RubberBandOptionValue("On", RubberBandStretcher::OptionSmoothingOn)},
                                 {"R2"}, {"R2", "R3"}),
            new RubberBandOption("Formant",
                                 {RubberBandOptionValue("Shifted", RubberBandStretcher::OptionFormantShifted), RubberBandOptionValue("Preserved", RubberBandStretcher::OptionFormantPreserved)},
                                 {"R2", "R3"}, {}),
            new RubberBandOption("Pitch",
                                 {RubberBandOptionValue("HighSpeed", RubberBandStretcher::OptionPitchHighSpeed), RubberBandOptionValue("HighQuality", RubberBandStretcher::OptionPitchHighQuality), RubberBandOptionValue("HighConsistency", RubberBandStretcher::OptionPitchHighConsistency)},
                                 {"R2", "R3"}, {"R3"}),
            new RubberBandOption("Channels",
                                 {RubberBandOptionValue("Apart", RubberBandStretcher::OptionChannelsApart), RubberBandOptionValue("Together", RubberBandStretcher::OptionChannelsTogether)},
                                 {"R2", "R3"}, {"R2", "R3"}),
        };
    }

    this->set_default_size(220, 156);
    this->set_title("Stretching");
    this->set_resizable(false);
    this->set_titlebar(m_HeaderBar);

    set_child(m_vertical_box);
    m_vertical_box.set_margin_start(10);
    m_vertical_box.set_margin_end(10);
    m_vertical_box.set_margin_top(10);
    m_vertical_box.set_margin_bottom(10);

    for (auto p_option : options)
    {
        // container
        auto option_horizontal_box = Gtk::Box();
        option_horizontal_box.set_hexpand();
        m_vertical_box.append(option_horizontal_box);

        // label
        auto option_label = Gtk::Label();
        option_label.set_text(p_option->m_name);
        option_label.set_halign(Gtk::Align::START);
        option_label.set_hexpand();
        option_label.set_margin_end(10);
        option_horizontal_box.append(option_label);

        // drop down
        Gtk::DropDown *option_dropdown = new Gtk::DropDown();
        option_dropdown->set_halign(Gtk::Align::END);
        p_option->widget = option_dropdown;

        // model + values
        std::vector<Glib::ustring> strings;
        for (auto value : p_option->m_values)        
        {            
            strings.push_back(value.m_name);
        }
        auto model = Gtk::StringList::create(strings);
        option_dropdown->set_model(model);
        option_horizontal_box.append(*option_dropdown);

        // signal changed
        option_dropdown->property_selected().signal_changed().connect(
            sigc::bind(sigc::mem_fun(*this, &RubberBandOptionsWindow::selected_item_changed), option_dropdown, p_option));
    }

    set_from_rubberband_option_bits(RubberBand::RubberBandStretcher::OptionEngineFiner | RubberBand::RubberBandStretcher::OptionPitchHighConsistency | RubberBand::RubberBandStretcher::OptionTransientsMixed);
    set_sensitive_from_revision();
    
    m_Button_apply.set_label("Appliquer");
    m_Button_apply.set_hexpand();    
    m_vertical_box.append(m_Button_apply);
    m_Button_apply.signal_clicked().connect(sigc::mem_fun(*this, &RubberBandOptionsWindow::on_apply));
    
}
void RubberBandOptionsWindow::on_apply(){

    auto bits = get_rubberband_option_bits();

    // setTransientsOption()
    // setDetectorOption()
    // setPhaseOption()
    // setFormantOption()
    // setPitchOption()

}