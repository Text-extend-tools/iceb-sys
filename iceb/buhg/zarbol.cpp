/*$Id:$*/
/*08.11.2014	07.11.2014	Белых А.И.	zarbol.c
Отчёт о больничных начисленных в заданном месяце
*/
#include "buhg.h"

class zarbol_rk
 {
  public:
  int tabn;
  class SPISOK mes_god;
  class masiv_din_double suma_bol;
  double itogo[5];

  zarbol_rk()
   {
    clear();
    tabn=0;
    memset(itogo,'\0',sizeof(itogo));
   }

  void clear()
   {
    suma_bol=0.;
    mes_god.free_class();
    suma_bol.free_class();
   }
 };

void zarbol_hap(int nomstr,int *kolstr,FILE *ff);
void zarbol_str(class zarbol_rk *rk,FILE *ff);

extern class iceb_tu_str kodpn_all; /*Все коды подоходного налога*/
extern class iceb_tu_str kods_esv_all;  /*Все коды удержания единого социального взноса*/
extern short kodvn; /*код военного налога*/


int zarbol(class spis_oth *oth)
{
char imaf[64];
char strsql[1024];
int kolstr=0,kolstr1=0;
SQL_str row;
class SQLCURSOR cur;
static class iceb_tu_str mes_god("");
static class iceb_tu_str kod_nah("");
class iceb_tu_str kod("");
class iceb_tu_str naim("");
int kom=0;
class zarbol_rk rk;
class iceb_fopen fil;
short m=0,g=0;

int tabnz=0;
int suma=0.;
int nom=0; 

class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт удержаний с больничного"));

VV.VVOD_SPISOK_add_MD(gettext("Дата..........(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код больничного......"));

VV.VVOD_SPISOK_add_data(mes_god.ravno(),8);
VV.VVOD_SPISOK_add_data(kod_nah.ravno(),10);

naz:;

clear();
 
helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

mes_god.new_plus(VV.VVOD_SPISOK_return_data(0));
kod_nah.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK3:
    kod.new_plus("");
    if(dirnach(1,&kod,&naim) == 0)
      VV.data_z_plus(1,kod.ravno());
    goto naz;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }



if(rsdat1(&m,&g,mes_god.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  goto naz;
 }
GDITE();
/*читаем наименование кода*/
sprintf(strsql,"select naik from Nash where kod=%d\n",kod_nah.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d %s!",gettext("Не найден код начисления"),kod_nah.ravno_atoi(),gettext("в справочнике начислений"));
  iceb_t_soob(strsql);
  goto naz;
 } 
naim.new_plus(row[0]);

sprintf(strsql,"select tabn,knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%02d' and knah=%d and suma <> 0. and prn='1' \
order by tabn asc",g,m,1,g,m,31,kod_nah.ravno_atoi());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи!"));
  return(1);
 }
sprintf(imaf,"bol11.lst");

if(fil.start(imaf,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Расчёт удержаний с больничного"),fil.ff);
fprintf(fil.ff,"%s:%s\n",gettext("Дата"),mes_god.ravno());
fprintf(fil.ff,"%s:%d %s\n",gettext("Начисление"),kod_nah.ravno_atoi(),naim.ravno());

zarbol_hap(1,NULL,fil.ff);

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  rk.tabn=atoi(row[0]);
  suma=atof(row[2]);
  if(tabnz != rk.tabn)
   {
    if(tabnz != 0)
      zarbol_str(&rk,fil.ff);
    tabnz=rk.tabn;
   }

  sprintf(strsql,"%02d.%04d",atoi(row[4]),atoi(row[3]));
  if((nom=rk.mes_god.find(strsql)) < 0)
   rk.mes_god.plus(strsql);

  rk.suma_bol.plus(suma,nom);

 }

fprintf(fil.ff,"ICEB_LST_END\n\
------------------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"),
rk.itogo[0],rk.itogo[1],rk.itogo[2],rk.itogo[3],rk.itogo[4]);

podpis(fil.ff);

fil.end();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт удержаний с больничного"));

int orient=iceb_t_ustpeh(oth->spis_imaf.ravno(0),3);

iceb_t_rnl(oth->spis_imaf.ravno(0),orient,NULL,&zarbol_hap);

return(0);
}
/***********************************************/
/*Вывод строки*/
/*************************************************/
void zarbol_str(class zarbol_rk *rk,FILE *ff)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str fio("");
double suma_podoh=0.;
//double suma_esv=0.;
double suma_vs=0.;
double suma_vid=0;
//class zarrnesvp_rek esvp;
short mes=0,god=0;
//float proc_esv=0;

sprintf(strsql,"select fio from Kartb where tabn=%d",rk->tabn);
if(readkey(strsql,&row,&cur) == 1)
 fio.new_plus(row[0]);

for(int nom=0; nom < rk->mes_god.kolih(); nom++)
 {

  rsdat1(&mes,&god,rk->mes_god.ravno(nom));
/*************  
  zarrnesvp(mes,god,&esvp);

  //Проверяем есть ли в списке инвалидов
  if(zarprtn(mes,god,rk->tabn,2) == 1)
   proc_esv=esvp.proc_esv_bol_inv;
  else
   proc_esv=esvp.proc_esv_bol;
   
  //вычисляем единый соц взнос
  suma_esv=rk->suma_bol.ravno(nom) * proc_esv/100.;
  suma_esv=okrug(suma_esv,0.01);
****************/
  /*вычисляем подоходный*/
//  suma_podoh=(rk->suma_bol.ravno(nom)-suma_esv)*0.15;
  suma_podoh=rk->suma_bol.ravno(nom)*0.15;
  suma_podoh=okrug(suma_podoh,0.01);

  /*военный сбор*/
  suma_vs=rk->suma_bol.ravno(nom)*0.015;
  suma_vs=okrug(suma_vs,0.01);
  
//  suma_vid=rk->suma_bol.ravno(nom)-suma_podoh-suma_esv-suma_vs;
  suma_vid=rk->suma_bol.ravno(nom)-suma_podoh-suma_vs;
  
  fprintf(ff,"%6d %-*.*s %s %10.2f %10.2f %10.2f %10.2f\n",
  rk->tabn,
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno(),
  rk->mes_god.ravno(nom),
  rk->suma_bol.ravno(nom),
  suma_podoh,
//  suma_esv,
  suma_vs,
  suma_vid);
  
  rk->itogo[0]+=rk->suma_bol.ravno(nom);
  rk->itogo[1]+=suma_podoh;
//  rk->itogo[2]+=suma_esv;
  rk->itogo[3]+=suma_vs;
  rk->itogo[4]+=suma_vid;

 } 
rk->clear();
}
/**************************************/
/*Шапка*/
/***************************************/
void zarbol_hap(int nomstr,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr+=4;
 
fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nomstr);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Таб н.|       Фамилия                |  Дата  |Больничный|Подоходный|Ед.соцвзн.|Военный з.|К выдаче  |\n"));
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
}
