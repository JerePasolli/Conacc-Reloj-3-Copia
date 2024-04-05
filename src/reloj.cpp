#include "../include/reloj.hpp"
#include "../include/statemanager.hpp"

/*COMPILAR GUI TODO JUNTO*/
// g++ src/reloj.cpp lib/main_view.cpp lib/user_view.cpp -o programa `pkg-config --cflags --libs gtk+-3.0`
// -export-dynamic -rdynamic

/*COMPILAR GUI INDIVIDUALMENTE Y ENLAZAR*/
// g++ -w -std=c++2a -Wall -Werror -Wconversion `pkg-config --cflags --libs gtk+-3.0`-export-dynamic -rdynamic -lm -c -o
// obj/user_view.o lib/user_view.cpp g++ -w -std=c++2a -Wall -Werror -Wconversion `pkg-config --cflags --libs
// gtk+-3.0`-export-dynamic -rdynamic -lm -c -o obj/main_view.o lib/main_view.cpp g++ -o programa -w -std=c++2a -Wall
// -Werror -Wconversion src/reloj.cpp obj/user_view.o obj/main_view.o `pkg-config --cflags --libs gtk+-3.0`

Reloj::Reloj()
{
    input_pipe = new _comunicacion(string(RELOJ_TAG));
    output_pipe = new _comunicacion(string(RELOJ_TAG));
    status = Init_Status;
    num_rows = 0;
    fingerprint_position = 0;
    longitud = 0;
    set_sigaction();
}

Reloj::~Reloj()
{
    delete input_pipe;
    delete output_pipe;
}

