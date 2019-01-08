/* $Id: zappodu.c,v 5.12 2011-01-13 13:43:25 sasa1 Exp $ */
/*08.10.2004	20.04.2000	Белых А.И.	zappodu.c
Запись подтверждающих записей для услуг
Если вернули 0-записали
             1-нет
*/
#include        <pwd.h>
#include        <time.h>
#include	"buhg.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif

extern double	okrcn;  /*Округление цены*/

int zappodu(int tp, //1-приход 2-расход
short dd,short md,short gd, //Дата документа
short dp,short mp,short gp, //Дата подтверждения документа
const char nomd[], //Номер документа
int metka, //0-материал 1-услуга
int kodzap, //Код материалла/услуги
double kolih, //Количество
double cena, //Цена
const char ei[], //Единица измерения
const char shetu[], //Счет учета
int podr, //Подразделение
int nomzap)
{
time_t 		vrem;
char		strsql[512];

cena=okrug(cena,okrcn);

time(&vrem);

sprintf(strsql,"insert into Usldokum2 \
values (%d,'%04d-%02d-%02d','%04d-%02d-%02d','%s',%d,%d,%.10g,%.10g,'%s','%s',%d,%d,%ld,%d)",
tp,gd,md,dd,gp,mp,dp,nomd,metka,kodzap,kolih,cena,ei,shetu,podr,iceb_t_getuid(),vrem,nomzap);
//printw("\n%s\n",strsql);
//OSTANOV();

if(sql_zap(&bd,strsql) != 0)
 {
  char bros[512];
  sprintf(bros,"zappodu-%s",gettext("Ошибка записи !"));
  msql_error(&bd,bros,strsql);
  return(1);
 }
return(0);
}
