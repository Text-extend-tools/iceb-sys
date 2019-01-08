/* $Id: zarsprpr.c,v 5.70 2013/09/26 09:43:46 sasa Exp $ */
/*17.05.2018    31.08.1994      Белых А.И.      zarsprpr.c
получение справки всех начислений и удержаний
за любой период
*/
#include        <errno.h>
#include        "buhg.h"

void rekvizkon(SQL_str,FILE*);
int zarsprpr_sub_start(short god,const char *kedrpou,const char *fio,const char *inn,const char *dolg,const char *adres,FILE *ff_ras);
int zarsprpr_sub_end(short mn,short gn,short mk,short gk,double suma,FILE *ff_ras);
void zarsprpr_zag(short mn,short gn,short mk,short gk,const char *harac,const char *fam,const char *inn,const char *dolg,const char *podr,int sovm,FILE *ff2);
void zarsprpr_ssu(short mn,short gn,short mk,short gk,class masiv_din_double *mes_nash,class masiv_din_double *mes_obud,FILE *ff);

extern short    ddd,mmm,ggg;
extern class iceb_tu_str kodpn_all;
extern short    *kodmp;   /*Коды материальной помощи*/
extern double   snmpz; /*Сумма не облагаемой материальной помощи заданная*/
extern char     *shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern short	*obud; /*Обязательные удержания*/
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short	kodindex;  /*Код индексации*/
extern class iceb_tu_str kods_esv_all;  /*Все коды удержания единого социального взноса*/
extern short kodvn; /*код военного сбора*/

int zarsprpr(long tabn,
short mn,short gn,
short mk,short gk,class spis_oth *oth)
{
int kodpen=0;  /*Код пенсионных отчислений*/
struct  tm      *bf;
time_t		tmr;
class iceb_tu_str bros("");
class iceb_tu_str str("");
short           koltb; /*Количество видов табелей*/
FILE            *ff,*ff1,*ff2,*ff_stud;
char            imaf[64],imaf1[64],imaf2[64],imafvn[64],imafshet[64];
char imaf_stud[64];
short           m,g;
double          inah,iuder;
double		inahb; //Итого начислено без кодов не входящих в расчёт подоходного налога
double          ino,inob,iuo,bb;
short           kon;     /*Количество статей начисления*/
short           kou;     /*Количество статей удержания*/
class iceb_tu_str nai("");
int             i,i1;
double		peno;  /*Пенсионное отчисление*/
double		ipeno;  /*Итого Пенсионное отчисление*/
double          podoh; /*Подоходный налог за месяц*/
double          ipodoh; /*Подоходный налог за год*/
class iceb_tu_str fam("");
class iceb_tu_str fambuh("");
double          sld;
double          skm,iskm;
double          alim,ialim;
double		fbezr,ifbezr;
short           idn,ikdn;
double          ihas;
short           fo;
short           sovm; /*0-нет 1- Совместитель*/
double		sal;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		kodt;
float		dnei,has,kdnei;
short		prn;
int		knah;
double		sym;
short		dz,mz,gz;
class iceb_tu_str inn("");
short		ots=5;
class iceb_tu_str dolg("");
int		kodalim;
double		saldb;
class iceb_tu_str harac("");
class iceb_tu_str shet("");
class iceb_tu_str kodnvrspr("");
double          soc_strax=0.;
double          isoc_strax=0.;
double suma_indeks=0.;
double isuma_indeks=0.;
class iceb_tu_str kedrpou("");
class iceb_tu_str podr("");
class iceb_tu_str naim_mes("");
SQLCURSOR curr;

sprintf(strsql,"select fio,sovm,inn,dolg,harac,podr from Kartb where tabn=%ld",tabn);
if(readkey(strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %ld !",gettext("Не найден табельный номер"),tabn);
  iceb_t_soob(strsql);
  return(1);
 }


fam.new_plus(row[0]);
inn.new_plus(row[2]);
dolg.new_plus(row[3]);

harac.new_plus(row[4]);

podr.new_plus(row[5]);

sovm=0;
if(row[1][0] == '1')
     sovm=1;

VVOD DANET(1);
fo=0;
DANET.VVOD_SPISOK_add_MD(gettext("Развернутый отчёт ?"));

fo=danet(&DANET,2,stdscr);

clear();
GDITE();


iceb_t_poldan("Код удержания алиментов",&kodalim,"zaralim.alx");

iceb_t_poldan("Коды начислений не входящие в справку о доходах",&kodnvrspr,"zarnast.alx");

printw("%ld %s.\n",tabn,fam.ravno());
printw("%s\n",gettext("Создаем нужные массивы"));
refresh();
/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kon == 0)
 {
  iceb_t_soob(gettext("Не введены начисления !"));
  return(1);
 }

masiv_din_int MKN; //Массив кодов начислений
i=0;
while(cur.read_cursor(&row) != 0)
  MKN.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Uder");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kou == 0)
 {
  iceb_t_soob(gettext("Не введены удержания !"));
  return(1);
 }

masiv_din_int MKU; //Массив кодов удержаний

i=0;
while(cur.read_cursor(&row) != 0)
  MKU.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Tabel");

if((koltb=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(koltb == 0)
 {
  iceb_t_soob(gettext("Не введены виды табеля !"));
  return(1);
 }

masiv_din_int MKVT; //Массив кодов видов табеля

i=0;
while(cur.read_cursor(&row) != 0)
  MKVT.plus(atoi(row[0]),-1);

int kol_mes=period(1,mn,gn,1,mk,gk,1);

class masiv_din_double mes_obud; /*все обязательные удержания по месяцам*/
mes_obud.make_class(kol_mes*obud[0]);

class masiv_din_double mes_nah; /*массив всех начислений по месяцам*/
mes_nah.make_class(kol_mes);

class masiv_din_double MSKN; //Массив содержимого кодов начислений
class masiv_din_double MSKU; //Массив содержимого кодов удержаний
MSKN.make_class(kon);
MSKU.make_class(kou);

class masiv_din_double DNI; //Массив количества рабочих дней
class masiv_din_double KDNI; //Массив количества календарных дней
class masiv_din_double HASI; //Массив количества отработанных часов
DNI.make_class(koltb);
KDNI.make_class(koltb);
HASI.make_class(koltb);

double suma_obud=0.; //Сумма обязательных удержаний за месяц
double suma_obudi=0.; //Сумма обязательных удержаний за период
double suma_vs=0.; /*Сумма военного сбора*/
double suma_vsi=0.; /*Сумма военного сбора итого*/

char imaf_sub[64];

FILE *ff_sub;
sprintf(imaf_sub,"sub%ld.lst",tabn);
if((ff_sub = fopen(imaf_sub,"w")) == NULL)
 {
  error_op_nfil(imaf_sub,errno,"");
  return(1);
 }


sprintf(imaf,"sp1%ld.lst",tabn);
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imaf1,"sp2%ld.lst",tabn);
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return(1);
 }

sprintf(imaf2,"sp3%ld.lst",tabn);
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }
sprintf(imaf_stud,"sp_stud%ld.lst",tabn);
if((ff_stud = fopen(imaf_stud,"w")) == NULL)
 {
  error_op_nfil(imaf_stud,errno,"");
  return(1);
 }
