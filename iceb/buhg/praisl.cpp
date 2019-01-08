/* $Id: praisl.c,v 5.40 2013/09/26 09:43:37 sasa Exp $ */
/*16.04.2017    06.08.1998      Белых А.И.      praisl.c
Программа печати прайс листа
*/
#define	   KOLST  80 /*Количество строк на листе*/
#include        <errno.h>
#include	"buhg.h"

void            kols2(const char[],const char[]);
void            sstr(short*,short*,short,const char*,class SPISOK *nvc,FILE*);
void            sprais(short,class SPISOK *nvc,FILE*);

extern double   ostg[4],ostm[4];
extern double	kursue;/*Курс условной единицы*/

void            praisl()
{
class SPISOK nvc;
class iceb_tu_str bros("");
struct  tm      *bf;
time_t          tmm;
int		km;
int		kom=0,kom1=0;
char		imaf[64],imaf1[64];
double		cena;
FILE		*ff;
int		i;
int		kgrm,kgrmz;
class iceb_tu_str sut("");
double		ost;
short		msh; /*0-нет товара с нужным счетом >0 есть*/
short		kst; /*Количество строк*/
short		kli; /*Количество листов*/
class iceb_tu_str podsh("");
short		mec;
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[1024];
int		skl,sklz;
class iceb_tu_str naimgr("");
class iceb_tu_str skladi(""),grup("");
class masiv_din_int kvc;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(gettext("Формирование прайс листа"));
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать только товары которые имеются в наличии"));
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать весь список товаров"));
MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));


naz:;
clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 3 :
  case -1 :
   return;
   break;

  case 2 :
    iceb_redfil("prais.alx",0);
    goto naz;
 }

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка с ценами в гривнях"));
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка с ценами в У.Е."));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

if(kom1 == 2 || kom1 == -1)
  return;


VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Группа..(,,)."));
if(kom == 0)
   VV.VVOD_SPISOK_add_MD(gettext("Склад..(,,).."));
VV.VVOD_SPISOK_add_data(128);
VV.VVOD_SPISOK_add_data(128);
naz1:;


helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F10",gettext("выход"),NULL);

i=VV.vvod(0,1,1,-1,-1);

switch(i)
 {
  case FK2:
  case PLU:
   break;
  case FK10:
  case ESC:
    return;
  default:
   goto naz1;
 }

skladi.new_plus(VV.VVOD_SPISOK_return_data(1));
grup.new_plus(VV.VVOD_SPISOK_return_data(0));


GDITE();
clear();

sprintf(imaf1,"prais%d.tmp",getpid());

SQL_str row_alx;
class SQLCURSOR cur_alx;

sprintf(strsql,"select str from Alx where fil='prais.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"prais.alx");
  iceb_t_soob(strsql);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  printw("%s",row_alx[0]);
  
  if(polen(row_alx[0],&bros,1,'|') != 0)
    continue;
  if(SRAV("Счета учета товаров",bros.ravno(),0) == 0)
   {
    polen(row_alx[0],&sut,2,'|');
    continue;
   }   

//  kvc[kc]=(short)ATOFM(bros);
  kvc.plus(atoi(bros.ravno()));
  polen(row_alx[0],&nvc,2,'|');
 }

printw("\n");
int kc=nvc.kolih();
if(kc == 0)
 {
  iceb_t_soob(gettext("Не найдено в файле ни одной цены !"));
  goto naz;
 }

if(sut.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены счета учета товаров !"));
//  goto naz;
 }
time(&tmm);
bf=localtime(&tmm);
printw(gettext("Готовим распечатку прайс листа.\n"));

GDITE();

sprintf(strsql,"select * from Material order by kodgr asc");
SQLCURSOR cur;
if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }



if(kolstr2 == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }
printw(gettext("Печатаем.\n"));
refresh();

