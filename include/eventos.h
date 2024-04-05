#ifndef _EVENTOS_H
#define _EVENTOS_H

#include <stdio.h>
#include <string>

#include "mysqldb.hpp"

using std::string;
using std::cout;
using std::endl;

#define CHECK_IN_EVENT		0
#define CHECK_OUT_EVENT		1
#define GUARDIA_EVENT_SBT	2
#define GUARDIA_EVENT_SBDNI	3

class _eventos
{
	private:
		class 	_sql sql;
		class 	_fecha fecha_sistema;
		char 	fecha_actual[11];

	public:
		void 	obtener_conexion(MYSQL con);
		void 	cerrar_conexion();
        void    check_in( char *nombre, char* legajo );
		bool 	get_current_date(char *date_buffer);
		
};

#endif
