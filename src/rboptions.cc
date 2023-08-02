#include "./rboptions.h"

// https://stackoverflow.com/questions/15441157/gtkmm-multiple-windows-popup-window
// https://rafaelmardojai.pages.gitlab.gnome.org/pygobject-guide/gtk4/controls/dropdown.html

RubberBandOptionsWindow::RubberBandOption::RubberBandOption(std::string name, std::vector<std::string> values, std::vector<std::string> engines, std::vector<std::string> needs_restart)
{
    m_name = name;
    m_values = values;
    m_engines = engines;
    m_needs_restart = needs_restart;
}

void RubberBandOptionsWindow::selected_item_changed(const Gtk::DropDown *dropdown, RubberBandOption *rubber_band_option)
{
    auto name = rubber_band_option->m_name;
    int selected = dropdown->get_selected();
    auto selected_value = rubber_band_option->m_values[selected];

    printf("changed %s to %s\n", name.c_str(), selected_value.c_str());
}
RubberBandOptionsWindow::RubberBandOptionsWindow()
    : m_vertical_box(Gtk::Orientation::VERTICAL, 8)
{

    options = {
        new RubberBandOption("Engine", {"Faster", "Finer"}, {"R2", "R3"}, {"R2", "R3"}),
        new RubberBandOption("Transients", {"Crisp", "Mixed", "Smooth"}, {"R2"}, {}),
        new RubberBandOption("Detector", {"Compound", "Percussive", "Soft"}, {"R2"}, {}),
        new RubberBandOption("Phase", {"Laminar", "Independent"}, {"R2"}, {}),
        new RubberBandOption("Threading", {"Auto", "Never", "Always"}, {"R2", "R3"}, {"R2", "R3"}), // maybe available later for R3
        new RubberBandOption("Window", {"Standard", "Short", "Long"}, {"R2", "R3"}, {"R2", "R3"}),
        new RubberBandOption("Smoothing", {"Off", "On"}, {"R2"}, {"R2", "R3"}),
        new RubberBandOption("Formant", {"Shifted", "Preserved"}, {"R2", "R3"}, {}),
        new RubberBandOption("Pitch", {"HighSpeed", "HighQuality", "HighConsistency"}, {"R2", "R3"}, {"R3"}),
        new RubberBandOption("Channels", {"Apart", "Together"}, {"R2", "R3"}, {"R2", "R3"}),
    };

    // std::map<RubberBandOption, Gtk::DropDown> drop_downs;
    // std::list<Gtk::DropDown&> drop_downs;

    this->set_default_size(220, 100);
    this->set_title("Stretching");
    this->set_resizable(false);
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
        // option_dropdown.set_sensitive(false);
        option_dropdown->set_halign(Gtk::Align::END);

        // values
        std::vector<Glib::ustring> strings;
        for (auto value : p_option->m_values)
        {
            strings.push_back(value);
        }
        auto model = Gtk::StringList::create(strings);
        option_dropdown->set_model(model);
        option_horizontal_box.append(*option_dropdown);
        option_dropdown->property_selected().signal_changed().connect(
            sigc::bind(sigc::mem_fun(*this, &RubberBandOptionsWindow::selected_item_changed), option_dropdown, p_option));
    }

    auto option_validate_button = Gtk::Button();
    option_validate_button.set_label("Appliquer");
    option_validate_button.set_hexpand();
    m_vertical_box.append(option_validate_button);
}