sprintf(imaf,"prais%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff,"%s\n\n%s\n",gettext("ПРАЙС ЛИСТ"),iceb_t_get_pnk("00",0));

kst=7;
if(kom == 0)
 {
  kst++;
  fprintf(ff,gettext("Товары есть на складе\n")); 
 }
if(kom1 == 1)
 {
  kst++;
  fprintf(ff,gettext("Цены указаны в У.Е. Курс %.6g\n"),kursue); 
 }
if(skladi.ravno()[0] != '\0')
 {
  kst++;
  fprintf(ff,gettext("Склады %s\n"),skladi.ravno()); 
 }
if(grup.ravno()[0] != '\0')
 {
  kst++;
  fprintf(ff,gettext("Группа: %s\n"),grup.ravno()); 
 }

fprintf(ff,"\
%s %d.%d.%d%s %s: %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

sprais(kc,&nvc,ff);

/*полка над наименованием*/
podsh.new_plus("-------------------------------------");
/*полка над ценой*/
for(i=0; i< kc; i++)
  podsh.plus("---------");

kli=1;
kolstr1=kgrm=kgrmz=km=0;
while(cur.read_cursor(&row) != 0)
 {
  
/*
  printw("%s %s %s\n",row[0],row[1],row[2]);
  refresh();
*/  

 kolstr1++;
 strzag(LINES-1,0,kolstr2,kolstr1);

 if(atof(row[6]) < 0.01) //Товары с нулевой ценой в прайс не попадут
  continue;

 printw("%s %s %s\n",row[0],row[1],row[2]);
 refresh();
  

 if(proverka(grup.ravno(),row[1],0,0) != 0)
   continue;

  /*Смотрим на каких счетах учета товар и если нужно проверяем
  остаток*/
  sprintf(strsql,"select sklad,nomk,shetu from Kart where kodm=%s \
order by sklad asc",row[0]);
  SQLCURSOR cur1;
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    printw("%s\n",sql_error(&bd));
    OSTANOV();
   }

  ost=0.; 
  sklz=msh=0;
  if(kolstr != 0)
   while(cur1.read_cursor(&row1) != 0)
    {
     if(proverka(sut.ravno(),row1[2],1,0) != 0)
        continue;
     if(proverka(skladi.ravno(),row1[0],0,0) != 0)
        continue;

     skl=atoi(row1[0]);
     if(kom == 0 && sklz != skl)
      {
       ost+=ostdok1(dt,mt,gt,skl,atoi(row[0]));

       sklz=skl;
      }
     msh++;
    } 

  
  if(msh == 0)
    continue;
  if(kom == 0 && ost < 0.00001)
    continue;
    
  km++;

  kgrm=atoi(row[1]);
  if(kgrmz != kgrm)
   {
    /*Читаем код группы материалла*/
    sprintf(strsql,"select naik from Grup where kod=%s",
    row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
        beep();
        printw("Не нашли наименование группы материалов по коду %d",kgrm);
        OSTANOV();
        continue;
     }
    naimgr.new_plus(row1[0]);
    if(kgrmz != 0)
     {
      sstr(&kst,&kli,kc,podsh.ravno(),&nvc,ff);
      fprintf(ff,"%s\n",podsh.ravno());
     }

    sstr(&kst,&kli,kc,podsh.ravno(),&nvc,ff);
    fprintf(ff,"       <<< %s >>>\n",naimgr.ravno());
    kgrmz=kgrm;
   }
  
  if(atof(row[7]) != 0.)
    fprintf(ff,"|%-*.*s|%4.4g|",iceb_tu_kolbait(30,row[2]),iceb_tu_kolbait(30,row[2]),row[2],atof(row[7]));
  else
    fprintf(ff,"|%-*.*s|    |",
    iceb_tu_kolbait(30,row[2]),
    iceb_tu_kolbait(30,row[2]),
    row[2]);

  /*Печатаем цены*/
  for(i=0; i< kc ; i++)
   {
    mec=0;
    cena=0.;
    if(kvc.ravno(i) == 0)
      cena=atof(row[6]);
    if(kom1 == 1 && kursue != 0.)
     {
      cena=cena/kursue;
      cena=okrug(cena,0.01);
     } 

    if(cena == 0.)
     fprintf(ff,"%8s|"," ");    
    else
     {
      if(mec == 0)
        fprintf(ff,"%8.2f|",cena);    
      if(mec == 1)
        fprintf(ff,"%7.2f%%|",cena);    

     }
   } 

  fprintf(ff,"\n"); 
  sstr(&kst,&kli,kc,podsh.ravno(),&nvc,ff);

 }


fprintf(ff,"%s\n",podsh.ravno());

fclose(ff);

kols2(imaf,imaf1);
unlink(imaf1);
printw_vr(tmm);

printw(gettext("Количество материалов %d\n"),km);
printw(gettext("Количество листов %d\n"),kli/2);
OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка прайс листа"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);

iceb_t_rabfil(&oth,"");

}

