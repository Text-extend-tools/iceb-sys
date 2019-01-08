/* $Id: dirzarsl.c,v 5.37 2014/02/28 05:13:47 sasa Exp $ */
/*14.07.2015    19.06.1995      Белых А.И.      dirzarsl.c
Программа ввода и корректировки сальдо по табельному номеру
*/
#include        <errno.h>
#include        "buhg.h"

class dirzarsl_p
 {
  public:
   class iceb_tu_str tabnom;
   class iceb_tu_str god;
   class iceb_tu_str fio;
   int metka_poi; /*0-поиск не выполнять 1-выполнять*/

   dirzarsl_p()
    {
     clear();
     metka_poi=0;
    }

   void clear()
    {
     tabnom.new_plus("");
     god.new_plus("");
     fio.new_plus("");
    }
 };
 
void vzar8(VVOD*,short,short);
void sozvzar8(VVOD*);
void shzr8(short,double,double,class dirzarsl_p *poi);
int dirzarsl_prov(SQL_str row,class dirzarsl_p *poi);
int dirzarsl_poi(class dirzarsl_p *poi);

extern short    mmm,ggg;

void            dirzarsl()
{
class dirzarsl_p poi;
class iceb_t_mstr spis_menu(0);
struct  tm      *bf;
time_t          tmm;
FILE            *ff;
int             kom,K;
short           i;
long            tbn=0;
short           gg=0;
double          sumsp,sumso,sumspb,sumsob;
char            imaf[64];
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
int		prc;
double		sal=0.,salb=0.;
double		sumsal=0.,sumsalb=0.;
class iceb_tu_str fio("");
class iceb_tu_str vstr("");
class spis_oth oth;
short		mspz=0; /*метка соответствия первой записи*/

SQLCURSOR curr;
clear();

//gg=ggg;
kom=0;
pozz=pozz1=0;
//Создаем класс меню
VVOD VV(0);
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

clear();

if(poi.metka_poi == 1 && poi.god.getdlinna() > 1)
 gg=poi.god.ravno_atoi();
else
 gg=ggg;
 
short           kls=LINES-5;
sumsal=sumsalb=0.;

sprintf(strsql,"select * from Zsal where god=%d order by tabn asc",gg);
//printw("\n%s-%s\n",__FUNCTION__,strsql);
//OSTANOV();
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
 
if(kolstr == 0)
 {
lll:;
  shzr8(gg,sumsal,sumsalb,&poi);

  VVOD SOOB(1);
  sprintf(strsql,gettext("Не найдено ни одной записи в %d году !"),gg);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == FK3 || K == FK5)
      goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;
 }
else
 {
  while(cur.read_cursor(&row) != 0)
   {
    if(dirzarsl_prov(row,&poi) != 0)
      continue;
    sumsal+=atof(row[2]);
    sumsalb+=atof(row[5]);
   }
 }
naz:;

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
pozz1=0;
mspz=2;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;

  if(dirzarsl_prov(row,&poi) != 0)
    continue;

  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    fio.new_plus(row1[0]); 
  else
    fio.new_plus("");
    
  sprintf(strsql,"%s|%-5s|%8.2f|%8.2f|%.*s",
  row[0],row[1],atof(row[2]),atof(row[5]),
  iceb_tu_kolbait(25,fio.ravno()),fio.ravno());

  spis_menu.spis_str.plus(strsql);
  i++;
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


shzr8(gg,sumsal,sumsalb,&poi);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,2,0,NULL,strsql,prc,&K,"",0);

if(kom == -7) //Изменение размеров экрана
 goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  GDITE();
  for(i=0; i < mspz  ; )
   {
    pozz++;
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(dirzarsl_prov(row,&poi) != 0)
      continue;
    i++;
   }
  kom=kls;
  goto naz;




 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);

    if(dirzarsl_prov(row,&poi) != 0)
      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  if(pozz + kls >= kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  GDITE();
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(dirzarsl_prov(row,&poi) != 0)
      continue;

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;

  polen(str_vib.ravno(),&tbn,2,'|');

  polen(str_vib.ravno(),&sal,3,'|');

  polen(str_vib.ravno(),&salb,4,'|');

 }

vper:;

