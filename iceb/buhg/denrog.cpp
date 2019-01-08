/* $Id: denrog.c,v 5.33 2013/09/26 09:43:29 sasa Exp $ */
/*28.03.2018	06.05.2001	Белых А.И.	denrog.c
Получение списка работников с датой рождения
*/
#include        <errno.h>
#include        "buhg.h"

extern short	mmm,ggg;

int denrog(class spis_oth *oth)
{
struct  tm      *bf;
time_t           tmm;
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str kateg("");
int		i;
short		dn,mn,gn,dk,mk,gk;
short		dr,mr,gr;
int		kolstr;
SQL_str		row;
char		strsql[512];
FILE		*ff;
char		imaf[64];
char		pol[64];
int		vozr=0;
int		kom1=0;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
class iceb_tu_str data1("");
class iceb_tu_str data2("");

VVOD MENU(3);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка работников с датой рождения"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м).."));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Категория.....(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),6);
VV.VVOD_SPISOK_add_data(dat2.ravno(),6);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kateg.ravno(),128);


naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

i=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
podr.new_plus(VV.VVOD_SPISOK_return_data(2));
kateg.new_plus(VV.VVOD_SPISOK_return_data(3));

switch(i)
 {
  case ESC:
  case FK10:
    return(1);

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_3_1_1.txt");
   goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;


      case 0 :
        kod.new_plus("");
        naim.new_plus("");
        if(dirzarpodr(1,&kod,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(2,podr.ravno());
        goto naz;

      case 1 :
        vibrek("Kateg",&kateg);
        VV.VVOD_SPISOK_zapis_data(3,kateg.ravno());
        goto naz;

      }
     break;
           
  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }
data1.new_plus(dat1.ravno());
data2.new_plus(dat2.ravno());

sprintf(strsql,".%d",ggg);
data1.plus(strsql);
if(dat2.ravno()[0] != '\0')
  data2.plus(strsql);

if(rsdatp(&dn,&mn,&gn,data1.ravno(),&dk,&mk,&gk,data2.ravno()) != 0)
  goto naz;

GDITE();
clear();

time(&tmm);
bf=localtime(&tmm);

sprintf(strsql,"select tabn,fio,podr,kateg,denrog,pl from Kartb where \
datk = '0000-00-00' order by pl,denrog asc");
SQLCURSOR cur;
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

sprintf(imaf,"spisok%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
fprintf(ff,"%s\n\n\
%s %d.%d %s %d.%d\n\
%s %d.%d.%d%s  %s:%02d:%02d\n",
iceb_t_get_pnk("00",0),
gettext("Список работающих с датой рождения за период с"),
dn,mn,
gettext("до"),
dk,mk,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(podr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr.ravno());
if(kateg.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Категория"),kateg.ravno());
 
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Т/н|              Фамилия                   |Дата рожд.|Возраст| Пол\n"));
fprintf(ff,"\
--------------------------------------------------------------------------\n");

int kolstr1=0;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%4s %-40s\n",row[0],row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(podr.ravno(),row[2],0,0) != 0)
    continue;
  if(proverka(kateg.ravno(),row[3],0,0) != 0)
    continue;

  if(row[4] == NULL || row[4][0] == '\0')
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s",row[0],row[1],row[4]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введена дата рождения !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    continue;
   }

  dr=0;
  mr=0;
  gr=0;
  rsdat(&dr,&mr,&gr,row[4],2);
  int	ii=10;
  int	ii1=10;
  if((ii=sravmydat(dr,mr,ggg,dn,mn,ggg)) >= 0 && 
  (ii1=sravmydat(dr,mr,ggg,dk,mk,ggg)) <= 0)
   {
 //   printw("%d.%d %d.%d %d.%d/ %d %d\n",dr,mr,dn,mn,dk,mk,ii,ii1);
    sprintf(pol,"%s",gettext("мужчина"));
    if(row[5][0] == '1')
      sprintf(pol,"%s",gettext("женщина"));

    vozr=ggg-gr;

    printw("%4s %-*.*s %02d.%02d.%d %7d %s\n",
    row[0],
    iceb_tu_kolbait(30,row[1]),iceb_tu_kolbait(30,row[1]),row[1],
    dr,mr,gr,vozr,pol);

    fprintf(ff,"%4s %-*s %02d.%02d.%d %7d %s\n",
    row[0],
    iceb_tu_kolbait(40,row[1]),row[1],
    dr,mr,gr,vozr,pol);
   }
 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
podpis(ff);
fclose(ff);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка списка работников с датой рождения"));
iceb_t_ustpeh(imaf,1);
return(0);
}
