/*$Id: zartarrozrs.c,v 5.13 2013-04-07 10:22:45 sasa Exp $*/
/*28.03.2018	28.11.2008	Белых А.И.	zartarrozrs.c
Расчёт тарифов на базе тарифной сетки
*/

#include <errno.h>
#include "buhg.h"





int zartarrozrs(class spis_oth *oth)
{

static class iceb_tu_str podr("");
static class iceb_tu_str tabnom("");
int kom=0,kom1=0;
char strsql[512];
int tbn=0;
int pdr=0;
class iceb_tu_str fiov("");


VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт тарифов на базе тарифной таблицы"));

VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер...(,,)."));

VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("наcтройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

podr.new_plus(VV.data_ravno(0));
tabnom.new_plus(VV.data_ravno(1));

class iceb_tu_str naim("");

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


  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
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
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(0,podr.ravno());
        break;

      case 1 :
        pdr=tbn=0;
        fiov.new_plus("");
        if(dirtab(&tbn,&fiov,pdr,1) == 0)
          VV.data_z_plus(1,tbn);
          
        break;
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  case FK5: /*Работа с файлом настройки*/
    iceb_redfil("zartarroz1.alx",0);
    goto naz;

  default:
     goto naz;
 }

GDITE();

int kolstr;
SQL_str row_alx;
class SQLCURSOR cur_alx;
sprintf(strsql,"select str from Alx where fil='zarproc.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarproc.alx");
  iceb_t_soob(strsql);
  return(1);
 }


FILE *ff_ot;
char imaf_ot[64];
sprintf(imaf_ot,"tart%d.lst",getpid());

if((ff_ot=fopen(imaf_ot,"w")) == NULL)
 { 
  error_op_nfil(imaf_ot,errno,"");
  return(1);
 }





iceb_t_zagolov(gettext("Расчёт окладов на базе тарифной сетки"),0,0,0,0,0,0,ff_ot);

if(podr.ravno()[0] != '\0')
 fprintf(ff_ot,"%s:%s\n",gettext("Подразделение"),podr.ravno());
if(tabnom.ravno()[0] != '\0')
 fprintf(ff_ot,"%s:%s\n",gettext("Табельный номер"),tabnom.ravno());
 
fprintf(ff_ot,"\
--------------------------------------------------------------------------------------\n");
fprintf(ff_ot,"\
  Т/н   | Фамилия, имя, отчество       |   Должность        |Ном.тар./розр|  Тариф   |\n");

fprintf(ff_ot,"\
--------------------------------------------------------------------------------------\n");

class iceb_tu_str tabn;
class iceb_tu_str rozr;
class iceb_tu_str fio;
int podri=0;
class iceb_tu_str dolg;
double tarif=0.;
double itarif=0.;
SQL_str row;
class SQLCURSOR cur;
int ntar=0, nraz=0;
zartarroz1(); /*читаем таблицу*/
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(polen(row_alx[0],&tabn,1,'|') != 0)
   continue;

  if(tabn.ravno_atoi() == 0)
   continue;

  if(proverka(tabnom.ravno(),tabn.ravno(),0,0) != 0)
   continue;

  polen(row_alx[0],&rozr,5,'|');

  if(pole2(rozr.ravno(),'/') < 1)
   continue;

  sprintf(strsql,"select fio,dolg,podr from Kartb where tabn=%d",tabn.ravno_atoi());
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn.ravno_atoi());
    iceb_t_soob(strsql);
    continue;
   }
  fio.new_plus(row[0]);
  dolg.new_plus(row[1]);

  podri=atoi(row[2]);

  if(proverka(podr.ravno(),podri,0,0) != 0)
   continue;

  polen(rozr.ravno(),&ntar,1,'/');
  polen(rozr.ravno(),&nraz,2,'/');
  
  tarif=zartarroz1(ntar,nraz,0,0,0);

  itarif+=tarif;  

  fprintf(ff_ot,"%8s %-*.*s %-*.*s %-13s %10.2f\n",
  tabn.ravno(),
  iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
  iceb_tu_kolbait(20,dolg.ravno()),iceb_tu_kolbait(20,dolg.ravno()),dolg.ravno(),
  rozr.ravno(),tarif);

  if(iceb_tu_strlen(fio.ravno()) > 30)
   fprintf(ff_ot,"%8s %s\n","",iceb_tu_adrsimv(30,fio.ravno()));
 }

fprintf(ff_ot,"\
--------------------------------------------------------------------------------------\n");
fprintf(ff_ot,"%*s:%10.2f\n",iceb_tu_kolbait(74,gettext("Итого")),gettext("Итого"),itarif);


podpis(ff_ot);

fclose(ff_ot);



oth->spis_imaf.plus(imaf_ot);
oth->spis_naim.plus(gettext("Расчёт тарифов на базе тарифной таблицы"));
iceb_t_ustpeh(imaf_ot,3);
return(0);
}

