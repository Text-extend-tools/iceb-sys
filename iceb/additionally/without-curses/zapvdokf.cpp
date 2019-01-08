/* $Id: zapvdok.c,v 5.23 2013/08/13 05:50:01 sasa Exp $ */
/*22.03.2017    26.09.1997      Белых А.И.      zapvdokf.c
Запись в документ
*/
#include        <pwd.h>
#include        <time.h>
#include "bezcurses.h"

int zapvdok(short dd,short md,short gd,int skll,int nkk,
int km,const char nnn[],double kol,double cen,
const char ei[],double nds,int mnds,int voztar,int tipz,
int nomkarp,//номер карточки парного документа
const char *shet, //Счет списания/получения
const char *nomz) //Номер заказа
{
char		strsql[1024];

cen=okrug(cen,0.01);
nds=okrug(nds,0.01);


sprintf(strsql,"insert into Dokummat1 \
values ('%04d-%02d-%02d',%d,'%s',%d,%d,%.10g,%.10g,'%s',%d,%.10g,%d,%d,%ld,\
%d,%d,'%s','%s','%s',%d,'%s')",
gd,md,dd,skll,nnn,nkk,km,kol,cen,ei,voztar,nds,mnds,iceb_t_getuid(),time(NULL),
tipz,nomkarp,shet,"",nomz,0,"");

if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }
return(0);
}
