/*$Id: mord1.c,v 5.15 2013/09/26 09:43:35 sasa Exp $*/
/*15.07.2015	24.08.2004	Белых А.И.	mord1.c
Расчёт и распечатка журналов ордеров 1,2,3 для бюджетных организаций
*/
#include <errno.h>
#include "buhg.h"

double mord1_rm(class SPISOK *spisok_dat,class SPISOK *shet,class masiv_din_double *mas_sum,FILE *ff);

extern short	startgodb; /*Стартовый год*/

int mord1(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str shet("");
static class iceb_tu_str nomer_mo("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
int kom=0;

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт мемориальных ордеров 1,2,3."));

VV.VVOD_SPISOK_add_MD(gettext("Номер мемориального ордера..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Номер счета.................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Дата начала........(д.м.г)..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.........(д.м.г)..."));//3

VV.VVOD_SPISOK_add_data(nomer_mo.ravno(),4);
VV.VVOD_SPISOK_add_data(shet.ravno(),32);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);      


kom=VV.vvod(0,1,1,-1,-1);

nomer_mo.new_plus(VV.data_ravno(0));
shet.new_plus(VV.data_ravno(1));
dat1.new_plus(VV.data_ravno(2));
dat2.new_plus(VV.data_ravno(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(bros);
   goto naz;
 
  case FK2:
  case PLU:
    break;

  case FK3:
    vibrek("Plansh",&shet);
    VV.data_plus(1,shet.ravno());
    goto naz;

  case FK4:
   VV.VVOD_clear_data();         
   goto naz;

  case FK10:
  case ESC:
    return(1);
     
  default:
   kom=0;
   goto naz;
 }
GDITE();
time_t tmmn=time(NULL);

int nomer_moi=nomer_mo.ravno_atoi();
if(nomer_moi < 1 && nomer_moi > 3)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Неправильно введён номер мемориального ордера!"));
  sprintf(strsql,"%d",nomer_moi);
  SOOB.VVOD_SPISOK_add_MD(strsql);
    
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }

if(shet.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён счёт !"));
  goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

/*создаём список дат*/
class SPISOK spis_dat;
short d=dn,m=mn,g=gn;
while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);
//  printw("%s\n",strsql);
  spis_dat.plus(strsql);
  dpm(&d,&m,&g,1);
 }

/*создаём список дебетовых счетов*/
class SPISOK spis_ds;
sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and sh like '%s%%' and deb <> 0. order by shk asc",
gn,mn,dn,gk,mk,dk,shet.ravno());
int kolstr=0;

class SQLCURSOR cur;
SQL_str row;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
int metka_nal_prov=0;
if(kolstr == 0)
  metka_nal_prov=1;
//printw("Список дебетовых счетов:\n");
while(cur.read_cursor(&row) != 0)
 {
  spis_ds.plus(row[0]);
//  printw("%s\n",row[0]);
 }
/*создаём список кредитовых счетов*/
class SPISOK spis_ks;
sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and sh like '%s%%' and kre <> 0. order by shk asc",
gn,mn,dn,gk,mk,dk,shet.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
  metka_nal_prov+=1;

if(metka_nal_prov == 2)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  goto naz;
 }
  
//printw("Список кредитовых счетов:\n");
while(cur.read_cursor(&row) != 0)
 {
  spis_ks.plus(row[0]);
//  printw("%s\n",row[0]);
 }

int kolih_debs=spis_ds.kolih();
int kolih_kres=spis_ks.kolih();
 

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;


/*Узнаём стартовое сальдо для счёта*/
sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns like '%s%%'",godn,shet.ravno());


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
double start_deb=0.;
double start_kre=0.;

while(cur.read_cursor(&row) != 0)
 {
  start_deb+=atof(row[1]);
  start_kre+=atof(row[2]);
 }

/*Узнаём сальдо на начало периода*/

d=1;
m=1;
g=godn;

