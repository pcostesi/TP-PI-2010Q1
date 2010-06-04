/* SIMILAR A GETNUM, SOLO QUE SI A CONTINUACION DEL NUMERO HAY CARACTERES
** EXTRA QUE NO SEAN ESPACIOS, LO TOMA COMO INVALIDO
*/

#include <stdio.h>
#include "getnum.h"
#include <string.h>
#include <ctype.h>

#define BORRA_BUFFER while (getchar() != '\n')


/* Verifica que en el buffer solo hayan quedado espacios */
static int
finalBuffer()
{
	int c;
	int flag = 1;
	while ( ( c= getchar()) != '\n')
		if (! isspace(c))
			flag = 0;
	
	return flag;
}

/* lee un entero */
int 
getint(const char mensaje[], ... )
{
	int n, salir = 0;
	va_list ap;
		
	do	
	{
		va_start(ap, mensaje);
		vprintf(mensaje, ap);
		va_end(ap);

		if ( scanf("%d",&n) != 1)
			BORRA_BUFFER;
		else
			salir = finalBuffer();
		if ( !salir)
			printf("\nDato incorrecto\n");

	} while (! salir);
	return n;
}

/* lee un float */
float 
getfloat(const char mensaje[], ...)
{
	float n;
	int salir = 0;
	va_list ap;
		
	do	
	{
		va_start(ap, mensaje);
		vprintf(mensaje, ap);
		va_end(ap);

		if ( scanf("%g",&n) != 1)
			BORRA_BUFFER;
		else
			salir = finalBuffer();
		if ( !salir)
			printf("\nDato incorrecto\n");
	} while (! salir);
	return n;
}

/* lee un double */
double 
getdouble(const char mensaje[], ...)
{
	double n;
	int salir = 0;
	va_list ap;
		
	do	
	{
		va_start(ap, mensaje);
		vprintf(mensaje, ap);
		va_end(ap);

		if ( scanf("%lg",&n) != 1)
			BORRA_BUFFER;
		else
			salir = finalBuffer();
		if ( !salir)
			printf("\nDato incorrecto\n");
	} while (! salir);
	return n;
}

/* lee S,s,N, o n  */
int 
yesNo(const char mensaje[], ...)
{
	int op;
	int salir = 0;
	va_list ap;
		
	do	
	{
		va_start(ap, mensaje);
		vprintf(mensaje, ap);
		va_end(ap);

		op = getchar();
		if ( strchr("SsNn", op) == NULL)
		{
			printf("\nDato incorrecto\n");
			if (op != '\n')
				BORRA_BUFFER;
		}
		else
			salir = 1;
	} while (! salir);
	BORRA_BUFFER;
	return strchr("Ss", op) != NULL;
}
