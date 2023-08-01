#include <gtkmm.h>

class RubberBandOptionsWindow : public Gtk::Window
{
public:
    RubberBandOptionsWindow();
    // void selected_item_changed(const Gtk::DropDown* dropdown, const Glib::RefPtr<Gtk::ListItem>& item);
protected:
    Gtk::Box m_vertical_box;
    class RubberBandOption
    {        
    public:
        RubberBandOption(std::string m_name, std::vector<std::string> values, std::vector<std::string> engines);
        std::string m_name;
        std::vector<std::string> m_values;
        std::vector<std::string> m_engines;
    };
    std::vector<RubberBandOption> options;
};
