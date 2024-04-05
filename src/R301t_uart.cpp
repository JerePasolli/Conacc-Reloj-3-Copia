#include "../include/R301t_uart.hpp"
#include <unistd.h>

// Compilacion del programa:
// g++ src/R301t_uart.cpp lib/log.cc lib/common.cc lib/fecha_hora.cc lib/comunicacion.cc -o build/R301t -lwiringPi

R301t::R301t()
{
    input_pipe = new _comunicacion(string(R301t_TAG));
    output_pipe = new _comunicacion(string(R301t_TAG));

    set_sigaction();
}

R301t::~R301t()
{
    delete input_pipe;
    delete output_pipe;
}

int R301t::start()
{
    int respUser;
    int confirmationCode;
    unsigned char memFingerChar[2];
    unsigned short memAlocate = 0x00;
    bool exitoF = false;
    char cmd_char[4];
    if (!input_pipe->abrir(PIPE_HUELLA, LECTURA, CREAR))
    {
        sprintf(log_buffer, "%sError al abrir pipe", R301t_TAG);
        log.grabar(log_buffer, Error_Log);
        exit_proc = true;
    }
    else if (wiringPiSetupGpio() == -1)
    {
        sprintf(log_buffer, "%sError inicializando WiringPi", R301t_TAG);
        log.grabar(log_buffer, Error_Log);
    }
    else
    {
        sprintf(log_buffer, "%sIniciando", R301t_TAG);
        log.grabar(log_buffer, Control_Log);
        pinMode(4, INPUT);
    }

    while (!exit_proc)
    {
        /*Si llega un cmd por el pipe lec*/
        if (input_pipe->leer_cabecera(&cabecera_resp_com, 0, 1000))
        {
            memcpy(buff_answer_com, "\0", sizeof(buff_answer_com));
            length_cab_com = cabecera_resp_com.retornar_longitud();
            length_cab_com = input_pipe->leer_cuerpo(buff_answer_com, length_cab_com);
            cabecera_resp_com.retornar_comando(cmd_char_com);
            cmd_com =
                static_cast<system_command>(atoi(cmd_char_com)); // Comando recibido en cmd_com

            /* Acciono segun el comando recibido */
            switch (cmd_com)
            {
            case Cmd_Fingerprint_Search:
                output_pipe->abrir(PIPE_RELOJ, ESCRITURA, CONSULTA);
            buscar:
                confirmationCode = searchFinger(memFingerChar);
                if (confirmationCode == 0x01)
                {
                    cout << "Coloque el dedo en el sensor" << endl;
                    output_pipe->send_command(PIPE_RELOJ, PIPE_HUELLA,
                                              Cmd_Reloj_Not_Finger_In_Sensor);
                    /*Espero a que apoye el dedo*/
                    while (digitalRead(23))
                        ; // Cambiar por un metodo menos intrusivo
                    goto buscar;
                }
                output_pipe->cerrar();
                break;
            case Cmd_Fingerprint_Enroll:
                /*Recibir datos del pipe y mandarlo a memAlocate*/
                memAlocate = (buff_answer_com[0] << 8) | buff_answer_com[1];
                enrollFinger(memAlocate);
                break;
            case Cmd_Fingerprint_System_Param:
                confirmationCode = sendAndRecieveCmd(ReadSysPara, 12, buff_data);
                sprintf(cmd_char, "%d", Cmd_Reloj_Fingerprint_found);
                output_pipe->enviar(cmd_char, buff_data, sizeof(buff_data),
                                    const_cast<char*>(PIPE_HUELLA), 1);
                break;
            case Cmd_Fingerprint_Nro_Huellas:
                confirmationCode = sendAndRecieveCmd(TemplateNum, 12, buff_data);
                sprintf(cmd_char, "%d", Cmd_Reloj_Fingerprint_found);
                output_pipe->enviar(cmd_char, buff_data, sizeof(buff_data),
                                    const_cast<char*>(PIPE_HUELLA), 1);
                break;
            case Cmd_Fingerprint_Compare:
                break;
            case Cmd_Fingerprint_Delete_One:
                break;
            case Cmd_Fingerprint_Delete_All:
                break;
            case Cmd_Fingerprint_Exit:
                exit_proc = true;
            default:
                break;
            }
        }
        if (!digitalRead(23))
        {
            sprintf(log_buffer, "%sPresencia en el sensor, buscando huella", R301t_TAG);
            log.grabar(log_buffer, Event_Log);

            confirmationCode = searchFinger(memFingerChar);
            cout << "memFinger: " << hex << setw(2) << setfill('0') << uppercase
                 << static_cast<int>(memFingerChar[0]) << hex << setw(2) << setfill('0')
                 << uppercase << static_cast<int>(memFingerChar[1]) << endl;
            sleep(2);
        }
    }
    sprintf(log_buffer, "%sSaliendo", R301t_TAG);
    log.grabar(log_buffer, Control_Log);

    return 0;
}

