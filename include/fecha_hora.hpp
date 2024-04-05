#ifndef _FECHA_HORA_H
#define _FECHA_HORA_H

#include <iostream>
#include <time.h>

#include "common.hpp"

class _hora
{
  private:
    int hora;
    int minutos;
    int segundos;
    time_t juldate;

  public:
    long comparar(int h[3]);
    void tomar_hora_sistema();
    int retornar_hora();
    void hora_char(char* cadena, bool b_segundos);
    void tomar_hora_char(char* cadena, bool b_segundos);
    _hora()
    {
        hora = 0;
        minutos = 0;
        segundos = 0;
    }
};

class _fecha
{
  private:
    int dia;
    int mes;
    int anio;
    int juliano;

  public:
    void tomar_fecha_sistema();
    void fecha_char(char* cadena, bool y2k, bool invertir);
    void tomar_fecha_char(char* cadena, bool y2k);
    void tomar_fecha_char_invertida(char* cadena, bool y2k);
};

class _config_fecha_hora
{
  private:
    class _fecha fecha_sistema;
    class _hora hora_sistema;
    char fecha[13];
    char hora[8];
    int caracteres_fecha;
    int caracteres_hora;

  public:
    _config_fecha_hora();
};

#endif
