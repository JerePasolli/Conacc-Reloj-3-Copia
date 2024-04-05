#include "../include/fecha_hora.hpp"

_config_fecha_hora::_config_fecha_hora()
{
    strcpy(fecha, "");
    strcpy(hora, "");
    caracteres_fecha = 0;
    caracteres_hora = 0;
}

void _fecha::fecha_char(char* cadena, bool y2k, bool invertir)
{
    char dd[3], mm[3], aa[5];

    if (dia >= 0 && mes >= 0 && anio >= 0)
    {
        sprintf(dd, "%02d", dia);
        sprintf(mm, "%02d", mes);
        sprintf(aa, "%04d", anio);

        if (!invertir)
        {
            if (y2k)
                sprintf(cadena, "%s/%s/%s", dd, mm, aa);
            else
                sprintf(cadena, "%s/%s/%.2s", dd, mm, aa);
        }
        else
        {
            if (y2k)
                sprintf(cadena, "%s/%s/%s", aa, mm, dd);
            else
                sprintf(cadena, "%.2s/%s/%s", aa, mm, dd);
        }
    }
    else
        sprintf(cadena, "  /  /  ");
}

void _fecha::tomar_fecha_char_invertida(char* cadena, bool y2k)
{
    tomar_fecha_sistema();
    fecha_char(cadena, y2k, true);
}

void _fecha::tomar_fecha_char(char* cadena, bool y2k)
{
    tomar_fecha_sistema();
    fecha_char(cadena, y2k, false);
}

void _fecha::tomar_fecha_sistema()
{
    time_t time3;
    struct tm* tiempo;

    time3 = time(NULL);
    tiempo = localtime(&time3);
    anio = tiempo->tm_year + 1900;
    mes = tiempo->tm_mon + 1;
    dia = tiempo->tm_mday;
    juliano = tiempo->tm_yday;
}

void _hora::hora_char(char* cadena, bool b_segundos)
{
    if (((hora + minutos + segundos) >= 0))
    {
        if (b_segundos)
        {
            sprintf(cadena, "%02i:%02i:%02i", hora, minutos, segundos);
        }
        else
        {
            sprintf(cadena, "%02i:%02i", hora, minutos);
        }
    }
    else
    {
        strcpy(cadena, "  :  :  ");
    }
}
void _hora::tomar_hora_char(char* cadena, bool b_segundos)
{
    tomar_hora_sistema();
    hora_char(cadena, b_segundos);
}

void _hora::tomar_hora_sistema()
{
    time_t time3;
    struct tm* tiempo;
    time3 = time(NULL);
    tiempo = localtime(&time3);
    hora = tiempo->tm_hour;
    minutos = tiempo->tm_min;
    segundos = tiempo->tm_sec;
}