/**********/
/*Шапка*/
/**********/
void sprais(short kc,
class SPISOK *nvc,
FILE *ff)
{
short		i;


/*Верхняя полка над наименованием*/
fprintf(ff,"\
-------------------------------------");
/*полка под ценой*/
for(i=0; i< kc; i++)
  fprintf(ff,"---------");
fprintf(ff,"\n");

fprintf(ff,gettext("|    Н а и м е н о в а н и е   |Крат|"));

for(i=0; i< kc; i++)
  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(8,nvc->ravno(i)),iceb_tu_kolbait(8,nvc->ravno(i)),nvc->ravno(i));
fprintf(ff,"\n");

/*Нижняя полка над наименованием*/
fprintf(ff,"\
-------------------------------------");
/*полка под ценой*/
for(i=0; i< kc; i++)
  fprintf(ff,"---------");
fprintf(ff,"\n");

}

/************************/
/*Счетчик строк и листов*/
/************************/
void sstr(short *kst, //Количество строк
short *kli, //Количество листов
short kc, 
const char *podsh,
class SPISOK *nvc,
FILE *ff)
{
*kst=*kst+1;
if(*kst >= KOLST)
 {
  fprintf(ff,"%s\n",podsh);
  fprintf(ff,"\f");
  sprais(kc,nvc,ff);
  *kli=*kli+1;
  *kst=3;
 }  
}

/*************************************/
/*Преобразование файла в две колонки*/
/*************************************/
void kols2(const char imaf[],const char imaf1[])
{
class iceb_tu_str nmo(""),gor(""),nsh(""),grb(""),mfo(""),pkod(""),tel("");
short		i;
FILE		*ff,*ff1;
//char		mas1[KOLST+5][512];
class SPISOK mas1;
//char		mas2[KOLST+5][512];
class SPISOK mas2;
short		shet;
short		kls,kr;
class iceb_tu_str str1("");
char		str[1024];
SQL_str         row;
SQLCURSOR       cur;

iceb_t_cp(imaf,imaf1,0);
/*Читаем реквизиты организации свои */

sprintf(str,"select * from Kontragent where kodkon='00'");
if(readkey(str,&row,&cur) == 1)
 {
  nmo.new_plus(row[1]);
  gor.new_plus(row[3]);
  pkod.new_plus(row[5]);
  mfo.new_plus(row[6]);
  nsh.new_plus(row[7]);
  grb.new_plus(row[4]);
  tel.new_plus(row[10]);
 } 

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

fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/
kr=kls=shet=0;
while(FGETSS(str,sizeof(str),ff1) != 0)
 {
  if(str[0] == '\f')
   {
    polen(str,&str1,2,'\f');
    strcpy(str,str1.ravno());

    if(shet == 0)
      shet++;
    else
     {
      for(i=0; i < kr; i++)
       {
        fprintf(ff,"%-*s",
        iceb_tu_kolbait(70,mas1.ravno(i)),
        mas1.ravno(i));

        if(i < mas2.kolih())
         fprintf(ff," %s", mas2.ravno(i));

        fprintf(ff,"\n");
       }
  
      fprintf(ff,gettext("%s %s. Р/с %s в %s МФО %s Код ОКПО %s Тел. %s\n"),
      nmo.ravno(),gor.ravno(),nsh.ravno(),grb.ravno(),mfo.ravno(),pkod.ravno(),tel.ravno());

      fprintf(ff,"\f");
      mas1.free_class();
      mas2.free_class();
      kr=shet=0;
     }
    kls=0;
   }   

  if(shet == 0)
   {
    mas1.plus(str);
    kr++;
    if(kr >= KOLST+5)
     {
      beep();
      printw("Криминал !!!\n");      
      OSTANOV();
      break;
     }
    kls++;
   }

  if(shet == 1)
   {
    mas2.plus(str);
    kls++;
   }
 }

for(i=0; i < kr; i++)
 {
  fprintf(ff,"%-*s",
  iceb_tu_kolbait(70,mas1.ravno(i)),
  mas1.ravno(i));
  if( i < mas2.kolih())
   fprintf(ff," %s",mas2.ravno(i));
  fprintf(ff,"\n");
 }
fprintf(ff,gettext("%s %s. Р/с %s в %s МФО %s Код ОКПО %s Тел. %s\n"),
nmo.ravno(),gor.ravno(),nsh.ravno(),grb.ravno(),mfo.ravno(),pkod.ravno(),tel.ravno());


fclose(ff);
fclose(ff1);

}
