/* $Id: zapkar.c,v 5.17 2013/09/26 09:43:44 sasa Exp $ */
/*14.03.2013    26.09.1997      Белых А.И.      zapkar.c
Запись новой карточки в базу
Если вернули 0 - записали
	     1 - такая запись уже есть - не записали
             2 - ошибка записи
*/

#include	"buhg.h"


short zapkar(int skll,int  nkk,int km,double ccn,const char *eii,
const char *shu,double krt,double fas,int kodt,double nds,int mnds,
double cenav,
short denv,short mesv,short godv, //Дата ввода в эксплуатацию (для малоценки)
const char *innom, //Инвентарный номер (для малоценки)
const char *rnd, //Регистрационный номер для мед. препарата
const char *nomz, //Номер заказа
short deng,short mesg,short godg, //Конечная дата годности товара
int mv) //0-ввести новую 1-исправить старую
{
char		strsql[512];
extern double	okrcn;  /*Округление цены*/
time_t		vrem;
char		bros[512];
/*
printw("\nzapkar-skll:%d nkk:%d km:%d cnn:%f eii:%s shu:%s krt:%f fas:%f kodt:%d nds:%f mnds:%d cenav:%f mv:%d\n",
skll,nkk,km,ccn,eii,shu,krt,fas,kodt,nds,mnds,cenav,mv);
OSTANOV();
*/

ccn=okrug(ccn,okrcn);
nds=okrug(nds,0.01);
krt=okrug(krt,0.000001);
fas=okrug(fas,0.000001);
cenav=okrug(cenav,okrcn);
time(&vrem);

if(mv == 0)
  sprintf(strsql,"insert into Kart \
values (%d,%d,%d,%d,'%s','%s',%.10g,%.10g,%.10g,%.10g,%.10g,%d,%d,%ld,\
'%04d-%d-%d','%s','%s','%s','%04d-%02d-%02d')",
skll,nkk,km,mnds,eii,shu,ccn,cenav,krt,nds,fas,kodt,iceb_t_getuid(),vrem,
godv,mesv,denv,innom,rnd,nomz,godg,mesg,deng);

if(mv == 1)
  sprintf(strsql,"update Kart \
set \
sklad=%d,\
nomk=%d,\
kodm=%d,\
mnds=%d,\
ei='%s',\
shetu='%s',\
cena=%.10g,\
cenap=%.10g,\
krat=%.10g,\
nds=%.10g,\
fas=%.10g,\
kodt=%d,\
ktoi=%d,\
vrem= %ld,\
datv='%04d-%d-%d',\
innom='%s',\
rnd='%s',\
nomz='%s',\
dat_god='%04d-%02d-%02d' \
where sklad=%d and nomk=%d",
skll,nkk,km,mnds,eii,shu,ccn,cenav,krt,nds,fas,kodt,iceb_t_getuid(),vrem,
godv,mesv,denv,innom,rnd,nomz,godg,mesg,deng,
skll,nkk);

//printw("\n%s\n",strsql);
//OSTANOV();

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Такая запись уже есть
   {
    sprintf(bros,"%s-%s",__FUNCTION__,gettext("Карточка с такими реквизитами уже есть !"));    
    msql_error(&bd,bros,strsql);
   }
  else
   {      
    sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка"));
    msql_error(&bd,bros,strsql);
    return(2);
   }
  return(1);
 }

return(0);
}