switch (K)
 {
  case ESC : /*Выход*/
  case FK10 : /*Выход*/
     return;
     break;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp3_1_9.txt");
   clear();
   goto naz;


  case FK2:   /*Ввести запись*/
  case PLU:  

    VV.VVOD_delete();
    sozvzar8(&VV);
    vzar8(&VV,gg,0);
    goto naz1;

  case SFK2:   /*Корректировать запись*/
  case ENTER:   /*Корректировать запись*/
    VV.VVOD_delete();
    sozvzar8(&VV);
    sprintf(strsql,"%ld",tbn);
    VV.VVOD_SPISOK_zapis_data(0,strsql);
    sprintf(strsql,"%.2f",sal);
    VV.VVOD_SPISOK_zapis_data(1,strsql);
    sprintf(strsql,"%.2f",salb);
    VV.VVOD_SPISOK_zapis_data(2,strsql);
    vzar8(&VV,gg,1);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(iceb_t_pbpds(1,gg) != 0)
     goto naz1;
    
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz1;

    sprintf(strsql,"delete from Zsal where god=%d and tabn=%ld",gg,tbn);

    sql_zapis(strsql,0,0);
    goto naz1;

  case FK4:       /*Поиск*/

    dirzarsl_poi(&poi);
    
    goto naz1;

  case FK5:       /*Распечатать*/

    clear();
    refresh();

    sprintf(imaf,"sl%d.lst",gg);
    if((ff = fopen(imaf,"w")) == NULL)
     {
      error_op_nfil(imaf,errno,"");
      goto naz1;
     }
    time(&tmm);
    bf=localtime(&tmm);
    
    
    fprintf(ff,"%s\n\n%s %d%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
    iceb_t_get_pnk("00",0),
    gettext("Перечень сальдо за"),
    gg,
    gettext("г."),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);

    if(poi.metka_poi == 1)    
     {
      if(poi.god.getdlinna() > 1)
       fprintf(ff,"%s:%s\n",gettext("Год"),poi.god.ravno());

      if(poi.tabnom.getdlinna() > 1)
       fprintf(ff,"%s:%s\n",gettext("Табельный номер"),poi.tabnom.ravno());

      if(poi.fio.getdlinna() > 1)
       fprintf(ff,"%s:%s\n",gettext("Фамилия"),poi.fio.ravno());
     }     
    fprintf(ff,"\
-------------------------------------------------------------------------\n");
    fprintf(ff,gettext("Дата| Т/Н |Фамилия , Имя  , Отчество      |   Сальдо     |Бюджетное сал.|\n"));
    
    fprintf(ff,"\
-------------------------------------------------------------------------\n");
    
    sumsp=sumso=sumspb=sumsob=0.;
    cur.poz_cursor(0);
    while(cur.read_cursor(&row) != 0)
     {

      if(dirzarsl_prov(row,&poi) != 0)
       continue;
      sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
       fio.new_plus(row1[0]);
      else
       fio.new_plus("");
      sal=atof(row[2]);
      salb=atof(row[5]);

      fprintf(ff,"%s %-5s %-*.*s %14.2f %14.2f\n",
      row[0],row[1],
      iceb_tu_kolbait(31,fio.ravno()),iceb_tu_kolbait(31,fio.ravno()),fio.ravno(),
      sal,salb);

      if(sal >=0.)
        sumsp+=sal;
      else
        sumso+=sal;
      if(salb >=0.)
        sumspb+=salb;
      else
        sumsob+=salb;
     }
    fprintf(ff,"\
--------------------------------------------------------------------------\n\
%*s:%14.2f %14.2f\n",iceb_tu_kolbait(42,gettext("Итого")),gettext("Итого"),sumsp+sumso,sumspb+sumsob);

    printw("\
%*s - %10.2f\n\
%*s - %10.2f\n\
%*s - %10.2f\n",
iceb_tu_kolbait(15,gettext("Положительное")),
gettext("Положительное"),
sumsp,
iceb_tu_kolbait(15,gettext("Отрицательное")),
gettext("Отрицательное"),
sumso,
iceb_tu_kolbait(15,gettext("Итого")),
gettext("Итого"),
sumsp+sumso);

    fprintf(ff,"\
%*s - %10.2f\n\
%*s - %10.2f\n\
%*s - %10.2f\n",
iceb_tu_kolbait(15,gettext("Положительное")),
gettext("Положительное"),sumsp,
iceb_tu_kolbait(15,gettext("Отрицательное")),
gettext("Отрицательное"),sumso,
iceb_tu_kolbait(15,gettext("Итого")),
gettext("Итого"),sumsp+sumso);

    fprintf(ff,"\n\%s\n\
%*s - %10.2f\n\
%*s - %10.2f\n\
%*s - %10.2f\n",
gettext("Бюджет"),
iceb_tu_kolbait(15,gettext("Положительное")),
gettext("Положительное"),sumspb,
iceb_tu_kolbait(15,gettext("Отрицательное")),
gettext("Отрицательное"),sumsob,
iceb_tu_kolbait(15,gettext("Итого")),
gettext("Итого"),sumspb+sumsob);

    fclose(ff);
    OSTANOV();

    oth.clear();
    oth.spis_imaf.plus(imaf);
    oth.spis_naim.plus(gettext("Распечатка списка стартовых сальдо"));
    iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);
    iceb_t_rabfil(&oth,"");
    goto naz1;

  default:
    goto naz;
    break;
 }
}

/*******/
/*Ввод */
/*******/

