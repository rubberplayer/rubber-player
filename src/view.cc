#include "./view.h"

double TimeRatioRange::get_value_for_display(double display_value)
{
    return std::pow(2, display_value);
}
std::string TimeRatioRange::get_value_string_for_label_display(double display_value)
{
    double value = get_value_for_display(display_value);
    if (value == 1)
        return "no change";
    else if (value > 1)
        return std::to_string((int)std::round(value));
    else
        return "1/" + std::to_string((int)std::round(1 / value));
}

double PitchScaleRange::get_value_for_display(double display_value)
{
    return std::pow(2, display_value / 12);
}
std::string PitchScaleRange::get_value_string_for_label_display(double display_value)
{
    return std::to_string((int)std::round(display_value));
}

void MainWindow::create_actions()
{
    auto refActions = Gio::SimpleActionGroup::create();
    refActions->add_action("preferences", sigc::mem_fun(*this, &MainWindow::on_preferences));
    refActions->add_action("about", sigc::mem_fun(*this, &MainWindow::on_about));
    refActions->add_action("quit", sigc::mem_fun(*this, &MainWindow::on_file_quit));
    refActions->add_action("open", sigc::mem_fun(*this, &MainWindow::on_button_open_clicked));
    refActions->add_action("toggle-selections-panel", sigc::mem_fun(*this, &MainWindow::toggle_selections_list_visibility));
    refActions->add_action("selections-list-add", sigc::mem_fun(*this, &MainWindow::on_button_selections_list_add));
    refActions->add_action("selections-list-remove-selected", sigc::mem_fun(*this, &MainWindow::on_button_selections_list_remove));
    refActions->add_action("selections-list-rename", sigc::mem_fun(*this, &MainWindow::on_button_selections_list_rename));
    refActions->add_action("toggle-play", sigc::mem_fun(*this, &MainWindow::on_button_play_clicked));
    refActions->add_action("toggle-pitch-scale", sigc::mem_fun(*this, &MainWindow::toggle_pitch_scale_visibility));

    insert_action_group("win", refActions);
}
std::shared_ptr<Gio::Menu> MainWindow::create_main_menu()
{

    // Create the menu:
    auto win_menu = Gio::Menu::create();

    // File menu:
    // auto menu_file = Gio::Menu::create();
    // win_menu->append_submenu("_File", menu_file);
    // auto file_section1 = Gio::Menu::create();
    // file_section1->append("_New", "example.new");
    // file_section1->append("_Open", "example.open");
    // file_section1->append("_Save", "example.save");
    // file_section1->append("Save _As...", "example.saveas");
    // menu_file->append_section(file_section1);
    // auto file_section2 = Gio::Menu::create();
    // file_section2->append("_Quit", "example.quit");
    // menu_file->append_section(file_section2);

    // Preferences menu:
    auto menu_pref = Gio::Menu::create();
    // win_menu->append_submenu("_Preferences", menu_pref);
    win_menu->append("_Preferences", "win.preferences");

    // Help menu:
    // auto menu_help = Gio::Menu::create();
    win_menu->append("_About", "win.about");

    win_menu->append("_Quit", "win.quit");

    // Add the menu to the menubar.
    // m_Menubar.set_menu_model(win_menu);
    // m_HeaderBar.pack_end(m_Menubar);
    //
    return win_menu;
}

MainWindow::MainWindow() : m_VBox0(Gtk::Orientation::VERTICAL, 8),
                           m_VBox_sound(Gtk::Orientation::VERTICAL, 8),
                           m_Button_play("play", true),
                           m_Button_open("open", true)

