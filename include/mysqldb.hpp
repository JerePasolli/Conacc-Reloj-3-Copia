#ifndef _MYSQLDB_H
#define _MYSQLDB_H

#include "common.hpp"
#include "log.hpp"

#include <mysql/mysql.h>

#include <iostream>

#define VERSION 322
#define REINTENTOS 20
#define ESPERAR 2
#define LOG_MYSQL false

#define INTENTOS_QUERY 3
#define INTENTO_TIMEOUT_US 100000

#define MYSQL_LOG_FILE const_cast<char*>("mysql.log")

class _sql
{
  private:
    MYSQL conexion;           // Conexion a MySQL server. Utilizar _mysql.retornar_conexion().
    MYSQL_RES* resultado;     // Puntero a la estructura que contiene el resultado de una consulta.
    MYSQL_ROW registro;       // Estructura utilizada para manejar un registro en particular.
    MYSQL_FIELD* campos;      // Estructura que contiene la definicion de los campos.
    long cant_campos;         // Cantidad de campos retornados por la ultima consulta.
    long long cant_registros; // Cantidad de registros retornados por la ultima consulta.
    class _log_sistema log;
    bool resultado_almacenado;
    long ultimo_acceso;
    class _jul_date jdate;
    MYSQL cn_aux;

  public:
    _sql();
    // Prueba4
    ~_sql()
    {
        liberar_resultado();
    }
    void obtener_conexion(MYSQL con)
    {
        conexion = con;
    } // Establecer la conexion provista por un objeto _mysql.
    void cerrar_conexion();
    // int ejecutar_sql(char *sql);
    int ejecutar_sql(const char* sql);
    int ejecutar(const char* sql);
    // int ejecutar(char *sql);                                 //Ejecutar una consulta SQL.
    unsigned int error();                           // Retornar el codigo del error presentado.
    int grabar_resultado();                         // Capturar el resultado de una consulta SQL.
    void liberar_resultado();                       // Liberar la memoria utilizada para almacenar el resultado.
    long long cantidad_registros();                 // Obtener la cantidad de registros del resultado de la consulta.
    long cantidad_campos();                         // Obtener la cantidad de campos de la consulta.
    void grabar_campos();                           // Cargar los campos en la estructura 'campos'.
    int resultado_consulta();                       // Obtener el resultado de la consulta, los campos y longitudes.
    long indice_campo(char* nombre);                // Obtener el indice de un campo sabiendo su nombre.
    long longitud_campo(long indice);               // Obtener la longitud el tipo de dato del campo.
    char* valor(long ind_campo, long num_registro); // Obtener el valor de un campo de un registro en particular.
    char* retornar_valor(
        long ind_campo, long num_registro); // Devolver un string con el valor de un campo de un registro en particular.
    long retornar_ultimo_acceso();
    void actualizar_ultimo_acceso();
    void guardar_nombre_legajo(MYSQL_ROW fila);
    MYSQL_RES* get_last_result();
};

class _mysql
{
  private:
    MYSQL* conexion; // Conexion a MySQL server.
    class _log_sistema log;
    int estado; // 0=cerrado, 1=abierto, 2=base seleccionada
    long ultimo_acceso;

  public:
    int seleccionar_base(char* base); // Seleccionar base de datos por defecto
    int conectar(char* host, char* usuario, char* password,
                 unsigned int port); // Conectarse al motor de base de datos (local o remoto).
    void desconectar();              // Desconectarse del motor de base de datos.
    unsigned int error();            // Retornar el codigo del error presentado.
    MYSQL retornar_conexion()
    {
        return *conexion;
    }
    char* status();
    int host();
    _mysql();
};

#endif
