#include "../include/mysqldb.hpp"

_mysql::_mysql()
{
    estado = 0;

    log.cambiar_archivo(MYSQL_LOG_FILE);
}

int _mysql::host()
{
    char* inf;

    inf = const_cast<char*>(mysql_get_host_info(conexion));

    if (inf == NULL)
        return 0;
    else
    {
        if (strcmp(inf, "mysql server has gone away") == 0)
            return 0;
        else
            return 1;
    }
}

char* _mysql::status()
{
    return const_cast<char*>(mysql_stat(conexion));
}

unsigned int _mysql::error()
{
    return mysql_errno(conexion);
}

/*!
 *
 * Funcion importante para realizar la conexion con la base de datos que se quiera.
 *  @param char * base
 * @return int rta -> 1 si fue exitoso.
 *                 -> 0 en caso de error.
 */
int _mysql::seleccionar_base(char* base)
{
    int rta = 0;
    char mensaje[250];

    if (estado == 1)
        rta = mysql_select_db(conexion, base);
    else
        rta = -1;

    if (rta == 0)
    {

        sprintf(mensaje, "Mysql sel: Base '%s' seleccionada", base);
        if (LOG_MYSQL)
            log.grabar(mensaje, Info_Log);

        estado = 2;
        rta = 1;
    }
    else
    {

        sprintf(mensaje, "Mysql sel: Error al seleccionar la base '%s'", base);
        if (LOG_MYSQL)
            log.grabar(mensaje, Error_Log);

        rta = 0;
    }

    return rta;
}

/*!
 *
 * Funcion que realiza la conexion con la base de datos que se pase como parámetro
 *  @param char * hots
 * @param char * usuario
 * @param char * password
 * @param unsigned int port
 * @return int rta -> 1 si fue exitoso.
 *                 -> 0 en caso de error.
 */
int _mysql::conectar(char* host, char* usuario, char* password, unsigned int port)
{
    if (estado == 1 || estado == 2)
        desconectar();

    conexion = mysql_init(static_cast<MYSQL*>(0L));

    if (conexion == 0L)
    {
        cout << "Failure in mysql_init..." << endl;
        return 0;
    }

    if (strcmp(host, "localhost"))
        conexion = mysql_real_connect(conexion, host, usuario, password, "dino", port, NULL, 0);
    else
        conexion = mysql_real_connect(conexion, host, usuario, password, NULL, 3333, NULL, 0);

    if (conexion == NULL)
    {
        estado = 0;
        return 0;
    }
    else
    {
        char mensaje[250];
        sprintf(mensaje, "MySQL con: Conectado. host='%s'. usuario='%s'", host, usuario);
        if (LOG_MYSQL)
            log.grabar(mensaje, Info_Log);
        estado = 1;
        return 1;
    }
}

/*!
 *
 * Cerrar la conexion con la base de datos.
 *
 */
void _mysql::desconectar()
{
    mysql_close(conexion);
    estado = 0;
}

void _sql::actualizar_ultimo_acceso()
{
    ultimo_acceso = time(NULL);
}

long _sql::retornar_ultimo_acceso()
{
    return ultimo_acceso;
}

void _sql::cerrar_conexion()
{
    liberar_resultado();
}

/*!
 *
 * Ejecutar consulta SQL y grabar resultado.
 *
 */
int
// _sql::ejecutar_sql( char *sql )
_sql::ejecutar_sql(const char* sql)
{
    int rta;

    rta = ejecutar(sql);

    if (rta)
        rta = resultado_consulta();

    return rta;
}

/*!
 *
 * retorna un valor específicico leído de un campo de base de datos
 *
 */
char* _sql::retornar_valor(long ind_campo, long num_registro)
{
    long indice, l_registro;

    if (longitud_campo(ind_campo))
    {
        indice = ind_campo;
        l_registro = num_registro;
        return valor(indice, l_registro);
    }

    return NULL;
}

/*!
 *
 * Compara todos los campos con el nombre que se le pasa como argumento. Si encuentra uno
 * igual, retorna el numero de índice donde se encuentra.
 *  @param char * nombre
 * @return int indice
 */
long _sql::indice_campo(char* nombre)
{
    long indice;
    int rta = 0;

    for (indice = 0; (indice < cant_campos) && (!rta); indice++)
    {
        if (strcmp(campos[indice].name, nombre) == 0)
            rta = 1;
    }

    if (rta)
        return indice - 1;
    else
        return -1;
}
/*!
 *
 * retorna la longitud del campo
 *  @param long indice
 * @return long longitud
 */
long _sql::longitud_campo(long indice)
{
    long longitud;

    if (indice < cant_campos)
    {
        if (campos)
            longitud = campos[indice].length;

        return longitud;
    }
    else
        return 0;
}

char* _sql::valor(long ind_campo, long num_registro)
{
    if ((ind_campo >= 0) && (ind_campo < cant_campos) && (num_registro >= 0) && (num_registro < cant_registros))
    {
        mysql_data_seek(resultado, num_registro);
        registro = mysql_fetch_row(resultado);
        return registro[ind_campo];
    }
    else
        return NULL;
}

int _sql::resultado_consulta()
{
    int rta;

    rta = grabar_resultado();
    if (rta)
    {
        cant_campos = cantidad_campos();
        cant_registros = cantidad_registros();
        grabar_campos();
    }
    return rta;
}

void _sql::grabar_campos()
{
    campos = mysql_fetch_fields(resultado);
}

_sql::_sql()
{
    resultado_almacenado = false;
    resultado = NULL;
    registro = NULL;
    campos = NULL;
    cant_campos = 0;
    cant_registros = 0;
    // ultimo_acceso = jdate.julian_date();
    ultimo_acceso = time(NULL);
}

/**
 * @brief Ejecuta la query sql
 *
 * @param sql query a ejecutar en formato de string
 * @return int 0 en caso de error
 */
int _sql::ejecutar(const char* sql)
// _sql::ejecutar( char *sql )
{
    int err, intentos;

    err = 1;
    intentos = 0;

    while (err > 0 && intentos < INTENTOS_QUERY)
    {
        err = mysql_query(&conexion, sql);
        intentos++;

        if (err)
            usleep(INTENTO_TIMEOUT_US);
    }

    if (err)
    {
        char log_buffer[512];
        sprintf(log_buffer, "%s%s", "MYSQL:     ", mysql_error(&conexion));
        log.grabar(log_buffer, Error_Log);

        sprintf(log_buffer, "%s%s", "MYSQL:     ", sql);
        log.grabar(log_buffer, Debug_Log);
        return 0;
    }
    else
    {
        ultimo_acceso = time(NULL);
        return 1;
    }
}

/*!
 *
 * retorna la cantidad de filas que tiene una la tabla.
 */
long long _sql::cantidad_registros()
{
    return mysql_num_rows(resultado);
}

long _sql::cantidad_campos()
{
    return mysql_num_fields(resultado);
}

void _sql::liberar_resultado()
{
    if (resultado_almacenado)
    {
        mysql_free_result(resultado);
        resultado_almacenado = false;
    }
}

int _sql::grabar_resultado()
{
    liberar_resultado();
    resultado = mysql_store_result(&conexion);

    if (resultado == NULL)
    {
        return 0;
    }
    else
    {
        resultado_almacenado = true;
        return 1;
    }
}

unsigned int _sql::error()
{
    return mysql_errno(&conexion);
}

MYSQL_RES* _sql::get_last_result()
{
    return this->resultado;
}