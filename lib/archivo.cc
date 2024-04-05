#include "../include/archivo.h"

int _archivo::abrir(char* nombre, int modo)
{
    int rta = 0, fd;

    fd = open(nombre, modo);
    if (fd > 0)
    {
        rta = 1;
        descriptor = fd;
        strcpy(archivo, nombre);
    }

    return rta;
}

void _archivo::cerrar()
{
    close(descriptor);
}

int _archivo::enviar(unsigned char* buffer, int longitud, int tout_seg, int tout_mseg, bool enviar_buffer_completo)
{
    int i = 0, rta = 1;

    fd_set w_fds;
    struct timeval tv;

    if (enviar_buffer_completo)
    {
        rta = write(descriptor, const_cast<unsigned char*>(&buffer[i]), longitud);
    }
    else
    {
        for (i = 0; ((i < longitud) && (rta)); i++)
        {
            FD_ZERO(&w_fds);
            FD_SET(descriptor, &w_fds);
            tv.tv_sec = tout_seg;
            tv.tv_usec = tout_mseg;
            rta = select(descriptor + 1, NULL, &w_fds, NULL, ((tout_seg + tout_mseg) > 0) ? &tv : NULL);
            if (rta)
            {
                rta = write(descriptor, const_cast<unsigned char*>(&buffer[i]), 1);
            }
        }
    }

    return rta;
}

void _archivo_por_linea::abrir(char* nombre)
{
    archivo.open(nombre);
}

void _archivo_por_linea::cerrar()
{
    archivo.close();
}

bool _archivo_config::retornar_bool(char* variable, bool por_defecto)
{
    bool rta;

    if (obtener_valor(variable))
    {
        rta = retornar_bool();
    }
    else
    {
        strcpy(ultimo_valor, (por_defecto) ? "SI" : "NO");
        rta = por_defecto;
    }

    return rta;
}

/*!
 * Lee el archivo de configuración en busca de la variable indicada en los parámetros cuyo valor es del tipo string,
 * y si no existe escribe el valor por defecto.
 *
 * @param char* variable
 * @param char* por_defecto
 *
 * @return char* valor
 */

char* _archivo_config::retornar_string(char* variable, char* por_defecto)
{

    if (!obtener_valor(variable))
    {
        strcpy(ultimo_valor, por_defecto);
    }

    return retornar_string();
}

/*!
 * Lee el archivo de configuración en busca de la variable indicada en los parámetros cuyo valor es del tipo int,
 * y si no existe escribe el valor por defecto.
 *
 * @param char* variable
 * @param char* por_defecto
 *
 * @return int* valor
 */
int _archivo_config::retornar_int(char* variable, int por_defecto)
{
    int rta;

    if (obtener_valor(variable))
    {
        rta = retornar_int();
    }
    else
    {
        sprintf(ultimo_valor, "%d", por_defecto);
        rta = por_defecto;
    }

    return rta;
}

/*!
 * Lee el archivo de configuración en busca de la variable indicada en los parámetros cuyo valor es del tipo long,
 * y si no existe escribe el valor por defecto.
 *
 * @param char* variable
 * @param char* por_defecto
 *
 * @return long* valor
 */
long _archivo_config::retornar_long(char* variable, long por_defecto)
{
    long rta;

    if (obtener_valor(variable))
    {
        rta = retornar_long();
    }
    else
    {
        sprintf(ultimo_valor, "%li", por_defecto);
        rta = por_defecto;
    }

    return rta;
}

/*!
 * Lee el archivo de configuración en busca de la variable indicada en los parámetros cuyo valor es del tipo bool,
 * y si no existe escribe el valor por defecto.
 *
 * @param char* variable
 * @param char* por_defecto
 *
 * @return bool* valor
 */
bool _archivo_config::retornar_bool()
{
    char aux[50];
    bool rta = false;

    upper(aux, retornar_string());
    if ((strcmp(aux, "SI") == 0) || (strcmp(aux, "TRUE") == 0) || (strcmp(aux, "VERDADERO") == 0))
    {
        rta = true;
    }

    return rta;
}

