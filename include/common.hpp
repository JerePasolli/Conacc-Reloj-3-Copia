#ifndef _COMMON_H
#define _COMMON_H

#include <chrono>
#include <csignal>
#include <cstring>
#include <ctime>
#include <random>
#include <string>
#include <sys/stat.h>

#define SI true
#define NO false

/* Strings para evitar warnings */
#define RELOJ_STR const_cast<char*>("reloj")
#define RELOJ3_STR const_cast<char*>("reloj_3")
#define PIPE_HUELLA static_cast<const char*>("hue")
#define PIPE_RELOJ static_cast<const char*>("rel")
#define PIPE_CAMARA static_cast<const char*>("cam")

#define CMD_PATH static_cast<const char*>("/usr/local/conacc/reloj/")
#define RESTART_CMD static_cast<const char*>("reiniciar.sh")
#define REBOOT_CMD static_cast<const char*>("reboot now")
#define SHUTDOWN_CMD static_cast<const char*>("shutdown 0")

#define RELOJ_CONF_FILE const_cast<char*>("reloj.conf")
#define USUARIO_STR const_cast<char*>("USUARIO")
#define BDD_STR const_cast<char*>("BASE_DE_DATOS")
#define LOCALHOST_STR const_cast<char*>("localhost")
#define CLAVE_STR const_cast<char*>("CLAVE")

/* Sintaxis: Cmd_<destino>_<comando>           */
enum system_command
{
    Cmd_All_Null, /* Comando '0' recibido a veces por error */
    /*Definir aqui todos los comandos que se van a utilizar*/
    Cmd_Fingerprint_System_Param,
    Cmd_Fingerprint_Nro_Huellas,
    Cmd_Fingerprint_Search,
    Cmd_Fingerprint_Compare,
    Cmd_Fingerprint_Enroll,
    Cmd_Fingerprint_Delete_One,
    Cmd_Fingerprint_Delete_All,
    Cmd_Fingerprint_Exit,
    Cmd_Fingerprint_To_Read,
    Cmd_Fingerprint_To_Wait,
    Cmd_Reloj_Not_Finger_In_Sensor,
    Cmd_Reloj_Fingerprint_found,
    Cmd_Reloj_Fingerprint_Not_found,
    Cmd_Reloj_Fingerprint_Error,
    Cmd_Reloj_Enroll_Ok,
    Cmd_Reloj_Enroll_Failed,
    Cmd_Reloj_Enroll_Failed_Memory,
    Cmd_Reloj_Enroll_Failed_Finger,
    Cmd_Facerecognition_Search,
    Cmd_Facerecognition_Get_Info
};

#define INT 1
#define LONG 2
#define STRING 3

class _jul_date
{
  private:
    time_t juldate;

  public:
    long julian_date()
    {
        juldate = time(NULL);
        return static_cast<long>(juldate);
    }
};

void modificador(char*, int, int);
std::string get_cmd_string_from_code(system_command);
void ltrim(char*, char*, long);
long valor_long(char*, long);
void set_sigaction();
void restart_conacc();
void reboot_os();
void shutdown_os();
void seed_random_number();
int get_random_number();

#endif
