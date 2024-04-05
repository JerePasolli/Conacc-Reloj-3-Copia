#ifndef _LOG_H
#define _LOG_H

#include "fecha_hora.hpp"

#include <cstring>
#include <fcntl.h>
#include <string>
#include <unistd.h>

#define GRABAR_LOG true

#define FECHA_SIZE 10
#define HORA_SIZE 8
#define TAG_SIZE 11
#define MENSAJE_SIZE 256

#define ARCHIVO_LOG static_cast<const char*>("/home/pi/sistema.log")

#define DEFAULT_TAG static_cast<const char*>("[         ]")
#define CONTROL_TAG static_cast<const char*>("[ Control ]")
#define INFO_TAG static_cast<const char*>("[  Info   ]")
#define ERROR_TAG static_cast<const char*>("[  Error  ]")
#define EVENT_TAG static_cast<const char*>("[  Event  ]")
#define DEBUG_TAG static_cast<const char*>("[  Debug  ]")

enum log_type
{
    Info_Log,
    Control_Log,
    Error_Log,
    Event_Log,
    Debug_Log
};

using std::cout;
using std::endl;
using std::string;

struct log_contenido
{
    char fecha[FECHA_SIZE];
    char sep1;
    char hora[HORA_SIZE];
    char sep2;
    char tag[TAG_SIZE];
    char sep3;
    char mensaje[MENSAJE_SIZE];
};

class _log_sistema
{
  private:
    char log_file[50];
    log_contenido contenido;

  public:
    _log_sistema();
    int grabar(char*, log_type);
    void cambiar_archivo(char* nombre_archivo)
    {
        strcpy(log_file, nombre_archivo);
    }
};

#endif