/*m_Button_load("play", true)*/
{
    set_title(APPLICATION_NAME);
    set_default_size(800, 400);
    printf("id:%d\n", get_id());

    // auto refActions = Gio::SimpleActionGroup::create();
    //  refActions->add_action("quit", sigc::mem_fun(*this, &Example_Builder::on_file_quit));
    //  refActions->add_action("about", sigc::mem_fun(*this, &Example_Builder::on_help_about));
    //  refActions->add_action("help", sigc::mem_fun(*this, &Example_Builder::on_help_help));
    // refActions->add_action("new", sigc::mem_fun(*this, &Example_Builder::on_not_implemented));
    // refActions->add_action("open", sigc::mem_fun(*this, &MainWindow::on_not_implemented));
    //  refActions->add_action("save", sigc::mem_fun(*this, &Example_Builder::on_not_implemented));
    //  refActions->add_action("save-as", sigc::mem_fun(*this, &Example_Builder::on_not_implemented));
    //  refActions->add_action("copy", sigc::mem_fun(*this, &Example_Builder::on_not_implemented));
    //  refActions->add_action("cut", sigc::mem_fun(*this, &Example_Builder::on_not_implemented));
    //  refActions->add_action("paste", sigc::mem_fun(*this, &Example_Builder::on_not_implemented));
    // insert_action_group("win", refActions);

    auto controller = Gtk::ShortcutController::create();
    controller->set_scope(Gtk::ShortcutScope::GLOBAL);
    add_controller(controller);

    //  controller->add_shortcut(Gtk::Shortcut::create(
    //    Gtk::KeyvalTrigger::create(GDK_KEY_n, Gdk::ModifierType::CONTROL_MASK),
    //    Gtk::NamedAction::create("win.new")));

    // https://developer.gnome.org/hig/reference/keyboard.html
    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(GDK_KEY_o, Gdk::ModifierType::CONTROL_MASK),
        Gtk::NamedAction::create("win.open")));

    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(GDK_KEY_b, Gdk::ModifierType::CONTROL_MASK),
        Gtk::NamedAction::create("win.toggle-selections-panel")));

    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(GDK_KEY_comma, Gdk::ModifierType::CONTROL_MASK),
        Gtk::NamedAction::create("win.preferences")));

    controller->add_shortcut(Gtk::Shortcut::create(
        Gtk::KeyvalTrigger::create(GDK_KEY_d, Gdk::ModifierType::CONTROL_MASK),
        Gtk::NamedAction::create("win.selections-list-add")));

    m_p_selection_db = new SelectionDB();
    m_p_selection_db->start();

    set_child(m_VBox0);

    create_actions();
    auto main_menu = create_main_menu();

    // open file button
    m_HeaderBar.pack_start(m_Button_open);
    m_Button_open.set_action_name("win.open");

    // m_HeaderBar.pack_start(m_PopoverMenuBar_open_recent);
    // m_PopoverMenuBar_open_recent.set_icon_name("open-menu-symbolic");
    // recent_manager = Gtk::RecentManager::get_default();
    // {
    //   auto recent_menu = Gio::Menu::create();
    //   m_PopoverMenuBar_open_recent.set_menu_model(recent_menu);
    //   for (auto recent_item_info : recent_manager->get_items())
    //   {
    //     recent_menu->append(recent_item_info->get_uri());
    //     for (auto application : recent_item_info->get_applications())
    //     {
    //     }
    //   }
    //
    // }
    // menu button
    // m_HeaderBar.pack_end(m_MenuButton);
    m_MenuButton.set_icon_name("open-menu-symbolic");
    m_MenuButton.set_menu_model(main_menu);

    m_HeaderBar.pack_end(m_ToggleButton_selections_shown);
    m_ToggleButton_selections_shown.set_label("show selections");
    m_ToggleButton_selections_shown.set_action_name("win.toggle-selections-panel");

    m_HeaderBar.pack_end(m_ToggleButton_pitch_scale_shown);
    m_ToggleButton_pitch_scale_shown.set_label("show pitch scale");
    m_ToggleButton_pitch_scale_shown.set_action_name("win.toggle-pitch-scale");

    // preferences button
    m_HeaderBar.pack_end(m_Button_preferences);
    m_Button_preferences.set_icon_name("gtk-preferences");
    m_Button_preferences.set_action_name("win.preferences");

    if (USE_HEADERBAR_TITLEBAR)
    {
        set_titlebar(m_HeaderBar);
        // menu button
    }
    else
    {
        // m_VBox0.append(m_HeaderBar);
        m_VBox0.append(m_HeaderBar);
        m_HeaderBar.set_show_title_buttons(false);
        Gtk::Box empty_box;
        m_HeaderBar.set_title_widget(empty_box);
    }

    m_Waveform.set_hack_sound_start_sound_end_sound_position(&player.m_sound_start, &player.m_sound_end, &player.m_sound_position);

    // left_part : selections
    m_VBox0.append(m_HBox1);
    m_HBox1.set_margin(8);
    m_HBox1.set_margin_top(0);

    m_HBox1.append(m_VBox_selections);
    m_VBox_selections.set_hexpand(false);
    m_VBox_selections.set_visible(false);
    m_VBox_selections.set_orientation(Gtk::Orientation::VERTICAL);

    m_VBox_selections.append(m_HBox_selection_buttons);
    m_HBox_selection_buttons.set_orientation(Gtk::Orientation::HORIZONTAL);
    m_HBox_selection_buttons.set_margin_bottom(5);

    m_HBox_selection_buttons.append(m_Label_selections_label);
    m_Label_selections_label.set_label("Selections");
    m_Label_selections_label.set_margin_end(20 * 4);
    m_Label_selections_label.set_hexpand();

    m_HBox_selection_buttons.append(m_Button_rename_selection);
    m_Button_rename_selection.set_icon_name("text-editor");
    m_Button_rename_selection.set_has_frame(false);
    m_Button_rename_selection.set_action_name("win.selections-list-rename");
    m_Button_rename_selection.set_visible(false);
    
    m_HBox_selection_buttons.append(m_Button_add_selection);
    m_Button_add_selection.set_icon_name("list-add");
    m_Button_add_selection.set_has_frame(false);
    m_Button_add_selection.set_action_name("win.selections-list-add");

    m_HBox_selection_buttons.append(m_Button_remove_selection);
    m_Button_remove_selection.set_icon_name("list-remove");
    m_Button_remove_selection.set_has_frame(false);
    m_Button_remove_selection.set_action_name("win.selections-list-remove-selected");

    m_VBox_selections.append(m_ScrolledWindow_selections);
    m_ScrolledWindow_selections.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_ScrolledWindow_selections.set_child(m_SelectionsListBox);
    m_SelectionsListBox.set_waveform(&m_Waveform);
    m_ScrolledWindow_selections.set_expand();

    m_HBox1.append(m_Separator);
    m_Separator.set_orientation(Gtk::Orientation::VERTICAL);
    m_Separator.set_margin(8);
    m_Separator.set_visible(false);

    // toggle_selections_list_visibility();

    // right part : sound + speed + play

    m_HBox1.append(m_VBox_sound);

    // vertical box container
    // m_VBox_sound.set_margin(16);
    m_VBox_sound.set_margin_start(8);

    // waveform frame and Waveform (DrawingArea)
    m_VBox_sound.append(m_Frame_Waveform);
    m_Waveform.set_content_width(180);
    m_Waveform.set_content_height(100);
    m_Waveform.set_expand();
    m_Frame_Waveform.set_child(m_Waveform);

    // m_HBox_time_ratio(, 8),
    {
        // time ratio row
        m_HBox_time_ratio.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_HBox_time_ratio.set_spacing(8);

        // time ratio label
        m_Label_time_ratio.set_text("Time Ratio");
        m_Label_time_ratio.set_valign(Gtk::Align::END);
        m_Label_time_ratio.set_margin_bottom(8);
        m_Label_time_ratio.set_margin_start(4);
        m_HBox_time_ratio.append(m_Label_time_ratio);

        // time ratio scale
        m_Scale_time_ratio.set_draw_value(false);
        m_Scale_time_ratio.set_range(m_time_ratio_range.display_min, m_time_ratio_range.display_max);
        m_Scale_time_ratio.set_value(0);
        on_time_ratio_value_changed();
        m_Scale_time_ratio.set_hexpand();
        m_Scale_time_ratio.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_time_ratio_value_changed));
        double marks[] = {1.0, -1.0, 2.0, 3.0, 0.0, 666.0};
        int i_mark = 0;
        for (;;)
        {
            if (marks[i_mark] == 666.0)
                break;
            m_Scale_time_ratio.add_mark(marks[i_mark], Gtk::PositionType::TOP, m_time_ratio_range.get_value_string_for_label_display(marks[i_mark]));
            i_mark++;
        }
        m_HBox_time_ratio.append(m_Scale_time_ratio);

        // time ratio image
        m_Label_time_ratio_img.set_text("🐢");
        m_Label_time_ratio_img.set_valign(Gtk::Align::END);
        m_Label_time_ratio_img.set_margin_bottom(8);
        m_Label_time_ratio_img.set_margin_start(4);
        m_HBox_time_ratio.append(m_Label_time_ratio_img);

        // time ratio entry
        m_Entry_time_ratio.set_valign(Gtk::Align::END);
        m_HBox_time_ratio.append(m_Entry_time_ratio);
        m_VBox_sound.append(m_HBox_time_ratio);
    }
    {
        // pitch_scale row
        m_HBox_pitch_scale.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_HBox_pitch_scale.set_spacing(8);
        m_HBox_pitch_scale.set_visible(false);

        // pitch_scale label
        m_Label_pitch_scale.set_text("Pitch Scale");
        m_Label_pitch_scale.set_valign(Gtk::Align::END);
        m_Label_pitch_scale.set_margin_bottom(8);
        m_Label_pitch_scale.set_margin_start(4);
        m_HBox_pitch_scale.append(m_Label_pitch_scale);

        // pitch_scale scale
        m_Scale_pitch_scale.set_draw_value(false);
        m_Scale_pitch_scale.set_range(m_pitch_scale_range.display_min, m_pitch_scale_range.display_max);
        m_Scale_pitch_scale.set_value(0);
        on_pitch_scale_value_changed();
        m_Scale_pitch_scale.set_hexpand();
        m_Scale_pitch_scale.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_pitch_scale_value_changed));
        /// double marks[] = {1.0, -1.0, 2.0, 3.0, 0.0, 666.0};
        std::vector<double> marks = {-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7};
        int i_mark = 0;
        for (double mark : marks)
        {
            m_Scale_pitch_scale.add_mark(mark, Gtk::PositionType::TOP, m_pitch_scale_range.get_value_string_for_label_display(mark));
            i_mark++;
        }
        m_HBox_pitch_scale.append(m_Scale_pitch_scale);

        // pitch_scale image
        m_Label_pitch_scale_img.set_text("⑂"); // 🎵
        m_Label_pitch_scale_img.set_valign(Gtk::Align::END);
        m_Label_pitch_scale_img.set_margin_bottom(8);
        m_Label_pitch_scale_img.set_margin_start(4);
        m_HBox_pitch_scale.append(m_Label_pitch_scale_img);

        // pitch_scale entry
        m_Entry_pitch_scale.set_valign(Gtk::Align::END);
        m_HBox_pitch_scale.append(m_Entry_pitch_scale);
        m_VBox_sound.append(m_HBox_pitch_scale);
    }

    // play button
    m_VBox_sound.append(m_Button_play);
    m_Button_play.set_action_name("win.toggle-play");

    // open file dialog
    m_Dialog_open_audio_file = Gtk::FileChooserNative::create("Please choose an audio file",
                                                              *this,
                                                              Gtk::FileChooser::Action::OPEN,
                                                              "Choose",
                                                              "Cancel");
    m_Dialog_open_audio_file->set_transient_for(*this);
    m_Dialog_open_audio_file->set_modal(true);
    m_Dialog_open_audio_file->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::on_open_audio_file_dialog_response), m_Dialog_open_audio_file));

    const GType ustring_type = Glib::Value<Glib::ustring>::value_type();
    auto target = Gtk::DropTarget::create(ustring_type, Gdk::DragAction::COPY);
    target->signal_drop().connect(sigc::mem_fun(*this, &MainWindow::on_button_drop_drop_data), false);
    add_controller(target);

    // auto gtk_builder = new Gtk::Builder();
    // std::string ui_file_path("/home/vivien/src/test-cambalache/test cambalache.ui");
    // Glib::RefPtr<Gtk::Builder> gtk_builder = Gtk::Builder::create_from_file(ui_file_path);
}
void MainWindow::toggle_pitch_scale_visibility()
{
    bool visible = m_HBox_pitch_scale.is_visible();
    m_ToggleButton_pitch_scale_shown.set_active(!visible);
    m_HBox_pitch_scale.set_visible(!visible);
}
void MainWindow::toggle_selections_list_visibility()
{
    bool visible = m_VBox_selections.is_visible();
    // bool visible = m_ToggleButton_selections_shown.get_active();
    m_ToggleButton_selections_shown.set_active(!visible);
    m_VBox_selections.set_visible(!visible);
    m_Separator.set_visible(!visible);
}
void MainWindow::on_button_selections_list_rename()
{
    auto s = m_SelectionsListBox.rename_selected();
    printf("rename\n");
}
void MainWindow::on_button_selections_list_remove()
{
    auto s = m_SelectionsListBox.remove_selected();
    if (s != NULL)
    {
        m_p_selection_db->remove_selection(sound.path, s->m_selection_start_frame, s->m_selection_end_frame);
    }
}
void MainWindow::on_button_selections_list_add()
{
    if (!sound.is_loaded())
        return;

    long selection_start_frame = m_Waveform.get_selection_start_frame();
    long selection_end_frame = m_Waveform.get_selection_end_frame();
    std::string label = m_SelectionsListBox.add_context(selection_start_frame, selection_end_frame, &sound);
    m_p_selection_db->insert_selection(sound.path, selection_start_frame, selection_end_frame, label);
}