/**
 * @brief Calcula el CheckSum de buff
 *
 * @param buff cadane a la cual calcular checksum
 * @param ln tamaño de la cadena total
 * @return unsigned short sum Valor del checksum calculado
 */
unsigned short R301t::calculateCheckSum(unsigned char* buff, int ln)
{
    // ln es la longitud de la cadena sin contar el CheckSum
    unsigned short sum = 0;
    for (int i = 6; i < ln; i++)
    {
        sum += buff[i];
    }
    buff[ln] = (sum >> 8) & 0xFF;
    buff[ln + 1] = sum & 0xFF;
    return sum;
}

/**
 * @brief parsear el comando recibido por el dispositivo.
 *
 * @param buff Cadena a parsear
 * @param ln largo de cadena
 * @param data cadena en donde guardo data que enviaron
 * @return codigo de confirmacion
 */
int R301t::parseCmd(unsigned char buff[256], int ln, unsigned char data[256])
{
    int data_ln;
    int ret;
    unsigned short checkSum, aux;
    /*Los primeros 6 bytes son iguales siempre*/
    if (buff[0] != HEADER_0 || buff[1] != HEADER_1 || buff[2] != ADDER_0 || buff[3] != ADDER_1 ||
        buff[4] != ADDER_2 || buff[5] != ADDER_3 || buff[6] != PID_ACK_PACKET)
    {
        cerr << "Error al parsear [HEADERS]" << endl;
        return 1;
    }
    if ((buff[8] + 0x09) != ln)
    {
        cerr << "Error de tamaño de buffer" << endl;
        return 1;
    }

    /*El tamaño de la data, es lo que me dice - el checksum*/
    data_ln = buff[8] - 0x03;

    if (data_ln != 0x00)
    {
        for (int i = 0; i < data_ln; i++)
        {
            data[i] = buff[i + 10];
        }
    }
    checkSum = calculateCheckSum(buff, ln - 2);
    if ((buff[ln - 2] != ((checkSum >> 8) & 0xFF)) || (buff[ln - 1] != (checkSum & 0xFF)))
    {
        cout << "Error CheckSum" << endl;
    }

    return buff[9];
}

/**
 * @brief Envia los comando pasados como parametro y espera recibir comando y los parsea
 *
 * @param cmd Cadeana que se quiere enviar
 * @param ln longitud de cadena
 * @param sleep tiempo entre envio de cadena y recepcion de datos
 * @return codigo de confirmacion
 */
