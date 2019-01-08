/*$Id: balans_f1.c,v 5.27 2013/09/26 09:43:28 sasa Exp $*/
/*30.03.2016	15.03.2005	Белых А.И.	balans_f1.c
Расчёт баланса по форме 1 (Додаток до Пооложення (стандарту) бухгалтерского обліку 2
*/
#include <errno.h>
#include "buhg.h"

void balans_f1_r(int metka_ras,int metkabd,const char *sheta,GLKTMP *PROM,int nomer_kod,double *suma_kod1,double *suma_kod2,SPISOK *kod);
void balans_f1_r1(int metkabd,const char *sheta,GLKTMP *PROM,double *k1,double *k2,int nomer_kod);
void balans_f1_r2(const char *kodi,double *k1,double *k2,int nomer_kod,double *suma_kod1,double *suma_kod2,SPISOK *kod);

extern char     *sbshet; //Список бюджетных счетов

void  balans_f1(int metkabd, //0-хозрасчёт 1-бюджет
short dk,short mk,short gk,
char *imaf,GLKTMP *PROM)
{
char strsql[1024];
SQL_str row;
SQLCURSOR cur;
class iceb_tu_str naim_org("");
class iceb_tu_str adres("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;


//читаем реквизиты организации
sprintf(strsql,"select naikon,adres,pnaim from Kontragent where kodkon='00'");

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  if(row[2][0] != '\0')
   naim_org.new_plus(row[2]);
  else
   naim_org.new_plus(row[0]);

  adres.new_plus(row[1]);
  
   
 }


class iceb_tu_str bros("");

sprintf(strsql,"select str from Alx where fil='glk_balans_n.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s glk_balans_n.alx!",gettext("Не найдены настройки"));
  iceb_t_soob(strsql);
  return;
 }

class SPISOK kod;
class SPISOK sheta;

class iceb_tu_str stroka("");
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&bros,1,'|') != 0)
   continue;
  if(bros.ravno()[0] == '\0')
   continue;
  kod.plus(bros.ravno());
  polen(row_alx[0],&bros,2,'|');
  iceb_t_fplus(1,bros.ravno(),&sheta,&cur_alx);

 }
double suma_kod1[kod.kolih()]; //Суммы по кодам
double suma_kod2[kod.kolih()]; //Суммы по кодам
memset(&suma_kod1,'\0',sizeof(suma_kod1));
memset(&suma_kod2,'\0',sizeof(suma_kod2));

for(int ii=0; ii < kod.kolih(); ii++) //Сначала считаем настройки со счетами
  balans_f1_r(0,metkabd,sheta.ravno(ii),PROM,ii,suma_kod1,suma_kod2,&kod);
for(int ii=0; ii < kod.kolih(); ii++) //Потом считаем настройки с номерами строк
  balans_f1_r(1,metkabd,sheta.ravno(ii),PROM,ii,suma_kod1,suma_kod2,&kod);



sprintf(strsql,"select str from Alx where fil='glk_balans.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки glk_balans.alx");
  return;
 }

