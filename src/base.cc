#include <gtkmm.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <regex>
#include "./name.h"
#include "./player.h"
#include "./sound.h"
#include "./waveform.h"
#include "./rboptions.h"
#include "./selections.cc"

#define USE_HEADERBAR_TITLEBAR false

/////////////////////////////

//////////////////////////////////
///
/// https://docs.gtk.org/gtk4/class.FileDialog.html since 4.10
///
// char** GLOBAL_ARGV;

class TimeRatioRange
{
public:
  double display_min = -2;
  double display_max = 4;
  double get_value_for_display(double display_value)
  {
    return std::pow(2, display_value);
  }
  std::string get_value_string_for_label_display(double display_value)
  {
    double value = get_value_for_display(display_value);
    if (value == 1)
      return "unchanged";
    else if (value > 1)
      return std::to_string((int)std::round(value));
    else
      return "1/" + std::to_string((int)std::round(1 / value));
  }
};

class PitchRange
{
public:
  double display_min = -4;
  double display_max = 2;
  double get_value_for_display(double display_value)
  {
    return std::pow(2, display_value);
  }
  std::string get_value_string_for_label_display(double display_value)
  {
    double value = get_value_for_display(display_value);
    if (value == 1)
      return "no change";
    else if (value > 1)
      return std::to_string((int)std::round(value));
    else
      return "1/" + std::to_string((int)std::round(1 / value));
  }
};

class MainWindow : public Gtk::Window
{
public:
  MainWindow();

  // headerbar
  Gtk::HeaderBar m_HeaderBar;
  Gtk::MenuButton m_MenuButton;
  Gtk::Button m_Button_preferences;

  void create_actions();
  std::shared_ptr<Gio::Menu> create_main_menu();

  Gtk::Box m_VBox0;

  Gtk::Box m_HBox1;

  Gtk::ToggleButton m_ToggleButton_selections_shown;
  void toggle_selections_list_visibility();

  Gtk::Box m_VBox_selections;
  Gtk::ScrolledWindow m_ScrolledWindow_selections;
  Gtk::Box m_HBox_selection_buttons;
  SelectionsListBox m_SelectionsListBox;
  Gtk::Label m_Label_selections_label;
  Gtk::Button m_Button_add_selection;
  void on_button_selections_list_add();
  Gtk::Button m_Button_remove_selection;
  void on_button_selections_list_remove();

  Gtk::Separator m_Separator;

  Gtk::Box m_VBox_sound;
  Gtk::Frame m_Frame_Waveform;
  Waveform m_Waveform;

  Gtk::ToggleButton m_Button_play;
  void on_button_play_clicked();

  Gtk::Button m_Button_open;
  void on_button_open_clicked();
  // Gtk::MenuButton m_PopoverMenuBar_open_recent;

  Gtk::Box m_HBox_time_ratio;
  TimeRatioRange m_time_ratio_range;
  Gtk::Label m_Label_time_ratio;
  Gtk::Scale m_Scale_time_ratio;
  Gtk::Label m_Label_time_ratio_img;
  Gtk::Entry m_Entry_time_ratio;
  void on_time_ratio_value_changed();

  // Gtk::Scale::SlotFormatValue m_Scale_time_ratio_format_value;
  // char* m_Scale_time_ratio_format_value(double value);
  /*
    PitchRange m_pitch_range;
    Gtk::Label m_Label_pitch;
    Gtk::Scale m_Scale_pitch;
    void on_pitch_value_changed();
    Gtk::Entry m_Entry_pitch;
  */
  // std::shared_ptr<Gtk::RecentManager> recent_manager;
  Player player;
  Sound sound;

  // Gtk::FileDialog m_FileDialog_open_audio_file;

