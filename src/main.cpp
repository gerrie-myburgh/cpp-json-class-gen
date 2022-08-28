#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/applicationwindow.h>
#include <gtkmm-3.0/gtkmm/builder.h>
#include <gtkmm-3.0/gtkmm/box.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/filechooser.h>
#include <gtkmm-3.0/gtkmm/window.h>

#include <gtkmm-3.0/gtkmm/messagedialog.h>

#include <string>
#include <fstream>

#include <boost/filesystem.hpp>

#include "gen_cpp_source.hpp"
#include "gen_hpp_source.hpp"

#include "utils.hpp"

using namespace std;
using namespace boost;

/**
 * @brief Generate the cpp/hpp files
 * 
 * @param thePath 
 * @param fileName 
 * @param cppDir 
 * @param hppDir 
 */
void generate(boost::filesystem::path thePath, string fileName)
{
    ifstream in;
    in.open(fileName.c_str());
    mainName = thePath.stem().generic_string();

    json::error_code ec;
    json::value inJson = read_json(in, ec);
    const json::object obj = inJson.as_object();
    ofstream out;
    out.open(hppDir + "/" + mainName + "_" + mainName + ".hpp", ofstream::out);
    get_object_hpp(obj, mainName, out);

    ofstream outCpp;
    fileName = cppDir + "/" + mainName + "_" + mainName + ".cpp";
    outCpp.open(fileName, ofstream::out);
    get_object_cpp(obj, mainName, outCpp);
    outCpp.close();
}

class MainWindow : public Gtk::ApplicationWindow
{
    Glib::RefPtr<Gtk::Builder> ui;
    Glib::RefPtr<Gtk::Application> app;
    Gtk::Box *box;
    Gtk::Button *generate_code;
    Gtk::Button *exit_button;
    Gtk::FileChooser *json_file_button;
    Gtk::FileChooser *src_file_button;
    Gtk::FileChooser *inc_file_button;
    Gtk::ApplicationWindow *win;

public:
    MainWindow(Glib::RefPtr<Gtk::Application> application) : ui(Gtk::Builder::create_from_file("gtk-test-03.ui")),
                                                             app(application)
    {

        if (ui)
        {
            ui->get_widget("mainWindow", win);
            ui->get_widget("theBox", box);
            ui->get_widget("exitButton", exit_button);
            ui->get_widget("sourceJsonFile", json_file_button);
            ui->get_widget("srcLocation", src_file_button);
            ui->get_widget("includeLocation", inc_file_button);
            ui->get_widget("generateCode", generate_code);

            generate_code->signal_clicked().connect(
                [this]()
                {
                    auto fileName = json_file_button->get_filename();

                    boost::filesystem::path thePath(fileName.c_str());
                    if (!boost::filesystem::is_regular_file(thePath) || !thePath.has_stem())
                    {
                        auto message = Gtk::MessageDialog(fileName + " is not a regular file");
                        message.run();
                        return;
                    }
                    cppDir = src_file_button->get_filename();
                    hppDir = inc_file_button->get_filename();

                    if (src_file_button->get_filename() == "" || inc_file_button->get_filename() == "")
                    {
                        auto message = Gtk::MessageDialog("CPP or HPP Folder undefined");
                        message.run();
                        return;
                    }
                    generate(thePath, fileName);
                });

            exit_button->signal_clicked().connect(
                [this]()
                { app->quit(); });

            box->show_all();
        }
    }

    Gtk::ApplicationWindow &getWindow()
    {
        return *win;
    }
};

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(
        argc, argv,
        "org.gtkmm.example.HelloApp");
    MainWindow mainWindow(app);

    if(argc > 1)
    {
        auto fileName = string(argv[1]);
        boost::filesystem::path thePath(fileName.c_str());
        cppDir = string(argv[2]);
        hppDir = string(argv[3]);
        generate(thePath, fileName);
    }
    else
    {
        return app->run(mainWindow.getWindow());
    }
}
