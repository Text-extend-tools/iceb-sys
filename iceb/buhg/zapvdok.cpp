/* $Id: zapvdok.c,v 5.23 2013/08/13 05:50:01 sasa Exp $ */
/*22.03.2017    26.09.1997      Белых А.И.      zapvdok.c
Запись в документ
*/
#include	"buhg.h"

extern double	okrcn;  /*Округление цены*/

int zapvdok(short dd,short md,short gd,int skll,int nkk,
int km,const char nnn[],double kol,double cen,
const char ei[],double nds,int mnds,int voztar,int tipz,
int nomkarp,//номер карточки парного документа
const char *shet, //Счет списания/получения
const char *nomz, //Номер заказа
int metka_imp_tov, /*метка импортного товара 0-не импорт 1-импорт*/
const char *kdstv) /*код діяльності сільськогосподарського товаровиробника*/
{
char		strsql[1024];
cen=okrug(cen,okrcn);
nds=okrug(nds,0.01);


sprintf(strsql,"insert into Dokummat1 \
values ('%04d-%02d-%02d',%d,'%s',%d,%d,%.10g,%.10g,'%s',%d,%.10g,%d,%d,%ld,\
%d,%d,'%s','%s','%s',%d,'%s')",
gd,md,dd,skll,nnn,nkk,km,kol,cen,ei,voztar,nds,mnds,iceb_t_getuid(),time(NULL),
tipz,nomkarp,shet,"",nomz,metka_imp_tov,kdstv);

if(sql_zapis(strsql,1,0) != 0)
  return(1);
return(0);
}
