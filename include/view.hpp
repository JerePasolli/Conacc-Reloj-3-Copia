#include <ctime>
#include <ctype.h>
#include <filesystem>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <iostream>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <vector>


#ifndef VIEW_HPP
#define VIEW_HPP

#define MAX_DNI_SIZE = 7

class User_Window;
class Action_Window;

class BaseWindow {
public:
    virtual void show() = 0;
    virtual void hide() = 0;
};

class Main_Window: public BaseWindow
{
  private:
    GtkBuilder* builder;
    GtkWidget* window;
    GtkWidget* enter_button;
    GtkWidget* one_button;
    GtkWidget* two_button;
    GtkWidget* three_button;
    GtkWidget* four_button;
    GtkWidget* five_button;
    GtkWidget* six_button;
    GtkWidget* seven_button;
    GtkWidget* eight_button;
    GtkWidget* nine_button;
    GtkWidget* zero_button;
    GtkWidget* delete_button;
    GtkEntry* entry;
    GtkLabel* datetime;
    std::string employee_id;
    User_Window* user_window;

  public:
    Main_Window(GtkBuilder* builder, GtkWidget* window, GtkWidget* enter_button, GtkWidget* one_button,
                GtkWidget* two_button, GtkWidget* three_button, GtkWidget* four_button, GtkWidget* five_button,
                GtkWidget* six_button, GtkWidget* seven_button, GtkWidget* eight_button, GtkWidget* nine_button,
                GtkWidget* zero_button, GtkWidget* delete_button, GtkEntry* entry, GtkLabel* datetime);
    ~Main_Window();
    void show() override;
    void hide() override;
    std::string get_employee_id();
    void set_employee_id(std::string employee_id);
    void add_number_to_id(std::string number);
    void show_id_in_entry();
    void delete_employee_id_number();
    GtkEntry* get_entry();
    GtkLabel* get_datetime_label();
    gboolean actualizar_hora();
    void set_user_view(User_Window* vista);
    void change_vista();
    User_Window* get_user_window();
    GtkWidget* get_main_window();
    GtkWidget* getEnterButton();
};

class User_Window: public BaseWindow
{
  private:
    GtkBuilder* builder;
    GtkWidget* user_window;
    GtkWidget* check_in_button;
    GtkWidget* check_out_button;
    GtkWidget* break_in_button;
    GtkWidget* break_out_button;
    GtkWidget* brief_in_button;
    GtkWidget* brief_out_button;
    Main_Window* main_window;
    Action_Window* action_window;
    GtkLabel* user_datetime;
    GtkLabel* employee_id_label;
    GtkLabel* employee_name_label;

  public:
    User_Window(GtkBuilder* builder, GtkWidget* user_window, GtkWidget* check_in_button, GtkWidget* check_out_button,
                GtkWidget* break_in_button, GtkWidget* break_out_button, GtkWidget* brief_in_button,
                GtkWidget* brief_out_button, GtkLabel* user_datetime, GtkLabel* employee_id_label,
                GtkLabel* employee_name_label);

    void show() override;
    void hide() override;
    void set_main_view(Main_Window* vista);
    void set_action_view(Action_Window* vista);
    Main_Window* get_main_view();
    Action_Window* get_action_view();
    void change_to_main_view();
    void change_to_action_view();
    GtkWidget* get_window();
    GtkWidget* getCheckInButton();
    GtkLabel* get_datetime_label();
    gboolean actualizar_hora();
    ~User_Window();
    GtkLabel* get_employee_id_label();
    GtkLabel* get_employee_name_label();
};

// class Action_Window
// {
//   private:
//     GtkBuilder* builder;
//     GtkWidget* action_window;
//     Main_Window* main_window;

//   public:
//     Action_Window(GtkBuilder* builder, GtkWidget* action_window);
//     void mostrar();
//     void set_main_view(Main_Window* vista);
//     GtkWidget* get_action_view();

// };
/*
** Handlers de los eventos de la Vista.
** Se declaran fuera de la clase ya que son
** manejadas por C.
*/
extern "C" void enter_clicked(GtkButton* b, gpointer user_data);
extern "C" void one_clicked(GtkButton* b, gpointer user_data);
extern "C" void two_clicked(GtkButton* b, gpointer user_data);
extern "C" void three_clicked(GtkButton* b, gpointer user_data);
extern "C" void four_clicked(GtkButton* b, gpointer user_data);
extern "C" void five_clicked(GtkButton* b, gpointer user_data);
extern "C" void six_clicked(GtkButton* b, gpointer user_data);
extern "C" void seven_clicked(GtkButton* b, gpointer user_data);
extern "C" void eight_clicked(GtkButton* b, gpointer user_data);
extern "C" void nine_clicked(GtkButton* b, gpointer user_data);
extern "C" void zero_clicked(GtkButton* b, gpointer user_data);
extern "C" void delete_clicked(GtkButton* b, gpointer user_data);
extern "C" void back_clicked(GtkButton* b, gpointer user_data);
gboolean actualizar_hora_static(gpointer user_data);
gboolean actualizar_hora_static_user(gpointer user_data);
extern "C" void call_signal(GtkButton* b, gpointer user_data);

#endif // VIEW_HPP
