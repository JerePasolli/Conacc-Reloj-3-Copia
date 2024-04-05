// #include "../include/view.hpp"
// /*! @brief
// ** Constructor de la clase.
// ** Se encarga de inicializar GTK y mostrar la ventana
// ** principal.
// ** @param gladeFilename el nombre del archivo glade
// ** @param mainWindowname el nombre de la ventana principal
// */
// Action_Window::Action_Window(GtkBuilder* builder, GtkWidget* action_window)
// {
//     this->builder = builder;
//     this->action_window = action_window;
//     this->main_window = NULL;
// }

// /*!
// ** @brief
// ** Muestra la ventana de action.
// */
// void Action_Window::mostrar()
// {
//     if (GTK_IS_WIDGET(this->action_window))
//     {
//         // gtk_widget_show_all(this->user_window);
//         gtk_widget_show(this->action_window);
//     }
// }

// void 
// Action_Window::set_main_view(Main_Window* vista)
// {
//     this->main_window = vista;
// }

// GtkWidget*
// Action_Window:: get_action_view()
// {
//     return this->action_window;
// }