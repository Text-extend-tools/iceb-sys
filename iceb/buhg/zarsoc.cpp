/* $Id: zarsoc.c,v 5.69 2014/02/28 05:13:48 sasa Exp $ */
/*02.09.2016	03.04.2000	Белых А.И.	zarsoc.c
Вычисление необходимых соц отчислений на фонд зарплаты и запись их в таблицу
*/
#include        "buhg.h"

void zarsoc_esv(short mr,short gr,int tabn);


extern double   okrg; /*Округление*/
extern char	*shetb; /*Бюджетные счета*/
extern short    *kodbl;  /*Код начисления больничного*/
extern class iceb_tu_str kod_dop_nah_bol; /*коды начилений дополнительно входящие в расчёт удержания ЕСВ с больничного*/

extern int kodf_esv;
extern int kodf_esv_bol;
extern int kodf_esv_bol_inv;
extern int kodf_esv_inv;
extern int kodf_esv_dog;
extern int kodf_esv_vs;
extern class iceb_tu_str kod_zv_gr;

extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_bol_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern int kod_esv_vs;
extern int zar_pr_metka_r;

void zarsoc(short mr,short gr,int tabn)
{

zar_pr_start(3,tabn,mr,gr);

if(sravmydat(1,mr,gr,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) >= 0)
 zarsoc_esv(mr,gr,tabn);
else
 {
  char strsql[512];
  sprintf(strsql,"%s-Начисления на фонд оплаты до %d.%d.%d не выполняются!\n",__FUNCTION__,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV);
  zar_pr_ins(strsql);
 }

zar_pr_end();

}
/************************************************/
void zarsoc_esv(short mr,short gr,int tabn)
{
char strsql[1024];
SQLCURSOR cur;
int kolstr=0;
SQL_str row;
int knah=0;
double suma=0.;
float dproc_sr=0.;
float dproc_bl=0.;
float dproc_in=0.;
float dproc_dg=0.;
float dproc_vs=0.;
float proc_vr=0.; /*процент взятый в расчёт*/
int kodf_esv_vr=0;

double suma_bol=0.;
double suma_bolb=0.;
double suma_vr=0.;
double suma_vrb=0.;

class iceb_tu_str knvr_vr("");
class iceb_tu_str shet_vr("");
class iceb_tu_str shetbu_vr(""); /*взятый в расчёт*/

class iceb_tu_str knvr("");
class iceb_tu_str shet("");
class iceb_tu_str shetbu("");

class iceb_tu_str knvr_bol("");
class iceb_tu_str shet_bol("");
class iceb_tu_str shetbu_bol("");

class iceb_tu_str knvr_inv("");
class iceb_tu_str shet_inv("");
class iceb_tu_str shetbu_inv("");

class iceb_tu_str knvr_dog("");
class iceb_tu_str shet_dog("");
class iceb_tu_str shetbu_dog("");

class iceb_tu_str knvr_vs("");
class iceb_tu_str shet_vs("");
class iceb_tu_str shetbu_vs("");


//Проверяем есть ли удержание
/**************************************
sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv);
if(readkey(strsql) <=  0)
 {  
  sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
  datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_bol);
  if(readkey(strsql) <=  0)
   {
    sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
    datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_inv);
    if(readkey(strsql) <=  0)
    {
     sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
     datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_dog);
     if(readkey(strsql) <=  0)
     {
      sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
      datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_vs);
      if(readkey(strsql) <=  0)
       {
        sprintf(strsql,"select tabn from Zarp where datz>='%d-%d-01' and \
        datz <= '%d-%d-31' and tabn=%d and prn='2' and knah=%d",gr,mr,gr,mr,tabn,kod_esv_bol_inv);
        if(readkey(strsql) <=  0)
         {
          sprintf(strsql,"У работника нет кода удержания социального взноса в списке его удержаний\n");
          zar_pr_ins(strsql);
          return;
         }
       }
     }
    }     
  }
 }
************************************/
if(kodf_esv == 0)
 {
  sprintf(strsql,"%s-Код фонда единого социального взноса равен нолю!\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return; 
 }
kodf_esv_vr=kodf_esv;
if(kodf_esv_bol == 0)
 {
  sprintf(strsql,"%s-Код фонда единого социального взноса для больничных равен нолю!\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return; 
 }




/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s\nНе найдены код фонда %d\n",__FUNCTION__,strsql,kodf_esv);
  zar_pr_ins(strsql);
  return;
 }

shet.new_plus(row[0]);
knvr.new_plus(row[1]);
shetbu.new_plus(row[2]);

if(shet.getdlinna() <= 1)
 {
  sprintf(strsql,"Не введён счёт для фонда %d!\n",kodf_esv);
  zar_pr_ins(strsql);
  return;
 }

sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kodf_esv,knvr.ravno(),shetbu.ravno());
zar_pr_ins(strsql);
 
int kod_bol=kodf_esv_bol;
int metka_inv=zarprtn(mr,gr,tabn,2);

if(metka_inv == 1)
 {
  if(kodf_esv_bol_inv == 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса c больничных для инвалидов равен нолю!\n");
    zar_pr_ins(strsql);
    return; 
   }

  kod_bol=kodf_esv_bol_inv; 

 }


/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kod_bol);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\nНе найдены код фонда %d\n",strsql,kod_bol);
  zar_pr_ins(strsql);
  return;
 }

