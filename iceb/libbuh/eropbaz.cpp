/*$Id: eropbaz.c,v 1.10 2013/05/17 14:56:27 sasa Exp $*/
/*19.10.2009	22.12.2001	Белых А.И.	eropbaz.c
Выдача сообщения на экран об ошибке открытия базы данных
*/
#include	"buhl.h"


void  eropbaz(const char *imabaz,int kto,const char *name,
int vih) //0-завершение работы прогаммы 1-нет
{
char bros[512];
char bros1[512];

sprintf(bros,"%s:%d %s",gettext("Логин"),kto,name);
sprintf(bros1,"%s %s:",gettext("Ошибка открытия базы данных"),imabaz);
msql_error(&bd,bros1,bros);

if(vih == 0)
 {
  endwin();
  exit(1);
 }
}

/******************************************/

void  eropbaz(const char *imabaz,int kto,const char *name,
int vih, //0-завершение работы прогаммы 1-нет
FILE *ff)
{
char bros[512];
char bros1[512];

fprintf(ff,"%s:%d %s",gettext("Логин"),kto,name);
fprintf(ff,"%s %s:",gettext("Ошибка открытия базы данных"),imabaz);

sprintf(bros,"%s:%d %s",gettext("Логин"),kto,name);
sprintf(bros1,"%s %s:",gettext("Ошибка открытия базы данных"),imabaz);

msql_error(&bd,bros1,bros,ff);

if(vih == 0)
 {
  endwin();
  exit(1);
 }
}