class iceb_fopen fil_vn;
sprintf(imafvn,"spvn%ld.lst",tabn);
if(fil_vn.start(imafvn,"w") != 0)
 return(1);
char imaf_podoh[64];
FILE *ff_podoh;
sprintf(imaf_podoh,"spodoh%d.lst",getpid());
if((ff_podoh = fopen(imaf_podoh,"w")) == NULL)
 {
  error_op_nfil(imaf_podoh,errno,"");
  return(1);
 }

class iceb_tu_str adres("");
/*Читаем реквизиты организации*/
sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&cur) != 1)
 {
  iceb_t_soob(gettext("Не найден контрагент с кодом 00 !"));
 }
else
 {
  adres.new_plus(row[3]);
  rekvizkon(row,ff1);
  rekvizkon(row,ff2);
  rekvizkon(row,ff_stud);
  rekvizkon(row,fil_vn.ff);
  rekvizkon(row,ff_podoh);
  kedrpou.new_plus(row[5]);
 }

zarsprpr_sub_start(gn,kedrpou.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),adres.ravno(),ff_sub);
zarsprpr_zag(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,ff1);
zarsprpr_zag(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,fil_vn.ff);
zarsprpr_zag(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,ff_podoh);
zarsprpr_zag(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,ff2);
zarsprpr_zag(mn,gn,mk,gk,harac.ravno(),fam.ravno(),inn.ravno(),dolg.ravno(),podr.ravno(),sovm,ff_stud);

fprintf(ff1,"\
-------------------------------------------\n");
fprintf(ff1,gettext("\
Месяц  | Начислено |  Единый  |Налог с дох|\n\
       |           |соц. взнос|одов физиче|\n\
       |           |          |ских лиц   |\n"));

fprintf(ff1,"\
-------------------------------------------\n");





fprintf(ff_podoh,"\
-----------------------------------------\n");
fprintf(ff_podoh,gettext("\
Месяц  |   Начислено   |Подоходный налог|\n"));
fprintf(ff_podoh,"\
-----------------------------------------\n");

fprintf(ff2,"\
%*s---------------------------------------------\n\
%*s%s\n\
%*s---------------------------------------------\n",
ots," ",
ots," ",gettext("|  Дата |   Начислено   |Удержано| Получено |"),
ots," ");

fprintf(ff_stud,"\
--------------------------------------\n");
fprintf(ff_stud,gettext("\
|  Дата |   Начислено   | В том числe|\n\
|       |               | индексация |\n"));
/*       123456789012345 123456789010*/
fprintf(ff_stud,"\
--------------------------------------\n");

if(fo == 1)
  fprintf(ff,"%ld %s\n\
%s %d.%d%s %s %d.%d%s\n",tabn,fam.ravno(),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));

printw("\n%ld %s\n",tabn,fam.ravno());

//Создаем класс
class SPISOK NASH;
masiv_din_double NASHD(0);
class SPISOK UDER;
masiv_din_double UDERD(0);
double suma_esv=0.;
double isuma_esv=0.;
iceb_t_poldan("Код пенсионного отчисления",&kodpen,"zarnast.alx");