shet_bol.new_plus(row[0]);
knvr_bol.new_plus(row[1]);
shetbu_bol.new_plus(row[2]);

if(shet_bol.getdlinna() <= 1)
 {
  sprintf(strsql,"Не введён счёт для фонда %d!\n",kod_bol);
  zar_pr_ins(strsql);
  return;
 }

sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kod_bol,knvr_bol.ravno(),shetbu_bol.ravno());
zar_pr_ins(strsql);

/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_inv);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\nНе найдены код фонда %d\n",strsql,kodf_esv_inv);
  zar_pr_ins(strsql);
  return;
 }

shet_inv.new_plus(row[0]);
knvr_inv.new_plus(row[1]);
shetbu_inv.new_plus(row[2]);

sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kodf_esv_inv,knvr_inv.ravno(),shetbu_inv.ravno());
zar_pr_ins(strsql);

/*Читаем коды не входящие в расчёт и счёта*/
sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_dog);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\nНе найдены код фонда %d\n",strsql,kodf_esv_dog);
  zar_pr_ins(strsql);
  return;
 }

shet_dog.new_plus(row[0]);
knvr_dog.new_plus(row[1]);
shetbu_dog.new_plus(row[2]);

if(shet_dog.getdlinna() <= 1)
 {
  sprintf(strsql,"Не введён счёт для фонда %d!\n",kodf_esv_dog);
  zar_pr_ins(strsql);
  return;
 }

sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kodf_esv_dog,knvr_dog.ravno(),shetbu_dog.ravno());
zar_pr_ins(strsql);

/*Читаем коды не входящие в расчёт и счёта*/
if(kodf_esv_vs != 0)
 {
  sprintf(strsql,"select shet,kodn,shetb from Zarsoc where kod=%d",kodf_esv_vs);
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s\nНе найдены код фонда %d\n",strsql,kodf_esv_vs);
    zar_pr_ins(strsql);
    return;
   }
  shet_vs.new_plus(row[0]);
  knvr_vs.new_plus(row[1]);
  shetbu_vs.new_plus(row[2]);
 }

sprintf(strsql,"Коды не входящие в расчёт для %d-%s\nБюдж. счёт-%s\n",
kodf_esv_vs,knvr_vs.ravno(),shetbu_vs.ravno());
zar_pr_ins(strsql);


/*Читаем действующие на дату расчёта проценты*/
sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv,gr,mr);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv,mr,gr);
  zar_pr_ins(strsql);
 return;
 }
dproc_sr=atof(row[0]);

/*Читаем действующие на дату расчёта проценты*/
sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kod_bol,gr,mr);
if(readkey(strsql,&row,&cur) != 1)
 {
  zar_pr_ins(strsql);
  sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kod_bol,mr,gr);
  return;
 }
dproc_bl=atof(row[0]);




sprintf(strsql,"\
Процент с работника:%.2f\n\
Процент с больничного:%.2f\n",
dproc_sr,
dproc_bl);

zar_pr_ins(strsql);

if(shetb != NULL)
 {
  sprintf(strsql,"Бюджетные счета:%s\n",shetb);
  zar_pr_ins(strsql);
 }
 
proc_vr=dproc_sr;
knvr_vr.new_plus(knvr.ravno());
shet_vr.new_plus(shet.ravno());
shetbu_vr.new_plus(shetbu.ravno());




if(metka_inv == 1)
 {
  sprintf(strsql,"Находится в списке инвалидов процент\n");
  zar_pr_ins(strsql);
  if(kodf_esv_inv == 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса для инвалидов равен нолю!\n");
    zar_pr_ins(strsql);
    return; 
   }
  /*Читаем действующие на дату расчёта проценты*/
  sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_inv,gr,mr);
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_inv,mr,gr);
    zar_pr_ins(strsql);
    return;
   }
  dproc_in=atof(row[0]);

  kodf_esv_vr=kodf_esv_inv;
  proc_vr=dproc_in;
  knvr_vr.new_plus(knvr_inv.ravno());
  shet_vr.new_plus(shet_inv.ravno());
  shetbu_vr.new_plus(shetbu_inv.ravno());
 }

