#include "../include/common.hpp"

bool sigterm;
typedef std::mt19937 rng_type;
rng_type rng;

/**
 * @brief Rutina de recepcion de SIGTERM
 *
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
void sigterm_handler(int signum, siginfo_t* info, void* ptr)
{
    sigterm = true;
}
#pragma GCC diagnostic pop

/**
 * @brief Retorna string con el nombre del comando indicado por code
 *
 * @param code numero de comando
 * @return String con el nombre del comando
 */
std::string get_cmd_string_from_code(system_command code)
{
    std::string cmd_str;

    switch (code)
    {
    case Cmd_All_Null:
        cmd_str = std::string("null");
        break;

        /*Agregar todos los comandos utilizados*/

    default:
        cmd_str = std::string("default");
        break;
    }

    return cmd_str;
}

/**
 *  Funcion para crear el modificador utilizado en sprinf, printf, etc... para
 *  dar formato al dato... (ej.  "%05d", "%12s" );
 */
void modificador(char* modif, int tipo_dato, int cantidad)
{
    char aux[10];
    strcpy(aux, "%d");
    switch (tipo_dato)
    {
    case INT:
        strcat(aux, "d");
        break;
    case LONG:
        strcat(aux, "li");
        break;
    case STRING:
        strcat(aux, "s");
        break;
    }
#pragma GCC diagnostic ignored "-Wrestrict"
    strcpy(modif, "%");
    sprintf(aux, aux, cantidad);
    strcat(modif, aux);
#pragma GCC diagnostic pop
}

/**
 *  Funcion para eliminar los caracteres en blanco de una cadena.
 *  Elimina los blancos de la cadena2 y arma la cadena1.
 */
void ltrim(char* cadena1, char* cadena2, long longitud_cadena2)
{
    long l, cont = 0;
    bool saltear = false;
    for (l = 0; l < longitud_cadena2; l++)
    {
        if (cadena2[l] != ' ' || saltear)
        {
            if (saltear == false)
                saltear = true;
            cadena1[cont] = cadena2[l];
            cont++;
        }
    }
    cadena1[cont] = 0;
}

/**
 * @brief Funcion llamada para setear la rutina de accion
 * cuando una SIGTERM es recibida por algun proceso.
 *
 * Esto se hizo debido a que a veces la PC se apaga y, antes de
 * hacerlo, el SO envia esta señal a todos los procesos, entonces
 * dichos procesos tienen tiempo de retornar correctamente.
 *
 * La rutina de interrupcion es la funcion 'sigterm_handler', que
 * simplemente setea un flag de sigterm para que los procesos
 * se den cuenta de la señal recibida.
 *
 */
void set_sigaction()
{
    static struct sigaction _sigact;
    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sigterm_handler;
    _sigact.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &_sigact, NULL);
}

/**
 * @brief Funcion comun llamada para reiniciar el sistema
 */
void restart_conacc()
{
    char cmd[128];
    sprintf(cmd, "%s./%s", CMD_PATH, RESTART_CMD);
    system(cmd);
}

/**
 * @brief Funcion comun llamada para reiniciar el sistema operativo
 */
void reboot_os()
{
    system(REBOOT_CMD);
}

/**
 * @brief Funcion comun llamada para apagar el sistema operativo
 */
void shutdown_os()
{
    system(SHUTDOWN_CMD);
}

/**
 * @brief Inicializa el generador de numeros random
 *
 */
void seed_random_number()
{
    rng_type::result_type const seedval = time(NULL);
    rng.seed(seedval);
}

/**
 * @brief Retorna un numbero random entre 0 y 99 (inclusivo)
 *
 * @return int numero random
 */
int get_random_number()
{
    std::uniform_int_distribution<rng_type::result_type> udist(0, 99);

    return udist(rng);
}