  Glib::RefPtr<Gtk::FileChooserNative> m_Dialog_open_audio_file;
  void on_open_audio_file_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative> &m_Dialog_open_audio_file);

  Glib::RefPtr<Gtk::EventControllerKey> m_Keypressed;
  // void on_key_pressed(guint keyval, int keycode, Gdk::ModifierType state);
  // bool on_key_pressed(const Gdk::Event* event);
  bool on_key_pressed(const unsigned int a, const unsigned int b, const Gdk::ModifierType c);
  /*
    long m_selection_start;
    long m_selection_end;
    void set_selection_bounds(long selection_start, long selection_end);
  */
  void load_sound(std::string path);

  bool on_button_drop_drop_data(const Glib::ValueBase &value, double, double);

  // message dialog;
  /*
   explicit MessageDialog(const Glib::ustring& message,
   bool use_markup = false,
   MessageType type = MessageType::INFO,
   ButtonsType buttons = ButtonsType::OK,
    bool modal = false);
   */
  // Glib::ustring message = "error";
  Gtk::MessageDialog *m_pMessageDialog;

  void show_message_dialog(std::string m1, std::string m2);
  RubberBandOptionsWindow *rubberband_options_window;

  void on_preferences();
  void on_file_quit();
  void on_about();
};
/*
void MainWindow::set_selection_bounds(long selection_start, long selection_end)
{
  m_selection_start = selection_start;
  m_selection_end = selection_end;
}
*/
void MainWindow::on_preferences()
{
  std::cout << "preferences..." << std::endl;
  rubberband_options_window = new RubberBandOptionsWindow;
  rubberband_options_window->set_from_rubberband_option_bits(player.get_rubberband_flag_options());
  rubberband_options_window->show();
  rubberband_options_window->set_player(&player);
}
void MainWindow::on_file_quit()
{
  std::cout << "file quit..." << std::endl;
}
void MainWindow::on_about()
{
  std::cout << "about..." << std::endl;
}
void MainWindow::create_actions()
{
  auto refActions = Gio::SimpleActionGroup::create();
  refActions->add_action("preferences", sigc::mem_fun(*this, &MainWindow::on_preferences));
  refActions->add_action("about", sigc::mem_fun(*this, &MainWindow::on_about));
  refActions->add_action("quit", sigc::mem_fun(*this, &MainWindow::on_file_quit));
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

  /*
    // Color submenu:
    auto submenu_color = Gio::Menu::create();
    submenu_color->append("_Red", "example.red");
    submenu_color->append("_Green", "example.green");
    submenu_color->append("_Blue", "example.blue");
    menu_pref->append_submenu("_Color", submenu_color);

    // Shape submenu:
    auto submenu_shape = Gio::Menu::create();
    submenu_shape->append("_Square", "example.square");
    submenu_shape->append("_Rectangle", "example.rectangle");
    submenu_shape->append("_Circle", "example.circle");
    menu_pref->append_submenu("_Shape", submenu_shape);
  */
  // Help menu:
  // auto menu_help = Gio::Menu::create();
  win_menu->append("_About", "win.about");

  win_menu->append("_Quit", "win.quit");

  // win_menu->append_submenu("_Help", menu_help);

  // Add the menu to the menubar.
  // m_Menubar.set_menu_model(win_menu);
  // m_HeaderBar.pack_end(m_Menubar);
  //
  return win_menu;
}

MainWindow::MainWindow() : m_VBox0(Gtk::Orientation::VERTICAL, 8),
                           m_VBox_sound(Gtk::Orientation::VERTICAL, 8),
                           // m_HBox_time_ratio(Gtk::Orientation::HORIZONTAL, 8),
                           m_Button_play("play", true),
                           m_Button_open("open", true)

