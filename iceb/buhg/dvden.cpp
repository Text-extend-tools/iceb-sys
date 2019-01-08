/* $Id: dvden.c,v 5.22 2013/09/26 09:43:32 sasa Exp $ */
/*14.07.2015	31.05.1999	Белых А.И.	dvden.c
Движение товаров за день
*/
#include        <errno.h>
#include        "buhg.h"

void		dkols2(char[],char[]);
void		dkols21(char[],char[]);


int dvden(class spis_oth *oth)
{
struct  tm      *bf;
time_t          tmm;
int		kom;
short		dp,mp,gp;
short		dn,mn,gn;
short		dk,mk,gk;
char		imaf[64],imaf1[64],imaf2[64];
int		sklad,kodm1,kodm;
FILE		*ff1,*ff2;
double		kolih,itmp,itmr,ost;
class iceb_tu_str nn("");
class iceb_tu_str bros("");
int		tp;
class iceb_tu_str naim(""),naimz("");
char		strsql[512];
long		kolstr,kolstr1;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

SQL_str         row,row1;
SQLCURSOR curr;
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт движения материалов за день"));

VV.VVOD_SPISOK_add_MD(gettext("Склад..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата...."));
VV.VVOD_SPISOK_add_data(5);
VV.VVOD_SPISOK_add_data(11);
sprintf(strsql,"%d.%d.%d",dt,mt,gt);
VV.VVOD_SPISOK_zapis_data(1,strsql);

naz:;

clear();


helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F10",gettext("виход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);
switch(kom)
 {
  case FK2:
  case PLU:
    break;
  case FK10:
  case ESC:
    return(1);

 default:
    goto naz;
 }
if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён склад !"));
  goto naz;
 }

if(rsdat(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(1),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }
clear();
GDITE();

time(&tmm);
bf=localtime(&tmm);

sklad=(int)ATOFM(VV.VVOD_SPISOK_return_data(0));

dp=dk=dn;
mp=mk=mn;
gp=gk=gn;

dpm(&dp,&mp,&gp,2);

sprintf(strsql,"select kodm,nomd,ei,kolih,tipz from Dokummat1 where \
datd='%d-%02d-%02d' and sklad=%d",
gn,mn,dn,sklad);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %d.%d.%d",gettext("Не найдено ни одной записи !"),dn,mn,gn);
  iceb_t_soob(strsql);
  return(1);
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={"buhkom"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kodgr char(32) not null,\
naimmat char(255) not null,\
kodmt int not null,\
nomdok char(32) not null DEFAULT '',\
ei char(32) not null DEFAULT '',\
kolih double(16,6) NOT NULL DEFAULT 0.,\
tipz tinny not null DEFAULT 0) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

kolstr1=0;
class iceb_tu_str naimmat("");
class iceb_tu_str nomdok("");
class iceb_tu_str ei("");

while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
   /*Узнаем наименование материалла*/
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %s\n",gettext("Не найдено наименование материалла !"),row[0]);
    OSTANOV();
    continue;
   }
  naimmat.new_plus(row1[1]);
  nomdok.new_plus(row[1]);
  ei.new_plus(row[2]);

  sprintf(strsql,"insert into %s values ('%s','%s',%d,'%s','%s',%.10g,%d)",
  imatmptab,
  row1[0],
  naimmat.ravno_filtr(),
  atoi(row[0]),
  nomdok.ravno_filtr(),
  ei.ravno_filtr(),
  atof(row[3]),
  atoi(row[4]));  
  
  sql_zapis(strsql,1,0);    

 }
printw(gettext("Печатаем\n"));
refresh();
GDITE();

SQL_str rowtmp;
class SQLCURSOR curtmp;
int kolstrtmp=0;

sprintf(strsql,"select * from %s order by kodgr,naimmat,kodmk,nomdok,ei,kolih,tipz asc",imatmptab);
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sprintf(imaf1,"dv1%d.tmp",getpid());

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

sprintf(imaf2,"dv2%d.tmp",getpid());

if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }

