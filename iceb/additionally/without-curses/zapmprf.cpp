/* $Id: zapmpr.c,v 5.22 2013/08/13 05:50:23 sasa Exp $ */
/*01.03.2011	32.09.1998	Белых А.И.	zapmprf.c
Запись проводок в оперативную память
*/
#include        <math.h>
#include        "bezcurses.h"


void zapmpr(short g,short m,short d, //Дата
const char *sh, //Счет
const char *shk, //Счет корреспондент
const char *kor1, //Контрагент счета
const char *kor2, //Контрагент счета корреспондента
double deb, //Сумма в дебете
double kre, //Сумма в кредите
const char *kom, //Коментарий
int kolpr, //Количество проводок 1- для внебалансового счета 2-для балансового
int kekv, //код экономической классификации расходов
class SPISOK *sp_prov, /*Список проводок*/
class masiv_din_double *sum_prov_dk, /*Суммы по дебету/кредиту проводов*/
FILE *ff_prot)
{
char		bros[1024];
class iceb_tu_str kontr1("");
class iceb_tu_str kontr2("");
int		metkaprov=0;

if(kolpr == 1)
  metkaprov=-1;

kodkontr(&kontr1,&kontr2,kor1,kor2);

//Округлять будем при выполнении проводок
//deb=okrug(deb,0.01);
//kre=okrug(kre,0.01);

if(fabs(deb) > 0.009 && fabs(kre) > 0.009)
 {
  printf("%s-%s (%.2f) %s (%.2f) %s!\n",__FUNCTION__,gettext("Дебет"),deb,gettext("и кредит"),kre,gettext("не равны нолю"));
  printf("%s-%s!!\n",__FUNCTION__,gettext("Попытка сделать логически не правильную проводку"));
  return;
 }


memset(bros,'\0',sizeof(bros));

if(fabs(deb) > 0.009)
 sprintf(bros,"%d.%d.%d|%s|%s|%s|%s|%s|%d|%d|1|%d",
 d,m,g,sh,shk,kontr1.ravno(),kontr2.ravno(),kom,kolpr,metkaprov,kekv);

if(fabs(kre) > 0.009)
 sprintf(bros,"%d.%d.%d|%s|%s|%s|%s|%s|%d|%d|2|%d",
 d,m,g,sh,shk,kontr1.ravno(),kontr2.ravno(),kom,kolpr,metkaprov,kekv);

if(bros[0] == '\0')
  return;

if(ff_prot != NULL)
 fprintf(ff_prot,"%s:%s => deb=%f kre=%f\n",__FUNCTION__,bros,deb,kre);

int nomer=0;

if((nomer=sp_prov->find(bros)) < 0)
 {
  sp_prov->plus(bros);
  sum_prov_dk->plus(deb,-1);
  sum_prov_dk->plus(kre,-1);
 }
else
 {
  sum_prov_dk->plus(deb,nomer*2);
  sum_prov_dk->plus(kre,nomer*2+1);
 }

}


