/*$Id: zapmpr1.c,v 5.21 2013/09/26 09:43:56 sasa Exp $*/
/*28.03.2013	32.09.1998	Белых А.И.	zapmpr1.c
Запись проводок из пямяти в базу
*/
#include        <math.h>
#include        "buhl.h"


void zapmpr1(const char *nn, //Номер документа
const char *kop, //Код операции
int skk,    //Код склада/подразделения
time_t vrem, //Время записи
const char *kto, //Метка подсистемы
short dd,short md,short gd, //Дата документа
int tipz, //1-приход 2-расход 0-не определено
class SPISOK *sp_prov, /*Список проводок*/
class masiv_din_double *sum_prov_dk, /*Суммы по дебету/кредиту проводов*/
FILE *ff_prot)
{
short		i;
short		d,m,g;
class iceb_tu_str datpr("");
class iceb_tu_str sh(""),shk("");
class iceb_tu_str kor1(""),kor2("");
class iceb_tu_str kom("");
double		deb,kre;
double		bb;
int		kolpr=0;
int		metkaprov=0;
int             kekv=0;

int kp=sp_prov->kolih();

if(kp == 0)
 return;
if(ff_prot != NULL)
  fprintf(ff_prot,"\n%s-kp=%d nn=%s kto=%s kop=%s Дата:%d.%d.%d\n",
  __FUNCTION__,kp,nn,kto,kop,dd,md,gd);

for(i=0 ; i < kp; i++)
 {

  polen(sp_prov->ravno(i),&datpr,1,'|');
  rsdat(&d,&m,&g,datpr.ravno(),0);

  polen(sp_prov->ravno(i),&sh,2,'|');

  polen(sp_prov->ravno(i),&shk,3,'|');

  polen(sp_prov->ravno(i),&kor1,4,'|');

  polen(sp_prov->ravno(i),&kor2,5,'|');

  polen(sp_prov->ravno(i),&kom,6,'|');
  polen(sp_prov->ravno(i),&kolpr,7,'|');

  polen(sp_prov->ravno(i),&metkaprov,8,'|');

  polen(sp_prov->ravno(i),&kekv,10,'|');
    
  if(ff_prot != NULL)    
   fprintf(ff_prot,"%s:%d.%d.%d %-4s %-4s %-4s %-4s %8.2f %8.2f %s\n",
   __FUNCTION__,d,m,g,sh.ravno(),shk.ravno(),kor1.ravno(),kor2.ravno(),sum_prov_dk->ravno(i*2),sum_prov_dk->ravno(i*2+1),kom.ravno());

//  sqlfiltr(kom,sizeof(kom));
  bb=sumvpr(sh.ravno(),shk.ravno(),dd,md,gd,nn,skk,kom.ravno_filtr(),vrem,metkaprov,kto);

  deb=sum_prov_dk->ravno(i*2);
  kre=sum_prov_dk->ravno(i*2+1);
  if(fabs(deb) > 0.009)
   deb-=bb;
  if(fabs(kre) > 0.009)
   kre-=bb;

  deb=okrug(deb,0.01);
  kre=okrug(kre,0.01);
         
  if(fabs(kre) > 0.009 || fabs(deb) > 0.009 )
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s:Записываем проводку: deb=%f kre=%f\n",__FUNCTION__,deb,kre);     
    zapprov(metkaprov,g,m,d,sh.ravno(),shk.ravno(),kor1.ravno(),kor2.ravno(),kto,nn,kop,deb,kre,kom.ravno_filtr(),kolpr,skk,vrem,gd,md,dd,tipz,kekv);
   }
 }

}