int Reloj::start()
{
    /* Configuracion inicial desde base de datos */
    cargar_configuracion_archivo();

    /* Apertura de base de datos */
    if (!abrir_conexion())
    {
        sprintf(log_buffer, "%sError al conectar a la BDD", RELOJ_TAG);
        log.grabar(log_buffer, Error_Log);
        // goto end_of_process_1;
    }

    /* Conexion a base de datos */
    if (!mysql.seleccionar_base(var_conf.base_de_datos))
    {
        sprintf(log_buffer, "%sError al seleccionar la BDD", RELOJ_TAG);
        log.grabar(log_buffer, Error_Log);
        // goto end_of_process_0;
    }

    /* Conexion con el resto de los objetos que usan la BDD*/
    conectar_objetos();

    /* Inicializacion de objetos y estructuras*/
    objects_init();

    /* Creación de objetos correspondientes a las interfaces*/
    Main_Window Main_Window(builder, window, enter_button, one_button, two_button, three_button, four_button,
                            five_button, six_button, seven_button, eight_button, nine_button, zero_button,
                            delete_button, entry, datetime);

    User_Window User_Window(builder, user_window, check_in_button, check_out_button, break_in_button, break_out_button,
                            brief_in_button, brief_out_button, user_datetime, employee_id_label, employee_name_label);

    /* Creación del manejador de cambios entre interfaces*/
    StateManager stateManager(&Main_Window);    


    /* Registro de callbacks para funciones de las interfaces*/
    stateManager.registerCallback(StateManager::EventType::EnterClicked, [&]() {
        stateManager.changeState(&User_Window);
    });

    stateManager.registerCallback(StateManager::EventType::CheckIn, [&]() {
        stateManager.changeState(&Main_Window);
    });

    /* Conexión de señales de objetos de la interfaz para ser manejadas por stateManager*/
    g_signal_connect(Main_Window.getEnterButton(), "clicked", G_CALLBACK(StateManager::enterClickedHandler), &stateManager);
    g_signal_connect(User_Window.getCheckInButton(), "clicked", G_CALLBACK(StateManager::checkInClickedHandler), &stateManager);

    /*Fork del proceso reloj*/
    pid_t pid = fork();

    if (pid == 0)
    {
        /* PROCESO HIJO:
        ** El proceso hijo atiende los periféricos externos (sensor de huellas, cámara, etc)
        ** y realiza acciones en base a lo que lee en su NAMED PIPE asociado.
        ** Cuando sucede un evento que requiere comunicación con el proceso padre (el proceso
        ** padre está atendiendo la interfaz gráfica) se lo comunica escribiendo información
        ** en el PIPE que comunica al proceso padre y al hijo.
        **/

        close(data.pipefd[0]); // Cerrar el extremo de lectura de la tubería en el proceso hijo

        if (!input_pipe->abrir(PIPE_RELOJ, LECTURA, CREAR))
        {
            sprintf(log_buffer, "%sError al abrir pipe", RELOJ_TAG);
            log.grabar(log_buffer, Error_Log);
            exit_proc = true;
        }
        else
        {
            sprintf(log_buffer, "%sIniciando", RELOJ_TAG);
            log.grabar(log_buffer, Control_Log);
        }

        while (1)
        {
            /* Se verifica la existencia de comandos encolados */
            if (input_pipe->leer_cabecera(&cabecera_resp_com, 0, 1000) && !exit_proc)
            {
                char cmd_char[4];
                system_command cmd;

                memcpy(buffer_resp_com, "\0", sizeof(buffer_resp_com));
                longitud = cabecera_resp_com.retornar_longitud();
                longitud = input_pipe->leer_cuerpo(buffer_resp_com, longitud);
                cabecera_resp_com.retornar_comando(cmd_char);
                cabecera_resp_com.retornar_tuberia_respuesta(archivo);

                /*	Parseo de comando. Se convierte de char* a int para mejor analisis */
                cmd = static_cast<system_command>(atoi(cmd_char));
                sprintf(log_buffer, "%sSe recibe comando :) '%s' (%d)", RELOJ_TAG,
                        get_cmd_string_from_code(cmd).c_str(), cmd);
                log.grabar(log_buffer, Info_Log);

                /* En el siguiente switch se evalúa que comando llegó por el NAMED PIPE.
                ** En base al comando que llegue, se cambia el estado del programa.
                */
                switch (cmd)
                {
                case Cmd_Reloj_Fingerprint_found:
                    change_status(Read_Status);
                    check_database = true;
                    break;

                case Cmd_Reloj_Fingerprint_Not_found:
                    change_status(Not_Founding);
                    break;

                default:
                    break;
                }
            }

            /* El siguiente switch case evalúa el estado del programa, y en base a eso se
            ** realiza la acción que corresponda
            */
            switch (status)
            {
            case Init_Status:
                break;

            case Read_Status:
                
                if (check_database)
                {
                    /*Obtiene la posición de memoria de la huella que llegó por el pipe*/
                    fingerprint_position =
                        static_cast<int>(buffer_resp_com[0]) * 256 + static_cast<int>(buffer_resp_com[1]);
                    consult = "SELECT nombre, legajo FROM huellas where id = " + std::to_string(fingerprint_position);

                    sql.ejecutar_sql(consult.c_str());
                    result = sql.get_last_result();
                    num_rows = mysql_num_rows(result);

                    if (num_rows == 0)
                    {
                        /*Si no encuentra nada en la base de datos, sale*/
                        check_database = false;
                        break;
                    }
                    else
                    {
                        /*Si encuentra un registro en la base de datos, obtiene 
                        el nombre junto con el legajo y se lo envía al padre*/
                        
                        fila = mysql_fetch_row(result);
                        
                        data.nombre = (char*)malloc(strlen(fila[0]) + 1);
                        if (data.nombre == NULL)
                        {
                            perror("Error al asignar memoria para el nombre");
                            exit(EXIT_FAILURE);
                        }
                        strcpy(data.nombre, fila[0]);

                        data.legajo = (char*)malloc(strlen(fila[1]) + 1);
                        if (data.legajo == NULL)
                        {
                            perror("Error al asignar memoria para el legajo");
                            exit(EXIT_FAILURE);
                        }
                        strcpy(data.legajo, fila[1]);
                        
                        data.employee = (char*)malloc(strlen(data.nombre) + strlen(data.legajo) + 2);
                        sprintf(data.employee, "%s\t%s", data.nombre, data.legajo);
                        write(data.pipefd[1], data.employee, strlen(data.employee) + 1);

                        eventos.check_in(data.nombre, data.legajo);
                        
                    }
                    check_database = false;
                    break;
                }
                else
                {
                    break;
                }
            case Not_Founding:
                    /*TODO: Acá debe ir la lógica para cuando no encuentra una huella
                    y debe solicitar ingreso por DNI*/
                break;
            default:
                break;
            }
        }
        close(data.pipefd[1]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        /*Este es el código del proceso padre. Queda atendiendo la interfaz,
        y chequea constantemente si hay una escritura en el pipe por donde el hijo 
        le envia información
        */

        close(data.pipefd[1]); 

        guint idle_id = g_idle_add(check_pipe_callback, &data);
        gtk_widget_show_all(window);
        gtk_main();

        close(data.pipefd[0]);
    }

    // end_of_process_0:
    // mysql.desconectar();

    // end_of_process_1:
    // sprintf( log_buffer, "%sSaliendo", RELOJ_TAG );
    // log.grabar( log_buffer, Control_Log );

    return 0;
}

/*!
** @brief
** Esta función la chequea el padre.
** Si detecta que se escribió en un pipe, entonces
** ejecuta la señal para cambiar de interfaz
** @param
** data: Puntero a la estructura Data.
*/
gboolean check_pipe_callback(gpointer data)
{

    char buffer[1024];
    struct pollfd fds[1];
    int ret;
    Data* info = (Data*)data;

    // Configurar la estructura pollfd para el descriptor del pipe
    fds[0].fd = info->pipefd[0];
    fds[0].events = POLLIN;

    // Esperar hasta que haya datos disponibles en el pipe o hasta que se produzca un error
    ret = poll(fds, 1, 0);

    if (ret == -1)
    {
        perror("poll");
        return TRUE; // Volver a intentar en el próximo ciclo de eventos
    }
    else if (ret > 0)
    {
        ssize_t nbytes = read(info->pipefd[0], buffer, sizeof(buffer));
        if (nbytes > 0)
        {

            buffer[nbytes] = '\0';

            // Separar los datos en nombre y legajo utilizando el tabulador como delimitador
            char* nombre = strtok(buffer, "\t");
            char* legajo = strtok(NULL, "\t");

            gtk_label_set_text(info->employee_id_label, nombre);
            gtk_label_set_text(info->employee_name_label, legajo);

            g_signal_emit_by_name(info->button, "clicked");
        }
    }

    return TRUE; // Mantener el temporizador activo
}

/**
 * @brief Lee las configuraciones desde el archivo de configuracion
 *
 */
void Reloj::cargar_configuracion_archivo()
{
    archivo_config.abrir(RELOJ_CONF_FILE);

    strcpy(var_conf.usuario, archivo_config.retornar_string(USUARIO_STR, RELOJ_STR));
    strcpy(var_conf.clave, archivo_config.retornar_string(CLAVE_STR, RELOJ_STR));
    strcpy(var_conf.base_de_datos, archivo_config.retornar_string(BDD_STR, RELOJ3_STR));

    archivo_config.cerrar();
}

/*
 *	Intenta conectar a base de datos hasta que se acaban los reintentos.
 */
int Reloj::abrir_conexion()
{
    int rta;
    int error_mysql = 0;

    do
    {
        rta = mysql.conectar(LOCALHOST_STR,    /*IP_HOST */
                             var_conf.usuario, /*USUARIO*/
                             var_conf.clave,   /*PASSWORD*/
                             3333);
        if (!rta)
        {
            error_mysql++;
            sleep(ESPERAR);
        }
    } while (error_mysql < REINTENTOS && !rta);

    return rta;
}

/*!
 *Se distribuye la conexion de la base de datos con todos los objetos que necesitan acceso a datos.
 */
void Reloj::conectar_objetos()
{
    MYSQL conAux;
    conAux = mysql.retornar_conexion();
    sql.obtener_conexion(conAux);
    eventos.obtener_conexion(conAux);
}

/**
 * @brief Cambio y log del estado de conacc
 *
 * @param new_status nuevo estado a setear
 */
void Reloj::change_status(reloj_status new_status)
{
    if (status != new_status)
    {
        sprintf(log_buffer, "%s%s%s%s%s%s", RELOJ_TAG, "Cambio de estado de '", get_status_string(status).c_str(),
                "' a '", get_status_string(new_status).c_str(), "'");
        log.grabar(log_buffer, Info_Log);
        status = new_status;
    }
}

/**
 * @brief Retorna un string acorde al estado pasado por parametro, para fines
 * de loggeo.
 *
 * @param status_code numero de estado
 * @return string que contiene el nombre del estado
 */
string Reloj::get_status_string(reloj_status status_code)
{
    std::string ret;

    switch (status_code)
    {
    case Init_Status:
        ret = "INIT";
        break;

    case Read_Status:
        ret = "READ";
        break;
    default:
        ret = "ERR";
        break;
    }

    return ret;
}

/**
 * @brief Inicializa objetos y estructuras
 */
void 
Reloj::objects_init()
{
    builder= gtk_builder_new();
    gtk_init(NULL, NULL);
    guint a = gtk_builder_add_from_file(builder, "/home/pi/Conacc-Reloj-3/glade/mainWindow.glade", NULL);
    
    if (a != 1){
        perror("Error al intentar abrir el archivo .glade");
    }

    /*Inicialización de objetos correspondientes a la ventana main_window*/
    window         = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    enter_button   = GTK_WIDGET(gtk_builder_get_object(builder, "enter_button"));
    one_button    = GTK_WIDGET(gtk_builder_get_object(builder, "one_button"));
    two_button     = GTK_WIDGET(gtk_builder_get_object(builder, "two_button"));
    three_button   = GTK_WIDGET(gtk_builder_get_object(builder, "three_button"));
    four_button    = GTK_WIDGET(gtk_builder_get_object(builder, "four_button"));
    five_button    = GTK_WIDGET(gtk_builder_get_object(builder, "five_button"));
    six_button    = GTK_WIDGET(gtk_builder_get_object(builder, "six_button"));
    seven_button   = GTK_WIDGET(gtk_builder_get_object(builder, "seven_button"));
    eight_button  = GTK_WIDGET(gtk_builder_get_object(builder, "eight_button"));
    nine_button    = GTK_WIDGET(gtk_builder_get_object(builder, "nine_button"));
    zero_button    = GTK_WIDGET(gtk_builder_get_object(builder, "zero_button"));
    delete_button  = GTK_WIDGET(gtk_builder_get_object(builder, "delete_button"));
    entry          = GTK_ENTRY(gtk_builder_get_object(builder, "user_entry"));
    datetime       = GTK_LABEL(gtk_builder_get_object(builder, "datetime"));
    
    /*Inicialización de objetos correspondientes a la ventana user_window*/
    user_window = GTK_WIDGET(gtk_builder_get_object(builder, "user_window"));
    check_in_button = GTK_WIDGET(gtk_builder_get_object(builder, "check_in_button"));
    check_out_button = GTK_WIDGET(gtk_builder_get_object(builder, "check_out_button"));
    break_in_button = GTK_WIDGET(gtk_builder_get_object(builder, "break_in_button"));
    break_out_button = GTK_WIDGET(gtk_builder_get_object(builder, "break_out_button"));
    brief_in_button = GTK_WIDGET(gtk_builder_get_object(builder, "brief_in_button"));
    brief_out_button = GTK_WIDGET(gtk_builder_get_object(builder, "brief_out_button"));
    user_datetime = GTK_LABEL(gtk_builder_get_object(builder, "user_datetime"));
    employee_id_label = GTK_LABEL(gtk_builder_get_object(builder, "employee_id_label"));
    employee_name_label = GTK_LABEL(gtk_builder_get_object(builder, "employee_name_label"));

    
    /*Creación del pipe para comunicación entre proceso padre e hijo*/
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /*Inicialización de la estructura Data para los parámetros
    compartidos entre padre e hijo*/
    data.pipefd[0] = pipefd[0];
    data.pipefd[1] = pipefd[1];
    data.button = enter_button;
    data.employee_id_label = employee_id_label;
    data.employee_name_label = employee_name_label;
}

int main()
{                        
    Reloj reloj = Reloj();
    return reloj.start();
}