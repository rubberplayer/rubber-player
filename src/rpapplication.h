#ifndef GTKMM_EXAMPLEAPPLICATION_H
#define GTKMM_EXAMPLEAPPLICATION_H

#include <gtkmm.h>
#include "view.h"

class RPApplication : public Gtk::Application
{
protected:
    RPApplication();

public:
    static Glib::RefPtr<RPApplication> create();

protected:
    // Override default signal handlers:
    void on_startup() override;
    // void on_activate() override;
    void on_open(const Gio::Application::type_vec_files &files,
                 const Glib::ustring &hint) override;
};

#endif /* GTKMM_EXAMPLEAPPLICATION_H */