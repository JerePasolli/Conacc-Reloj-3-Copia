#ifndef _ARCHIVO_H
#define _ARCHIVO_H

#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>

using std::ifstream;

class _archivo
{
  private:
    int descriptor;
    char archivo[80];

  public:
    int retornar_descriptor()
    {
        return descriptor;
    }
    int abrir(char* nombre, int modo);
    void cerrar();
    int enviar(unsigned char* buffer, int lingitud, int tout_seg, int tout_mseg, bool enviar_buffer_completo);
};

/**
 * @brief Clase para lectura de archivos linea por linea
 *
 */
class _archivo_por_linea
{
  protected:
    ifstream archivo;

  public:
    void abrir(char* nombre);
    void cerrar();
};

/**
 * @brief Clase para manejo de archivo de configuraciones. (Texto plano).
 *
 */
class _archivo_config : public _archivo_por_linea
{
  private:
    char ultima_variable[50];
    int indice_variable;
    char ultimo_valor[100];
    int indice_valor;
    char archivo_config[40];

  protected:
    void cerear_variable_valor();
    void cargar_valor(char caracter);
    void cargar_variable(char caracter);
    bool comparar_variable(char* variable);

  public:
    bool obtener_valor(char* variable);
    bool obtener_valor(char* variable, char* valor);
    void obtener_valor(char* variable, char* valor, char* por_defecto);
    void abrir(char* nombre);
    void cerrar();
    void upper(char* destino, char* origen);
    bool retornar_bool();
    char* retornar_string();
    int retornar_int();
    long retornar_long();
    bool retornar_bool(char* variable, bool por_defecto);
    char* retornar_string(char* variable, char* por_defecto);
    int retornar_int(char* variable, int por_defecto);
    long retornar_long(char* variable, long por_defecto);
};

#endif
