/*$Id: uosoik.c,v 5.4 2013/08/13 05:49:57 sasa Exp $*/
/*16.07.2015	26.12.2011	Белых А.И.	uosoik.c
Описание инвентарных карточек по учёту основынх средств форма N0З-7
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"

int uosoik(class spis_oth *oth)
{
static class iceb_tu_str datan("");
static class iceb_tu_str datak("");
static class iceb_tu_str grupabu("");
static class iceb_tu_str grupanu("");

int kom=0,kom1=0;

class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка описи инветарных карточек (типовая форма NoОЗ-7)"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Группа бух. учёта...."));
VV.VVOD_SPISOK_add_MD(gettext("Группа нал. учёта...."));

VV.VVOD_SPISOK_add_data(datan.ravno(),11);
VV.VVOD_SPISOK_add_data(datak.ravno(),11);
VV.VVOD_SPISOK_add_data(grupabu.ravno(),16);
VV.VVOD_SPISOK_add_data(grupanu.ravno(),16);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("группа"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

datan.new_plus(VV.VVOD_SPISOK_return_data(0));
datak.new_plus(VV.VVOD_SPISOK_return_data(1));
grupabu.new_plus(VV.VVOD_SPISOK_return_data(2));
grupanu.new_plus(VV.VVOD_SPISOK_return_data(3));

class iceb_tu_str kod("");
class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
//   GDITE();
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп бухгалтерского учёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп налогового учёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
        goto naz;
    
      case 0 :
        if(diruosgrbu(1,&kod,&naim) == 0)
          VV.data_plus(2,kod.ravno());
        break;

      case 1 :
        if(diruosgrnu(1,&kod,&naim) == 0)
          VV.data_plus(3,kod.ravno());
                 
        break;
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;
char strsql[1024];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

if(rsdatp(&dn,&mn,&gn,datan.ravno(),&dk,&mk,&gk,datak.ravno()) != 0)
 goto naz;
if(grupabu.getdlinna() <= 1 && grupanu.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введена группа!"));
  goto naz;
 }

class iceb_tu_str naimgrbu("");
/*проверяем есть ли группа в списке*/
if(grupabu.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",grupabu.ravno());
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдена группа бух. учёта"),grupabu.ravno());
    iceb_t_soob(strsql);
    goto naz;
   }  
  naimgrbu.new_plus(row[0]);
 }
class iceb_tu_str naimgrnu("");
/*проверяем есть ли группа в списке*/
if(grupanu.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",grupanu.ravno());
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдена группа бух. учёта"),grupanu.ravno());
    iceb_t_soob(strsql);
    goto naz;
   }  
  naimgrnu.new_plus(row[0]);
 }

class iceb_tu_str naimkg(naimgrbu.ravno());
if(naimkg.getdlinna() <= 1)
 naimkg.new_plus(naimgrnu.ravno());
 
if((kolstr=cur.make_cursor(&bd,"select innom,naim from Uosin")) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

int kolstr1=0;
FILE *ff;
char imaf[64];
sprintf(imaf,"uosoik%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
class iceb_tu_str edrpou("");

if(readkey("select kod from Kontragent where kodkon='00'",&row1,&cur1) == 1)
 edrpou.new_plus(row1[0]);
 
class iceb_tu_str organ(iceb_t_get_pnk("00",0));

fprintf(ff,"%-*.*s Типова форма NoОЗ-7\n\
%40s Затверджена наказом Мінстату України\n\
%40s від 29.12.95 р. No352\n\n\
Ідентифікаційний код ЄДРПОУ %-*s   Код за УКУД\n",
iceb_tu_kolbait(40,organ.ravno()),
iceb_tu_kolbait(40,organ.ravno()),
organ.ravno(),
"","",
iceb_tu_kolbait(10,edrpou.ravno()),
edrpou.ravno());


fprintf(ff,"\n\
                 Опис інвентарних карток по обліку\n\
                      основних засобів\n\
                  за період з %02d.%02d.%04d р.\n\
                       по %02d.%02d.%04d р.\n\n",
dn,mn,gn,
dk,mk,gk);

int podr=0;
int kmol=0;
int innom=0;
class iceb_tu_str datdok("");
class iceb_tu_str nomdok("");
class iceb_tu_str rashod("");
fprintf(ff,"\
--------------------------------------------------------------------------------\n");

fprintf(ff,"\
Найменування класіфікаційної групи %s\n",naimkg.ravno());

fprintf(ff,"\
--------------------------------------------------------------------------------\n");

fprintf(ff,"\
Картки|Інвентарні|   Найменування об'єкта                 |Відмітка про вибуття|\n");

fprintf(ff,"\
--------------------------------------------------------------------------------\n");


int nomzap=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  innom=atoi(row[0]);

  /*смотрим был ли на начало периода*/
  if(poiin(innom,dn,mn,gn,&podr,&kmol) != 0)
   continue;

  

  /*определяем появилось ли в периоде*/
  sprintf(strsql,"select datd,nomd,tipz,kodop from Uosdok1 where \
datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and innom=%d and podt=1 \
order by datd asc, tipz desc limit 1",gn,mn,dn,gk,mk,dk,innom);
  if(readkey(strsql,&row1,&cur1) <= 0)
   continue;

  /*определяем группу*/
  class poiinpdw_data rkos;
  poiinpd(innom,mk,gk,&rkos);
  
  if(proverka(grupabu.ravno(),rkos.hnaby.ravno(),0,0) != 0)
   continue;  

  if(proverka(grupanu.ravno(),rkos.hna.ravno(),0,0) != 0)
   continue;  
  
  datdok.new_plus("");
  nomdok.new_plus("");
  rashod.new_plus("");
  
  if(atoi(row1[2]) == 2)
   {
    SQL_str row2;
    class SQLCURSOR cur2;
    sprintf(strsql,"select vido from Uosras where kod='%s'",row1[3]);
    if(readkey(strsql,&row2,&cur2) == 1)
     {
      if(atoi(row2[0]) == 0) /*внешняя тоесть списание*/
       {
        datdok.new_plus(iceb_tu_datzap(row1[0]));
        nomdok.new_plus(row1[1]);
        sprintf(strsql,"No%s від %s р.",nomdok.ravno(),datdok.ravno());
        rashod.new_plus(strsql);        
       }       
      
     }
   }
  fprintf(ff,"%6d|%10d|%-*.*s|%s\n",
  ++nomzap,
  innom,
  iceb_tu_kolbait(40,row[1]),
  iceb_tu_kolbait(40,row[1]),
  row[1],
  rashod.ravno());  

  for(int nom=40; nom < iceb_tu_strlen(row[1]); nom+=40)
    fprintf(ff,"%6s|%10s|%-*.*s|\n",
    "","",
    iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,row[1])),
    iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,row[1])),
    iceb_tu_adrsimv(nom,row[1]));  
 }

fprintf(ff,"\
--------------------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка описи инветарных карточек (типовая форма NoОЗ-7)"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);

return(0);
}

