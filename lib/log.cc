#include "../include/log.hpp"

_log_sistema::_log_sistema()
{
    sprintf(log_file, "%s", ARCHIVO_LOG);
}

int _log_sistema::grabar(char* log_str, log_type type)
{
    int handle;
    _fecha fecha;
    _hora hora;

    if (!GRABAR_LOG)
        return 1;

    switch (type)
    {
    case Info_Log:
        sprintf(contenido.tag, "%s", INFO_TAG);
        break;

    case Control_Log:
        sprintf(contenido.tag, "%s", CONTROL_TAG);
        break;

    case Error_Log:
        sprintf(contenido.tag, "%s", ERROR_TAG);
        break;

    case Event_Log:
        sprintf(contenido.tag, "%s", EVENT_TAG);
        break;

    case Debug_Log:
        sprintf(contenido.tag, "%s", DEBUG_TAG);
        break;

    default:
        sprintf(contenido.tag, "%s", DEFAULT_TAG);
        break;
    }

    if (type > Info_Log)
        cout << contenido.tag << " " << log_str << endl;

    fecha.tomar_fecha_char(contenido.fecha, true);
    contenido.sep1 = ' ';
    hora.tomar_hora_char(contenido.hora, true);
    contenido.sep2 = ' ';
    memset(contenido.mensaje, ' ', strlen(contenido.mensaje));
    contenido.sep3 = ' ';

    if (strlen(log_str) > 250)
        log_str[250] = 0;

    strcpy(contenido.mensaje, log_str);
    strcat(contenido.mensaje, "\n");

    handle = open(log_file, O_RDWR | O_CREAT | O_APPEND);
    if (handle)
    {
        int ret, offset;

        /* Diferencia entre el largo declarado de 'contenido.mensaje' y el largo real */
        offset = MENSAJE_SIZE - strlen(contenido.mensaje);

        ret = write(handle, static_cast<const void*>(&contenido), sizeof(contenido) - offset);
        close(handle);

        return ret;
    }
    else
        return 0;
}