int metka_dog=zarprtn(mr,gr,tabn,3); //Метка работника работающего по договору

if(metka_dog == 1)
 {
  sprintf(strsql,"Находится в списке работающих по договорам подряда\n");
  zar_pr_ins(strsql);

  if(kodf_esv_dog == 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса для договоров равен нолю!\n");
    zar_pr_ins(strsql);
    return; 
   }

  /*Читаем действующие на дату расчёта проценты*/
  sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_dog,gr,mr);
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_dog,mr,gr);
    zar_pr_ins(strsql);
    return;
   }

  dproc_dg=atof(row[0]);

  kodf_esv_vr=kodf_esv_dog;
  proc_vr=dproc_dg;
  knvr_vr.new_plus(knvr_dog.ravno());
  shet_vr.new_plus(shet_dog.ravno());
  shetbu_vr.new_plus(shetbu_dog.ravno());
 }
int metka_sovm=0;
sprintf(strsql,"select zvan,sovm from Zarn where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
if(readkey(strsql,&row,&cur) == 1)
 {
  metka_sovm=atoi(row[1]);  
  if(atoi(row[0]) != 0)
   if(proverka(kod_zv_gr.ravno(),row[0],0,0) != 0)
    {

     sprintf(strsql,"Военный %s != %s\n",kod_zv_gr.ravno(),row[0]);
     zar_pr_ins(strsql);
     if(kodf_esv_vs == 0)
      {
       sprintf(strsql,"Код фонда единого социального взноса для военных равен нолю!\n");
       zar_pr_ins(strsql);
       return; 
      }

     /*Читаем действующие на дату расчёта проценты*/
     sprintf(strsql,"select pr from Zaresv where kf=%d and datnd <= '%04d-%02d-01' order by datnd desc limit 1",kodf_esv_vs,gr,mr);
     if(readkey(strsql,&row,&cur) != 1)
      {
       sprintf(strsql,"Не найдены проценты для кода фонда %d на дату %d.%d\n",kodf_esv_dog,mr,gr);
       zar_pr_ins(strsql);
       return;
      }

     dproc_vs=atof(row[0]);
     kodf_esv_vr=kodf_esv_vs;
     proc_vr=dproc_vs;
     knvr_vr.new_plus(knvr_vs.ravno());
     shet_vr.new_plus(shet_vs.ravno());
     shetbu_vr.new_plus(shetbu_vs.ravno());
    }
 }
sprintf(strsql,"Берём в расчёт:\n\
Код фонда:%d\n\
Процент:%.2f\n\
Коды не входящие в расчёт:%s\n\
Счёт хозрасчётный:%s\n\
Счёт бюджетный:%s\n",
kodf_esv_vr,
proc_vr,
knvr_vr.ravno(),
shet_vr.ravno(),
shetbu_vr.ravno());

zar_pr_ins(strsql);

class zar_read_tn1h nastr_mz;
zar_read_tn1(1,mr,gr,&nastr_mz); /*читаем до блокировки таблиц*/



/*Блокируем таблицу*/
sprintf(strsql,"LOCK TABLES Zarsocz WRITE,Zarsoc WRITE,Zarp WRITE,Zarpr WRITE,Alx READ,icebuser READ,Kartb READ");
class lock_tables flag(strsql);
  


/*Удаляем все записи если они есть*/
sprintf(strsql,"delete from Zarsocz where datz='%d-%d-01' and tabn=%d",gr,mr,tabn);

sql_zapis(strsql,1,0);


sprintf(strsql,"select knah,suma,shet,godn,mesn from Zarp where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and tabn=%d and \
prn='1' and suma <> 0. order by knah asc",gr,mr,gr,mr,tabn);


/*
printw("\n%s\n",strsql);
refresh();
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдено ни одного начисления!");

  zar_pr_ins(strsql);

  return;

 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
  zar_pr_ins(strsql);

  knah=atoi(row[0]);

  suma=atof(row[1]);

  if(provkod(kodbl,knah) >= 0 || proverka(kod_dop_nah_bol.ravno(),knah,0,1) == 0)
   {
    if(proverka(knvr_bol.ravno(),row[0],0,1) == 0)
     continue;
    if(proverka(shetb,row[2],0,1) == 0)
     suma_bolb+=suma;
    else
     suma_bol+=suma;
   }
  else
   {
    if(proverka(knvr_vr.ravno(),row[0],0,1) == 0)
     continue;
    if(proverka(shetb,row[2],0,1) == 0)
      suma_vrb+=suma;
    else
      suma_vr+=suma;
   }
 }


sprintf(strsql,"suma_bol=%.2f suma_vr=%.2f suma_bolb=%.2f suma_vrb=%.2f proc_vr=%.2f\n",suma_bol,suma_vr,suma_bolb,suma_vrb,proc_vr);
zar_pr_ins(strsql);

if(sravmydat(1,mr,gr,1,1,2015) >= 0)
 {

  if(metka_sovm == 0 && metka_dog == 0) /*не работает совместителем или по договору*/
   if(suma_vr+suma_vrb+suma_bol < nastr_mz.minzar)
    {
     sprintf(strsql,"Сумма в расчёт %.2f меньше минимальной зарплаты.\n",suma_vr+suma_vrb+suma_bol);
     zar_pr_ins(strsql);

     short dnr=0,mnr=0,gnr=0;
     short dkr=0,mkr=0,gkr=0;
     int metka_rs_mz=0;
     /*Если принят или уволен то начисляется с фактической зарплаты*/
     sprintf(strsql,"select datn,datk from Kartb where tabn=%d",tabn);
     if(readkey(strsql,&row,&cur) == 1)
      {
       rsdat(&dnr,&mnr,&gnr,row[0],2);
       rsdat(&dkr,&mkr,&gkr,row[1],2);
       if(mnr == mr && gnr == gr)
        metka_rs_mz=1;       
       if(mkr == mr && gkr == gr)
        metka_rs_mz=1;       
      }     

    if(metka_rs_mz == 0)
     {  
  
      sprintf(strsql,"Расчёт производим от минимальной зарплаты %.2f",nastr_mz.minzar);
      zar_pr_ins(strsql);

      sprintf(strsql,"Увеличиваем на сумму %.2f-%.2f-%.2f-%.2f=%.2f\n",
      nastr_mz.minzar,suma_vr,suma_vrb,suma_bol,nastr_mz.minzar-(suma_vr+suma_vrb+suma_bol));
      zar_pr_ins(strsql);
      
      suma_vr+=nastr_mz.minzar-(suma_vr+suma_vrb+suma_bol);
     }
    else
     {
      sprintf(strsql,"Принят/уволен с работы. Налог считаем с фактической зарплаты. %02d.%02d.%d-%02d.%02d.%d\n",dnr,mnr,gnr,dkr,mkr,gkr);
      zar_pr_ins(strsql);
      
     }
    }
 }
 