while(sravmydat(d,m,g,dn,mn,gn) < 0)
 {

  sprintf(strsql,"select deb,kre from Prov where datp = '%04d-%02d-%02d' and sh like '%s%%'",
  g,m,d,shet.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  while(cur.read_cursor(&row) != 0)
   {
    start_deb+=atof(row[0]);
    start_kre+=atof(row[1]);
   }
  dpm(&d,&m,&g,1);
 }





/*создаём массивы для дебетовых и кредитовых сумм*/
class masiv_din_double kre_sum;
class masiv_din_double deb_sum;
if(kolih_kres != 0)
 kre_sum.make_class(spis_dat.kolih()*kolih_kres);
if(kolih_debs != 0)
 deb_sum.make_class(spis_dat.kolih()*kolih_debs);

/*смотрим проводки, заполняем массивы*/
d=dn; 
m=mn;
g=gn;
int nomer_dat=0;
int nomer_shk=0;
double suma_deb=0.;
double suma_kre=0.;
char denmesgod[64];
int kolih_dat=spis_dat.kolih();
int kolstr1=0;
for(int ii=0; ii < kolih_dat;ii++)
 {
  strzag(LINES-1,0,kolih_dat,++kolstr1);

  sprintf(strsql,"select shk,deb,kre from Prov where datp = '%04d-%02d-%02d' and sh like '%s%%'",
  g,m,d,shet.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  sprintf(denmesgod,"%02d.%02d.%04d",d,m,g);

  while(cur.read_cursor(&row) != 0)
   {
    if((nomer_dat=spis_dat.find(denmesgod)) < 0)
     {
      beep();
      printw("Не найдена дата %d.%d.%d в списке дат!\n",d,m,g);
      OSTANOV();
      continue;
     }    
    suma_deb=atof(row[1]);
    suma_kre=atof(row[2]);
  /*  
    printw("%d.%d.%d nomer_dat=%d %f %f\n",d,m,g,nomer_dat,suma_deb,suma_kre);
    printw("%s %s %s\n",row[0],row[1],row[2]);
    OSTANOV();
  */
    if(suma_deb != 0.)
     {
      if((nomer_shk=spis_ds.find(row[0],1)) < 0) 
       {
        beep();
        printw("Не найден счёт %s в списке дебетовых счетов!\n",row[0]);
        OSTANOV();
        continue;
       }
      deb_sum.plus(suma_deb,nomer_dat*kolih_debs+nomer_shk);
     }        
    else
     {
      if((nomer_shk=spis_ks.find(row[0],1)) < 0)
       {
        beep();
        printw("Не найден счёт %s в списке дебетовых счетов!\n",row[0]);
        OSTANOV();
        continue;
       }
      kre_sum.plus(suma_kre,nomer_dat*kolih_kres+nomer_shk);
     }
   }

  dpm(&d,&m,&g,1);
 }

GDITE();


FILE *ff;
char imaf[56];
sprintf(imaf,"mgo%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));

if(nomer_moi == 1)
 fprintf(ff,"%32sТипова форма N 380-авт (бюджет)\n","");
if(nomer_moi == 2)
 fprintf(ff,"%32sТипова форма N 381-авт (бюджет)\n","");
if(nomer_moi == 3)
 fprintf(ff,"%32sТипова форма N 382-авт (бюджет)\n","");
 
iceb_t_insfil("mord_start.alx",ff);

fprintf(ff,"\n%15s Меморіальний ордер N%s\n","",nomer_mo.ravno());
fprintf(ff,"%5s за період з %02d.%02d.%d р. по %02d.%02d.%d р.\n","",dn,mn,gn,dk,mk,gk);

if(nomer_moi == 1)
 fprintf(ff,"\nНакопичувальна відомість за касовими операціями\n\n");
if(nomer_moi == 2)
 fprintf(ff,"\nНакопичувальна відомість руху грошових коштів загального фонду\n\
в органах Державного казначейства України (установах банків)\n\n");

if(nomer_moi == 3)
 fprintf(ff,"Накопичувальна відомість руху грошових коштів спеціального фонду\n\
в органах Державного казначейства України (установах банків)\n\n");
  
fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x0F");  /*Ужатый режим*/

fprintf(ff,"Дебет субрахунку %s до кредіту субрахунків\n",shet.ravno());

double obor_deb=mord1_rm(&spis_dat,&spis_ds,&deb_sum,ff);

fprintf(ff,"\n\nКредіт субрахунку %s до дебету субрахунків\n",shet.ravno());

double obor_kre=mord1_rm(&spis_dat,&spis_ks,&kre_sum,ff);

fprintf(ff,"\x1B\x50"); /*10-знаков*/
fprintf(ff,"\x12");  /*Нормальный режим печати*/
fprintf(ff,"\n\n");

fprintf(ff,"\
Оборот за дебетом:                    %10.2f\n\
Оборот за кредітом:                   %10.2f\n\
Залишок на початок місяця:            %10.2f\n\
Залишок на кінець місяця:             %10.2f\n",
obor_deb,
obor_kre,
start_deb-start_kre,
start_deb-start_kre+obor_deb-obor_kre);

if(nomer_moi == 1)
  fprintf(ff,"\
Сумма оборотів за меморіальним ордером:%10.2f\n",obor_deb);

if(nomer_moi == 2 || nomer_moi == 3)
  fprintf(ff,"\
Сумма оборотів за меморіальним ордером:%10.2f\n",
obor_deb+obor_kre);




//imafn("mord_end.alx",&imaf_alx);

fprintf(ff,"\n\n");

iceb_t_insfil("mord_end.alx",ff);

podpis(ff);
fclose(ff);

//clear();
move(LINES-8,0);

printw("\n");
if(start_deb > start_kre)
 printw("\
Сальдо            %10.2f\n",start_deb-start_kre);
else
 printw("\
Сальдо            %10s %10.2f\n","",start_kre-start_deb);

printw("\
Обіг за період    %10.2f %10.2f\n\
Сальдо розгорнуте %10.2f %10.2f\n",
obor_deb,obor_kre,
start_deb+obor_deb,start_kre+obor_kre);


if(start_deb+obor_deb > start_kre+obor_kre)
 printw("\
Сальдо згорнуте   %10.2f\n",start_deb+obor_deb - start_kre-obor_kre);
else
 printw("\
Сальдо згорнуте   %10s %10.2f\n","",start_kre+obor_kre -start_deb-obor_deb);



printw_vr(tmmn);

OSTANOV();
oth->spis_imaf.plus(imaf);
sprintf(strsql,"Меморіальний ордер N%s",nomer_mo.ravno());

oth->spis_naim.plus(strsql);
iceb_t_ustpeh(oth->spis_imaf.ravno(0),0);
return(0);
}
/***********************************************/
/*Получение итога по горизонтали*/
/***********************************************/
double mord1_ig(int nomer_gor,int kolih_shet,class masiv_din_double *mas_sum)
{
double suma_gor=0.;
for(int ii=0; ii < kolih_shet; ii++)
  suma_gor+=mas_sum->ravno(nomer_gor*kolih_shet+ii); 

return(suma_gor);

}

/*************************************************************/
/*Распечатка массива*/
/***************************************************************/
/*возвращаем общюю сумму*/

double mord1_rm(class SPISOK *spisok_dat,class SPISOK *shet,class masiv_din_double *mas_sum,FILE *ff)
{
int kolih_shet=shet->kolih();

class iceb_tu_str liniq;
liniq.plus("----------------------");

for(int ii=0; ii < kolih_shet; ii++)
 liniq.plus("-----------");
 
fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"  Дата    |  Разом   |");

for(int ii=0; ii < kolih_shet; ii++)
 fprintf(ff,"%-*.*s|",iceb_tu_kolbait(10,shet->ravno(ii)),iceb_tu_kolbait(10,shet->ravno(ii)),shet->ravno(ii));

fprintf(ff,"\n");

fprintf(ff,"%s\n",liniq.ravno());
double suma=0.;
int kolih_dat=spisok_dat->kolih();
for(int nomer_dat=0; nomer_dat < kolih_dat; nomer_dat++)
 {
  if(mord1_ig(nomer_dat,kolih_shet,mas_sum) == 0.)
   continue;
  fprintf(ff,"%-10s %10.2f ",spisok_dat->ravno(nomer_dat),mord1_ig(nomer_dat,kolih_shet,mas_sum));

  for(int nomer_kol=0; nomer_kol < kolih_shet; nomer_kol++)
   {
    suma=mas_sum->ravno(nomer_dat*kolih_shet+nomer_kol);
    if(suma != 0.)
     fprintf(ff,"%10.2f ",suma);
    else
     fprintf(ff,"%10s ","");
   }
  fprintf(ff,"\n");    
 }
fprintf(ff,"%s\n",liniq.ravno());
double itogo=0.;

/*Расчитываем итоговую сумму*/
for(int nomer_dat=0; nomer_dat < kolih_dat; nomer_dat++)
  itogo+=mord1_ig(nomer_dat,kolih_shet,mas_sum);

fprintf(ff,"%*.*s %10.2f ",iceb_tu_kolbait(10,"Разом"),iceb_tu_kolbait(10,"Разом"),"Разом",itogo);

/*выводим итоги по колонкам*/
double itogo_kol=0.;

for(int nomer_sh=0; nomer_sh < kolih_shet; nomer_sh++)
 {
  itogo_kol=0.;
  for(int nomer_dat=0; nomer_dat < kolih_dat; nomer_dat++)
    itogo_kol+=mas_sum->ravno(nomer_dat*kolih_shet+nomer_sh);
  if(itogo_kol != 0.)
   fprintf(ff,"%10.2f ",itogo_kol);
  else
   fprintf(ff,"%10s ","");
 }
fprintf(ff,"\n");

return(itogo);

}