void MainWindow::on_time_ratio_value_changed()
{
    double value = m_time_ratio_range.get_value_for_display(m_Scale_time_ratio.get_value());
    m_Entry_time_ratio.get_buffer()->set_text(std::to_string(value));
    player.set_time_ratio(value);
}
void MainWindow::on_pitch_scale_value_changed()
{
    double value = m_pitch_scale_range.get_value_for_display(m_Scale_pitch_scale.get_value());
    m_Entry_pitch_scale.get_buffer()->set_text(std::to_string(value));
    player.set_pitch_scale(value);
}

void MainWindow::on_button_play_clicked()
{
    bool active = player.is_playing();
    // bool active = m_Button_play.get_active();
    if (active)
    {
        m_Button_play.set_active(false);
        player.stop_playing();
    }
    else
    {
        m_Button_play.set_active(true);
        player.start_playing();
    }
}

void MainWindow::load_sound(std::string path)
{
    m_Button_play.set_active(false);
    m_Waveform.set_sound(NULL);
    player.stop_playing_thread();
    sound.load(path);
    if (sound.is_loaded())
    {
        m_Waveform.set_sound(&sound);
        player.set_sound(&sound);
        m_SelectionsListBox.reset();
        auto rows = m_p_selection_db->load_sound_selections(path);
        for (auto row : (*rows))
        {
            std::string path = std::get<0>(row);
            long selection_start_frame = std::get<1>(row);
            long selection_end_frame = std::get<2>(row);
            std::string label = std::get<3>(row);
            m_SelectionsListBox.add_context(selection_start_frame, selection_end_frame, label);
        }
    }
    else
    {
        show_message_dialog("Error while loading", sound.get_error_string());
    }
}
/*void MainWindow::on_activate(Gio::Application app){

}*/
void MainWindow::show_message_dialog(std::string m1, std::string m2)
{
    Gtk::Window *parent = dynamic_cast<Gtk::Window *>(this);
    m_pMessageDialog = new Gtk::MessageDialog(*parent, std::string("a"), true, Gtk::MessageType::ERROR, Gtk::ButtonsType::NONE, true);
    m_pMessageDialog->set_message(std::string("<b>") + m1 + "</b>", true); // "⚠"
    m_pMessageDialog->set_secondary_text(m2, true);
    m_pMessageDialog->set_title("Problem !");
    m_pMessageDialog->show();
}

