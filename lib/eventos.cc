#include "../include/eventos.h"

void _eventos::obtener_conexion(MYSQL con)
{
	sql.obtener_conexion(con);
	fecha_sistema.tomar_fecha_char_invertida(&fecha_actual[0], true);
}

/**
 * @brief Inserta un evento de entrada con los parametros dados
 */
void
_eventos::check_in( char *nombre, char* legajo )
{
	char sql_cmd[ 300 ], date[ 32 ];

	get_current_date( date );

	sprintf( sql_cmd, "INSERT INTO eventos( tipo_evento, nombre, legajo, fecha, hora, tarjeta, valida, transmitido, observaciones ) VALUES(0,'%s','%s',CURRENT_DATE(),CURRENT_TIME(),'12345','si','no','ok')",
	nombre, legajo);
	
	sql.ejecutar_sql( sql_cmd );
}

bool
_eventos::get_current_date( char *date_buffer )
{
	char sql_cmd[100];

	sprintf( sql_cmd, "%s", "SELECT DATE_FORMAT( CURRENT_DATE(), '%d/%m/%Y');" );
	
	if( sql.ejecutar_sql( sql_cmd ) )
	{
		if( sql.cantidad_registros() > 0 )
		{
			strcpy( date_buffer, sql.retornar_valor( 0, 0 ) );
			return true;
		}
		else
			return false;
	}
	else
		return false;
}