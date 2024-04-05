#include "../include/comunicacion.hpp"

_comunicacion::_comunicacion(string new_log_tag)
{
    descriptor = 0;
    consulta = true;
    error_lectura = 0;
    log_tag = new_log_tag;
}

_comunicacion::_comunicacion()
{
    descriptor = 0;
    consulta = true;
    error_lectura = 0;
    log_tag = string("Comunicaciones:\t");
}

_cabecera::_cabecera()
{
    long ln;
    memcpy(version, "0100", 4);
    ln = sizeof(datos);
    memcpy(&datos, " ", ln);
}

void _cabecera::retornar_version(char* ver)
{
    long ln;
    ln = sizeof(version);
    ltrim(ver, version, ln);
}

/*!
 * Guarda en 'tuberia' el campo tuberia presente en la cabecera.
 * @param char* tuberia
 */
void _cabecera::retornar_tuberia_respuesta(char* tuberia)
{
    long ln;

    strcpy(tuberia, "");
    ln = sizeof(datos.tuberia_respuesta);
    if (ln)
        ltrim(tuberia, datos.tuberia_respuesta, ln);
}
/*!
 * Guarda en 'comando' el comando presente en la cabecera.
 * @param char* comando
 */
void _cabecera::retornar_comando(char* comando)
{
    long ln;
    ln = sizeof(datos.comando);
    ltrim(comando, datos.comando, ln);
}

/*!
 * Devuelve la longitud de los datos presentes en la cabecera.
 * @return long longitud de cabecera
 */
long _cabecera::retornar_longitud()
{
    long ln;
    char aux[21];

    ln = sizeof(datos.longitud);
    ltrim(aux, datos.longitud, ln);

    return atol(aux);
}

void _cabecera::cargar(char* comando_, long longitud_parametros_, long secuencia_, const char* tuberia_respuesta_)
{
    class _hora hora;
    class _fecha fecha;
    char aux[22], dato[22], modif[10];

    fecha.tomar_fecha_char(aux, true);
    modificador(modif, STRING, sizeof(datos.fecha));
    sprintf(dato, modif, aux);
    memcpy(datos.fecha, dato, sizeof(datos.fecha));

    hora.tomar_hora_char(aux, true);
    modificador(modif, STRING, sizeof(datos.hora));
    sprintf(dato, modif, aux);
    memcpy(datos.hora, dato, sizeof(datos.hora));

    modificador(modif, STRING, sizeof(datos.tuberia_respuesta));
    sprintf(dato, modif, tuberia_respuesta_);
    memcpy(datos.tuberia_respuesta, dato, sizeof(datos.tuberia_respuesta));

    modificador(modif, STRING, sizeof(datos.comando));
    sprintf(dato, modif, comando_);
    memcpy(datos.comando, dato, sizeof(datos.comando));

    modificador(modif, LONG, sizeof(datos.longitud));
    sprintf(dato, modif, longitud_parametros_);
    memcpy(datos.longitud, dato, sizeof(datos.longitud));

    modificador(modif, LONG, sizeof(datos.secuencia));
    sprintf(dato, modif, secuencia_);
    memcpy(datos.secuencia, dato, sizeof(datos.secuencia));
}

/*!
 * Cierra la tuberia y la eliminar si no es una consulta.
 *
 */
void _comunicacion::cerrar()
{

    if (descriptor)
    {
        close(descriptor);
        if ((strcmp(archivo, "") > 0) && (!consulta))
        {
            unlink(archivo);
        }
    }
}

/*!
 *Recrear y abre la tuberia a utilizar.
 * @return int ( error == 0 ) ( exito == 1 )
 */
