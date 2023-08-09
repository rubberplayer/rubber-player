#ifndef VIEW_H
#define VIEW_H

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
#include "./selections.h"
#include "./db.h"
#define USE_HEADERBAR_TITLEBAR false
/////////////////////////////
class TimeRatioRange
{
public:
  double display_min = -2;
  double display_max = 4;
  double get_value_for_display(double display_value);
  std::string get_value_string_for_label_display(double display_value);
};

class PitchScaleRange
{
public:
  double display_min = -7;
  double display_max = 7;
  double get_value_for_display(double display_value);
  std::string get_value_string_for_label_display(double display_value);
};
class MainWindow : public Gtk::ApplicationWindow
{
public:
  MainWindow();
  SelectionDB *m_p_selection_db;

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

  Gtk::Box m_HBox_pitch_scale;
  PitchScaleRange m_pitch_scale_range;
  Gtk::Label m_Label_pitch_scale;
  Gtk::Scale m_Scale_pitch_scale;
  Gtk::Label m_Label_pitch_scale_img;
  Gtk::Entry m_Entry_pitch_scale;
  void on_pitch_scale_value_changed();

  Gtk::ToggleButton m_ToggleButton_pitch_scale_shown;
  void toggle_pitch_scale_visibility();
  // std::shared_ptr<Gtk::RecentManager> recent_manager;
  Player player;
  Sound sound;

  // Gtk::FileDialog m_FileDialog_open_audio_file;

  Glib::RefPtr<Gtk::FileChooserNative> m_Dialog_open_audio_file;
  void on_open_audio_file_dialog_response(int response_id, Glib::RefPtr<Gtk::FileChooserNative> &m_Dialog_open_audio_file);

  Glib::RefPtr<Gtk::EventControllerKey> m_Keypressed;
  bool on_key_pressed(const unsigned int a, const unsigned int b, const Gdk::ModifierType c);
  void load_sound(std::string path);

  bool on_button_drop_drop_data(const Glib::ValueBase &value, double, double);

  Gtk::MessageDialog *m_pMessageDialog;

  void show_message_dialog(std::string m1, std::string m2);
  RubberBandOptionsWindow *rubberband_options_window;

  void on_preferences();
  void on_file_quit();
  void on_about();
  void on_not_implemented();

  //   static MainWindow* create();
    // void open_file_view(const Glib::RefPtr<Gio::File>& file);
};

#endif