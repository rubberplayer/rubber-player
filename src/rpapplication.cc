/* gtkmm example Copyright (C) 2016 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "rpapplication.h"
// #include "rpapplicationwindow.h"
#include <iostream>
#include <exception>

RPApplication::RPApplication()
    : Gtk::Application("org.guimbarde.rubber-player", Gio::Application::Flags::HANDLES_OPEN)
{
}

Glib::RefPtr<RPApplication> RPApplication::create()
{
  return Glib::make_refptr_for_instance<RPApplication>(new RPApplication());
}


void RPApplication::on_startup()
{
  // Call the base class's implementation.
  Gtk::Application::on_startup();

  // Add actions and keyboard accelerators for the menu.
  //  add_action("preferences", sigc::mem_fun(*this, &ExampleApplication::on_action_preferences));
  //  add_action("quit", sigc::mem_fun(*this, &ExampleApplication::on_action_quit));
  //  set_accel_for_action("app.quit", "<Ctrl>Q");
  auto view = new MainWindow();
  add_window(*view);
  view->show();
}

// void ExampleApplication::on_activate()
// {
// }

void RPApplication::on_open(const Gio::Application::type_vec_files &files,
                                 const Glib::ustring & /* hint */)
{

  for (auto file : files)
  {
    auto view = new MainWindow();
    add_window(*view);
    view->show();
    // const Glib::ustring basename = file->get_basename();
    const Glib::ustring path = file->get_path();
    //     printf("-*-*-*-*-* open file view %s :: %s \n",path.c_str(),basename.c_str());
    view->load_sound(path);
  }
}