int _comunicacion::abrir(const char* nombre, int modo, bool consultar)
{
    int rtamkfifo;
    int rta, fd;

    consulta = consultar;

    if (!consulta)
    {
        unlink(nombre);
        rtamkfifo = mkfifo(nombre, S_IFIFO);

        if (rtamkfifo < 0)
            perror("*****Perror mkfifo");

        chmod(nombre, S_IRWXU | S_IRWXG | S_IRWXO);
    }

    fd = open(nombre, modo);

    if (fd > 0)
    {
        rta = 1;
        descriptor = fd;

        strcpy(archivo, nombre);
    }
    else
    {
        rta = 0;
        descriptor = 0;
        consulta = consultar;

        if (strcmp(nombre, "1") != 0)
        {
            char log_buffer[512];

            sprintf(log_buffer, "%sNo se pudo abrir tuberia '%s' (%s)", log_tag.c_str(), nombre, strerror(errno));
            log.grabar(log_buffer, Error_Log);
        }

        if (modo == LECTURA && consultar)
            consulta = false;
    }

    return rta;
}

/*!
 * Objetivo: esperar la llegada de un comando.
 * @param int sec segundos
 * @param int usec microsegundos
 * @return int 1 Si hay dato disponible en la tuberia.
 *             0 Si no hay dato y/o sale por timeout.
 */
int _comunicacion::esperar(int sec, int usec)
{
    fd_set r_fds;
    struct timeval tv;
    int rta = 0;

    if (error_lectura > 3)
    {
        cerrar();
        abrir(archivo, LECTURA, true);
        error_lectura = 0;
    }
    if (descriptor)
    {
        FD_ZERO(&r_fds);
        FD_SET(descriptor, &r_fds);

        if ((sec + usec) > 0)
        {
            tv.tv_sec = sec;
            tv.tv_usec = usec;
        }
        else
        {
            tv.tv_sec = tout_sec_com;
            tv.tv_usec = tout_usec_com;
        }

        rta = select(descriptor + 1, &r_fds, NULL, NULL, ((tv.tv_sec + tv.tv_usec) > 0) ? &tv : NULL);
        if (rta)
        {
            if (FD_ISSET(descriptor, &r_fds))
            {
                rta = 1;
            }
        }
    }
    else
    {
        rta = 0;
    }

    return rta;
}

/*!
 * Objetivo: Recibir los bytes enviados mediante la "named pipe", guardando en cabecera_resp_com la cabecera leída.
 * @param _cabecera *cabecera_resp_com
 * @param int sec segundos a esperar
 * @param int usec microsegundos a esperar
 * @return int Cantidad de bytes recibidos;
 */
int _comunicacion::leer_cabecera(class _cabecera* cabecera_resp_com, int sec, int usec)
{

    int rta = 0;
    char ver[10];

    // Esperar la llegada de datos desde la tuberia.
    rta = esperar(sec, usec);

    if (rta)
    {
        // Primero se lee la version del protocolo.
        rta = read(descriptor, &cabecera_resp_com->version, sizeof(cabecera_resp_com->version));
        cabecera_resp_com->retornar_version(ver);

        // Aqui se deberia analizar la version para determinar la estructura
        // de datos restante de la cabecera.
        if (rta > 0)
        {
            if (strcmp(ver, "0100") == 0)
            {
                // Leer datos de la cabecera correspondiente a la version 01 revision 00.
                rta = read(descriptor, &cabecera_resp_com->datos, sizeof(cabecera_resp_com->datos));
            }
            else
            {
                rta = 0;
                char log_buffer[512];
                sprintf(log_buffer, "%s", "Error en version de protocolo");
                log.grabar(log_buffer, Error_Log);
            }
        }
        if (!rta)
            error_lectura++;
    }

    return rta;
}

/*!
 * Objetivo: Recibir los bytes enviados mediante la "named pipe", guardando en buffer_resp_com la longitud
 * enviada como parámetro del cuerpo presente en la named pipe.
 *
 * @param unsigned char *buffer_resp_com
 * @param long longitud
 * @return int Cantidad de bytes recibidos;
 */

