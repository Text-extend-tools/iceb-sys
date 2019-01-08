/*28.03.2018	02.03.2002	Белых А.И.	vcss.c
Программа распечатки ведомости начисления премии для 
Винницкого центра стандартизации и сертификации
*/
#include 	"../iceb.h"
#define         DVERSIQ "03.01.2017"
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include        <errno.h>
#include        <unistd.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        <memory.h>
#include        <pwd.h>
#include        <time.h>
#include        <locale.h>
#include        "../buhg/buhg.h"

int		zarnast(void);
int		vcssprem(void);
void	rasfam(short mn,short gn,long tabn,double *nahis,
double *nahisb,double *itoggor,double *itoggorb,double *itoggorpodr,
double *itoggorpodrb,char *stroka,FILE *ff,FILE *ffb);
//void 		vcsspremr(short,short,char*,char*);
void	sapka(short mn,short gn,int podr,class masiv_din_int *spisok,char *stroka,int metka,FILE *ff);
void		goritog(double*,double*,char*,int,int,FILE*);
int vcss_rn(class masiv_din_int *spisok);

void vcsspremr(short mn,short gn,class iceb_tu_str *podrp,char *grupa);

short           ddd,mmm,ggg; /*Месяц год*/
extern int	iscolor;
extern char		*putnansi;
extern char		*imabaz;
SQL_baza	bd;
extern char	*nameprinter;
int		ktorab=0;
extern char	*shetb; /*Бюджетные счета начислений*/
int kolih_nah=0;

int             main(int argc,char **argv,char **envp)
{
struct  tm      *bf;
long            tmm;

icebstart(argc,argv);

/*Ищем настройку*/
if(nastsys() != 0)
  goto kon;
if(zarnast() != 0)
 goto kon;

time(&tmm);
bf=localtime(&tmm);

ddd=bf->tm_mday;
if(bf->tm_mday > 14)
   mmm=bf->tm_mon+1;
else
   mmm=bf->tm_mon+1-1;
ggg=bf->tm_year+1900;
if(mmm == 0)
 {
  mmm=12;
  ggg-=1;
 }


while((vcssprem()) == 0)


kon:;

sql_closebaz(&bd);
VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("База закрыта. Программа закончила работу."));
soobsh(&SOOBN,-1,-1,1);
refresh();
endwin();
}
/****************************************/
/* Получение ведомости начисления премии*/
/****************************************/
int vcssprem()
{
static char	dat1[32];
static class iceb_tu_str     podr("");
static char     grupa[112];
int		kom=0;
class iceb_tu_str naimp("");
class iceb_tu_str kod("");
short		mn,gn;

if(dat1[0] == '\0')
 sprintf(dat1,"%d.%d",mmm,ggg);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG("Распечатка ведомости начисления премиальных");

VV.VVOD_SPISOK_add_data(dat1,8);
VV.VVOD_SPISOK_add_data(podr.ravno(),112);
VV.VVOD_SPISOK_add_data(grupa,sizeof(grupa));
VV.VVOD_SPISOK_add_MD("Дата..............(м.г).,.");
VV.VVOD_SPISOK_add_MD("Код подразделения....(,,).");
VV.VVOD_SPISOK_add_MD("Группа подразделения.(,,).");

naz:;

clear(); 

helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F3","підрозділи",
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

strncpy(dat1,VV.VVOD_SPISOK_return_data(0),sizeof(dat1)-1);
podr.new_plus(VV.VVOD_SPISOK_return_data(1));
strncpy(grupa,VV.VVOD_SPISOK_return_data(2),sizeof(grupa)-1);

naimp.new_plus("");
kod.new_plus("");
switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
// sprintf(bros,"%s/doc/%s",putnansi,"zarp4_2_2.txt");
// prosf(bros);
   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK2:
  case PLU:
   break;

  case FK3:
   
    if(dirzarpodr(2,&kod,&naimp) == 0)
     VV.data_z_plus(1,kod.ravno());
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  case FK5: //Файл со списком начислений
    iceb_redfil("vcss.alx",1);
    goto naz;

  default:
    goto naz;
 }

