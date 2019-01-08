/*$Id: uosisos.c,v 5.4 2013/08/13 05:49:57 sasa Exp $*/
/*16.07.2015	27.12.2011	Белых А.И.	uosisos.c
Инвентарный список основных средств NoОЗ-9
*/

#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"

int uosisos(class spis_oth *oth)
{
static class iceb_tu_str podr("");
static class iceb_tu_str grupabu("");
static class iceb_tu_str grupanu("");

int kom=0,kom1=0;

class VVOD MENU(3);
class VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка инвентарного списка основных средст (типовая форма NoОЗ-9)"));

VV.VVOD_SPISOK_add_MD(gettext("Подразделение......"));
VV.VVOD_SPISOK_add_MD(gettext("Группа бух. учёта.."));
VV.VVOD_SPISOK_add_MD(gettext("Группа нал. учёта.."));

VV.VVOD_SPISOK_add_data(podr.ravno(),16);
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

podr.new_plus(VV.VVOD_SPISOK_return_data(0));
grupabu.new_plus(VV.VVOD_SPISOK_return_data(1));
grupanu.new_plus(VV.VVOD_SPISOK_return_data(2));

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

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп бухгалтерского учёта"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп налогового учёта"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
        goto naz;
    
      case 0 :
        if(diruospod(1,&kod,&naim,0) == 0)
          VV.data_plus(0,kod.ravno());
        break;

      case 1 :
        if(diruosgrbu(1,&kod,&naim) == 0)
          VV.data_plus(1,kod.ravno());
        break;

      case 2 :
        if(diruosgrnu(1,&kod,&naim) == 0)
          VV.data_plus(2,kod.ravno());
                 
        break;
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

char strsql[1024];
SQL_str row,row1,row2;
class SQLCURSOR cur,cur1,cur2;

if(podr.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не ввели код подразделения!"));
  goto naz;
 }
sprintf(strsql,"select naik from Uospod where kod=%d",podr.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найдено код подразделения"),podr.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }  

class iceb_tu_str naimpodr(row[0]);


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
    goto naz;
   }  
  naimgrnu.new_plus(row[0]);
 }

class iceb_tu_str naimkg(naimgrbu.ravno());
if(naimkg.getdlinna() <= 1)
 naimkg.new_plus(naimgrnu.ravno());
 


class iceb_t_tmptab tabtmp;
const char *imatmptab={"uosisos"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datp DATE NOT NULL DEFAULT '0000-00-00',\
innom INT NOT NULL DEFAULT 0,\
bs DOUBLE(16,2) NOT NULL DEFAULT 0.,\
naim CHAR (255) NOT NULL DEFAULT '',\
datr DATE NOT NULL DEFAULT '0000-00-00',\
nomd CHAR(32) NOT NULL DEFAULT '',\
osnov CHAR(255) NOT NULL DEFAULT '',\
index(innom)) ENGINE = MYISAM",imatmptab);

if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
int kolstr=0;
sprintf(strsql,"select datd,tipz,kodop,nomd,osnov from Uosdok where podr=%d order by datd asc",podr.ravno_atoi());

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

int kolstr1=0;
int tipz=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  tipz=atoi(row[1]);
  /*проверяем код операции*/
  if(tipz == 1)  
   sprintf(strsql,"select vido from Uospri where kod='%s'",row[2]);
  if(tipz == 2)  
   sprintf(strsql,"select vido from Uosras where kod='%s'",row[2]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найден код операции"),row[2]);
    iceb_t_soob(strsql);
    continue;
   }

  if(atoi(row1[0]) == 2)
   continue;

  sprintf(strsql,"select innom,bs from Uosdok1 where datd='%s' and nomd='%s'",row[0],row[3]);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    class iceb_tu_str naim("");
    sprintf(strsql,"select naim from Uosin where innom=%s",row1[0]);
    if(readkey(strsql,&row2,&cur2) == 1)
     naim.new_plus(row2[0]);
     
    if(tipz == 1)
     sprintf(strsql,"insert into %s (datp,innom,bs,naim)  values('%s',%s,%s,'%s')",imatmptab,row[0],row1[0],row1[1],naim.ravno_filtr());

    if(tipz == 2)
     {
      /*ищем запись прихода*/
      sprintf(strsql,"select datp from %s where innom=%s and datp <= '%s' order by datp desc limit 1",imatmptab,row1[0],row[0]);
      if(readkey(strsql,&row2,&cur2) != 1)
       {
        iceb_t_soob(gettext("Не найдена запись прихода!"));
        continue;
       }
      
      sprintf(strsql,"update %s set datr='%s',nomd='%s',osnov='%s' where innom=%s and datp = '%s'",imatmptab,row[0],row[3],row[4],row1[0],row2[0]);
     }
    sql_zapis(strsql,1,0);
   } 
 }