int _comunicacion::leer_cuerpo(unsigned char* buffer_resp_com, long longitud)
{
    int i; //, error = 0, cab = 0;
    char msg[250], aux[10];
    long rta = 0;
    if (longitud && buffer_resp_com)
    {
        memset(reinterpret_cast<char*>(buffer_resp_com), 0x00, sizeof(buffer_resp_com));
        // Recibir la respuesta.
        rta = esperar(0, 0);
        if (rta)
        {
            rta = read(descriptor, buffer_resp_com, longitud);
            if (rta > 0)
            {
                if (LOG_COMANDOS_COM)
                {
                    strcpy(msg, "<- ");
                    for (i = 0; i < longitud; i++)
                    {
                        sprintf(aux, "%02x ", buffer_resp_com[i]);
                        strcat(msg, aux);
                    }
                    buffer_resp_com[longitud] = 0x00;
                    log.grabar(msg, Info_Log);
                }
            }
            else if (rta != longitud)
            {
                // Lectura de 0 bytes. (read 0)
                error_lectura++;
            }
        }
    }

    if (rta)
        rta = longitud;

    return rta;
}

/*!
 * Objetivo: Enviar un comando a un dispositivo mediante una "named pipe".
 *
 * @param char *comando
 * @param unsigned char *parametros
 * @param long ln_parametros
 * @param char *tuberia
 * @param long secuencia
 * @return int Cantidad de bytes enviados;
 */
int _comunicacion::enviar(char* comando, unsigned char* parametros, long ln_parametros, const char* tuberia,
                          long secuencia)
{
    long ln_cabecera;
    class _cabecera cabecera;

    unsigned char paquete[3000];

    cabecera.cargar(comando, ln_parametros, secuencia, tuberia);

    ln_cabecera = sizeof(cabecera);

    memcpy(&paquete[0], &cabecera, ln_cabecera);
    memcpy(&paquete[ln_cabecera], parametros, ln_parametros);

    return write(descriptor, const_cast<unsigned char*>(paquete), (ln_cabecera + ln_parametros));
}

/**
 * @brief Envio de comando mediante pipes a otro proceso
 *
 * @param dst pipe de destino
 * @param src pipe de origen
 * @param cmd comando a enviar
 * @param param parametros
 * @param param_size tamaño de parametros
 */
int _comunicacion::send_command(const char* dst, const char* src, system_command cmd, void* param, size_t param_size)
{
    char cmd_char[3];
    int ret_code;

    ret_code = abrir(dst, ESCRITURA, CONSULTA);

    if (ret_code)
    {
        sprintf(cmd_char, "%d", static_cast<int>(cmd));

        enviar(cmd_char, static_cast<unsigned char*>(param), static_cast<long>(param_size), src, 1);
        cerrar();
    }

    return ret_code;
}

/**
 * @brief Envio de comando mediante pipe a otro proceso. Wrapper, sin parametros de envio.
 *
 * @param dst pipe de destino
 * @param src pipe de origen
 * @param cmd comando a enviar
 */
int _comunicacion::send_command(const char* dst, const char* src, system_command cmd)
{
    return send_command(dst, src, cmd, NULL, 0);
}

/**
 * @brief Funcion, llamada por los procesos del sistema, para agregar o eliminar timeouts.
 * Se encarga de crear la estructura timeout y enviarsela al proceso timer, indicandole
 * si debe borrar o crear dicho timeout.
 *
 * @param cmd comando a ser accionado por el timeout
 * @param duration duracion del timeout
 * @param dst_pipe pipe de destino del comando
 * @param action Create_Timeout para crear timeout, Delete_Timeout para eliminarlo
 */
// void
// _comunicacion::manage_timeout( system_command cmd, int duration, string dst_pipe, timeout_action action )
// {
// 	timeout        tmt;
//    system_command timer_cmd;

//    if( action == Create_Timeout )
//       timer_cmd = Cmd_Timer_Add_Timeout;
//    else if( action == Delete_Timeout )
//       timer_cmd = Cmd_Timer_Delete_Timeout;
//    else
//       return;

// 	tmt.command       = cmd;
//    tmt.duration_time = duration;
//    sprintf( tmt.pipe_dst, "%s", dst_pipe.c_str() );

// 	if( abrir( PIPE_TIMER, ESCRITURA, CONSULTA ) )
// 	{
// 		char cmd_char[ 4 ];
// 		sprintf( cmd_char, "%d", timer_cmd );

// 		enviar(	cmd_char,
//                reinterpret_cast<unsigned char *>( &tmt ),
//                sizeof( tmt ),
//                const_cast<char *>( dst_pipe.c_str() ),
//                1
// 		);
// 		cerrar();
// 	}
// }