/*m_Button_load("play", true)*/
{
  set_title(APPLICATION_NAME);
  set_default_size(800, 400);

  set_child(m_VBox0);

  create_actions();
  auto main_menu = create_main_menu();

  // open file button
  m_HeaderBar.pack_start(m_Button_open);

  // m_HeaderBar.pack_start(m_PopoverMenuBar_open_recent);
  // m_PopoverMenuBar_open_recent.set_icon_name("open-menu-symbolic");
  // recent_manager = Gtk::RecentManager::get_default();
  // {
  //   auto recent_menu = Gio::Menu::create();
  //   m_PopoverMenuBar_open_recent.set_menu_model(recent_menu);
  //   for (auto recent_item_info : recent_manager->get_items())
  //   {
  //     std::cout << recent_item_info->get_uri() << std::endl;
  //     recent_menu->append(recent_item_info->get_uri());
  //     for (auto application : recent_item_info->get_applications())
  //     {
  //       std::cout << application << std::endl;
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
  m_ToggleButton_selections_shown.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::toggle_selections_list_visibility));

  // preferences button
  m_HeaderBar.pack_end(m_Button_preferences);
  m_Button_preferences.set_icon_name("gtk-preferences");
  m_Button_preferences.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_preferences));

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

  m_VBox_selections.set_orientation(Gtk::Orientation::VERTICAL);

  m_VBox_selections.append(m_HBox_selection_buttons);
  m_HBox_selection_buttons.set_orientation(Gtk::Orientation::HORIZONTAL);
  m_HBox_selection_buttons.set_margin_bottom(5);

  m_HBox_selection_buttons.append(m_Label_selections_label);
  m_Label_selections_label.set_label("Selections");
  m_Label_selections_label.set_margin_end(20 * 4);
  m_Label_selections_label.set_hexpand();

  m_HBox_selection_buttons.append(m_Button_add_selection);
  m_Button_add_selection.set_icon_name("list-add");
  m_Button_add_selection.set_has_frame(false);
  m_Button_add_selection.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_selections_list_add));

  m_HBox_selection_buttons.append(m_Button_remove_selection);
  m_Button_remove_selection.set_icon_name("list-remove");
  m_Button_remove_selection.set_has_frame(false);
  m_Button_remove_selection.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_selections_list_remove));

  m_VBox_selections.append(m_ScrolledWindow_selections);
  m_ScrolledWindow_selections.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  m_ScrolledWindow_selections.set_child(m_SelectionsListBox);
  m_SelectionsListBox.set_waveform(&m_Waveform);
  m_ScrolledWindow_selections.set_expand();

  m_HBox1.append(m_Separator);
  m_Separator.set_orientation(Gtk::Orientation::VERTICAL);
  m_Separator.set_margin(8);

  toggle_selections_list_visibility();

  // right part : sound + speed + play

  m_HBox1.append(m_VBox_sound);

  // vertical box container
  // m_VBox_sound.set_margin(16);
  m_VBox_sound.set_margin_start(8);

  // waveform frame and Waveform (DrawingArea)
  m_VBox_sound.append(m_Frame_Waveform);
  m_Waveform.set_content_width(140);
  m_Waveform.set_content_height(100);
  m_Waveform.set_expand();
  m_Frame_Waveform.set_child(m_Waveform);

  // m_HBox_time_ratio(, 8),
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

  // play button
  m_VBox_sound.append(m_Button_play);
  m_Button_play.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_play_clicked));

  m_Button_open.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_open_clicked));
  // m_Button_load.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_load_clicked));

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
void MainWindow::toggle_selections_list_visibility()
{
  bool visible = m_ToggleButton_selections_shown.get_active();
  m_VBox_selections.set_visible(visible);
  m_Separator.set_visible(visible);
}
void MainWindow::on_time_ratio_value_changed()
{
  double value = m_time_ratio_range.get_value_for_display(m_Scale_time_ratio.get_value());
  m_Entry_time_ratio.get_buffer()->set_text(std::to_string(value));
  player.set_time_ratio(value);
}
void MainWindow::on_button_selections_list_remove()
{
  m_SelectionsListBox.remove_selected();
}
void MainWindow::on_button_selections_list_add()
{
  if (!sound.is_loaded())
    return;

  long selection_start_frame = m_Waveform.get_selection_start_frame();
  long selection_end_frame = m_Waveform.get_selection_end_frame();

  long selection_left_frame = std::min(selection_start_frame, selection_end_frame);
  long selection_right_frame = std::max(selection_start_frame, selection_end_frame);

  double selection_left_seconds = sound.get_second_at_frame(selection_left_frame);
  double selection_right_seconds = sound.get_second_at_frame(selection_right_frame);
  double selection_duration_seconds = sound.get_second_at_frame(selection_right_frame - selection_left_frame);

  auto left_time_code = m_Waveform.regular_timecode_display(selection_left_seconds);
  auto right_time_code = m_Waveform.regular_timecode_display(selection_right_seconds);
  auto duration_time_code = m_Waveform.regular_timecode_display(selection_duration_seconds);

  m_SelectionsListBox.add_context(left_time_code, right_time_code, duration_time_code, selection_start_frame, selection_end_frame);
}