ipodoh=ipeno=ihas=iskm=ialim=ifbezr=sld=ino=inob=iuo=0.;
idn=ikdn=0;
m=mn;
kol_mes=0;
int nom_kod=0;
for(g=gn; g <=gk ; g++)
for(;  ; m++)
 {
  if(g == gk && m > mk)
   {
    break;
   }
  if(m == 13)
   {
    m=1;
    break;
   }
  
  kol_mes++;
  
  soc_strax=peno=podoh=0.;
  if(fo == 1)
   fprintf(ff,"\
==========================================================\n\
%d.%d%s\n",m,g,gettext("г."));

  /*Ищем табель за месяц*/
  sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
  mes=%d and tabn=%ld",g,m,tabn);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  suma_obud=0.;
  if(kolstr != 0)
  while(cur.read_cursor(&row) != 0)
   {
    kodt=atoi(row[0]);
    dnei=atof(row[1]);
    has=atof(row[2]);
    kdnei=atof(row[3]);
    
    /*Читаем наименование табеля*/
    sprintf(strsql,"select naik from Tabel where kod=%d", kodt);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s !\n",gettext("Не найден код табеля"),row[0]);
      OSTANOV();
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);
    
    if(fo == 1)
     fprintf(ff,"%d %-*s %s%2.f %s%3.2f %s%2.f\n",
     kodt,
     iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),
     gettext("д"),dnei,
     gettext("ч"),has,
     gettext("кд"),kdnei);

    if((i=MKVT.find(kodt)) != -1)
     {
      if((i=MKVT.find(kodt)) == -1)
       {
        beep();
        printw("%s %d %s ТН %ld %s %d.%d%s\n",
        gettext("Не найден код табеля"),kodt,
        gettext("для"),tabn,
        gettext("за"),m,g,
        gettext("г."));
        if(fo == 1)
         fprintf(ff,"%s %d %s ТН %ld %s %d.%d%s\n",
         gettext("Не найден код табеля"),kodt,
         gettext("для"),tabn,
         gettext("за"),m,g,
         gettext("г."));
        refresh();
       }
      else
       {
        DNI.plus((int)dnei,i);
        KDNI.plus((int)kdnei,i);
        HASI.plus(has,i);
       }
     }
   }

  if(fo == 1)
    fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  printw("%d.%d%s\n",m,g,gettext("г."));
  refresh();

  sal=zarsald(1,m,g,tabn,&saldb);

  sprintf(strsql,"select prn,knah,suma,datz,mesn,godn,kom,shet from \
Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%ld \
and suma <> 0. order by prn,knah asc",
  g,m,g,m,tabn);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }


  if(m == 1)
   {
    sld=sal;
   }

  fbezr=alim=skm=inah=iuder=inahb=0.;
  suma_vs=0.;
  suma_esv=0.;
  suma_indeks=0.;
  if(kolstr != 0)
  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    knah=atoi(row[1]);
    sym=atof(row[2]);
    rsdat(&dz,&mz,&gz,row[3],2);
    if(prn == 1)
     {
      mes_nah.plus(sym,kol_mes-1);
      if(kodindex == knah)
       {
        suma_indeks+=sym;
        isuma_indeks+=sym;
       }
      sprintf(strsql,"%d,%s",knah,row[7]);
      if((i1=NASH.find(strsql)) == -1)
       {
        NASH.plus(strsql);
        NASHD.plus(sym,-1);
       }      
      else
       {
        NASHD.plus(sym,i1);
       }
      if((i=MKN.find(knah)) != -1)
        MSKN.plus(sym,i);

      inah+=sym;
      if(proverka(kodnvrspr.ravno(),row[1],0,1) != 0)
        inahb+=sym;

      if(provkod(kodmp,knah) >= 0)
       {
	skm+=sym;
	iskm+=sym;
       }

     }

    if(prn == 2)
     {
      sprintf(strsql,"%d,%s",knah,row[7]);
      if((i1=UDER.find(strsql)) < 0)
       {
        UDER.plus(strsql);
        UDERD.plus(sym,-1);
       }      
      else
       {
        UDERD.plus(sym,i1);
       }
      if((i=MKU.find(knah)) != -1)
        MSKU.plus(sym,i);

      iuder+=sym;
      if(proverka(kodpn_all.ravno(),knah,0,1) == 0)
	podoh+=sym;
      if(knah == kodpen)
	peno+=sym;

      if(knah == kodsocstr)
       soc_strax+=sym;
       
      if(knah == kodalim)
       {
	alim+=sym;
	ialim+=sym;
       }

      if(knah == kodbzr)
       {
	fbezr+=sym;
	ifbezr+=sym;
       }

      if((nom_kod=provkod(obud,knah)) >= 0)
       {
        suma_obud+=sym;
        suma_obudi+=sym;
        mes_obud.plus(sym,((kol_mes-1)*obud[0])+nom_kod);
       }

      if(proverka(kods_esv_all.ravno(),knah,0,1) == 0)
       {
        suma_esv+=sym;
        isuma_esv+=sym;
       }
      if(kodvn == knah)
       {
        suma_vs+=sym;
        suma_vsi+=sym;
        
       }
     }

    if(fo == 1)
     {
      /*Читаем наименование начисления*/
      if(prn == 1)
        sprintf(strsql,"select naik from Nash where kod=%d",knah);
      if(prn == 2)
        sprintf(strsql,"select naik from Uder where kod=%d",knah);
         
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        beep();
        if(prn == 1)
         printw("%s %s !\n",
         gettext("Не найден код начисления"),row[3]);
        if(prn == 2)
         printw("%s %s !\n",
         gettext("Не найден код удержания"),row[3]);
        OSTANOV();
        nai.new_plus("");
       }
      else
        nai.new_plus(row1[0]);

      fprintf(ff,"%3d/%02d/%2s.%s %-*s : %10.2f %-*s %s\n",
      knah,dz,row[4],row[5],
      iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),
      sym,
      iceb_tu_kolbait(6,row[7]),row[7],row[6]);

     }
/*
    printw("%-40s: %15.2f\n",zr4.nan,zr4.sym);
    refresh();
  */

   }


  if(strstrm(harac.ravno(),gettext("Студент")) == 0)
   {
    /*для студента в справку выводтяся только месяцы по которым были расчёты*/
    sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d and  tabn=%ld",g,m,tabn);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      fprintf(ff1,"%02d.%d %11.2f %10.2f %11.2f\n",m,g,inahb,suma_esv*(-1), podoh*(-1));


      fprintf(ff_podoh,"%02d.%d %15.2f %15.2f\n",m,g,inahb,podoh*(-1));
      fprintf(ff2,"%*s|%02d.%d|%15.2f|%8.2f|%10.2f|\n",ots," ",m,g,inahb,suma_obud,inahb+suma_obud);
      fprintf(ff_stud,"|%02d.%d|%15.2f|%12.2f|\n",m,g,inahb,suma_indeks);
     }
   }
  else
   {
    fprintf(ff1,"%02d.%d %11.2f %10.2f %11.2f\n",m,g,inahb,suma_esv*(-1),podoh*(-1));


    fprintf(ff_podoh,"%02d.%d %15.2f %15.2f\n",m,g,inahb,podoh*(-1));
    fprintf(ff2,"%*s|%02d.%d|%15.2f|%8.2f|%10.2f|\n",ots," ",m,g,inahb,suma_obud,inahb+suma_obud);
    fprintf(ff_stud,"|%02d.%d|%15.2f|%12.2f|\n",m,g,inahb,suma_indeks);
   }
  mesc(m,0,&naim_mes);

  fprintf(ff_sub,"|%-*.*s|%10.2f|%17.2f|%10.2f|%17.2f|%10.2f|\n",
  iceb_tu_kolbait(8,naim_mes.ravno()),iceb_tu_kolbait(8,naim_mes.ravno()),naim_mes.ravno(),
  inahb,
  podoh,
  0.,
  0.,
  alim*(-1));
/*****************
  fprintf(ff_sub,"\
-------------------------------------------------------------------------------\n");  
***************/
  if(fo == 1)
    fprintf(ff,"\
     %s %15.2f\n\
     %s %15.2f\n\
     %s %15.2f\n\
     %s %15.2f\n",
     gettext("Сальдо         :"),sal,
     gettext("Итого начислено:"),inah,
     gettext("Итого удержано :"),iuder,
     gettext("К выдаче       :"),inah+iuder+sal);
  ino+=inah;
  inob+=inahb;
  iuo+=iuder;
  ipodoh+=podoh;
  ipeno+=peno;
  isoc_strax+=soc_strax;
 }

