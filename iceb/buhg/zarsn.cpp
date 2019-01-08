/*$Id: zarsn.c,v 5.1 2014/06/30 06:36:35 sasa Exp $*/
/*28.03.2018	15.05.2014	Белых А.И.	zarsn.c
Распечатка свода настроек начислений по работникам
*/

#include "buhg.h"

int zarsn(class spis_oth *oth)
{
char strsql[1024];
int tbn=0;
int kom1=0;
int kom2=0;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
class iceb_tu_str bros("");
int kolstr=0;
SQL_str row;
static class iceb_tu_str kod_podr("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str kod_kateg("");
static class iceb_tu_str kod_zvan("");
static class iceb_tu_str shetu("");

class VVOD MENU(3);

class VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории....(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код звания.......(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.............(,,).."));


VV.VVOD_SPISOK_add_data(kod_podr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_kateg.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_zvan.ravno(),128);
VV.VVOD_SPISOK_add_data(shetu.ravno(),128);

naz2:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom2=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

kod_podr.new_plus(VV.data_ravno(0));
tabnom.new_plus(VV.data_ravno(1));
kod_kateg.new_plus(VV.data_ravno(2));
kod_zvan.new_plus(VV.data_ravno(3));
shetu.new_plus(VV.data_ravno(4));

switch(kom2)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz2;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка званий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    kod.new_plus("");
    naim.new_plus("");
    switch (kom1)
     {
      case 5 :
      case -1:
        goto naz2;
    
      case 0 :
        if(dirzarpodr(2,&kod,&naim) == 0)
         VV.data_z_plus(0,kod.ravno());
        break;

      case 1 :
        if(dirtab(&tbn,&kod,0,1) == 0)
         {
          VV.data_z_plus(1,tbn);
         }                 
        break;

      case 2 :
        vibrek("Kateg",&kod);
        VV.data_z_plus(2,kod.ravno());
        break;

      case 3 :
        vibrek("Zvan",&kod);
        VV.data_z_plus(3,kod.ravno());
        break;

      case 4 :
        vibrek("Plansh",&kod);
        VV.data_z_plus(4,kod.ravno());
        break;
     }
    goto naz2;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz2;

  default:
     goto naz2;
 }



VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Сортировка по табельным номерам ?"));
int sortzap=danet(&DANET,2,stdscr);


clear(); /*Очистить экран и закрасить фоновым цветом*/

sprintf(strsql,"select * from Kartb");

if(sortzap == 1)
  bros.new_plus(" order by tabn asc");
if(sortzap == 2)
  bros.new_plus(" order by fio asc");
strcat(strsql,bros.ravno());

SQLCURSOR cur;
SQLCURSOR curr;
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


class masiv_din_int sp_tabnom;
class SPISOK sp_fio;
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;    
  if(proverka(kod_kateg.ravno(),row[5],0,0) != 0)
   continue;    
  if(proverka(kod_zvan.ravno(),row[3],0,0) != 0)
   continue;    
  if(proverka(shetu.ravno(),row[8],0,0) != 0)
   continue;    
  if(proverka(kod_podr.ravno(),row[4],0,0) != 0)
   continue;    
  if(row[7][0] != '0')
   continue;
  sp_tabnom.plus(atoi(row[0]),-1);
  sp_fio.plus(row[1]);  
 }

sprintf(strsql,"select str from Alx where fil='zarproc.alx' order by ns asc");
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
class masiv_din_int sp_kod_nah;
class masiv_din_int fsp_tabnom;
class masiv_din_int fsp_kod_nah;
class SPISOK fsp_tarif;
int kod_nah=0;
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;

  if(polen(row[0],&tbn,1,'|') != 0)
   continue;

  if(tbn == 0)
   continue;

  if(sp_tabnom.find(tbn) < 0)
   continue;

  if(polen(row[0],&kod_nah,3,'|') != 0)
   continue;

  if(kod_nah == 0)
   continue;


  if(polen(row[0],&kod,5,'|') != 0)
   continue;

  if(kod.ravno_atof() == 0.)
   continue;
   

  if(sp_kod_nah.find(kod_nah) < 0)
   sp_kod_nah.plus(kod_nah,-1);
   
  fsp_tabnom.plus(tbn,-1);
  fsp_kod_nah.plus(kod_nah,-1);
  fsp_tarif.plus(kod.ravno());
 }

/*Распечатка результата*/

class iceb_fopen fil;
char imaf[64];
sprintf(imaf,"sn%d.lst",getpid());
if(fil.start(imaf,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Свод настроек начислений"),fil.ff);
class iceb_tu_str liniq("-------------------------------------------------");

for(int nom=0; nom < sp_kod_nah.kolih()+1; nom++)
 liniq.plus("-----------");

fprintf(fil.ff,"%s\n",liniq.ravno());
 
fprintf(fil.ff,"\
 Т/н  |  Фамлия, Имя, Отчество       |  Оклад   |");

for(int nom=0; nom < sp_kod_nah.kolih(); nom++)
 fprintf(fil.ff,"%10d|",sp_kod_nah.ravno(nom));


fprintf(fil.ff,"%-*.*s|\n",
iceb_tu_kolbait(10,gettext("Итого")),
iceb_tu_kolbait(10,gettext("Итого")),
gettext("Итого"));

fprintf(fil.ff,"%s\n",liniq.ravno());
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
double okl=0.;
int has=0;

for(int nom_tn=0; nom_tn < sp_tabnom.kolih(); nom_tn++)
 {
  tbn=sp_tabnom.ravno(nom_tn);
  okl=oklad(tbn,mt,gt,&has);
  double itogo=okl;
  if(okl != 0.)
   fprintf(fil.ff,"%6d|%-*.*s|%10.2f|",
   tbn,
   iceb_tu_kolbait(30,sp_fio.ravno(nom_tn)),
   iceb_tu_kolbait(30,sp_fio.ravno(nom_tn)),
   sp_fio.ravno(nom_tn),
   okl);
  else
   fprintf(fil.ff,"%6d|%-*.*s|%10s|",
   tbn,
   iceb_tu_kolbait(30,sp_fio.ravno(nom_tn)),
   iceb_tu_kolbait(30,sp_fio.ravno(nom_tn)),
   sp_fio.ravno(nom_tn),
   "");
  
  for(int nom_kn=0; nom_kn < sp_kod_nah.kolih(); nom_kn++)
   {

    kod_nah=sp_kod_nah.ravno(nom_kn);
    int metka=0;
    for(int nom=0; nom < fsp_tabnom.kolih(); nom++)
     {
      
      if(fsp_tabnom.ravno(nom) != tbn)
       continue;
      if(fsp_kod_nah.ravno(nom) != kod_nah)
       continue;   

      fprintf(fil.ff,"%*.*s|",
      iceb_tu_kolbait(10,fsp_tarif.ravno(nom)),
      iceb_tu_kolbait(10,fsp_tarif.ravno(nom)),
      fsp_tarif.ravno(nom));

      itogo+=fsp_tarif.ravno_atof(nom);    

      metka=1;
      break;
     }
    if(metka == 0)
     fprintf(fil.ff,"%10s|","");
         
   }
  if(itogo == 0.)
   fprintf(fil.ff,"%10s|","");
  else
   fprintf(fil.ff,"%10.2f|",itogo);
  fprintf(fil.ff,"\n");
 }

fprintf(fil.ff,"%s\n",liniq.ravno());

fil.end();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Свод настроек начислений"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),3);

return(0);
}


