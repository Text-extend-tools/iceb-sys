/*$Id: buhkaogo.c,v 5.16 2013-04-18 09:26:03 sasa Exp $*/
/*13.07.2015	10.01.2008	Белых А.И.	buhkaogo.c
Расчёт "Картки алалітичного обліку готівкових операцій"
*/
#include        <errno.h>
#include        "buhg.h"
#define SHETKAS "30"

int buhkaogo_prov(int nomer_den,int kol_mes,int kol_shet_kekv,class masiv_din_double *prih,class masiv_din_double *rash);


int buhkaogo(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str dat1(""),dat2("");


VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Рaсчёт карточки аналитического учёта наличных операций"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 

naz:;

clear();
 
helstr(LINES-1,0,
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);


dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;
GDITE();
int kolstr=0;

/*Узнаём количество пар субсчёт-кекв*/
sprintf(strsql,"select distinct sh,kekv from Prov where datp >= '%04d-01-01' \
and datp <= '%04d-%02d-%02d' and sh like '%s%%'",gn,gk,mk,dk,SHETKAS);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
class SPISOK shet_kekv;
int kolstr1=0;
SQL_str row;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  sprintf(strsql,"%s|%s",row[0],row[1]);
  shet_kekv.plus(strsql);
 }
GDITE();
/*Создаём список день-месяц-год*/
class SPISOK mes_god;
short d=dn,m=mn,g=gn;

while( sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);
  mes_god.plus(strsql);
  dpm(&d,&m,&g,1);
 }

class masiv_din_double saldo;  /*Сальдо по счет-кекв на начало периода*/
saldo.make_class(shet_kekv.kolih());

int kol_mes=mes_god.kolih();
class masiv_din_double prih;  /*Сумма приходов за месяц*/
class masiv_din_double rash;  /*Сумма расходов за месяц*/

prih.make_class(kol_mes*shet_kekv.kolih());
rash.make_class(kol_mes*shet_kekv.kolih());

sprintf(strsql,"select datp,sh,deb,kre,kekv from Prov where datp >= '%04d-01-01' \
and datp <= '%04d-%02d-%02d' and sh like '%s%%'",gn,gk,mk,dk,SHETKAS);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

kolstr1=0;
int nomer_den=0;
int nomer_shk=0;
double deb=0.,kre=0.;
double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  rsdat(&d,&m,&g,row[0],2);
  

  sprintf(strsql,"%02d.%02d.%04d",d,m,g);
  nomer_den=mes_god.find(strsql);      
  
  sprintf(strsql,"%s|%s",row[1],row[4]);   
  nomer_shk=shet_kekv.find(strsql);

  deb=atof(row[2]);
  kre=atof(row[3]);
    
  if(sravmydat(d,m,g,dn,mn,gn) < 0)
   {
    if(deb != 0.)
     suma=deb;
    else
     suma=kre*-1;
     
    saldo.plus(suma,nomer_shk);
    continue;
   }  
  
  prih.plus(deb,nomer_shk*kol_mes+nomer_den);  
  rash.plus(kre,nomer_shk*kol_mes+nomer_den);  

 }
GDITE(); 
char imaf[64];
sprintf(imaf,"kaogo%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Карточка аналитического учёта наличных операций"),dn,mn,gn,dk,mk,gk,ff);

class iceb_tu_str shet("");
class iceb_tu_str kekv("");
double itogo_gor=0.;
class iceb_tu_str liniq;

liniq.plus("----------------------------");
int kol_shet_kekv=shet_kekv.kolih();

for(int i=0; i < kol_mes; i++)
 {

  if(buhkaogo_prov(i,kol_mes,kol_shet_kekv,&prih,&rash) == 0)
   continue;

  liniq.plus("---------------------------------");
 }
fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%s",gettext("Счёт   |КЭКЗ |Сальдо на |"));
for(int i=0; i < kol_mes; i++)
 {
  if(buhkaogo_prov(i,kol_mes,kol_shet_kekv,&prih,&rash) == 0)
   continue;
  fprintf(ff,"%-32s|",mes_god.ravno(i));
 }
fprintf(ff,"\n");

