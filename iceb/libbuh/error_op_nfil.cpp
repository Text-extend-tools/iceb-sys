/*$Id: error_op_nfil.c,v 5.5 2013/05/17 14:56:27 sasa Exp $*/
/*09.11.2008	16.10.2003	Белых А.И.	error_op_nfil.c
Выдача аварийного сообщения при невозможности открытия настроечного файла
*/
#include        "buhl.h"

void error_op_nfil(const char *imafil,int koderrno,const char *soob)
{
char st1[1024];

VVOD SOOB(1);
sprintf(st1,"%s:%d !",gettext("Ошибка открытия файла"),koderrno);
SOOB.VVOD_SPISOK_add_MD(st1);

sprintf(st1,"%s",strerror(koderrno));
SOOB.VVOD_SPISOK_add_MD(st1);
SOOB.VVOD_SPISOK_add_MD(imafil);

if(soob != NULL)
 if(soob[0] != '\0')
  SOOB.VVOD_SPISOK_add_MD(soob);
soobshw(&SOOB,stdscr,-1,-1,0,1);

}