void MainWindow::on_button_open_clicked()
{
    m_Dialog_open_audio_file->show();
}
std::vector<std::string> text_fileurls_list_to_path_list(std::string maybe_file_lines_list)
{
    using namespace std;
    std::vector<std::string> path_list;
    regex exp("file://(.+)[\\n\\r]*");
    smatch res;
    string str = maybe_file_lines_list;

    while (regex_search(str, res, exp))
    {
        path_list.push_back(res[1]);
        str = res.suffix();
    }
    return path_list;
}
bool MainWindow::on_button_drop_drop_data(const Glib::ValueBase &value, double, double)
{
    if (G_VALUE_HOLDS(value.gobj(), Glib::Value<Glib::ustring>::value_type()))
    {
        // We got the value type that we expected.
        Glib::Value<Glib::ustring> ustring_value;
        ustring_value.init(value.gobj());
        const Glib::ustring dropped_string = ustring_value.get();
        std::string maybe_file_lines_list(dropped_string);
        auto path_list = text_fileurls_list_to_path_list(maybe_file_lines_list);
        if (path_list.size() > 0)
        {
            auto first = path_list[0];
            auto unescaped = Glib::uri_unescape_string(first);
            load_sound(unescaped);
            return true;
        }
    }
    else
    {
        std::cerr << "Received unexpected data type \"" << G_VALUE_TYPE_NAME(value.gobj()) << "\" in button " << std::endl;
    }
    return false;
}
void MainWindow::on_open_audio_file_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative> &dialog)
{
    Glib::RefPtr<Gio::File> filename = m_Dialog_open_audio_file->get_file();
    if (filename == NULL)
    {
    }
    else
    {
        std::string path = filename->get_path();
        load_sound(path);
    }
    m_Dialog_open_audio_file->hide();
}
void MainWindow::on_not_implemented()
{
    std::cout << "The selected menu item is not implemented." << std::endl;
}
void MainWindow::on_preferences()
{
    rubberband_options_window = new RubberBandOptionsWindow;
    rubberband_options_window->set_from_rubberband_option_bits(player.get_rubberband_flag_options());
    rubberband_options_window->show();
    rubberband_options_window->set_player(&player);
}
void MainWindow::on_file_quit()
{
    std::cout << "TODO file quit..." << std::endl;
}
void MainWindow::on_about()
{
    std::cout << "TODO about..." << std::endl;
}
//
// MainWindow *MainWindow::create()
// {
//     printf("-*-*-*-*-* create\n");
//     auto window = new MainWindow();
//     return window;
// }
//
// void MainWindow::open_file_view(const Glib::RefPtr<Gio::File> &file)
// {
//     printf("-*-*-*-*-* open file view\n");
//     const Glib::ustring basename = file->get_basename();
//     const Glib::ustring path = file->get_path();
//     printf("-*-*-*-*-* open file view %s :: %s \n",path.c_str(),basename.c_str());
//     load_sound(path);
//}