sprintf(strsql,"select * from %s order by innom asc",imatmptab);
 
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

kolstr1=0;
FILE *ff;
char imaf[64];
sprintf(imaf,"uosisos%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
class iceb_tu_str edrpou("");

if(readkey("select kod from Kontragent where kodkon='00'",&row1,&cur1) == 1)
 edrpou.new_plus(row1[0]);
 
class iceb_tu_str organ(iceb_t_get_pnk("00",0));

fprintf(ff,"%-*.*s Типова форма NoОЗ-9\n\
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
                 Інвентарний список основних засобів\n\
               (за місцем їх знаходження, експлуатації)\n\
                 %s\n\
                 %s\n\n",naimpodr.ravno(),naimkg.ravno());

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
Інвентарна картка|          |                                        |Первісна   |        Вибуття (переміщення)        |\n\
   або запис     |Інвентарн |      Повне найменування об'єкта        |(балансова)|-------------------------------------|\n\
в інвентарній кн.|ий номер  |                                        |  вартість |      документ       |причина вибуття|\n\
-----------------|          |                                        |           |---------------------|(переміщення)  |\n\
номер |  дата    |          |                                        |           |   дата   |  номер   |               |\n");
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------\n");
class iceb_tu_str datap("");
class iceb_tu_str datar(""); 
int nomzap=0;
class poiinpdw_data rekos;
short d,m,g;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  rsdat(&d,&m,&g,row[0],2);

  poiinpd(atoi(row[1]),m,g,&rekos);

  if(proverka(grupabu.ravno(),rekos.hnaby.ravno(),0,0) != 0)
   continue;

  if(proverka(grupanu.ravno(),rekos.hna.ravno(),0,0) != 0)
   continue;
          
  datap.new_plus(iceb_tu_datzap(row[0]));  
  datar.new_plus(iceb_tu_datzap(row[4]));
  
  fprintf(ff,"%6d|%10s|%-10s|%-*.*s|%11s|%10s|%-*s|%-*.*s|\n",
  ++nomzap,
  datap.ravno(),
  row[1],
  iceb_tu_kolbait(40,row[3]),
  iceb_tu_kolbait(40,row[3]),
  row[3],
  row[2],
  datar.ravno(),
  iceb_tu_kolbait(10,row[5]),
  row[5],
  iceb_tu_kolbait(15,row[6]),
  iceb_tu_kolbait(15,row[6]),
  row[6]);
  int nom2=15;
  for(int nom=40; nom < iceb_tu_strlen(row[3]) || nom2 < iceb_tu_strlen(row[6]); nom+=40)
   {
    if(nom < iceb_tu_strlen(row[3]))
     fprintf(ff,"%6s|%10s|%10s|%-*.*s|%11s|%10s|%10s|",
     "","","",
     iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,row[3])),  
     iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,row[3])),  
     iceb_tu_adrsimv(nom,row[3]),
     "","","");
   else
     fprintf(ff,"%6s|%10s|%10s|%40s|%11s|%10s|%10s|",
     "","","","","","","");

   if(nom2 < iceb_tu_strlen(row[6]))
     fprintf(ff,"%-*.*s|",
     iceb_tu_kolbait(15,iceb_tu_adrsimv(nom2,row[6])),  
     iceb_tu_kolbait(15,iceb_tu_adrsimv(nom2,row[6])),  
     iceb_tu_adrsimv(nom2,row[6]));
   else
     fprintf(ff,"%15s|","");
   nom2+=15;
   
   fprintf(ff,"\n");
  }
 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------\n");

podpis(ff);

fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка инвентарного списка основных средст (типовая форма NoОЗ-9)"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);

return(0);

}