if(rsdat1(&mn,&gn,VV.VVOD_SPISOK_return_data(0)) != 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не верно введена дата начала !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }

vcsspremr(mn,gn,&podr,grupa);
return(0);
}
/*********************/
/*Формирование отчета*/
/*********************/
void vcsspremr(short mn,short gn,class iceb_tu_str *podrp,char *grupa)
{
char		strsql[1024];
char		bros[512];
char		bros1[512];
SQL_str		row,row1;
int		kolstr=0,kolstr1=0;
time_t		tmm;
long		tabn=0,tabnz=0;
int		podr=0,podrz=0;
int		knah=0;
double		suma=0.;
int		i=0,ipodr=0;
FILE		*ff,*ffb,*ffpodr;
char		imaf[40],imafb[40],imafpodr[40],imafpodrb[40];
char		stroka[1024];
SQLCURSOR       cur,cur1;
int		kolpodr=0;
char		naip[512];
class masiv_din_int spisok;
SQLCURSOR curr;

vcss_rn(&spisok);
kolih_nah=spisok.kolih();
if(kolih_nah == 0)
 {
  iceb_t_soob("Количество настроек равно нолю!");
  return;
 }
double		nahis[kolih_nah];  //Общее начисление
double		nahisb[kolih_nah]; //Бюджет
double		itoggor[kolih_nah+1];
double		itoggorb[kolih_nah+1];
double		itoggorpodr[kolih_nah+1];
double		itoggorpodrb[kolih_nah+1];

time(&tmm);
clear();

sprintf(strsql,"select kod from Podr order by kod asc");

if((kolpodr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }
int    maspodr[kolpodr];
double sumapodr[kolpodr*kolih_nah];
memset(&sumapodr,'\0',sizeof(sumapodr));
double sumapodrb[kolpodr*kolih_nah];  
memset(&sumapodrb,'\0',sizeof(sumapodrb));

i=0;
while(cur.read_cursor(&row) != 0)
  maspodr[i++]=atoi(row[0]); 

strcpy(stroka,"-----------------------");
for(i=0; i < kolih_nah+1; i++)
  strcat(stroka,"---------");

memset(bros,'\0',sizeof(bros));

for(i=0; i < kolih_nah; i++)
 {
  if(i != 0)
    strcat(bros," or ");
  sprintf(bros1,"knah=%d",spisok.ravno(i));
  strcat(bros,bros1);
 }

sprintf(strsql,"select tabn,knah,suma,podr,shet from Zarp where \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and prn='1' \
and suma <> 0. and (%s) order by podr,tabn asc",gn,mn,gn,mn,bros);
//printw("%s\n",strsql);
//OSTANOV();

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

sprintf(imaf,"prem%d_%d.lst",mn,getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта

sprintf(imafb,"premb%d_%d.lst",mn,getpid());

if((ffb = fopen(imafb,"w")) == NULL)
 {
  error_op_nfil(imafb,errno,"");
  return;
 }
fprintf(ffb,"\x1B\x45"); //режим выделенного шрифта

memset(&itoggorpodr,'\0',sizeof(itoggorpodr));
memset(&itoggorpodrb,'\0',sizeof(itoggorpodrb));
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(podrp->ravno(),row[3],0,0) != 0)
     continue;

  if(grupa[0] != '\0')
   {
     //узнаём к какой группе принадлежит подразделение
     sprintf(strsql,"select grup from Podr where kod=%s",row[3]);
     if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
      continue;

     if(proverka(grupa,row1[0],1,0) != 0)
       continue;
                  
   }
   
  tabn=atol(row[0]);
  podr=atoi(row[3]);
  knah=atoi(row[1]);
  suma=atof(row[2]);  
    
  if(tabnz != tabn)
   {
    if(tabnz != 0)
      rasfam(mn,gn,tabnz,nahis,nahisb,itoggor,itoggorb,itoggorpodr,itoggorpodrb,stroka,ff,ffb);
    memset(&nahis,'\0',sizeof(nahis));
    memset(&nahisb,'\0',sizeof(nahisb));
    tabnz=tabn;
   }

  if(podrz != podr)
   {
    printw("Підрозділ %d\n",podr);
    if(podrz != 0)
     {
      goritog(itoggor,itoggorb,stroka,1,1,ff);
      goritog(itoggor,itoggorb,stroka,1,2,ffb);
      fprintf(ff,"\f");
      fprintf(ffb,"\f");
     }    
    sapka(mn,gn,podr,&spisok,stroka,0,ff);
    sapka(mn,gn,podr,&spisok,stroka,0,ffb);
    memset(&itoggor,'\0',sizeof(itoggor));
    memset(&itoggorb,'\0',sizeof(itoggorb));
    for(ipodr=0; ipodr < kolpodr; ipodr++)
     if(maspodr[ipodr] == podr)
       break;

    podrz=podr;    
   }

  
  for(i=0; i < kolih_nah; i++)
   if(spisok.ravno(i) == knah)
    {
     nahis[i]+=suma;
     sumapodr[ipodr*kolih_nah+i]+=suma;
     if(proverka(shetb,row[4],0,1) == 0)
      {
       sumapodrb[ipodr*kolih_nah+i]+=suma;
       nahisb[i]+=suma;
      }
     break;
    }
 }

rasfam(mn,gn,tabnz,nahis,nahisb,itoggor,itoggorb,itoggorpodr,itoggorpodrb,stroka,ff,ffb);
goritog(itoggor,itoggorb,stroka,1,1,ff);
goritog(itoggor,itoggorb,stroka,1,2,ffb);

double itogo=0.;

sprintf(imafpodr,"premp%d_%d.lst",mn,getpid());
if((ffpodr = fopen(imafpodr,"w")) == NULL)
 {
  error_op_nfil(imafpodr,errno,"");
  return;
 }
fprintf(ffpodr,"\x1B\x45"); //режим выделенного шрифта

sapka(mn,gn,0,&spisok,stroka,1,ffpodr);
fprintf(ffpodr,"%s:\n",gettext("Хозрасчет"));
for(i=0; i < kolpodr;i++)
 {
  itogo=0.;
  for(int i1=0; i1 < kolih_nah ; i1++)
    itogo+=sumapodr[i*kolih_nah+i1]-sumapodrb[i*kolih_nah+i1];
  if(itogo == 0.)
    continue;

  memset(naip,'\0',sizeof(naip));
  sprintf(strsql,"select naik from Podr where kod=%d",maspodr[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1  )
    strncpy(naip,row[0],sizeof(naip)-1);
  fprintf(ffpodr,"%5d %-*.*s",
  maspodr[i],
  iceb_tu_kolbait(16,naip),
  iceb_tu_kolbait(16,naip),
  naip);

  itogo=0.;    
  for(int i1=0; i1 < kolih_nah ; i1++)
   {
    itogo+=sumapodr[i*kolih_nah+i1]-sumapodrb[i*kolih_nah+i1];
    fprintf(ffpodr," %8.2f",sumapodr[i*kolih_nah+i1]-sumapodrb[i*kolih_nah+i1]);
   }
  fprintf(ffpodr," %8.2f",itogo);

  fprintf(ffpodr,"\n");

 }

goritog(itoggorpodr,itoggorpodrb,stroka,0,1,ffpodr);
podpis(ffpodr);
fclose(ffpodr);

sprintf(imafpodrb,"prempb%d_%d.lst",mn,getpid());
if((ffpodr = fopen(imafpodrb,"w")) == NULL)
 {
  error_op_nfil(imafpodrb,errno,"");
  return;
 }
fprintf(ffpodr,"\x1B\x45"); //режим выделенного шрифта

sapka(mn,gn,0,&spisok,stroka,1,ffpodr);
fprintf(ffpodr,"%s:\n",gettext("Бюджет"));
for(i=0; i < kolpodr;i++)
 {
  itogo=0.;
  for(int i1=0; i1 < kolih_nah ; i1++)
    itogo+=sumapodrb[i*kolih_nah+i1];
  if(itogo == 0.)
    continue;

  memset(naip,'\0',sizeof(naip));
  sprintf(strsql,"select naik from Podr where kod=%d",maspodr[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1  )
    strncpy(naip,row[0],sizeof(naip)-1);
  fprintf(ffpodr,"%5d %-*.*s",
  maspodr[i],
  iceb_tu_kolbait(16,naip),
  iceb_tu_kolbait(16,naip),
  naip);

  itogo=0.;    
  for(int i1=0; i1 < kolih_nah ; i1++)
   {
    itogo+=sumapodrb[i*kolih_nah+i1];
    fprintf(ffpodr," %8.2f",sumapodrb[i*kolih_nah+i1]);
   }
  fprintf(ffpodr," %8.2f",itogo);

  fprintf(ffpodr,"\n");

 }
goritog(itoggorpodrb,itoggorpodrb,stroka,0,0,ffpodr);

podpis(ffpodr);
fclose(ffpodr);

podpis(ff);
podpis(ffb);

fclose(ff);
fclose(ffb);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus("Список работников (хозрасчёт)");
oth.spis_imaf.plus(imafb);
oth.spis_naim.plus("Список работников (бюджет)");
oth.spis_imaf.plus(imafpodr);
oth.spis_naim.plus("Список подразделений (хозрасчёт)");
oth.spis_imaf.plus(imafpodrb);
oth.spis_naim.plus("Список подразделений (бюджет)");

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);
 
iceb_t_rabfil(&oth,"");

}
/***********************/
/*Распечатка по фамилии*/
/************************/
void	rasfam(short mn,short gn,long tabn,double *nahis,
double *nahisb,double *itoggor,double *itoggorb,double *itoggorpodr,
double *itoggorpodrb,char *stroka,FILE *ff,FILE *ffb)
{
char		strsql[512];
SQL_str		row;
char		fio[512];
char		fio1[512];
int		i=0;
double		itogo=0.;
int		kold=0;
SQLCURSOR curr;

memset(fio,'\0',sizeof(fio));
memset(fio1,'\0',sizeof(fio1));
sprintf(strsql,"select fio from Kartb where tabn=%ld",tabn);
if(sql_readkey(&bd,strsql,&row,&curr) == 1  )
 {
  strncpy(fio1,row[0],sizeof(fio1)-1);
  POLE(fio1,fio,1,' ');
  strncat(fio," ",1);

  POLE(fio1,strsql,2,' ');
  strncat(fio,strsql,iceb_tu_kolbait(1,strsql));
  strncat(fio,".",1);
  POLE(fio1,strsql,3,' ');
  strncat(fio,strsql,iceb_tu_kolbait(1,strsql));
  strncat(fio,".",1);

 }
sprintf(strsql,"select dnei from Ztab where tabn=%ld and god=%d and \
mes=%d and kodt=1",tabn,gn,mn);
if(sql_readkey(&bd,strsql,&row,&curr) > 0  )
  kold=atoi(row[0]);

itogo=0.;

fprintf(ff,"\
%5ld %-*.*s %2d",
tabn,
iceb_tu_kolbait(13,fio),
iceb_tu_kolbait(13,fio),
fio,
kold);


for(i=0; i < kolih_nah ; i++)
 {
  itogo+=nahis[i]-nahisb[i];
  fprintf(ff," %8.2f",nahis[i]-nahisb[i]);
 }
fprintf(ff," %8.2f",itogo);
zar_read_tn(1,mn,gn,NULL); /*Чтение минимальной зарплаты*/
fprintf(ff,"\n");
fprintf(ff,"%5s %-15.2f %2s"," ",oklad(tabn,mn,gn,&i)," ");
fprintf(ff,"\n");
fprintf(ff,"%s\n",stroka);


itogo=0.;
for(i=0; i < kolih_nah ; i++)
  itogo+=nahisb[i];

if(itogo != 0.)
 {
  fprintf(ffb,"\
%5ld %-*.*s %2d",
  tabn,
  iceb_tu_kolbait(13,fio),
  iceb_tu_kolbait(13,fio),
  fio,
  kold);

  itogo=0.;
  for(i=0; i < kolih_nah ; i++)
   {
    itogo+=nahisb[i];
    fprintf(ffb," %8.2f",nahisb[i]);
   }
  fprintf(ffb," %8.2f",itogo);

  fprintf(ffb,"\n");
  fprintf(ffb,"%s\n",stroka);
 }

for(i=0; i < kolih_nah ; i++)
 {
  itoggor[i]+=nahis[i];
  itoggorb[i]+=nahisb[i];
  itoggorpodr[i]+=nahis[i];
  itoggorpodrb[i]+=nahisb[i];

  itoggor[kolih_nah]+=nahis[i];
  itoggorb[kolih_nah]+=nahisb[i];
  itoggorpodr[kolih_nah]+=nahis[i];
  itoggorpodrb[kolih_nah]+=nahisb[i];
 }
}
/******************/
/*Распечатка шапки*/
/********************/
void	sapka(short mn,short gn,int podr,
class masiv_din_int *spisok,
char *stroka,
int metka, //0-по фамилиям 1-по подразделениям
FILE *ff)
{
char		strsql[512];
SQL_str		row;
char		naip[512];
int		i=0;
SQLCURSOR curr;
fprintf(ff,"%s\n\n",iceb_t_get_pnk("00",0));

if(metka == 0)
 {
  memset(naip,'\0',sizeof(naip));
  sprintf(strsql,"select naik from Podr where kod=%d",podr);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1  )
    strncpy(naip,row[0],sizeof(naip)-1);

  fprintf(ff,"%s %d %s\n",gettext("Подразделение"),
  podr,naip);
 }
fprintf(ff,"%s %d.%d\n",gettext("Дата"),mn,gn);
fprintf(ff,"%s\n",stroka);

if(metka == 0)
  fprintf(ff,"\
 Т/н |   Призвище  |  |");
if(metka == 1)
  fprintf(ff,"\
 Код |   Підрозділ    |");

for(i=0; i < kolih_nah ; i++)
 fprintf(ff,"   %2d   |",spisok->ravno(i));

fprintf(ff," Разом  |");

fprintf(ff,"\n%s\n",stroka);

}
/************************************/
/*Распечатка горизонтального итога*/
/***********************************/
void	goritog(double *itoggor,double *itoggorb,char *stroka,
int metka, //0-подчеркнуть 1-нет
int metka2, //0-первый массив 1-первый минус второй 2-второй
FILE *ff)
{
int		i;

if(metka == 0)
 fprintf(ff,"%s\n",stroka);

if(metka2 == 0)
 {
  fprintf(ff,"%*s",iceb_tu_kolbait(22,"Разом"),"Разом");
  for(i=0; i < kolih_nah+1 ; i++)
   {
    fprintf(ff," %8.2f",itoggor[i]);
   }

  fprintf(ff,"\n");
  fprintf(ff,"%s\n",stroka);
 }

if(metka2 == 1)
 {
  fprintf(ff,"%*s",iceb_tu_kolbait(22,"Разом госпрозрахунок"),"Разом госпрозрахунок");
  for(i=0; i < kolih_nah+1 ; i++)
   {
    fprintf(ff," %8.2f",itoggor[i]-itoggorb[i]);
   }

  fprintf(ff,"\n");
  fprintf(ff,"%s\n",stroka);
 }

if(metka2 == 2)
 {
  fprintf(ff,"%*s",iceb_tu_kolbait(22,"Разом бюджет"),"Разом бюджет");
  for(i=0; i < kolih_nah+1 ; i++)
   {
    fprintf(ff," %8.2f",itoggorb[i]);
   }

  fprintf(ff,"\n");
  fprintf(ff,"%s\n",stroka);
 }

}
/*************************/
/*Чтение настройки*/
/****************/

int vcss_rn(class masiv_din_int *spisok)
{
FILE *ff;
class iceb_tu_str stroka("");
if(iceb_t_alxout("vcss.alx") != 0)
 return(1);
if((ff = fopen("vcss.alx","r")) == NULL)
 {
  error_op_nfil("vcss.alx",errno,"");
  return(1);
 }
int knah=0;
while(iceb_tu_fgets(&stroka,ff) == 0)
 {
  if(stroka.ravno()[0] == '#')
   continue;
  if((knah=stroka.ravno_atoi()) == 0)
    continue;
  spisok->plus(knah,-1);  
 }

fclose(ff);
unlink("vcss.alx");
return(0);

}