printw("\
-------------------------------------------------\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n",
gettext("Сальдо         :"),sld,
gettext("Итого начислено:"),ino,
gettext("Итого удержано :"),iuo,
gettext("К выдаче       :"),ino+iuo+sld);
refresh();

fprintf(ff_sub,"|%*.*s|%10.2f|%17.2f|%10.2f|%17.2f|%10.2f|\n",
iceb_tu_kolbait(8,gettext("Итого")),
iceb_tu_kolbait(8,gettext("Итого")),
gettext("Итого"),
ino,
ipodoh,
0.,
0.,
ialim*(-1));

fprintf(ff_sub,"\
-------------------------------------------------------------------------------\n");  


preobr(inob,&str,0);

fprintf(ff1,"\
-------------------------------------------\n");

fprintf(ff1,"%*s %11.2f %10.2f %11.2f\n\n",
iceb_tu_kolbait(7,gettext("Итого")),gettext("Итого"),
inob,isuma_esv*(-1),ipodoh*(-1));

fprintf(ff1,"%s:\n%-.*s\n",gettext("Общая сумма доходов"),iceb_tu_kolbait(60,str.ravno()),str.ravno());

zarsprpr_ssu(mn,gn,mk,gk,&mes_nah,&mes_obud,fil_vn.ff); /*распечатка справки со всеми налогами*/

fprintf(ff_podoh,"\
-----------------------------------------\n");
fprintf(ff_podoh,"%*s %15.2f %15.2f\n\n\
%s:\n%-.*s\n",
iceb_tu_kolbait(7,gettext("Итого")),gettext("Итого"),inob,ipodoh*(-1),
gettext("Общая сумма доходов"),
iceb_tu_kolbait(60,str.ravno()),str.ravno());

fprintf(fil_vn.ff,"\n%s:\n%-.*s\n",gettext("Общая сумма доходов"),iceb_tu_kolbait(60,str.ravno()),str.ravno());

if(iceb_tu_strlen(str.ravno()) > 60)
 {
  fprintf(ff1,"%s\n",iceb_tu_adrsimv(60,str.ravno()));
  fprintf(fil_vn.ff,"%s\n",iceb_tu_adrsimv(60,str.ravno()));
  fprintf(ff_podoh,"%s\n",iceb_tu_adrsimv(60,str.ravno()));
 }

fprintf(ff2,"\
%*s---------------------------------------------\n\
%*s %*s %15.2f %8.2f %10.2f\n\n\
%s:\n%s\n",
ots," ",
ots," ",
iceb_tu_kolbait(7,gettext("Итого")),gettext("Итого"),
inob,suma_obudi,inob+suma_obudi,
gettext("Общая сумма доходов"),str.ravno());


fprintf(ff_stud,"\
--------------------------------------\n");
fprintf(ff_stud,"%*s %15.2f %12.2f\n\n\
%s:\n%s\n",
iceb_tu_kolbait(8,gettext("Итого")),gettext("Итого"),
inob,isuma_indeks,
gettext("Общая сумма доходов"),str.ravno());


iceb_t_poldan("Табельный номер руководителя",&bros,"zarnast.alx");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   bros.new_plus(row[0]);
  else
   bros.new_plus("");
 }

iceb_t_poldan("Табельный номер бухгалтера",&fambuh,"zarnast.alx");
if(fambuh.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",fambuh.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   fambuh.new_plus(row[0]);
  else
   fambuh.new_plus("");
 }

time(&tmr);
bf=localtime(&tmr);

fprintf(ff1,"\n%s\n",gettext("Подоходный налог и отчисления в социальный фонды\nудержаны и перечислены полностью"));

fprintf(ff1,"\n\n\
    %s %s\n\n\
    %s %s\n\n",
gettext("Руководитель................"),bros.ravno(),
gettext("Главный бухгалтер..........."),fambuh.ravno());

fprintf(ff1,"\
    %02d.%02d.%d%s\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

fprintf(fil_vn.ff,"\n\n\%s\n\n\
    %s %s\n\n\
    %s %s\n\n\
    %02d.%02d.%d%s\n",
gettext("Налог на доходы с физических особ, военный сбор и единый социальный\n\
взнос удержаны та перечислены полностью.\n\
В течение года плательщику налога предоставлялась ставка 18%.\n"),
gettext("Руководитель................"),bros.ravno(),
gettext("Главный бухгалтер..........."),fambuh.ravno(),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));







fprintf(ff_podoh,"\n\n\
    %s %s\n\n\
    %s %s\n\n\
    %02d.%02d.%d%s\n",
gettext("Руководитель................"),bros.ravno(),
gettext("Главный бухгалтер..........."),fambuh.ravno(),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

fprintf(ff2,"\n\n\
    %s %s\n\n\
    %s %s\n\n\
    %02d.%02d.%d%s\n",
gettext("Руководитель................"),bros.ravno(),
gettext("Главный бухгалтер..........."),fambuh.ravno(),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

fprintf(ff_stud,"\n\n\
    %s %s\n\n\
    %s %s\n\n\
    %02d.%02d.%d%s\n",
gettext("Руководитель................"),bros.ravno(),
gettext("Главный бухгалтер..........."),fambuh.ravno(),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

if(fo == 1)
  fprintf(ff,"\
-------------------------------------------------\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n",
gettext("Сальдо         :"),sld,
gettext("Итого начислено:"),ino,
gettext("Итого удержано :"),iuo,
gettext("К выдаче       :"),ino+iuo+sld);

fprintf(ff,"%ld %s\n\
%s %d.%d%s %s %d.%d%s\n",tabn,fam.ravno(),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));

printw("\n%s:\n",gettext("Начисления"));
fprintf(ff,"\n%s:\n",gettext("Начисления"));
bb=0.;
for(i=0 ; i<kon;i++)
 {
  knah=MKN.ravno(i);
  sym=MSKN.ravno(i);
  if(sym == 0.)
    continue;
  sprintf(strsql,"select naik from Nash where kod=%d",knah);
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("%s %d !\n",
    gettext("Не найден код начисления"),knah);
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);

  printw("%3d %-*s %15.2f\n",knah,iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),sym);
  fprintf(ff,"%3d %-*s %15.2f\n",knah,iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),sym);

  bb+=sym;
 }

