/*$Id: error_op_nfil.c,v 5.5 2013/05/17 14:56:27 sasa Exp $*/
/*09.11.2008	16.10.2003	Белых А.И.	error_op_nfilf.c
Выдача аварийного сообщения при невозможности открытия настроечного файла
*/
#include "bezcurses.h"

void error_op_nfil(const char *imafil,int koderrno,const char *soob)
{

printf("%s:%d !\n",gettext("Ошибка открытия файла"),koderrno);

printf("%s\n",strerror(koderrno));
printf("%s\n",imafil);

if(soob != NULL)
 if(soob[0] != '\0')
  printf("%s\n",soob);

}