if(suma_bol != 0. || suma_bolb != 0.)
 {
  double sumao=suma_bol*dproc_bl/100.;
  double suma_b=suma_bolb*dproc_bl/100.;
  sumao=okrug(sumao,okrg);
  suma_b=okrug(suma_b,okrg);

  class iceb_tu_str shet_zap(shet_bol.ravno());

  if(shetbu_bol.getdlinna() > 1)
   shet_zap.plus(",",shetbu_bol.ravno());
     
  sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
  gr,mr,tabn,kod_bol,shet_zap.ravno(),suma_bol+suma_bolb,sumao+suma_b,dproc_bl,iceb_t_getuid(),time(NULL),suma_b,suma_bolb);
  sql_zapis(strsql,1,0);
 }


if(suma_vr != 0. || suma_vrb != 0.)
 {
  if(suma_vr >  nastr_mz.max_sum_for_soc) //Максимальная сумма с которой начисляются соц.отчисления
   {
    sprintf(strsql,"Cумма %.2f > %.2f Берём в расчёт %.2f\n",suma_vr, nastr_mz.max_sum_for_soc, nastr_mz.max_sum_for_soc);
    zar_pr_ins(strsql);

    suma_vr= nastr_mz.max_sum_for_soc;

   }
  if(suma_vrb >  nastr_mz.max_sum_for_soc) //Максимальная сумма с которой начисляются соц.отчисления
   {
    sprintf(strsql,"Cумма %.2f > %.2f Берём в расчёт %.2f\n",suma_vrb, nastr_mz.max_sum_for_soc, nastr_mz.max_sum_for_soc);
    zar_pr_ins(strsql);

    suma_vrb= nastr_mz.max_sum_for_soc;

   }


  double sumao=suma_vr*proc_vr/100.;
  double suma_b=suma_vrb*proc_vr/100.;
  sumao=okrug(sumao,okrg);
  suma_b=okrug(suma_b,okrg);
  class iceb_tu_str shet_zap(shet_vr.ravno());

  if(shetbu_vr.getdlinna() > 1)
   shet_zap.plus(",",shetbu_vr.ravno());
  
  sprintf(strsql,"insert into Zarsocz (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
values ('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
  gr,mr,tabn,kodf_esv_vr,shet_zap.ravno(),suma_vr+suma_vrb,sumao+suma_b,proc_vr,iceb_t_getuid(),time(NULL),suma_b,suma_vrb);
  sql_zapis(strsql,1,0);
 }
}