if(fo == 1)
 {
  printw("----------------------------------------------------\n\
%3s %*s %15.2f\n%s:\n"," ",iceb_tu_kolbait(40,gettext("Итого")),gettext("Итого"),
  bb,gettext("Удержания"));
  refresh();
  fprintf(ff,"----------------------------------------------------\n\
%3s %*s %15.2f\n%s:\n"," ",iceb_tu_kolbait(40,gettext("Итого")),gettext("Итого"),
  bb,gettext("Удержания"));
  bb=0.;
  for(i=0 ; i<kou;i++)
   {
    sym=MSKU.ravno(i);
    if(sym == 0.)
      continue;
    knah=MKU.ravno(i);

    sprintf(strsql,"select naik from Uder where kod=%d",knah);
       
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      printw("%s %d !\n",
      gettext("Не найден код удержания"),knah);
      OSTANOV();
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);

    printw("%3d %-*s %15.2f\n",knah,iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),sym);
    fprintf(ff,"%3d %-*s %15.2f\n",knah,iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),sym);
    bb+=sym;
   }

  printw("----------------------------------------------------\n\
%3s %*s %15.2f\n"," ",iceb_tu_kolbait(40,gettext("Итого")),gettext("Итого"),bb);
 }

/*Распечатываем суммы по табелям*/

printw("%s: %d\n",gettext("Количество видов табеля"),koltb);

fprintf(ff,"----------------------------------------------------\n\
%3s %*s %15.2f\n%s\n"," ",iceb_tu_kolbait(40,gettext("Итого")),gettext("Итого"),bb,
gettext("Табель:"));

fprintf(ff,"\
-----------------------------------------------------------------\n");
fprintf(ff,gettext("Код|           Наименование табеля          |Дни | Часы   |К.дни|\n"));
fprintf(ff,"\
-----------------------------------------------------------------\n");

for(i=0 ; i<koltb;i++)
 {
  dnei=DNI.ravno(i);
  kdnei=KDNI.ravno(i);
  has=HASI.ravno(i);
  if(dnei == 0 && kdnei == 0. && has == 0)
   continue;  

  knah=MKVT.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",
  knah);
  if(readkey(strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %d !\n",gettext("Не найден код табеля"),knah);
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);

  fprintf(ff,"%3d %-*s %4.f %8.8g %4.f\n",knah,iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),dnei,has,kdnei);

  idn+=(int)dnei;
  ikdn+=(int)kdnei;
  ihas+=has;
 }
fprintf(ff,"\
-----------------------------------------------------------------\n\
%3s %*s %4d %8.8g %4d\n"," ",iceb_tu_kolbait(40,gettext("Итого")),gettext("Итого"),idn,ihas,ikdn);




m=mn;
for(g=gn; g <=gk ; g++)
for(;  ; m++)
 {
  if(g == gk && m > mk)
   {
    break;
   }
  if(m == 13)
   {
    m=1;
    break;
   }
  fprintf(ff,"----------------- %d.%d%s -----------------\n",
  m,g,gettext("г."));
  vztb(tabn,m,g,&kon,&ihas,&kou,1,ff);
 }



zarsprpr_sub_end(mn,gn,mk,gk,ino+ialim,ff_sub);

podpis(ff);
podpis(ff1);
podpis(ff2);
podpis(fil_vn.ff);
podpis(ff_podoh);
podpis(ff_sub);
podpis(ff_stud);


fclose(ff);
fclose(ff_stud);
fclose(ff_sub);
fclose(ff1);
fclose(ff2);
fil_vn.end();
fclose(ff_podoh);

//Распечатка начислений/удержаний по счетам

sprintf(imafshet,"sp5%ld.lst",tabn);
if((ff = fopen(imafshet,"w")) == NULL)
   {
    error_op_nfil(imafshet,errno,"");
    return(1);
   }

fprintf(ff,"%ld %s\n\
%s %d.%d%s %s %d.%d%s\n",tabn,fam.ravno(),
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));

fprintf(ff,"\n%s:\n",gettext("Начисления"));
i=1;
bb=0.;
for(int nom=0; nom < NASH.kolih(); nom++)
 {
  bros.new_plus(NASH.ravno(nom));
  if(bros.ravno()[0] == '\0')
   break;
  polen(bros.ravno(),&str,1,',');
  polen(bros.ravno(),&shet,2,',');
  sprintf(strsql,"select naik from Nash where kod=%s",str.ravno());
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("%s %s !\n",
    gettext("Не найден код начисления"),str.ravno());
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);
  fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),iceb_tu_kolbait(5,shet.ravno()),shet.ravno(),
  NASHD.ravno(nom));
  bb+=NASHD.ravno(nom);
 }

fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n%s:\n"," ",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),
bb,gettext("Удержания"));
i=1;
bb=0.;

//while(UDER.pol_masiv_char(i++,bros.ravno()) == 0)
for(int nom=0; nom < UDER.kolih(); nom++)
 {
  bros.new_plus(UDER.ravno(nom));
  if(bros.ravno()[0] == '\0')
   break;
  polen(bros.ravno(),&str,1,',');
  polen(bros.ravno(),&shet,2,',');
  sprintf(strsql,"select naik from Uder where kod=%s",str.ravno());
     
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("%s %s !\n",gettext("Не найден код удержания"),str.ravno());
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);
  fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),iceb_tu_kolbait(5,shet.ravno()),shet.ravno(),
  UDERD.ravno(nom));
  //UDERD.ravno(i-2));
//  bb+=UDERD.ravno(i-2);
  bb+=UDERD.ravno(nom);
 }
fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

//Распечатка бюджет/не бюджет

if(shetb != NULL)
 {
  fprintf(ff,"\n%s:\n",gettext("Начисления"));
  fprintf(ff,"%s.\n",gettext("Бюджет"));
  i=1;
  bb=0.;
//  while(NASH.pol_masiv_char(i++,bros.ravno()) == 0)
  for(int nom=0; nom < NASH.kolih(); nom++)
   {
    bros.new_plus(NASH.ravno(nom));
    if(bros.ravno()[0] == '\0')
     break;
    polen(bros.ravno(),&str,1,',');
    polen(bros.ravno(),&shet,2,',');

    if(pole1(shetb,shet.ravno(),',',0,&i1) != 0)
      if(SRAV(shetb,shet.ravno(),0) != 0)
       continue;


    sprintf(strsql,"select naik from Nash where kod=%s",str.ravno());
       
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      printw("%s %s !\n",gettext("Не найден код начисления"),str.ravno());
      OSTANOV();
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);
    fprintf(ff,"%2s %-*s %*s %8.2f\n",
    str.ravno(),
    iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),
    iceb_tu_kolbait(5,shet.ravno()),shet.ravno(),
//    NASHD.ravno(i-2));
    NASHD.ravno(nom));
