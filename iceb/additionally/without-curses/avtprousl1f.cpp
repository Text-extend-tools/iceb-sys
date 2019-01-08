/*$Id: avtprousl1.c,v 5.8 2013/05/17 14:55:56 sasa Exp $*/
/*14.05.2012	14.04.2009	Белых А.И.	avtprousl1.c
Автоматическое выполнение проводок для приходных документов подсистемы учёта услуг
*/
#include <errno.h>
#include "bezcurses.h"

extern masiv_din_double snn; /*Суммы по накладных*/

extern double   okrg1; /*Округление*/

void avtprousl1(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot)
{
double	prockor=0.;
class iceb_t_avp avp;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[1024];
if(ff_prot != NULL)
 fprintf(ff_prot,"Автоматическое выполнение проводок\n\
Дата документа:%d.%d.%d Номер документа:%s tipz=%d\n",dd,md,gd,nomdok,tipz);
 
/*Читаем шапку документа*/
sprintf(strsql,"select podr,kontr,kodop,nds,sumkor,sumnds,pn,nomdp from Usldokum where datd='%04d-%02d-%02d' and nomd='%s' and tp=%d",
gd,md,dd,nomdok,tipz);

if(readkey(strsql,&row,&cur) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"strsql=%s\n",strsql);
  printf("%s %d.%d.%d %s\n",gettext("Не найдена шапка документа!"),dd,md,gd,nomdok);
  return;
 }

int podr=atoi(row[0]);

if(SRAV(row[1],"00-",1) == 0)
  avp.kontr_v_dok.new_plus("00");
else
  avp.kontr_v_dok.new_plus(row[1]);

avp.kodop.plus(row[2]);
double sumkor=atof(row[4]);
avp.suma_nds_vv=atof(row[5]);
float pnds=atof(row[6]);
int metka_nds=atoi(row[3]);
avp.nomer_dok.new_plus(row[7]);
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Подразделение:%d Контрагент:%s Код операции:%s Процент НДС:%f\n\
Сумма НДС введённая вручную:%.2f Сумма корректировки:%.2f Метка НДС=%d\n",  
  podr,avp.kontr_v_dok.ravno(),avp.kodop.ravno(),pnds,avp.suma_nds_vv,sumkor,metka_nds);
 }



sprintf(strsql,"select metka,kodzap,kolih,cena,shetu,shsp,nz from Usldokum1  where datd='%04d-%02d-%02d' and nomd='%s' and tp=%d",
gd,md,dd,nomdok,tipz);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"strsql=%s\n",strsql);
  return;
 }
prockor=0.;
if(sumkor != 0)
 {
  prockor=sumkor*100./snn.suma();
 }

class SPISOK sp_prov;
class masiv_din_double sum_prov_dk;

short dp1=0,mp1=0,gp1=0;
double cena_v_dok=0.;
double suma_podt=0.;
double i_suma_podt=0.;
double itogo_pod=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"\nЗапись в документе:%s %s %s %s %s %s %s\n\
---------------------------------------------------------------\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
    if(atoi(row[0]) == 0)
     sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
    if(atoi(row[0]) == 1)
     sprintf(strsql,"select naius from Uslugi where kodus=%s",row[1]);
    if(readkey(strsql,&row1,&cur1) == 1)
     fprintf(ff_prot,"%s\n",row1[0]);
     
   }
  avp.clear();
  avp.shetuh.new_plus(row[4]);
  avp.shetsp.new_plus(row[5]);

  cena_v_dok=atof(row[3]);

  /*Читаем подтвержденное количество*/
 
  sprintf(strsql,"select datp,kolih from Usldokum2 where podr=%d and \
nomd='%s' and datd='%04d-%02d-%02d' and tp=%d and metka=%s and \
kodzap=%s and shetu='%s' and nz=%s",
  podr,nomdok,gd,md,dd,tipz,row[0],row[1],row[4],row[6]);
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr == 0)
    continue;

  dp1=mp1=gp1=0;
  i_suma_podt=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Подтверждающая запись:%s %s\n",row1[0],row1[1]);

    suma_podt=atof(row1[1])*cena_v_dok;
    suma_podt=okrug(suma_podt,okrg1);
    i_suma_podt+=suma_podt;

    rsdat(&dp1,&mp1,&gp1,row1[0],2);

    if(sravmydat(avp.dp,avp.mp,avp.gp,dp1,mp1,gp1) < 0)
     {
      avp.dp=dp1; avp.mp=mp1; avp.gp=gp1;
     }
   }
  itogo_pod+=i_suma_podt;
  avp.suma_bez_nds=i_suma_podt+i_suma_podt*prockor/100.;
  
  if(metka_nds == 0)
    avp.suma_nds=avp.suma_bez_nds*pnds/100.;
  avp.suma_s_nds=avp.suma_nds+avp.suma_bez_nds;
  avp.suma_uheta=avp.suma_bez_nds; /*В услугах нет суммы учёта*/

  if(tipz == 1)
   iceb_t_avp(&avp,"avtprousl1.alx",&sp_prov,&sum_prov_dk,ff_prot);
  if(tipz == 2)
   iceb_t_avp(&avp,"avtprousl2.alx",&sp_prov,&sum_prov_dk,ff_prot);
    
 }

zapmpr1(nomdok,avp.kodop.ravno(),podr,time(NULL),ICEB_MP_USLUGI,dd,md,gd,tipz,&sp_prov,&sum_prov_dk,ff_prot);

if(itogo_pod == 0.)
 {
  printf("%s\n%d.%d.%d %s\n",gettext("Документ не подтверждён!"),dd,md,gd,nomdok);
  return;
 }

}

