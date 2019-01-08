/* $Id: rasnash.c,v 5.31 2013/09/26 09:43:38 sasa Exp $ */
/*28.03.2018	07.05.2001	Белих А.И.	rasnash.c
Распечатка всех начислений на человека
*/
#include        <errno.h>
#include        "buhg.h"

void 	rasnashr(short,short,const char*,const char*,FILE*);

extern short	mmm,ggg;

int rasnash(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str podr("");
static class iceb_tu_str kategr("");
time_t		tmm;
short		mn,gn;
char		imaf[64];
FILE		*ff;
int		kom=0;
int		kom1=0;

if(dat1.ravno()[0] == '\0')
 {
  char bros[64];
  sprintf(bros,"%d.%d",mmm,ggg);
  dat1.new_plus(bros);
 }
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Рапечатка свода начислений по работникам"));

VV.VVOD_SPISOK_add_MD(gettext("Дата ...........(м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории......(,,)."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kategr.ravno(),128);

naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
podr.new_plus(VV.data_ravno(1));
kategr.new_plus(VV.data_ravno(2));

class iceb_tu_str naimp("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(bros);
   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naimp) == 0)
          VV.data_z_plus(1,podr.ravno());
        goto naz;

      case 1 :
        vibrek("Kateg",&kategr,&naimp);
        VV.VVOD_SPISOK_zapis_data(1,kategr.ravno());
        goto naz;

     }

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

time(&tmm);
GDITE();
clear();

sprintf(imaf,"nash%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

rasnashr(mn,gn,podr.ravno(),kategr.ravno(),ff);

podpis(ff);
fclose(ff);
printw_vr(tmm);

OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Рапечатка свода начислений по работникам"));
iceb_t_ustpeh(imaf,0);
return(0);

}
/*************************/
/*Формирование распечатки*/
/*************************/
void	rasnashr(short mn,short gn,
const char *podr, //Подразделения
const char *kategor, //Код категории
FILE *ff)
{
char		strsql[512];
SQL_str		row,row1,row2;
int		kolstr,kolstr1,kolstr2;
class iceb_tu_str fio("");
short		dkm;
struct  tm      *bf;
time_t          tmm;
class iceb_tu_str naimkod("");
class iceb_tu_str nait("");
int		klst=0;

sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and \
mes=%d",gn,mn);
SQLCURSOR cur;
SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return;
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

time(&tmm);
bf=localtime(&tmm);

dpm(&dkm,&mn,&gn,5);

fprintf(ff,"%s\n\n\
%s\n\
%s 1.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n\n",
iceb_t_get_pnk("00",0),
gettext("Рапечатка свода начислений по работникам."),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
dkm,mn,gn,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(podr[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  printcod(ff,"Podr","kod","naik",0,podr,&klst);
 }
else
 fprintf(ff,gettext("По всем подразделениям.\n"));

if(kategor[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Коды категорий вошедшие в расчёт"),kategor);

kolstr1=0;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(podr,row[1],0,0) != 0)
    continue;
  if(proverka(kategor,row[2],0,0) != 0)
    continue;

  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    fio.new_plus(row1[0]);    
  else
    fio.new_plus("");

  fprintf(ff,"\n%-5s %-*.*s\n\
-------------------------------------------------------\n",
  row[0],
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno());

  /*Читаем табель*/
  sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where \
god=%d and mes=%d and tabn=%s",gn,mn,row[0]);
  SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,"",strsql);

  while(cur1.read_cursor(&row1) != 0)
   {
    /*Читаем наименование табеля*/
    sprintf(strsql,"select naik from Tabel where kod=%s",row1[0]);


    if(sql_readkey(&bd,strsql,&row2,&curr) != 1)
     {
      beep();
      printw("%s %s !\n",gettext("Не найден код табеля"),row2[0]);
      OSTANOV();
      continue;
     }
    else
      nait.new_plus(row2[0]);

     fprintf(ff,"%-*.*s: %2.fд. %3.fч. %2.fк.д.\n",
     iceb_tu_kolbait(40,nait.ravno()),iceb_tu_kolbait(40,nait.ravno()),nait.ravno(),
     atof(row1[1]),atof(row1[2]),atof(row1[3]));

    }

  sprintf(strsql,"select knah,suma,shet,kom from Zarp where \
datz >= '%d-%d-1' and datz <= '%d-%d-%d' and tabn=%s and prn='1' \
and suma <> 0. order by datz,knah asc",gn,mn,gn,mn,dkm,row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,"",strsql);

  while(cur1.read_cursor(&row1) != 0)
   {
    sprintf(strsql,"select naik from Nash where kod=%s",row1[0]);
    if(sql_readkey(&bd,strsql,&row2,&curr) == 1)
      naimkod.new_plus(row2[0]);
    else
      naimkod.new_plus("");
    fprintf(ff,"%2s %-*.*s %*s %8.2f %s\n",
    row1[0],
    iceb_tu_kolbait(30,naimkod.ravno()),iceb_tu_kolbait(30,naimkod.ravno()),naimkod.ravno(),
    iceb_tu_kolbait(5,row1[2]),row1[2],
    atof(row1[1]),row1[3]);
   }

  
 }

}