//    bb+=NASHD.ravno(i-2);
    bb+=NASHD.ravno(nom);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n\n%s.\n"," ",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),
  bb,
  gettext("Не бюджет"));


  i=1;
  bb=0.;
//  while(NASH.pol_masiv_char(i++,bros.ravno()) == 0)
  for(int nom=0; nom < NASH.kolih(); nom++)
   {
    bros.new_plus(NASH.ravno(nom));
    if(bros.ravno()[0] == '\0')
     break;
    polen(bros.ravno(),&str,1,',');
    polen(bros.ravno(),&shet,2,',');

    if(pole1(shetb,shet.ravno(),',',0,&i1) == 0 || SRAV(shetb,shet.ravno(),0) == 0)
       continue;


    sprintf(strsql,"select naik from Nash where kod=%s",str.ravno());
       
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      printw("%s %s !\n",gettext("Не найден код начисления"),str.ravno());
      OSTANOV();
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);
    fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),iceb_tu_kolbait(5,shet.ravno()),shet.ravno(),
    NASHD.ravno(nom));
//    NASHD.ravno(i-2));
//    bb+=NASHD.ravno(i-2);
    bb+=NASHD.ravno(nom);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

 }

if(shetbu != NULL)
 {
  fprintf(ff,"\n%s:\n",gettext("Удержания"));
  fprintf(ff,"%s.\n",gettext("Бюджет"));
  i=1;
  bb=0.;
//  while(UDER.pol_masiv_char(i++,bros.ravno()) == 0)
  for(int nom=0; nom < UDER.kolih(); nom++)
   {
    bros.new_plus(UDER.ravno(nom));
    if(bros.ravno()[0] == '\0')
     break;
    polen(bros.ravno(),&str,1,',');
    polen(bros.ravno(),&shet,2,',');

    if(pole1(shetbu,shet.ravno(),',',0,&i1) != 0)
      if(SRAV(shetbu,shet.ravno(),0) != 0)
       continue;


    sprintf(strsql,"select naik from Uder where kod=%s",str.ravno());
       
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      printw("%s %s !\n",gettext("Не найден код удержания"),str.ravno());
      OSTANOV();
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);
    fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),iceb_tu_kolbait(5,shet.ravno()),shet.ravno(),
    UDERD.ravno(nom));
//    UDERD.ravno(i-2));
//    bb+=UDERD.ravno(i-2);
    bb+=UDERD.ravno(nom);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n\n%s.\n"," ",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),
  bb,
  gettext("Не бюджет"));


  i=1;
  bb=0.;
//  while(UDER.pol_masiv_char(i++,bros.ravno()) == 0)
  for(int nom=0; nom < UDER.kolih(); nom++)
   {
    bros.new_plus(UDER.ravno(nom));
    if(bros.ravno()[0] == '\0')
     break;
    polen(bros.ravno(),&str,1,',');
    polen(bros.ravno(),&shet,2,',');

    if(pole1(shetbu,shet.ravno(),',',0,&i1) == 0 || SRAV(shetbu,shet.ravno(),0) == 0)
       continue;


    sprintf(strsql,"select naik from Uder where kod=%s",str.ravno());
       
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      printw("%s %s !\n",
      gettext("Не найден код удержания"),str.ravno());
      OSTANOV();
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);
    fprintf(ff,"%2s %-*s %*s %8.2f\n",str.ravno(),iceb_tu_kolbait(40,nai.ravno()),nai.ravno(),iceb_tu_kolbait(5,shet.ravno()),shet.ravno(),
    UDERD.ravno(nom));
//    UDERD.ravno(i-2));
//    bb+=UDERD.ravno(i-2);
    bb+=UDERD.ravno(nom);
   }


  fprintf(ff,"----------------------------------------------------\n\
%2s %*s: %8.2f\n"," ",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),bb);

 }

podpis(ff);

fclose(ff);

OSTANOV();


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Развёрнутая справка"));

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Справка с отчислениями в соц. фонды"));
oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Справка с общей суммой удержаний"));
oth->spis_imaf.plus(imafvn);
oth->spis_naim.plus(gettext("Справка со всеми удержаниями"));
oth->spis_imaf.plus(imafshet);
oth->spis_naim.plus(gettext("Справка начиcлений/удержаний по счетам"));
oth->spis_imaf.plus(imaf_podoh);
oth->spis_naim.plus(gettext("Справка с подоходным налогом"));
oth->spis_imaf.plus(imaf_sub);
oth->spis_naim.plus(gettext("Справка для оформления социальной помощи (жилищной субсидии)"));
oth->spis_imaf.plus(imaf_stud);
oth->spis_naim.plus(gettext("Справка для студентов"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),3);

