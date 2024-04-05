#ifndef _COMUNICACION_H
#define _COMUNICACION_H

#include <cerrno>
#include <unistd.h>

#include "common.hpp"
#include "log.hpp"

#define ECO 1
#define INICIALIZAR 2
#define RESETEAR 3
#define CERRAR 4
#define LEER 5
#define MOSTRAR 6
#define TERMINAR 255

using std::cout;
using std::endl;
using std::strerror;
using std::string;

#define LOG_COMANDOS_COM false

#define LECTURA (O_RDWR | O_NDELAY)
#define ESCRITURA (O_RDWR | O_NDELAY)
#define CREAR false
#define CONSULTA true

#define tout_sec_com 0
#define tout_usec_com 0

#define max_error_lectura 10

/* Parametro para la funciona 'manage_timeout' */
enum timeout_action
{
    Create_Timeout,
    Delete_Timeout
};

/* Estructura de timeout para enviar al proceso timer */
struct timeout
{
    int duration_time;
    system_command command;
    char pipe_dst[8];
    time_t creation_time;
};

/*!
 * Cabecera utilizada en el protocolo interno de comunicacion.
 */
struct _cabecera_datos
{
    char fecha[10];
    char hora[8];
    char tuberia_respuesta[20];
    char comando[20];
    char longitud[12];
    char secuencia[12];
};
/*!
 * Clase que contiene la estructura de una cabecera, y los métodos para cargar y retornar sus campos.
 *
 */
class _cabecera
{
  public:
    char version[4];
    struct _cabecera_datos datos;

    void cargar(char* comando_, long longitud_mensaje_, long secuencia_, const char* tuberia_respuesta_);
    void retornar_version(char* ver);
    void retornar_tuberia_respuesta(char* tuberia);
    void retornar_comando(char* comando);
    long retornar_longitud();
    _cabecera();
};

/*!
 * Clase encargada de enviar y recibir los datos de cabecera y cuerpo de los mensajes enviados en el protocolo
 * interno de comunicación.
 */
class _comunicacion
{
  private:
    int descriptor;
    bool consulta;
    _log_sistema log;
    unsigned char dispositivo;
    char archivo[80];
    int error_lectura;
    string log_tag;

  public:
    int esperar(int, int);
    int retornar_descriptor()
    {
        return descriptor;
    }
    int abrir(const char*, int, bool);
    void cerrar();
    int leer_cabecera(class _cabecera*, int, int);
    int leer_cuerpo(unsigned char*, long);
    int enviar(char*, unsigned char*, long, const char*, long);
    _comunicacion();
    explicit _comunicacion(string);
    int send_command(const char*, const char*, system_command, void*, size_t);
    int send_command(const char*, const char*, system_command);
    // void     manage_timeout( system_command, int, string, timeout_action );
};

#endif