void vzar8(VVOD *VV,short gg,short mr) //0-ввод нового 1-корректировка
{
int             par;
long		tbnz;
time_t		vrem;
char		strsql[512];

tbnz=(long)ATOFM(VV->VVOD_SPISOK_return_data(0));

VV->VVOD_SPISOK_add_MD(gettext("Табельный номер..."));
VV->VVOD_SPISOK_add_MD(gettext("Сальдо............"));
VV->VVOD_SPISOK_add_MD(gettext("Бюджетное сальдо.."));

naz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


par=VV->vvod(0,1,1,-1,-1);

if(par == FK2 || par == PLU)
 {
  if(VV->VVOD_SPISOK_return_data(0)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введён табельный номер !"));
    goto naz;
   }
  if(VV->VVOD_SPISOK_return_data(1)[0] == '\0')
   {
    iceb_t_soob(gettext("Не введено сальдо !"));
    goto naz;
   }

  if(iceb_t_pbpds(1,ggg) != 0)
   goto naz;

  if(gg != ggg)
   if(iceb_t_pbpds(1,gg) != 0)
    goto naz;

  time(&vrem);

  if(mr == 0)
   {
    sprintf(strsql,"insert into Zsal \
values (%d,%s,%.2f,%d,%ld,%.2f)",
gg,VV->VVOD_SPISOK_return_data(0),ATOFM(VV->VVOD_SPISOK_return_data(1)),iceb_t_getuid(),vrem,ATOFM(VV->VVOD_SPISOK_return_data(2)));
   }
   
  if(mr == 1)
   {
    sprintf(strsql,"update Zsal \
set \
god=%d,\
tabn=%d,\
saldo=%.2f,\
ktoz=%d,\
vrem=%ld,\
saldob=%.2f \
where god=%d and tabn=%ld",
    gg,
    VV->data_ravno_atoi(0),
    ATOFM(VV->VVOD_SPISOK_return_data(1)),
    iceb_t_getuid(),
    vrem,
    ATOFM(VV->VVOD_SPISOK_return_data(2)),
    gg,
    tbnz);
   }

  GDITE();
  if(sql_zapis(strsql,1,0) != 0)
   {
    goto naz;
   }


 }
clear(); /*Очистить экран и закрасить фоновым цветом*/
}

/****************/

void shzr8(short gg,double sumasal,double sumasalb,class dirzarsl_p *poi)
{
short		X=COLS-20,Y=2;
move(0,0);
printw("%s %d%s",gettext("Сальдо по табельным номерам на"),gg,
gettext("г."));
mvprintw(1,0,"   1     2       3        4             5");

move(Y,X);
printw(gettext("Расшифровка полей:"));

move(++Y,X);
printw(gettext("1-год"));

move(++Y,X);
printw(gettext("2-табельный номер"));

move(++Y,X);
printw(gettext("3-сальдо"));

move(++Y,X);
printw(gettext("4-б. сальдо"));
move(++Y,X);
printw(gettext("5-фамилия"));

Y+=2;

move(Y,X);
printw("%.2f",sumasal);
move(++Y,X);
printw("%.2f",sumasalb);


if(poi->metka_poi == 1)
 {
  move(Y+=2,X);
  attron(A_BLINK);

  printw(gettext("Поиск:"));

  if(poi->god.getdlinna() > 1)
    mvprintw(++Y,X,"%s %s",gettext("Год"),poi->god.ravno());

  if(poi->tabnom.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s",gettext("Таб.ном."),poi->tabnom.ravno());

  if(poi->fio.getdlinna() > 1)
    mvprintw(++Y,X,"%s:%s",gettext("Фамилия"),poi->fio.ravno());

  attroff(A_BLINK);
 } 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
}
/******************************/
void	sozvzar8(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(10);
VV->VVOD_SPISOK_add_data(20);
VV->VVOD_SPISOK_add_data(20);
}
/**********************/
/*ввод реквизитов поиска*/
/***************************/
int dirzarsl_poi(class dirzarsl_p *poi)
{
int N=0,K=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Год.................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.(,,)."));//1
VV.VVOD_SPISOK_add_MD(gettext("Фамилия.........(,,)."));//2

VV.VVOD_SPISOK_add_data(poi->god.ravno(),5);//0
VV.VVOD_SPISOK_add_data(poi->tabnom.ravno(),128);//1
VV.VVOD_SPISOK_add_data(poi->fio.ravno(),128);//2

naz:;


helstr(LINES-1,0,
"F2/+",gettext("запись"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   {
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) 
     {
     }

    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    poi->metka_poi=0;
    return(1);
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    poi->god.new_plus(VV.data_ravno(0));
    poi->tabnom.new_plus(VV.data_ravno(1));
    poi->fio.new_plus(VV.data_ravno(2));
    break;
   }
  if(K == FK4)
   {
    VV.VVOD_clear_data();
    goto naz;
   }
 }

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
poi->metka_poi=1;
return(0);
}
/****************************/
/*проверка реквизитов поиска*/
/******************************/
int dirzarsl_prov(SQL_str row,class dirzarsl_p *poi)
{
if(poi->metka_poi == 0)
 return(0);
 
if(proverka(poi->tabnom.ravno(),row[1],0,0) != 0)
 return(1);

if(poi->fio.getdlinna() > 1)
 {
  char strsql[512];
  SQL_str row1;
  class SQLCURSOR cur;
  sprintf(strsql,"select fio from Kartb where tabn=%d",atoi(row[1]));
  if(readkey(strsql,&row1,&cur) == 1)
   {
    if(proverka(poi->fio.ravno(),row1[0],4,0) != 0)
     return(1);
   }
 }
 
return(0);

}