return(0);

}
/***********************/
/*Реквизиты организации*/
/***********************/
void  rekvizkon(SQL_str row,FILE *ff)
{
int otstup=40;
int hag=41;
fprintf(ff,"%*s %s\n",otstup,"",row[1]);
int hag1=hag-strlen(gettext("Адрес"))-1;

//fprintf(ff,"%*s %s %-*.*s\n",otstup,"",gettext("Адрес"),hag1,hag1,row[3]);
fprintf(ff,"%*s %s %-*.*s\n",
otstup,"",
gettext("Адрес"),
iceb_tu_kolbait(hag1,row[3]),
iceb_tu_kolbait(hag1,row[3]),
row[3]);

for(int ii=hag1; ii < iceb_tu_strlen(row[3]); ii+=hag)
 fprintf(ff,"%*s %-*.*s\n",
 otstup,"",
 iceb_tu_kolbait(hag,iceb_tu_adrsimv(ii,row[3])),
 iceb_tu_kolbait(hag,iceb_tu_adrsimv(ii,row[3])),
 iceb_tu_adrsimv(ii,row[3]));
// fprintf(ff,"%*s %-*.*s\n",otstup,"",hag,hag,&row[3][ii]);

fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Код ЕГРПОУ"),row[5]);
fprintf(ff,"%*s %s %s %s %s\n",otstup,"",gettext("Р/С"),row[7],gettext("МФО"),row[6]);
//fprintf(ff,"%*s %s %s\n",otstup,"",gettext("МФО"),row[6]);

char bros[512];

sprintf(bros,"%s %s %s",gettext("в"),row[2],row[4]);
for(int ii=0; ii < iceb_tu_strlen(bros); ii+=hag)
  fprintf(ff,"%*s %-*.*s\n",
  otstup," ",
  iceb_tu_kolbait(hag,iceb_tu_adrsimv(ii,bros)),
  iceb_tu_kolbait(hag,iceb_tu_adrsimv(ii,bros)),
  iceb_tu_adrsimv(ii,bros));
//  fprintf(ff,"%*s %-*.*s\n",otstup," ",hag,hag,&bros[ii]);
  
//fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Налоговый номер"),row[8]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Номер свидетельства плательщика НДС"),row[9]);
fprintf(ff,"%*s %s %s\n",otstup,"",gettext("Телефон"),row[10]);
}
/*******************************************/
/*Заголовок справки для получения субсидии*/
/********************************************/
int zarsprpr_sub_start(short god,const char *kedrpou,const char *fio,const char *inn,const char *dolg,
const char *adres,
FILE *ff_ras)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str stroka("");
char stroka1[1024];

sprintf(strsql,"select str from Alx where fil='zarsns_start.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsns_start.alx");
  iceb_t_soob(strsql);
  return(1);
 }

class iceb_tu_str naim_kontr(iceb_t_get_pnk("00",0));

int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 10:
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_tu_kolbait(52-4,naim_kontr.ravno()),naim_kontr.ravno()); /*включение/выключение режима выделенной печати*/
     iceb_tu_vstav(&stroka,stroka1,20,79,1);
     break;

    case 12: /*адрес*/
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_tu_kolbait(55-4,adres),adres); /*включение/выключение режима выделенной печати*/
     iceb_tu_vstav(&stroka,stroka1,17,79,1);
     break;

    case 13:
     sprintf(stroka1,"\x1B\x45%s\x1B\x46",kedrpou); /*включение/выключение режима выделенной печати*/
     iceb_tu_vstav(&stroka,stroka1,31,79,1);
     break;

    case 17:
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_tu_kolbait(61-4,fio),fio); /*включение/выключение режима выделенной печати*/
     iceb_tu_vstav(&stroka,stroka1,11,79,1);
     break;

    case 18: /*наименование*/
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_tu_kolbait(79-51,naim_kontr.ravno()),naim_kontr.ravno()); /*включение/выключение режима выделенной печати*/
     iceb_tu_vstav(&stroka,stroka1,51,79,1);
     break;

    case 19: 
     if(iceb_tu_strlen(naim_kontr.ravno()) >= 79-51)
      {
       sprintf(stroka1,"\x1B\x45%s\x1B\x46",iceb_tu_adrsimv(79-51-1,naim_kontr.ravno())); /*включение/выключение режима выделенной печати*/
       iceb_tu_vstav(&stroka,stroka1,0,79,1);
      }
     break;

    case 21:
     sprintf(stroka1,"\x1B\x45%.*s\x1B\x46",iceb_tu_kolbait(79-14-4,dolg),dolg); /*включение/выключение режима выделенной печати*/
     iceb_tu_vstav(&stroka,stroka1,14,79,1);
     break;

    case 23:
     sprintf(stroka1,"\x1B\x45%s\x1B\x46",inn); /*включение/выключение режима выделенной печати*/
     iceb_tu_vstav(&stroka,stroka1,23,71,1);
     break;

    case 27:
     iceb_tu_vstav(&stroka,god,1,4,1);
     break;
   }
  fprintf(ff_ras,"%s",stroka.ravno());
 }

return(0);
}
/*************************/
/*концовка справки на субсидию*/
/******************************/
int zarsprpr_sub_end(short mn,short gn,short mk,short gk,double suma,FILE *ff_ras)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
class iceb_t_fioruk_rk rukov;
class iceb_t_fioruk_rk glavbuh;

char strsql[512];
class iceb_tu_str stroka("");
short dk=0;

dpm(&dk,&mk,&gk,5);

sprintf(strsql,"select str from Alx where fil='zarsns_end.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsns_end.alx");
  iceb_t_soob(strsql);
  return(1);
 }

iceb_t_fioruk(1,&rukov);
iceb_t_fioruk(2,&glavbuh);

int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 1:
     sprintf(strsql,"01.%02d.%04d %s %s %02d.%02d.%04d %s",
     mn,gn,
     gettext("г."),
     gettext("по"),
     dk,mk,gk,
     gettext("г."));
     
     iceb_tu_vstav(&stroka,strsql,33,62,1);
     break;

    case 2:
     memset(strsql,'\0',sizeof(strsql));
     preobr(suma,strsql,0);
     iceb_tu_vstav(&stroka,strsql,25,79,1);
     
     break;

    case 7:
     iceb_tu_vstav(&stroka,rukov.dolg.ravno(),0,33,1);
     sprintf(strsql,"%s %s",rukov.famil.ravno(),rukov.inic);
     iceb_tu_vstav(&stroka,strsql,54,79,1);
     break;

    case 15:
     sprintf(strsql,"%s %s",glavbuh.famil.ravno(),glavbuh.inic);
     iceb_tu_vstav(&stroka,strsql,54,79,1);
     break;
   }
  fprintf(ff_ras,"%s",stroka.ravno());
 }
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
fprintf(ff_ras,"%02d.%02d.%d %s N______\n",dt,mt,gt,gettext("г."));