int _archivo_config::retornar_int()
{
    return atoi(retornar_string());
}

long _archivo_config::retornar_long()
{
    return atol(retornar_string());
}

void _archivo_config::upper(char* destino, char* origen)
{
    int ln, i;

    strcpy(destino, "");
    ln = strlen(origen);
    for (i = 0; i < ln; i++)
    {
        destino[i] = static_cast<char>(toupper(origen[i]));
    }
    destino[ln] = 0;
}

void _archivo_config::abrir(char* nombre)
{
    strcpy(archivo_config, nombre);
}

void _archivo_config::cerrar()
{
    strcpy(archivo_config, "");
}

void _archivo_config::obtener_valor(char* variable, char* valor, char* por_defecto)
{
    if (!obtener_valor(variable, valor))
    {
        strcpy(valor, por_defecto);
    }
}

bool _archivo_config::comparar_variable(char* variable)
{
    if (strcmp(variable, ultima_variable) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void _archivo_config::cargar_valor(char caracter)
{
    ultimo_valor[indice_valor] = caracter;
    ultimo_valor[indice_valor + 1] = 0;
    indice_valor++;
}

void _archivo_config::cargar_variable(char caracter)
{
    ultima_variable[indice_variable] = static_cast<char>(toupper(caracter));
    ultima_variable[indice_variable + 1] = 0;
    indice_variable++;
}

void _archivo_config::cerear_variable_valor()
{
    strcpy(ultima_variable, "");
    indice_variable = 0;
    strcpy(ultimo_valor, "");
    indice_valor = 0;
}

char* _archivo_config::retornar_string()
{
    return ultimo_valor;
}

bool _archivo_config::obtener_valor(char* variable, char* valor)
{
    bool rta;

    rta = obtener_valor(variable);
    strcpy(valor, (rta) ? retornar_string() : "");

    return rta;
}

bool _archivo_config::obtener_valor(char* variable)
{
    char linea[1024], var_aux[50];
    int ln_variable, i, ln_linea;
    bool rta = false;
    int etapa;
    bool salir;

    upper(var_aux, variable);

    ln_variable = strlen(var_aux);
    _archivo_por_linea::abrir(archivo_config);
    while ((archivo.getline(linea, 1023, '\n')) && (!rta))
    {
        salir = false;
        etapa = 0;
        cerear_variable_valor();
        ln_linea = strlen(linea);
        if (ln_linea >= ln_variable)
        {
            /*scpy( aux, linea, 1 );
            if( strcmp( aux, "#" ) != 0 ) {*/
            if ((linea[0] == '[') /*&& ( linea[ln_linea-1] == ']' )*/)
            {
                for (i = 0; (i < ln_linea) && (etapa != 4) && (!salir) && (!rta); i++)
                {
                    switch (etapa)
                    {
                    case 0: // Buscar el primer corchete que marca el inicio del nombre de la variable.
                        if (linea[i] == '[')
                        {
                            etapa = 1;
                        }
                        break;
                    case 1: // Cargar el nombre de la variable hasta que se encuentra el corchete que delimita su fin.
                        if (linea[i] == ']')
                        {
                            if (comparar_variable(var_aux))
                            {
                                etapa = 2;
                            }
                            else
                            {
                                salir = true;
                            }
                        }
                        else
                        {
                            cargar_variable(linea[i]);
                        }
                        break;
                    case 2: // Buscar el tercer corchete que indica el comienzo del valor de la variable.
                        if (linea[i] == '[')
                        {
                            etapa = 3;
                        }
                        break;
                    case 3: // Cargar el valor de la variable hasta encontral el corchete que marca su fin.
                        if (linea[i] == ']')
                        {
                            rta = true;
                        }
                        else
                        {
                            cargar_valor(linea[i]);
                        }
                        break;
                    }
                }
            }
        }
    }
    archivo.clear();
    _archivo_por_linea::cerrar();

    return rta;
}
