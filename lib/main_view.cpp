#include "../include/view.hpp"
// Declaración de la función global

/*! @brief
** Constructor de la clase.
** Se encarga de inicializar GTK y mostrar la ventana
** principal.
** @param gladeFilename el nombre del archivo glade
** @param mainWindowname el nombre de la ventana principal
*/
Main_Window::Main_Window(GtkBuilder* builder, GtkWidget* window, GtkWidget* enter_button, GtkWidget* one_button,
                         GtkWidget* two_button, GtkWidget* three_button, GtkWidget* four_button, GtkWidget* five_button,
                         GtkWidget* six_button, GtkWidget* seven_button, GtkWidget* eight_button,
                         GtkWidget* nine_button, GtkWidget* zero_button, GtkWidget* delete_button, GtkEntry* entry,
                         GtkLabel* datetime)
{
    this->builder = builder;
    this->window = window;
    this->enter_button = enter_button;
    this->one_button = one_button;
    this->two_button = two_button;
    this->three_button = three_button;
    this->four_button = four_button;
    this->five_button = five_button;
    this->six_button = six_button;
    this->seven_button = seven_button;
    this->eight_button = eight_button;
    this->nine_button = nine_button;
    this->delete_button = delete_button;
    this->entry = entry;
    this->employee_id = "";
    this->datetime = datetime;
    this->user_window = NULL;

    /*Se conectan las señales de los objetos de la interfaz gráfica y se asigna un 
    handler a cada una para poder manejarlas*/
    g_signal_connect(one_button, "clicked", G_CALLBACK(one_clicked), (gpointer)this);
    g_signal_connect(two_button, "clicked", G_CALLBACK(two_clicked), (gpointer)this);
    g_signal_connect(three_button, "clicked", G_CALLBACK(three_clicked), (gpointer)this);
    g_signal_connect(four_button, "clicked", G_CALLBACK(four_clicked), (gpointer)this);
    g_signal_connect(five_button, "clicked", G_CALLBACK(five_clicked), (gpointer)this);
    g_signal_connect(six_button, "clicked", G_CALLBACK(six_clicked), (gpointer)this);
    g_signal_connect(seven_button, "clicked", G_CALLBACK(seven_clicked), (gpointer)this);
    g_signal_connect(eight_button, "clicked", G_CALLBACK(eight_clicked), (gpointer)this);
    g_signal_connect(nine_button, "clicked", G_CALLBACK(nine_clicked), (gpointer)this);
    g_signal_connect(zero_button, "clicked", G_CALLBACK(zero_clicked), (gpointer)this);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(delete_clicked), (gpointer)this);
    g_timeout_add_seconds(1, actualizar_hora_static, this);
}

/*!
** Destructor de la clase
*/
Main_Window::~Main_Window()
{
}

/*!
** @brief
** Muestra la ventana principal
*/
void Main_Window::show()
{
    if (GTK_IS_WIDGET(this->window))
    {
        gtk_widget_show(this->window);
    }
}

/*!
** @brief
** Oculta la vista de la clase.
*/
void Main_Window::hide()
{
    gtk_widget_hide_on_delete(this->window);
}

/*!
** @brief
** Devuelve el employee_id
*/
std::string Main_Window::get_employee_id()
{
    return this->employee_id;
}

/*!
** @brief
** Añade un número al employee id actual.
** Luego, lo muestra en la interfaz.
*/
void Main_Window::add_number_to_id(std::string number)
{
    set_employee_id(get_employee_id() += number);
    show_id_in_entry();
}

/*!
** @brief
** Muestra el employee_id en la entry de
** la interfaz.
*/
void Main_Window::show_id_in_entry()
{
    const gchar* new_text = get_employee_id().c_str();
    gtk_entry_set_text(get_entry(), new_text);
}

/*!
** @brief
** Setea un employee_id.
*/
void Main_Window::set_employee_id(std::string employee_id)
{
    this->employee_id = employee_id;
}

/*!
** @brief
** Devuelve la entry de la interfaz.
*/
GtkEntry* Main_Window::get_entry()
{
    return this->entry;
}

/*!
** @brief
** Elimina el último dígito del employee_id,
** y lo muestra actualizado en la entry.
*/
void Main_Window::delete_employee_id_number()
{
    std::string aux = get_employee_id();
    if (!aux.empty())
    {
        aux.pop_back();
        set_employee_id(aux);
        show_id_in_entry();
    }
}

/*!
** @brief
** Retorna el objeto fecha/hora de la vista.
*/
GtkLabel* Main_Window::get_datetime_label()
{
    return this->datetime;
}

/*!
** @brief
** Muestra la hora en la pantalla.
*/
gboolean Main_Window::actualizar_hora()
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

/*!
** @brief
** Setea una nueva vista a mostrar.
** @param
** vista: La vista a setear
*/
void Main_Window::set_user_view(User_Window* vista)
{
    this->user_window = vista;
}

/*!
** @brief
** Oculta la vista Main y muestra
** la vista user_window.
*/
void Main_Window::change_vista()
{
    get_user_window()->show();
    gtk_widget_hide(get_main_window());
}

/*!
** @brief
** Devuelve la user_window
*/
User_Window* Main_Window::get_user_window()
{
    return this->user_window;
}

GtkWidget* Main_Window::get_main_window()
{
    return this->window;
}

GtkWidget* 
Main_Window::getEnterButton()
{
    return this->enter_button;
}
/*********************************************************************/
/*************************HANDLER FUNCTIONS***************************/
/*********************************************************************/

/*!
** @brief
** Botón del número uno de la pantalla
*/
void one_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("1");
    }
}

/*!
** @brief
** Botón del número dos de la pantalla
*/
void two_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("2");
    }
}

/*!
** @brief
** Botón del número tres de la pantalla
*/
void three_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("3");
    }
}

/*!
** @brief
** Botón del número cuatro de la pantalla
*/
void four_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("4");
    }
}

/*!
** @brief
** Botón del número cinco de la pantalla
*/
void five_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("5");
    }
}

/*!
** @brief
** Botón del número seis de la pantalla
*/
void six_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("6");
    }
}

/*!
** @brief
** Botón del número siete de la pantalla
*/
void seven_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("7");
    }
}

/*!
** @brief
** Botón del número ocho de la pantalla
*/
void eight_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("8");
    }
}

/*!
** @brief
** Botón del número nueve de la pantalla
*/
void nine_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("9");
    }
}

/*!
** @brief
** Botón del número cero de la pantalla
*/
void zero_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->add_number_to_id("0");
    }
}

/*!
** @brief
** Botón de borrar de la pantalla.
*/
void delete_clicked(GtkButton* b, gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    if (vista)
    {
        vista->delete_employee_id_number();
    }
}

/*!
** @brief
** Actualiza la hora de la pantalla.
*/
gboolean actualizar_hora_static(gpointer user_data)
{
    Main_Window* vista = static_cast<Main_Window*>(user_data);
    return vista->actualizar_hora();
}