int R301t::sendAndRecieveCmd(unsigned char cmd[256], int ln, unsigned char data[256])
{
    int serial_port, j = 0, resp_length = 0;
    unsigned char buffer[256], confirmationCode;
    /*Open serial port serial0 with 57600 baudrate*/
    if ((serial_port = serialOpen("/dev/serial0", 57600)) < 0)
    {
        cerr << "No se pudo abrir el puerto serie." << endl;
        return 1;
    }
    /*Calcule the verification sum and insert in cmd*/
    calculateCheckSum(cmd, ln - 2);
    /*Send char into a finger sensor*/
    cout << "Host:";
    for (int i = 0; i < ln; i++)
    {
        serialPutchar(serial_port, cmd[i]);
        cout << " " << hex << setw(2) << setfill('0') << uppercase << static_cast<int>(cmd[i]);
    }
    cout << endl;

    while (serialDataAvail(serial_port) == 0)
    {
    }
    /*Recieve data from finger sensor*/
    cout << "R301t:";
    while (serialDataAvail(serial_port) > 0)
    {
        buffer[j] = serialGetchar(serial_port);
        cout << " " << hex << setw(2) << setfill('0') << uppercase << static_cast<int>(buffer[j]);
        resp_length = j + 1;
        j++;
        usleep(1500);
    }
    cout << endl;

    confirmationCode = parseCmd(buffer, j, data);
    switch (confirmationCode)
    {
    case 0x00:
        cout << "Ejecucion del comando satisfactoria" << endl;
        break;
    case 0x01:
        cout << "Error al enviar el paquete" << endl;
        break;
    case 0x02:
        cout << "Dedo no presentado en el sensor" << endl;
        break;
    case 0x03:
        cout << "Error Enroll Huella" << endl;
        break;
    case 0x06:
        cout << "Error al generar char por over-disorderly en imagen de huella" << endl;
        break;
    case 0x07:
        cout << "Error al generar char por lackness o imagen muy pequeña" << endl;
        break;
    case 0x08:
        cout << "Huellas no coinciden" << endl;
        break;
    case 0x09:
        cout << "No se pudo encontrar huella que coincida" << endl;
        break;
    case 0x0A:
        cout << "Error al combinar Chars" << endl;
        break;
    case 0x0B:
        cout << "Numero de PageID mas grande que library" << endl;
        break;
    case 0x0C:
        cout << "Error leyendo Template o Template invalido" << endl;
        break;
    case 0x0D:
        cout << "Error subiendo Template" << endl;
        break;
    case 0x0E:
        cout << "Modulo no pudo los paquetes de datos" << endl;
        break;
    case 0x0F:
        cout << "Eror subiendo imagen" << endl;
        break;
    case 0x10:
        cout << "Error eliminando Template" << endl;
        break;
    case 0x11:
        cout << "Error limpiando libreria de huellas" << endl;
        break;
    case 0x13:
        cout << "Contrasena incorrecta" << endl;
        break;
    case 0x15:
        cout << "Error generando imagen por falta de una imagen primaria valida" << endl;
        break;
    case 0x18:
        cout << "Error escribiendo memoria flash" << endl;
        break;
    case 0x19:
        cout << "Error de no definicion" << endl;
        break;
    case 0x1A:
        cout << "Numero de registro invalido" << endl;
        break;
    case 0x1B:
        cout << "Configuracion de registro incorrecta" << endl;
        break;
    case 0x1C:
        cout << "Numero de pagina de NotePad incorrecta" << endl;
        break;
    case 0x1D:
        cout << "Error para manejar la comunicacion del puerto" << endl;
        break;
    default:
        break;
    }
    serialClose(serial_port);
    return confirmationCode;
}

/**
 * @brief Funcion para buscar una huella en toda la memoria del lector
 *
 * @param unsignedshort Memoria en donde se ubica la huella encontrada
 *
 * @return Returna el valor de confirmacion 0 exito, demas son fallos
 */
