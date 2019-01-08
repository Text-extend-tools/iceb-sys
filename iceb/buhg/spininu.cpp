/* $Id: spininu.c,v 5.29 2013/09/26 09:43:40 sasa Exp $ */
/*28.03.2018    08.04.1994      Белых А.И.      spininu.c
Получит список работников не имеющих выбранного начисления
или удержания
*/
#include        <errno.h>
#include        "buhg.h"

extern short    mmm,ggg;

int spininu(class spis_oth *oth)
{
struct  tm      *bf;
time_t          tmm;
char            imaf[64];
FILE            *kaw;
int             kolp; /*Количесво людей*/
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[512];
class iceb_tu_str fio("");
long		tabn;
short		mn,gn,mk,gk;
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str nahis("");
static class iceb_tu_str uder("");
int		kom,kom1;
int		i1;
short		prn;
int		metka;

if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Получение списка работников, которые неимеют начисление/удержание"));
VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(nahis.ravno(),128);
VV.VVOD_SPISOK_add_data(uder.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код начисления....(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код удержания.....(,,)."));

naz:;


clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
nahis.new_plus(VV.data_ravno(3));
uder.new_plus(VV.data_ravno(4));

class iceb_tu_str kod("");
class iceb_tu_str naim("");

switch(kom)
 {
  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("zarp4_3_4.txt");
    goto naz;

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка удержаний"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;
      

      case 1 :
        if(dirnach(1,&nahis,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(3,nahis.ravno());
        goto naz;

      case 2 :
        if(diruder(1,&kod,&naim) == 0)
         uder.z_plus(kod.ravno());
        VV.VVOD_SPISOK_zapis_data(4,uder.ravno());
        goto naz;

     }
  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdat1(&mn,&gn,VV.VVOD_SPISOK_return_data(0)) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(rsdat1(&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конца !"));
   goto naz;
  }
if(nahis.ravno()[0] == '\0' && uder.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не задано ни начисления ни удержания !"));
  goto naz;
 }

clear(); /*Очистить экран и закрасить фоновым цветом*/
GDITE();

sprintf(strsql,"select tabn,fio,podr from Kartb order by tabn asc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного табельного номера !"));
  return(1);
 }

sprintf(imaf,"sn%d.lst",getpid());

if((kaw = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

time(&tmm);
bf=localtime(&tmm);
fprintf(kaw,"%s\n",iceb_t_get_pnk("00",0));

fprintf(kaw,"%s\n",gettext("Список работников не имеющих нач./удер."));


fprintf(kaw,"%s %d.%d.%d%s %s: %d:%d %s - %d.%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,
gettext("Текущая дата"),
mmm,ggg);
if(podr.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr.ravno());
if(nahis.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды начислений вошедшие в расчёт"),nahis.ravno());
if(uder.ravno()[0] != '\0')
 fprintf(kaw,"%s:%s\n",gettext("Коды удержаний вошедшие в расчёт"),uder.ravno());

fprintf(kaw,"-----------------------------------------------------------------------\n");


kolp=0;
printw("----------------------------------------------\n");
kolstr1=0;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(podr.ravno(),row[2],0,0) != 0)
    continue;
  
  tabn=atol(row[0]);
  fio.new_plus(row[1]);
  
  sprintf(strsql,"select prn,knah from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%ld",
  gn,mn,gk,mk,tabn);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
    continue;

  metka=0;
  while(cur1.read_cursor(&row1) != 0)
   {

    prn=atoi(row1[0]);

    if(prn == 1 && nahis.ravno()[0] == '-')
       continue;
    if(prn == 1 && nahis.ravno()[0] != '\0')
     if(pole1(nahis.ravno(),row1[1],',',0,&i1) == 0 || SRAV(nahis.ravno(),row1[1],0) == 0)
      {
       metka++;        
       continue;
      }

    if(prn == 2 && uder.ravno()[0] == '-')
       continue;
       
    if(prn == 2 && uder.ravno()[0] != '\0')
     if(pole1(uder.ravno(),row1[1],',',0,&i1) == 0 || SRAV(uder.ravno(),row1[1],0) == 0)
      {
       metka++;        
       continue;
      }
   }
  if(metka == 0)
   {
    printw("%-6ld %s\n",tabn,fio.ravno());
    fprintf(kaw,"%-7ld %s\n",tabn,fio.ravno());
    kolp++;
   }
 }

fprintf(kaw,"-----------------------------------------------------------------------\n");
fprintf(kaw,"%s: %d",
gettext("Количество работников"),kolp);

printw("----------------------------------------------\n");
printw("%s: %d",
gettext("Количество работников"),kolp);

podpis(kaw);
fclose(kaw);

OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список работников не имеющих нач./удер."));

iceb_t_ustpeh(imaf,1);

return(0);
}