void MainWindow::on_button_play_clicked()
{
  bool active = m_Button_play.get_active();
  printf("on_button_play_clicked clicked : active : %b\n", active);
  if (active)
  {
    player.start_playing();
  }
  else
  {
    player.stop_playing();
  }
}

void MainWindow::load_sound(std::string path)
{
  m_Button_play.set_active(false);
  m_Waveform.set_sound(NULL);
  player.stop_playing_thread();
  printf("load sound a filename path %s\n", path.c_str());
  // sound.load("/home/vivien/Bureau/redhouse-clip-mono.flac");
  sound.load(path);
  if (sound.is_loaded())
  {
    m_Waveform.set_sound(&sound);
    player.set_sound(&sound);
  }
  else
  {
    show_message_dialog("Error while loading", sound.get_error_string());
  }
}

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
    std::cout << "Received unexpected data type \"" << G_VALUE_TYPE_NAME(value.gobj()) << "\" in button " << std::endl;
  }
  return false;
}
void MainWindow::on_open_audio_file_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative> &dialog)
{
  Glib::RefPtr<Gio::File> filename = m_Dialog_open_audio_file->get_file();
  if (filename == NULL)
  {
    printf("nothing selected\n");
  }
  else
  {
    std::string path = filename->get_path();
    load_sound(path);
  }
  m_Dialog_open_audio_file->hide();
}

#include <json/json.h>
#include <json/value.h>
#include <fstream>
void json_test()
{
  std::ifstream people_file("./people.json", std::ifstream::binary);
  Json::Value people;
  people_file >> people;

  std::cout << people << std::endl; // This will print the entire json object.

  const Json::Value selections = people["selections"];
  std::cout << "size:" << selections.size() << std::endl;
  for (unsigned int index = 0; index < selections.size(); ++index)
  {
    std::cout << selections[index]["frame_start"].asLargestInt() << std::endl;
    std::cout << selections[index]["frame_end"].asLargestInt() << std::endl;
  }
}

#include "./db.h"

void sqlite_test()
{
  SelectionDB *selectionDb = new SelectionDB();
  selectionDb->open_database();
  selectionDb->create_tables();
  bool done = selectionDb->insert_selection("/path/to/sound3.wav", 26666, 29999, "xa comment");
  std::cout << "done" << done << std::endl;
  auto rows = selectionDb->load_sound_selections("/path/to/sound3.wav");

  for (auto row : (*rows))
  {
    std::string path = std::get<0>(row);
    long frame_start = std::get<1>(row);
    long frame_end = std::get<2>(row);
    std::string label = std::get<3>(row);
    std::cout << path << " "
              << frame_start << " " 
              << frame_end << " " 
              << label << " " 
              << std::endl;
  }
}
int main(int argc, char *argv[])
{
  // json_test();
  sqlite_test();
  auto app = Gtk::Application::create(); //"org.gtkmm.examples.base");
  return app->make_window_and_run<MainWindow>(argc, argv);
}