int R301t::searchFinger(unsigned char memFound[2])
{
    int confirmationCode = 1;
    unsigned char buff_data[256];
    char cmd_char[4];
    output_pipe->abrir(PIPE_RELOJ, ESCRITURA, CONSULTA);
    if (!sendAndRecieveCmd(GenImg, 12,
                           buff_data)) // Generar imagen de huella
    {
        Img2Tz[10] = static_cast<unsigned char>(1);
        if (!sendAndRecieveCmd(Img2Tz, 13,
                               buff_data)) // Guardar imagen en buffer
        {
            confirmationCode = sendAndRecieveCmd(Search, 17,
                                                 buff_data); // Buscar buffer en memoria
            if (confirmationCode == 0)                       // Respuesta cuando encontro huella
            {
                memFound[0] = buff_data[0];
                memFound[1] = buff_data[1];
                cout << "Huella encontrada en posicion: " << dec << memFound << endl;
                sprintf(cmd_char, "%d", Cmd_Reloj_Fingerprint_found);
            }
            else if (confirmationCode == 9)
            { // Respuesta cuando no coincide
                cout << "Huella no encontrada" << endl;
                sprintf(cmd_char, "%d", Cmd_Reloj_Fingerprint_Not_found);
            }
            else
                sprintf(cmd_char, "%d", Cmd_Reloj_Fingerprint_Error);
        }
        else
            sprintf(cmd_char, "%d", Cmd_Reloj_Fingerprint_Error);
    }
    else
        sprintf(cmd_char, "%d", Cmd_Reloj_Fingerprint_Error);

    output_pipe->enviar(cmd_char, memFound, sizeof(memFound), const_cast<char*>(PIPE_HUELLA), 1);
    output_pipe->cerrar();
    return confirmationCode;
}

/**
 * @brief Funcion que se encarga de hacer enroll de un dedo nuevo en un espacio de memoria especificado
 *
 * @param memoryLocate parametro de memoria, deben ser 2 bytes
 * @return int codigo de confirmacion
 */
int R301t::enrollFinger(unsigned short memoryLocate)
{
    unsigned char buff_data[256];
    int confirmationCode;
    bool exitoF = false; // Flag que me permite determinar que se realizo con exito el ciclo
    /*Generar 2 imagenes, guardarlo en los 2 buffers*/
    output_pipe->abrir(PIPE_RELOJ, ESCRITURA, CONSULTA);
    for (int i = 1; i < 3; i++)
    {
        confirmationCode = sendAndRecieveCmd(GenImg, 12,
                                             buff_data); // Gen finger image
        if (!confirmationCode)
        {
            Img2Tz[10] = static_cast<unsigned char>(i);
            confirmationCode = sendAndRecieveCmd(Img2Tz, 13,
                                                 buff_data); // Save image i in bufferi
            if (confirmationCode)
                break;
            if (!confirmationCode && (i == 2))
                exitoF = true;
        }
    }
    if (exitoF)
    {
        /*Registrar los dos buffers como un template*/
        if (sendAndRecieveCmd(RegModel, 12, buff_data) == 0)
        {
            /*Todo esto de donde se guarda, se tiene que hacer una logica con la base de datos*/
            Store[11] = (memoryLocate >> 8) & 0xFF;
            Store[12] = memoryLocate;

            /*Guardar el template en la memoria indicada*/
            if (sendAndRecieveCmd(Store, 15, buff_data) == 0)
            {
                cout << "Huella almacenada correctamente en PageID: 0x" << hex << setw(2)
                     << setfill('0') << uppercase << static_cast<int>(Store[11]) << hex << setw(2)
                     << setfill('0') << uppercase << static_cast<int>(Store[12]) << endl;

                output_pipe->send_command(PIPE_RELOJ, PIPE_HUELLA, Cmd_Reloj_Enroll_Ok);
            }
            else
            {
                output_pipe->send_command(PIPE_RELOJ, PIPE_HUELLA, Cmd_Reloj_Enroll_Failed_Memory);
            }
        }
        else
        {
            output_pipe->send_command(PIPE_RELOJ, PIPE_HUELLA, Cmd_Reloj_Enroll_Failed);
        }
    }
    else
    {
        if (!digitalRead(23))
            output_pipe->send_command(PIPE_RELOJ, PIPE_HUELLA, Cmd_Reloj_Enroll_Failed);
        else
            output_pipe->send_command(PIPE_RELOJ, PIPE_HUELLA, Cmd_Reloj_Enroll_Failed_Finger);
    }
    output_pipe->cerrar();
    return confirmationCode;
}

int main()
{
    R301t r301t;
    return r301t.start();
}