return(0);

}
/****************************************/
/*Заголовок*/
/*******************************/
void zarsprpr_zag(short mn,short gn,
short mk,short gk,
const char *harac,
const char *fam,
const char *inn,
const char *dolg,
const char *podr,
int sovm,
FILE *ff2)
{
char strsql[512];

if(strstrm(harac,gettext("Студент")) == 0)
 {
  fprintf(ff2,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n\
%s %s %s",
gettext("С п р а в к а"),
gettext("о заработной плате за период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."),
gettext("Работник"),
fam,
gettext("работает"));

  if(sovm == 0)
   {
    if(harac[0] == '\0')
      fprintf(ff2," %s\n",gettext("постоянно."));
    else
      fprintf(ff2," %s.\n",harac);
   }
  else
   fprintf(ff2," %s\n",gettext("по совместительству."));

  fprintf(ff2,"%s %s\n",gettext("Идентификационный код"),inn);
  fprintf(ff2,"%s: %s\n",gettext("Должность"),dolg);
 }
else
 {
  SQL_str row;
  class SQLCURSOR cur;
  class iceb_tu_str naipodr("");
  
  sprintf(strsql,"select naik from Podr where kod=%d",atoi(podr));
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naipodr.new_plus(row[0]);
  else
    naipodr.new_plus("");

/************************
  fprintf(ff2,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n",
gettext("С п р а в к а"),
gettext("о начислении стипендии за период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));
  fprintf(ff2,"%s. %s %s\n",
  gettext("Учится на дневном отделении"),
  gettext("Группа"),naipodr.ravno());
  
  fprintf(ff2,"%s %s\n",gettext("Студент"),fam);
***********************/

  fprintf(ff2,"\n\
	      %s\n\
%s %s\n\
%s %d.%d %s %s %d.%d %s\n",
gettext("С п р а в к а"),
gettext("Выдана студенту"),
fam,
gettext("про начисление стипендии за период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."));

  fprintf(ff2,"%s. %s %s\n",
  gettext("Учится на дневном отделении"),
  gettext("Группа"),naipodr.ravno());
  

  fprintf(ff2,"%s %s\n",gettext("Идентификационный код"),inn);

 }
}

/************************************/
/*распечатка справки со всеми удержаниями*/
/*******************************************/
void zarsprpr_ssu(short mn,short gn,
short mk,short gk,
class masiv_din_double *mes_nash,
class masiv_din_double *mes_obud,
FILE *ff)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str naim_uder("");
const int razmer_kol=11;
class iceb_tu_str liniq("--------------------");
int kol_mes=period(1,mn,gn,1,mk,gk,1);

for(int nom=0; nom < obud[0]; nom++)
 {
  if(mes_obud->prov_kol(kol_mes,obud[0],nom) == 0)
   continue;
  liniq.plus("------------");
 }

liniq.plus("------------");/*получено*/

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%-*.*s|%-*.*s|",
iceb_tu_kolbait(7,gettext("Дата")),
iceb_tu_kolbait(7,gettext("Дата")),
gettext("Дата"),
iceb_tu_kolbait(razmer_kol,gettext("Начислено")),
iceb_tu_kolbait(razmer_kol,gettext("Начислено")),
gettext("Начислено"));
float max_dlinna_naim=0;
/*первая строка шапки*/
for(int nom=0; nom < obud[0]; nom++)
 {
  if(mes_obud->prov_kol(kol_mes,obud[0],nom) == 0)
   continue;
  sprintf(strsql,"select naik from Uder where kod=%d",obud[nom+1]);
  if(readkey(strsql,&row,&cur) == 1)
   naim_uder.new_plus(row[0]);
  else
   naim_uder.new_plus("");
     
  fprintf(ff,"%-*.*s|",
  iceb_tu_kolbait(razmer_kol,naim_uder.ravno()),
  iceb_tu_kolbait(razmer_kol,naim_uder.ravno()),
  naim_uder.ravno());

  if(iceb_tu_strlen(naim_uder.ravno()) > max_dlinna_naim)
   max_dlinna_naim=iceb_tu_strlen(naim_uder.ravno());
     
 }
fprintf(ff,"%-*.*s|\n",
iceb_tu_kolbait(razmer_kol,gettext("Получено")),
iceb_tu_kolbait(razmer_kol,gettext("Получено")),
gettext("Получено"));

float kol_strok=max_dlinna_naim/razmer_kol;


/*последующие строки шапки*/
int adres_str=razmer_kol;
for(float ks=1.; ks < kol_strok;ks+=1.)
 {
  fprintf(ff,"       |           |");
  for(int nom=0; nom < obud[0]; nom++)
   {
    if(mes_obud->prov_kol(kol_mes,obud[0],nom) == 0)
     continue;
    sprintf(strsql,"select naik from Uder where kod=%d",obud[nom+1]);
    if(readkey(strsql,&row,&cur) == 1)
     naim_uder.new_plus(row[0]);
    else
     naim_uder.new_plus("");
       
    fprintf(ff,"%-*.*s|",
    iceb_tu_kolbait(razmer_kol,iceb_tu_adrsimv(adres_str,naim_uder.ravno())),
    iceb_tu_kolbait(razmer_kol,iceb_tu_adrsimv(adres_str,naim_uder.ravno())),
    iceb_tu_adrsimv(adres_str,naim_uder.ravno()));

   }
  fprintf(ff,"           |\n");
  adres_str+=razmer_kol;  
 }

fprintf(ff,"%s\n",liniq.ravno());

short d=1,m=mn,g=gn;

double itogo_str=0.;
for(int nom=0; nom < kol_mes; nom++)
 {
  itogo_str=0.;
  fprintf(ff,"%02d.%04d|%*.2f|",m,g,razmer_kol,mes_nash->ravno(nom));
  itogo_str+=mes_nash->ravno(nom);
  for(int nom1=0; nom1 < obud[0];nom1++)
   {
    if(mes_obud->prov_kol(kol_mes,obud[0],nom1) == 0)
     continue;
    fprintf(ff,"%*.2f|",razmer_kol,mes_obud->ravno(nom*obud[0]+nom1));
    itogo_str+=mes_obud->ravno(nom*obud[0]+nom1);
   }     
  
  fprintf(ff,"%*.2f|\n",razmer_kol,itogo_str);
  dpm(&d,&m,&g,3);
 }
fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%*.*s|%*.2f|",
iceb_tu_kolbait(7,gettext("Итого")),
iceb_tu_kolbait(7,gettext("Итого")),
gettext("Итого"),
razmer_kol,
mes_nash->suma());

/*распечатываем итоговую строку*/
for(int nom=0; nom < obud[0]; nom++)
 {
  if(mes_obud->prov_kol(kol_mes,obud[0],nom) == 0)
     continue;
  fprintf(ff,"%*.2f|",razmer_kol,mes_obud->suma_kol(kol_mes,obud[0],nom));
 }
fprintf(ff,"%*.2f|\n",razmer_kol,mes_nash->suma()+mes_obud->suma());
}

