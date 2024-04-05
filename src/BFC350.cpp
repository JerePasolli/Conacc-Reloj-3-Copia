#include "../include/BFC350.hpp"
#include <cstdio>
#include <wiringPi.h>

// Compilacion del programa:
// g++ BFC350.cpp lib/log.cc lib/common.cc lib/fecha_hora.cc lib/comunicacion.cc -o build/BFC350 -lwiringPi

BFC350::BFC350()
{
    input_pipe = new _comunicacion(string(BFC350_TAG));
    output_pipe = new _comunicacion(string(BFC350_TAG));

    set_sigaction();
}

BFC350::~BFC350()
{
    delete input_pipe;
    delete output_pipe;
}

int BFC350::start()
{
    unsigned short confirmationCode, crcCode;

    if (wiringPiSetupGpio() == -1)
    {
        std::cerr << "Error al inicializar WiringPi." << std::endl;
        return 1;
    }
    /*Configuro los pines del UART3*/
    pinModeAlt(0, 4);
    pinModeAlt(1, 4);

    // crcCode = calculateCRC16(CMD_GET_SYSTEM_INFO, 17);
    // CMD_GET_SYSTEM_INFO[17] = (crcCode >> 8) & 0xFF;
    // CMD_GET_SYSTEM_INFO[18] = crcCode & 0xFF;
    confirmationCode = sendAndRecieveCmd(CMD_TEST, 19, 500000);
    std::cout << "Confirmation Code: " << std::hex << std::setw(4) << std::setfill('0')
              << static_cast<int>(confirmationCode);
              
    while (!exit_proc)
    {
        /*Si llega un cmd por el pipe lec*/
        /*if (input_pipe->leer_cabecera(&cabecera_resp_com, 0, 1000))
        {
            memcpy(buff_answer_com, "\0", sizeof(buff_answer_com));
            length_cab_com = cabecera_resp_com.retornar_longitud();
            length_cab_com = input_pipe->leer_cuerpo(buff_answer_com, length_cab_com);
            cabecera_resp_com.retornar_comando(cmd_char_com);
            cmd_com =
                static_cast<system_command>(atoi(cmd_char_com)); // Comando recibido en cmd_com
        

            switch (cmd_com)
            {
                case Cmd_Facerecognition_Get_Info:
                    confirmationCode = sendAndRecieveCmd(CMD_GET_SYSTEM_INFO, 19, 500000);
                    std::cout << "Confirmation Code: " << std::hex << std::setw(4) << std::setfill('0')
                            << static_cast<int>(confirmationCode);
                break;*/
                
                /*case Cmd_Facerecognition_Search:
                    output_pipe->abrir(PIPE_RELOJ, ESCRITURA, CONSULTA);
                buscar:
                    confirmationCode = searchFace(memFingerChar);
                    if (confirmationCode == 0x01)
                    {
                        
                    }
                    output_pipe->cerrar();
                    break;

                default:
                    break;

            }
        }*/
    }
    return 0;
}

/*int BFC350::searchFace(unsigned char memFound[2])
{
    int confirmationCode = 1;
    unsigned char buff_data[256];
    char cmd_char[4];
    output_pipe->abrir(PIPE_RELOJ, ESCRITURA, CONSULTA);



    return confirmationCode;
}*/

/**
 * @brief Calcula el CRC16 de buff
 *
 * @param buff cadane a la cual calcular checksum
 * @param ln tamaño de la cadena total
 * @return unsigned short sum Valor del CRC16 calculado
 */
unsigned short BFC350::calculateCRC16(unsigned char* buff, int ln)
{
    // ln es la longitud de la cadena sin contar el CheckSum
    unsigned int i;
    unsigned short result = 0;
    std::string result2;

    for (i = 0; i < ln; i++)
    {
        result = (result << 8) ^ CRC16Table[(result >> 8) ^ (unsigned short)*buff++];
    }

    std::ostringstream ss;
    ss << std::hex << std::setw(4) << static_cast<int>(result) << std::endl;
    result2 = ss.str();
    //result2 = result2[2] + result2[3] + result2[0] + result2[1];
    std::cout << result2 << std::endl;
   
    std::cout << "CRC16: " << std::hex << std::setw(4) << static_cast<int>(result) << std::endl;
    //std::cout << "CRC16_inverted: " << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(result2) << std::endl;
    return result;
}

/**
 * @brief parsear el comando recibido por el dispositivo.
 *
 * @param buff Cadena a parsear
 * @param ln largo de cadena
 * @param data cadena en donde guardo data que enviaron
 * @return codigo de confirmacion
 */
unsigned short BFC350::parseCmd(unsigned char buff[550], int ln, unsigned char data[256])
{
    short ret;
    if (buff[0] != HEADER_0 || buff[1] != HEADER_1)
    {
        std::cerr << "Error de Headers" << std::endl;
        return 1;
    }
    ret = (static_cast<unsigned short>(buff[17]) << 8) | buff[18];
    return ret;
}

/**
 * @brief Envia los comando pasados como parametro y espera recibir comando y los parse
 *
 * @param cmd Cadena que se quiere enviar
 * @param ln longitud de cadena
 * @param sleep tiempo entre envio de cadena y recepcion de datos
 * @return codigo de confirmacion
 */
int BFC350::sendAndRecieveCmd(unsigned char cmd[550], int ln, int sleep)
{
    int serial_port, j = 0, resp_length;
    unsigned char buffer[550], dataBuff[550];
    unsigned short CRC, confirmationCode;
    if ((serial_port = serialOpen("/dev/ttyUSB0", 115200)) < 0)
    {
        std::cerr << "No se pudo abrir el puerto serie." << std::endl;
        return 1;
    }
    /*Calcular CRC16*/
    CRC = calculateCRC16(cmd, ln - 2);
    cmd[ln - 2] = (CRC >> 8) & 0xFF;
    cmd[ln - 1] = CRC & 0xFF;

    std::cout << "Host:";
    for (int i = 0; i < ln; i++)
    {
        serialPutchar(serial_port, cmd[i]);
        std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(cmd[i]);
    }
    std::cout << std::endl;

    //usleep(1000000);
    while(!serialDataAvail(serial_port));
    /*Recibir datos del BFC350*/
    std::cout << "BFC350:";
    while (serialDataAvail(serial_port) > 0)
    {
        buffer[j] = serialGetchar(serial_port);
        std::cout << " | " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[j]);
        resp_length = j + 1;
        j++;
        usleep(1000);
    }
    std::cout << std::endl;

    confirmationCode = parseCmd(buffer, j, dataBuff);
    return confirmationCode;
}

int main()
{
    BFC350 bfc350;
    return bfc350.start();
}