fprintf(ff1,"%s\n%s %d\n\
%s %d.%d.%d%s\n\
%s %d.%d.%d%s\n%s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Движение материалов по складу"),
sklad,
gettext("За"),
dn,mn,gn,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff2,"%s\n%s %d\n\
%s %d.%d.%d%s\n\
%s %d.%d.%d%s\n%s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Движение материалов по складу"),
sklad,
gettext("За"),
dn,mn,gn,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

kolstr1=0;
ost=0.;
itmp=itmr=kodm=kodm1=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {

  strzag(LINES-1,0,kolstrtmp,kolstr1);
  naim.new_plus(rowtmp[1]);
  kodm=atoi(rowtmp[2]);
  nn.new_plus(rowtmp[3]);
  ei.new_plus(rowtmp[4]);
  kolih=atof(rowtmp[5]);
  tp=atoi(rowtmp[6]);



  if(kodm1 != kodm)
   {
    if(kodm1 != 0)
     {
      fprintf(ff1,"%*s %10.2f\n",iceb_tu_kolbait(21,gettext("Итого приход")),gettext("Итого приход"),itmp);
      fprintf(ff1,"%*s %10.2f\n",iceb_tu_kolbait(21,gettext("Итого расход")),gettext("Итого расход"),itmr);
      fprintf(ff1,"%*s %10.2f\n",iceb_tu_kolbait(21,gettext("Остаток")),gettext("Остаток"),ost+itmp-itmr);
      fprintf(ff2,"%-5d %-*.*s ",kodm1,iceb_tu_kolbait(30,naimz.ravno()),iceb_tu_kolbait(30,naimz.ravno()),naimz.ravno());
      fprintf(ff2,"%8.8g %8.8g %8.8g %8.8g\n",ost,itmp,itmr,ost+itmp-itmr);
     }
    ost=0.;
    ost=ostdok1(dp,mp,gp,sklad,kodm);

    fprintf(ff1,"\n%-5d %-*.*s\n%*s %10.2f\n",
    kodm,
    iceb_tu_kolbait(34,naim.ravno()),iceb_tu_kolbait(34,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(21,gettext("Остаток на начало дня")),gettext("Остаток на начало дня"),ost);

    kodm1=kodm;
    naimz.new_plus(naim.ravno());
    itmp=itmr=0.;
   }

  if(tp == 1)
    itmp+=kolih;  
  if(tp == 2)
    itmr+=kolih;  

  if(tp == 1)
   bros.new_plus("+");
  if(tp == 2)
   bros.new_plus("-");
  fprintf(ff1,"%-*s %-4s %10.2f %s\n",
  iceb_tu_kolbait(16,nn.ravno()),nn.ravno(),ei.ravno(),kolih,bros.ravno());
  
 }
fprintf(ff1,"%*s %10.2f\n",iceb_tu_kolbait(21,gettext("Итого приход")),gettext("Итого приход"),itmp);
fprintf(ff1,"%*s %10.2f\n",iceb_tu_kolbait(21,gettext("Итого расход")),gettext("Итого расход"),itmr);
fprintf(ff1,"%*s %10.2f\n",iceb_tu_kolbait(21,gettext("Остаток")),gettext("Остаток"),ost+itmp-itmr);
fprintf(ff2,"%-5d %-*.*s ",kodm1,iceb_tu_kolbait(30,naimz.ravno()),iceb_tu_kolbait(30,naimz.ravno()),naimz.ravno());
fprintf(ff2,"%8.8g %8.8g %8.8g %8.8g\n",ost,itmp,itmr,ost+itmp-itmr);

fclose(ff1);
fclose(ff2);

sprintf(imaf,"dv%d%d%d.lst",dn,mn,gn);
dkols2(imaf,imaf1);
unlink(imaf1);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Движение материаллов за день (форма 1)"));
sprintf(imaf,"dv1%d%d%d.lst",dn,mn,gn);
dkols21(imaf,imaf2);
unlink(imaf2);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Движение материаллов за день (форма 2)"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

printw_vr(tmm);


OSTANOV();
return(0);
}
/*************************************/
/*Преобразование файла в две колонки*/
/*************************************/
void		dkols2(char imaf[],char imaf1[])
{
short		i;
FILE		*ff,*ff1;
#define	   KOLST  80 /*Количество строк на листе*/
char		mas1[KOLST][128];
char		mas2[KOLST][128];
char		mas3[KOLST][128];
short		shet;
short		kls,kr;
char		str[1024];
int		koll;

if((ff1 = fopen(imaf1,"r")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

//fprintf(ff,"\x1b\x6C%c",9); /*Установка левого поля*/
fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/
//fprintf(ff,"\x1B\x4D"); /*12-знаков*/
//fprintf(ff,"\x0F");     /*Ужатый режим*/
memset(mas1,'\0',sizeof(mas1));
memset(mas2,'\0',sizeof(mas2));
memset(mas3,'\0',sizeof(mas3));

koll=kr=kls=shet=0;
while(FGETSS(str,sizeof(str),ff1) != (char)NULL)
 {
  
  if(shet == 3)
   {
    for(i=0; i < kr; i++)
     {
 /*       printw("%s\n",mas1[i]);*/
      fprintf(ff,"%-40s %-40s %s\n",mas1[i],mas2[i],mas3[i]);
     }
    koll++;
    fprintf(ff,"\f");
    memset(mas1,'\0',sizeof(mas1));
    memset(mas2,'\0',sizeof(mas2));
    memset(mas3,'\0',sizeof(mas3));
    kr=shet=0;
    kls=0;
   }

  if(shet == 0)
   {
    strncpy(mas1[kls++],str,sizeof(mas1[0]));
    kr++;
   }

  if(shet == 1)
   {
    strncpy(mas2[kls++],str,sizeof(mas2[0]));
   }
  if(shet == 2)
   {
    strncpy(mas3[kls++],str,sizeof(mas3[0]));
   }
  if(kls >= KOLST)
   {
    shet++;
    kls=0;
   }
 }

for(i=0; i < kr; i++)
  fprintf(ff,"%-40s %-40s %s\n",mas1[i],mas2[i],mas3[i]);

//fprintf(ff,"\x1B\x50"); /*10-знаков*/
//fprintf(ff,"\x12");
fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
//fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/

fclose(ff);
fclose(ff1);
printw("%s %d\n",gettext("Количество листов"),koll);
}

/*************************************/
/*Преобразование файла в две колонки*/
/*************************************/
void		dkols21(char imaf[],char imaf1[])
{
short		i;
FILE		*ff,*ff1;
#define	   KOLST  80 /*Количество строк на листе*/
char		mas1[KOLST][128];
char		mas2[KOLST][128];
short		shet;
short		kls,kr;
char		str[1024];
int		koll;

if((ff1 = fopen(imaf1,"r")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

//fprintf(ff,"\x1b\x6C%c",5); /*Установка левого поля*/
fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/
//fprintf(ff,"\x1B\x4D"); /*12-знаков*/
//fprintf(ff,"\x0F");     /*Ужатый режим*/
memset(mas1,'\0',sizeof(mas1));
memset(mas2,'\0',sizeof(mas2));

koll=kr=kls=shet=0;
while(FGETSS(str,sizeof(str),ff1) != (char)NULL)
 {
  
  if(shet == 2)
   {
    for(i=0; i < kr; i++)
     {
 /*       printw("%s\n",mas1[i]);*/
      fprintf(ff,"%-75s %s\n",mas1[i],mas2[i]);
     }
    koll++;
    fprintf(ff,"\f");
    memset(mas1,'\0',sizeof(mas1));
    memset(mas2,'\0',sizeof(mas2));
    kr=shet=0;
    kls=0;
   }

  if(shet == 0)
   {
    strncpy(mas1[kls++],str,sizeof(mas1[0]));
    kr++;
   }

  if(shet == 1)
   {
    strncpy(mas2[kls++],str,sizeof(mas2[0]));
   }
  if(kls >= KOLST)
   {
    shet++;
    kls=0;
   }
 }

for(i=0; i < kr; i++)
  fprintf(ff,"%-75s %s\n",mas1[i],mas2[i]);

//fprintf(ff,"\x1B\x50"); /*10-знаков*/
//fprintf(ff,"\x12");
fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
//fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/

fclose(ff);
fclose(ff1);
printw("%s %d\n",gettext("Количество листов"),koll);
}
