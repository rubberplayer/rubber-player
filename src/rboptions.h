#include <rubberband/RubberBandStretcher.h>
#include <gtkmm.h>

class RubberBandOptionsWindow : public Gtk::Window
{
protected:
    Gtk::Box m_vertical_box;
    class RubberBandOptionValue
    {
    public:
        RubberBandOptionValue(std::string name, RubberBand::RubberBandStretcher::Option rubberband_option_flag);
        std::string m_name;
        RubberBand::RubberBandStretcher::Option m_rubberband_option_flag;
    };
    class RubberBandOption
    {
    public:
        RubberBandOption(std::string m_name, std::vector<RubberBandOptionValue> values, std::vector<std::string> engines, std::vector<std::string> needs_restart);
        std::string m_name;
        std::vector<RubberBandOptionValue> m_values;
        std::vector<std::string> m_engines;
        std::vector<std::string> m_needs_restart;
        Gtk::DropDown *widget = NULL;
    };
    std::vector<RubberBandOption *> options;

public:
    RubberBandOptionsWindow();
    void selected_item_changed(const Gtk::DropDown *dropdown, RubberBandOption *rubber_band_option);
    void set_sensitive_from_revision();
    void set_from_rubberband_option_bits(RubberBand::RubberBandStretcher::Options bits);
};