fprintf(ff,"%s",gettext("          |     |  начало  |"));
for(int i=0; i < kol_mes; i++)
 {
  if(buhkaogo_prov(i,kol_mes,kol_shet_kekv,&prih,&rash) == 0)
   continue;

  fprintf(ff,"%s",gettext(" Приход   | Расход   | Сальдо   |"));
 }
/*           1234567890 1234567890 1234567890 */
fprintf(ff,"\n");

fprintf(ff,"%s\n",liniq.ravno());

for(nomer_shk=0; nomer_shk < shet_kekv.kolih(); nomer_shk++)
 {
  polen(shet_kekv.ravno(nomer_shk),&shet,1,'|');
  polen(shet_kekv.ravno(nomer_shk),&kekv,2,'|');
  
  fprintf(ff,"%-*s %-5s",
  iceb_tu_kolbait(10,shet.ravno()),shet.ravno(),
  kekv.ravno());
  itogo_gor=saldo.ravno(nomer_shk);
  
  fprintf(ff," %10.2f",itogo_gor);
  for(nomer_den=0; nomer_den < kol_mes; nomer_den++)
   {
    if(buhkaogo_prov(nomer_den,kol_mes,kol_shet_kekv,&prih,&rash) == 0)
     continue;

    if(prih.ravno(nomer_shk*kol_mes+nomer_den) != 0.)
      fprintf(ff,"|%10.2f",prih.ravno(nomer_shk*kol_mes+nomer_den));
    else
      fprintf(ff,"|%10s","");
    
    if(rash.ravno(nomer_shk*kol_mes+nomer_den) != 0.)
      fprintf(ff," %10.2f",rash.ravno(nomer_shk*kol_mes+nomer_den));
    else
      fprintf(ff," %10s","");

    itogo_gor+=prih.ravno(nomer_shk*kol_mes+nomer_den)-rash.ravno(nomer_shk*kol_mes+nomer_den);
    if(itogo_gor != 0.)
      fprintf(ff," %10.2f",itogo_gor);
    else
      fprintf(ff," %10s","");
    
   }
  fprintf(ff,"|\n");
    
 }
fprintf(ff,"%s\n",liniq.ravno());
/*Печатаем горизонтальную итоговую строку*/
fprintf(ff,"%*s|%10.2f",iceb_tu_kolbait(16,gettext("Итого")),gettext("Итого"),saldo.suma());
double itogo_saldo=0.;
for(nomer_den=0; nomer_den < kol_mes; nomer_den++)
 {
  double itogo_prih=0.;
  double itogo_rash=0.;

  for(nomer_shk=0; nomer_shk < shet_kekv.kolih(); nomer_shk++)
   {
    itogo_prih+=prih.ravno(nomer_shk*kol_mes+nomer_den);
    itogo_rash+=rash.ravno(nomer_shk*kol_mes+nomer_den);     
//    itogo_saldo+=prih.ravno(nomer_shk*kol_mes+nomer_den)-rash.ravno(nomer_shk*kol_mes+nomer_den);
   } 
  itogo_saldo+=itogo_prih-itogo_rash;
  if(itogo_prih == 0. && itogo_rash == 0.)
   continue;
  fprintf(ff,"|%10.2f %10.2f %10.2f",itogo_prih,itogo_rash,itogo_saldo);

 }
fprintf(ff,"\n");
fprintf(ff,"\n\
  Виконавець _____________________   _______________________  _________________________      Перевірив _____________________   _______________________  _________________________\n\
                   (посада)                   (підпис)          (ініціали і призвище)                          (посада)                (підпис)           (ініціали і призвище)\n");
podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Карточка аналитического учёта наличных операций"));
iceb_t_ustpeh(imaf,3);
return(0);

}
/****************************/
/*Проверить колонки на ноль*/
/**************************/
/*Если вернули 0 пусто*/
int buhkaogo_prov(int nomer_den,int kol_mes,int kol_shet_kekv,class masiv_din_double *prih,class masiv_din_double *rash)
{

for(int nomer_shk=0; nomer_shk < kol_shet_kekv; nomer_shk++)
 {
  if(prih->ravno(nomer_shk*kol_mes+nomer_den) != 0.)
   return(1);
  if(rash->ravno(nomer_shk*kol_mes+nomer_den) != 0.)
   return(1);
 } 

return(0);
}