// #include <json/json.h>
// #include <json/value.h>
// #include <fstream>
// void json_test()
// {
//   std::ifstream people_file("./people.json", std::ifstream::binary);
//   Json::Value people;
//   people_file >> people;
//
//   std::cout << people << std::endl; // This will print the entire json object.
//
//   const Json::Value selections = people["selections"];
//   std::cout << "size:" << selections.size() << std::endl;
//   for (unsigned int index = 0; index < selections.size(); ++index)
//   {
//     std::cout << selections[index]["frame_start"].asLargestInt() << std::endl;
//     std::cout << selections[index]["frame_end"].asLargestInt() << std::endl;
//   }
// }
//
// void sqlite_test()
// {
//   SelectionDB *selectionDb = new SelectionDB();
//   selectionDb->start();
//   bool done = selectionDb->insert_selection("/path/to/sound3.wav", 26666, 29999, "xa comment");
//   std::cout << "done" << done << std::endl;
//   auto rows = selectionDb->load_sound_selections("/path/to/sound3.wav");
//
//   for (auto row : (*rows))
//   {
//     std::string path = std::get<0>(row);
//     long frame_start = std::get<1>(row);
//     long frame_end = std::get<2>(row);
//     std::string label = std::get<3>(row);
//     std::cout << path << " "
//               << frame_start << " "
//               << frame_end << " "
//               << label << " "
//               << std::endl;
//   }
// }
// int main(int argc, char *argv[])
// {
//   // json_test();
//   //  sqlite_test();
//   auto app = Gtk::Application::create("org.guimbarde.rubber-player", Gio::Application::Flags::HANDLES_OPEN | Gio::Application::Flags::NON_UNIQUE);
//   // auto app = Gtk::Application::create("org.guimbarde.rubber-player");
//   return app->make_window_and_run<MainWindow>(argc, argv);
// }
//