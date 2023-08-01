#include "./rboptions.h"

// https://stackoverflow.com/questions/15441157/gtkmm-multiple-windows-popup-window
// https://rafaelmardojai.pages.gitlab.gnome.org/pygobject-guide/gtk4/controls/dropdown.html

RubberBandOptionsWindow::RubberBandOption::RubberBandOption(std::string name, std::vector<std::string> values, std::vector<std::string> engines)
{
    m_name = name;
    m_values = values;
    m_engines = engines;
}
RubberBandOptionsWindow::RubberBandOptionsWindow()
    : m_vertical_box(Gtk::Orientation::VERTICAL, 8)
{

    std::vector<RubberBandOption> optionss = {
        RubberBandOption("Engine", {"Faster", "Finer"}, {"R2", "R3"}),
        RubberBandOption("Transients", {"Crisp", "Mixed", "Smooth"}, {"R2"}),
        RubberBandOption("Detector", {"Compound", "Percussive", "Soft"}, {"R2"}),
        RubberBandOption("Phase", {"Laminar", "Independent"}, {"R2"}),
        RubberBandOption("Threading", {"Auto", "Never", "Always"}, {"R2", "R3"}), // later for R3
        RubberBandOption("Window", {"Standard", "Short", "Long"}, {"R2", "R3"}),
        RubberBandOption("Smoothing", {"Off", "On"}, {"R2"}),
        RubberBandOption("Formant", {"Shifted", "Preserved"}, {"R2", "R3"}),
        RubberBandOption("Pitch", {"HighSpeed", "HighQuality", "HighConsistency"}, {"R2", "R3"}),
        RubberBandOption("Channels", {"Apart", "Together"}, {"R2", "R3"}),
    };

    //std::map<RubberBandOption, Gtk::DropDown> drop_downs;
    //std::list<Gtk::DropDown&> drop_downs;

    this->set_default_size(220, 100);
    this->set_title("Stretching");
    this->set_resizable(false);
    set_child(m_vertical_box);
    m_vertical_box.set_margin_start(10);
    m_vertical_box.set_margin_end(10);
    m_vertical_box.set_margin_top(10);
    m_vertical_box.set_margin_bottom(10);

    for (auto option : optionss)
    {
        // container
        auto option_horizontal_box = Gtk::Box();
        option_horizontal_box.set_hexpand();
        m_vertical_box.append(option_horizontal_box);

        // label
        auto option_label = Gtk::Label();
        option_label.set_text(option.m_name);
        option_label.set_halign(Gtk::Align::START);
        option_label.set_hexpand();
        option_label.set_margin_end(10);
        option_horizontal_box.append(option_label);

        // drop down
        auto option_dropdown = Gtk::DropDown();
        // option_dropdown.set_sensitive(false);
        option_dropdown.set_halign(Gtk::Align::END);

        // values
        std::vector<Glib::ustring> strings;
        for (auto value : option.m_values)
        {
            strings.push_back(value);
        }
        auto model = Gtk::StringList::create(strings);
        option_dropdown.set_model(model);
        option_horizontal_box.append(option_dropdown);

     //   drop_downs.emplace(std::pair{option,option_dropdown});
    
        //drop_downs[option] = option_dropdown; 
        //drop_downs.push_back(option_dropdown);
    }

    auto option_validate_button = Gtk::Button();
    option_validate_button.set_label("Appliquer");
    option_validate_button.set_hexpand();
    m_vertical_box.append(option_validate_button);

    printf("-----\n");
}

// void RubberBandOptionsWindow::selected_item_changed(const Gtk::DropDown *dropdown)
// {
//     printf("pas rendu\n");
// }

//         //option_dropdown.set_selected(1);
//         //printf("option_dropdown sele %d\n",option_dropdown.get_selected());
// /*        auto connection = option_dropdown.property_selected_item().signal_changed().connect(
//             sigc::bind(
//                 sigc::mem_fun(
//                     *this,
//                     &RubberBandOptionsWindow::selected_item_changed)));
//
//
//         */