sprintf(imaf,"b_f1_%d.lst",getpid());
FILE *ff_bal;
if((ff_bal = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
fprintf(ff_bal,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

int nomer_koda=0;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  stroka.new_plus(row_alx[0]);
  if(SRAV("Підприємство",stroka.ravno(),1) == 0)
   {
    iceb_tu_vstav(&stroka,naim_org.ravno(),14,66,1);
   }
  if(SRAV("Адреса",stroka.ravno(),1) == 0)
   {
    iceb_tu_vstav(&stroka,adres.ravno(),8,66,1);
   }
  if(strstrm(stroka.ravno(),"на_______") == 1)
   {
    sprintf(strsql,"%02d.%02d.%d р.",dk,mk,gk);
    iceb_tu_vstav(&stroka,strsql,29,68,1);
   }
  

  if(polen(stroka.ravno(),&bros,3,'|') == 0)
  if(polen(stroka.ravno(),strsql,sizeof(strsql),4,'|') == 0)
   if(bros.ravno()[0] != '\0' && strsql[0] == '\0')
    {
     bros.ud_lp(); //удаляем лидирующие пробелы
     if((nomer_koda=kod.find(bros.ravno())) < 0)
      {
       sprintf(strsql,gettext("Не найден код строки %s в настройках !"),bros.ravno());
       iceb_tu_vstav(&stroka,strsql,73,184,1);
//       iceb_t_soob(strsql);
      }
     else
      {
       sprintf(strsql,"%12.2f",suma_kod1[nomer_koda]);
       iceb_tu_vstav(&stroka,strsql,73,84,1);

       sprintf(strsql,"%12.2f",suma_kod2[nomer_koda]);
       iceb_tu_vstav(&stroka,strsql,86,97,1);

      }
    }  
  fprintf(ff_bal,"%s",stroka.ravno());
 }
podpis(ff_bal);
fclose(ff_bal);

}

/**********************************/
/*Расчёт*/
/**********************************/

void balans_f1_r(int metka_ras,int metkabd,const char *sheta,GLKTMP *PROM,
int nomer_kod,double *suma_kod1,double *suma_kod2,SPISOK *kod)
{
double k1=0.;
double k2=0.;
if(sheta[0] == '\0')
 return;

if(pole2(sheta,':') == 0) //если нет ":" значит суммируются уже посчитанные колонки
 {
  if(metka_ras == 0)
   return;
  balans_f1_r2(sheta,&k1,&k2,nomer_kod,suma_kod1,suma_kod2,kod);

  suma_kod1[nomer_kod]+=k1;
  suma_kod2[nomer_kod]+=k2;

  return;
 }

if(metka_ras == 1)
 return;

int kolpol=pole2(sheta,',');
if(kolpol == 0)
 {
  balans_f1_r1(metkabd,sheta,PROM,&k1,&k2,nomer_kod);
  suma_kod1[nomer_kod]+=k1;
  suma_kod2[nomer_kod]+=k2;
  return;
 }
 
class iceb_tu_str shet_metka("");
for(int i=0; i < kolpol; i++)
 {
  polen(sheta,&shet_metka,i+1,',');
  if(shet_metka.ravno()[0] == '\0')
   continue;
  balans_f1_r1(metkabd,shet_metka.ravno(),PROM,&k1,&k2,nomer_kod);
 }

suma_kod1[nomer_kod]+=k1;
suma_kod2[nomer_kod]+=k2;
}
/***********************/
/*                      */
/*************************/
void balans_f1_r1(int metkabd,const char *sheta,GLKTMP *PROM,double *k1,double *k2,
int nomer_kod)
{
class iceb_tu_str shet("");
class iceb_tu_str metka("");
class iceb_tu_str znak("");
polen(sheta,&znak,1,':');
polen(sheta,&shet,2,':');
polen(sheta,&metka,3,':');
int nom_shet;
for(nom_shet=0; nom_shet < PROM->masf.kolih(); nom_shet++)
 {
  if(SRAV(shet.ravno(),PROM->masf.ravno(nom_shet),1) != 0)
   continue;

  if(sbshet != NULL)
   {
    if(metkabd == 0) //хозрасчёт
     if(proverka(sbshet,PROM->masf.ravno(nom_shet),1,0) == 0)
       continue;

    if(metkabd == 1) //бюджет
     if(proverka(sbshet,PROM->masf.ravno(nom_shet),1,0) != 0)
       continue;
   }

  if(metka.ravno()[0] == 'k') //кредит
   {
    if(znak.ravno()[0] == '+')
     {
      *k1+=PROM->ssk.ravno(nom_shet);
      *k2+=PROM->kre_end.ravno(nom_shet);
     }
    if(znak.ravno()[0] == '-')
     {
      *k1-=PROM->ssk.ravno(nom_shet);
      *k2-=PROM->kre_end.ravno(nom_shet);
     }

   }
  if(metka.ravno()[0] == 'd') //дебит
   {
    if(znak.ravno()[0] == '+')
     {
      *k1+=PROM->ssd.ravno(nom_shet);
      *k2+=PROM->deb_end.ravno(nom_shet);
     }
    if(znak.ravno()[0] == '-')
     {
      *k1-=PROM->ssd.ravno(nom_shet);
      *k2-=PROM->deb_end.ravno(nom_shet);
     }
   }

    
 }

}
/**************************************/
/*Подсчёт итогов по кодам */
/***************************************/
void balans_f1_r2(const char *kodip,double *k1,double *k2,
int nomer_kod,double *suma_kod1,double *suma_kod2,SPISOK *kod)
{
*k1=*k2=0.;
int nom_kod=0;
short metka=1;
class iceb_tu_str kodi("");
kodi.new_plus(kodip);
int kolpol=pole2(kodi.ravno(),',');
if(kolpol == 0)
 {
  if(kodi.ravno_atoi() < 0)
   metka=-1;  

  kodi.udsimv("+-");
      
  if((nom_kod=kod->find(kodi.ravno())) < 0)
   return;

  *k1+=suma_kod1[nom_kod]*metka;
  *k2+=suma_kod2[nom_kod]*metka;

 }
class iceb_tu_str kodd("");

for(int i=0; i < kolpol; i++)
 {

  if(polen(kodi.ravno(),&kodd,i+1,',') != 0)
   break;
  if(kodd.ravno()[0] == '\0')
   continue;
  metka=1;
  if(atoi(kodd.ravno()) < 0)
   metka=-1;

//  ud_simv(kodd,"+-");
  kodd.udsimv("+-");
  if((nom_kod=kod->find(kodd.ravno())) < 0)
   continue;
  *k1+=suma_kod1[nom_kod]*metka;
  *k2+=suma_kod2[nom_kod]*metka;
 }
}
