#include "../include/view.hpp"

User_Window::User_Window(GtkBuilder* builder, GtkWidget* user_window, GtkWidget* check_in_button,
                         GtkWidget* check_out_button, GtkWidget* break_in_button, GtkWidget* break_out_button,
                         GtkWidget* brief_in_button, GtkWidget* brief_out_button, GtkLabel* user_datetime,
                         GtkLabel* employee_id_label, GtkLabel* employee_name_label)
{

    this->builder = builder;
    this->user_window = user_window;
    this->check_in_button = check_in_button;
    this->check_out_button = check_out_button;
    this->break_in_button = break_in_button;
    this->break_out_button = break_out_button;
    this->brief_in_button = brief_in_button;
    this->brief_out_button = brief_out_button;
    this->user_datetime = user_datetime;
    this->main_window = NULL;
    this->employee_id_label = employee_id_label;
    this->employee_name_label = employee_name_label;

    // g_signal_connect(check_in_button, "clicked", G_CALLBACK(back_clicked), (gpointer)this);
    // g_signal_connect(check_out_button, "clicked", G_CALLBACK(back_clicked), (gpointer)this);
    // g_signal_connect(break_in_button, "clicked", G_CALLBACK(back_clicked), (gpointer)this);
    // g_signal_connect(break_out_button, "clicked", G_CALLBACK(back_clicked), (gpointer)this);
    // g_signal_connect(brief_in_button, "clicked", G_CALLBACK(back_clicked), (gpointer)this);
    // g_signal_connect(brief_out_button, "clicked", G_CALLBACK(back_clicked), (gpointer)this);
    g_timeout_add_seconds(1, actualizar_hora_static_user, this);
}

/*!
** Destructor de la clase
*/
User_Window::~User_Window()
{
}

/*!
** @brief
** Muestra la ventana de usuario.
*/
void User_Window::show()
{
    if (GTK_IS_WIDGET(this->user_window))
    {
        // gtk_widget_show_all(this->user_window);
        gtk_widget_show(this->user_window);
    }
}

/*!
** @brief
** Oculta la vista de la clase.
*/
void User_Window::hide()
{
    gtk_widget_hide_on_delete(this->user_window);
}

/*!
** @brief
** Setea otra vista para tener de referencia en la clase.
** @param
**  vista: La vista a setear de referencia.
*/
void User_Window::set_main_view(Main_Window* vista)
{
    this->main_window = vista;
}

GtkWidget*
User_Window:: getCheckInButton()
{
    return this->check_in_button;
}
/*!
** @brief
** Retorna la vista main_window
*/
Main_Window* User_Window::get_main_view()
{
    return this->main_window;
}

void 
User_Window::set_action_view(Action_Window* vista)
{
    this->action_window = vista;
}

Action_Window* User_Window::get_action_view()
{
    return this->action_window;
}

GtkLabel* User_Window::get_employee_id_label()
{
    return this->employee_id_label;
}

GtkLabel* User_Window::get_employee_name_label()
{
    return this->employee_name_label;
}


GtkWidget* User_Window::get_window()
{
    return this->user_window;
}

/*!
** @brief
** Retorna el objeto fecha/hora de la vista.
*/
GtkLabel* User_Window::get_datetime_label()
{
    return this->user_datetime;
}

/*********************************************************************/
/*************************HANDLER FUNCTIONS***************************/
/*********************************************************************/

/*!
** @brief
** Actualiza la hora de la pantalla.
*/
gboolean actualizar_hora_static_user(gpointer user_data)
{
    User_Window* vista = static_cast<User_Window*>(user_data);
    return vista->actualizar_hora();
}

/*!
** @brief
** Muestra la hora en la pantalla.
*/
gboolean User_Window::actualizar_hora()
{

    std::time_t tiempo_actual = std::time(nullptr);
    std::tm* tiempo = std::localtime(&tiempo_actual);

    char fecha[6]; // dd/mm
    std::strftime(fecha, sizeof(fecha), "%d/%m", tiempo);

    char hora[6]; // HH:MM
    std::strftime(hora, sizeof(hora), "%H:%M", tiempo);

    std::string fecha_hora = fecha;
    fecha_hora += " ";
    fecha_hora += hora;

    gtk_label_set_text(get_datetime_label(), fecha_hora.c_str());

    return TRUE;
}