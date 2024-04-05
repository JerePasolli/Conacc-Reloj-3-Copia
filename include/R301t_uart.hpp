#ifndef R301T_UART_HPP
#define R301T_UART_HPP

#include "common.hpp"
#include "comunicacion.hpp"
#include "log.hpp"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

using namespace std;

#define GenImgUSleep 250000
#define Img2TzUSleep 500000
#define ReadSysUSleep 100000
#define R301t_TAG static_cast<const char*>("R301t: ")
unsigned char HEADER_0 = 0xEF;
unsigned char HEADER_1 = 0x01;
unsigned char ADDER_0 = 0xFF;
unsigned char ADDER_1 = 0xFF;
unsigned char ADDER_2 = 0xFF;
unsigned char ADDER_3 = 0xFF;
unsigned char PID_COMMAND_PACKET = 0x01;
unsigned char PID_DATA_PACKET = 0x02;
unsigned char PID_ACK_PACKET = 0x07;
unsigned char PID_END_PACKET = 0x08;
unsigned char LENGHT_0 = 0x00;
unsigned char LENGHT_1 = 0x03;
unsigned char INSTRUCTION_CODE = 0x0F;
unsigned char PSW_0 = 0x00;
unsigned char PSW_1 = 0x00;
unsigned char PSW_2 = 0x00;
unsigned char PSW_3 = 0x00;
unsigned char CHECKSUM = 0x13;

enum status
{
    Init_Status,
    Read_Status,
    Wait_Status,
    Enroll_Status,
    Find_Status
};

class R301t
{
  public:
    R301t();
    ~R301t();
    int start();

  private:
    _comunicacion *input_pipe, *output_pipe;
    _log_sistema log;
    _cabecera cabecera_resp_com;
    char log_buffer[512];
    bool exit_proc = false;
    long length_cab_com;
    unsigned char buff_answer_com[200], buff_data[256];
    char cmd_char_com[4];
    system_command cmd_com;
    unsigned char ReadSysPara[12] = {HEADER_0, HEADER_1,           ADDER_0,  ADDER_1, ADDER_2,
                                     ADDER_3,  PID_COMMAND_PACKET, LENGHT_0, 0x03,    0x0F};
    unsigned char GenImg[12] = {HEADER_0, HEADER_1,           ADDER_0,  ADDER_1, ADDER_2,
                                ADDER_3,  PID_COMMAND_PACKET, LENGHT_0, 0x03,    0x01};
    unsigned char Img2Tz[13] = {HEADER_0, HEADER_1,           ADDER_0,  ADDER_1, ADDER_2,
                                ADDER_3,  PID_COMMAND_PACKET, LENGHT_0, 0x04,    0x02};
    unsigned char RegModel[12] = {HEADER_0, HEADER_1,           ADDER_0,  ADDER_1, ADDER_2,
                                  ADDER_3,  PID_COMMAND_PACKET, LENGHT_0, 0x03,    0x05};
    unsigned char Store[15] = {HEADER_0,           HEADER_1, ADDER_0, ADDER_1, ADDER_2, ADDER_3,
                               PID_COMMAND_PACKET, LENGHT_0, 0x06,    0x06,    0x02};
    unsigned char TemplateNum[12] = {HEADER_0, HEADER_1,           ADDER_0,  ADDER_1, ADDER_2,
                                     ADDER_3,  PID_COMMAND_PACKET, LENGHT_0, 0x03,    0x1d};
    unsigned char Search[17] = {HEADER_0,
                                HEADER_1,
                                ADDER_0,
                                ADDER_1,
                                ADDER_2,
                                ADDER_3,
                                PID_COMMAND_PACKET,
                                LENGHT_0,
                                0x08,
                                0x04,
                                /*BufferID cambiar*/ 0x01,
                                /*StartPage cambiar*/ 0x00,
                                0x01,
                                /*pageNum cambiar*/ 0x0B,
                                0xB8};
    unsigned short calculateCheckSum(unsigned char* buff, int ln);
    int parseCmd(unsigned char buff[256], int ln, unsigned char data[256]);
    int sendAndRecieveCmd(unsigned char buff[256], int ln, unsigned char data[256]);
    int searchFinger(unsigned char memFound[2]);
    int enrollFinger(unsigned short memoryLocate);
};

#endif