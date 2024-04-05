#ifndef RELOJ_HPP
#define RELOJ_HPP
#include "archivo.h"
#include "common.hpp"
#include "comunicacion.hpp"
#include "log.hpp"
#include "mysqldb.hpp"
#include "view.hpp"
#include "eventos.h"
#include <chrono>
#include <cstring>
#include <gtk/gtk.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <poll.h>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
// #include <wiringPi.h>
#include <wiringSerial.h>


#define RELOJ_TAG static_cast<const char*>("RELOJ: ")
#define MAX_LENGTH 32


/* Estos datos y funciones se utilizan para comunicar al proceso hijo con el padre.
** Como el padre es el único que puede accionar sobre la interfaz gráfica,
** al momento de suceder eventos que requieran tomar acción en la interfaz,
** el proceso hijo se lo debe comunicar al padre mediante la escritura de
** un pipe. La estructura Data debe contener todos los objetos y datos
** que intervienen en la acción. Por ejemplo, si se quiere triggerear una señal
** de un botón, la referencia al botón debe estar incluída en Data.
*/
typedef struct
{
    int pipefd[2];
    GtkWidget* button;
    char* nombre = nullptr;
    char* legajo = nullptr;
    char* employee = nullptr;
    GtkLabel* employee_id_label;
    GtkLabel* employee_name_label;
} Data;

struct variables_configuracion
{
    char usuario[30];
    char clave[30];
    char base_de_datos[30];
    char version[30];
};

enum reloj_status
{
    Init_Status,
    Read_Status,
    Not_Founding
};

class Reloj
{
  private:
    /*Objetos/variables para la comunicación y lectura del named pipe*/
    _comunicacion *input_pipe, *output_pipe;
    _log_sistema log;
    _cabecera cabecera_resp_com;
    _archivo_config archivo_config;
    char log_buffer[512];
    bool exit_proc = false;
    long length_cab_com;
    unsigned char buff_answer_com[200], buff_data[256];
    char cmd_char_com[4];
    system_command cmd_com;
    unsigned char buffer_resp_com[10];
    long longitud;
    char archivo[22];
    /*Objetos/variables para la configuración y uso de la base de datos*/
    variables_configuracion var_conf;
    _mysql mysql;
    _sql sql;
    MYSQL_RES* result;
    MYSQL_ROW fila;
    std::string consult;
    int num_rows;
    bool check_database = false;
    int fingerprint_position;
    /*Objetos/variables para controlar el flujo de operación del proceso Reloj */
    reloj_status status;
    class _eventos eventos;

    GtkBuilder* builder;
    /*Objetos para la creación e interacción de la ventana Main Window*/
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

    /*Objetos para la creación e interacción de la ventana User Window*/
    GtkWidget* user_window;
    GtkWidget* check_in_button;
    GtkWidget* check_out_button;
    GtkWidget* break_in_button;
    GtkWidget* break_out_button;
    GtkWidget* brief_in_button;
    GtkWidget* brief_out_button;
    GtkLabel* user_datetime;
    GtkLabel* employee_id_label;
    GtkLabel* employee_name_label;

    /*Objetos para la creación e interacción de la ventana Action Window*/
    GtkWidget* action_window;

    Data data;

  public:
    Reloj();
    ~Reloj();
    int start();
    void cargar_configuracion_archivo();
    int abrir_conexion();
    void conectar_objetos();
    void change_status(reloj_status new_status);
    string get_status_string(reloj_status status_code);
    void objects_init();
    // void connect_signals();
};




gboolean check_pipe_callback(gpointer data);
#endif