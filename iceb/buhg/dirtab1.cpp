/* $Id: dirtab1.c,v 5.20 2013/05/17 14:55:59 sasa Exp $ */
/*14.07.2015	05.06.2005	Белых А.И.	dirtab1.c
Просмотр списка табельных номеров без возможности работать с карточками
*/
#include        <errno.h>
#include        "buhg.h"
class dirtab1_poi
 {
  public:
   class iceb_tu_str fio;
   class iceb_tu_str inn;
   int		mp;  /*0-всех 1-работающих 2-уволенных*/
   int		pod;
   class iceb_tu_str mpz;

  dirtab1_poi()
   {
    clear();
    mp=0;
    pod=0;
    mpz.plus("");
   }
  void clear()
   {
    fio.new_plus("");
    inn.new_plus("");
   }
 };
 
void		sapptab1(class dirtab1_poi*);
int provp1(SQL_str roww,class dirtab1_poi *poi);
void dirtar1_rs(class SQLCURSOR *cur,class dirtab1_poi *poi);


int dirtab1(int *tbn,//Табельный номер
class iceb_tu_str *fio, //Фамилия
int podr) //Код подразделения
{
class dirtab1_poi poi;
int		kom,kom1;
long		pozz,pozz1;
long		kolstr;
SQL_str         row;
char		strsql[1024];
int		i;
int		K;
int		prc;
int  	tabel=0;
short		msor; /*0-по фамилиям 1-по табельным номерам*/
short		mspz; /*метка соответствия первой записи*/
class iceb_t_mstr spis_menu(0);

//Создаем класс меню
VVOD VV(0);
msor=0;

poi.pod=podr;
kom=pozz=pozz1=0;

naz1:;
clear();
GDITE();
short		kls=LINES-5;

K=0;
if(poi.pod == 0)
 {
  if(msor == 0)
    sprintf(strsql,"select tabn,fio,datk,inn,sovm from Kartb order by fio asc");
  else
    sprintf(strsql,"select tabn,fio,datk,inn,sovm from Kartb order by tabn asc");
 }
else
 {
  if(msor == 0)
   sprintf(strsql,"select tabn,fio,datk,inn,sovm from Kartb where podr=%d \
order by fio asc",poi.pod);
  else
   sprintf(strsql,"select tabn,fio,datk,inn,sovm from Kartb where podr=%d \
order by tabn asc",poi.pod);
 } 
/*
printw("%s\n",strsql);
OSTANOV();
*/
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {

lll:;

  sapptab1(&poi);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == FK5)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;

  goto vper;

 }

naz:;
GDITE();
clear();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i < kls ;)
 {

  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;


  if(provp1(row,&poi) != 0)
    continue;

  poi.mpz.new_plus("");
  if(row[4][0] == '1')
   poi.mpz.plus("S");      

  sprintf(strsql,"%-5s|%-*.*s|%1s",
  row[0],
  iceb_tu_kolbait(40,row[1]),iceb_tu_kolbait(40,row[1]),row[1],
  poi.mpz.ravno());

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

sapptab1(&poi);

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
  for(i=0; i < mspz  ; )
//  for(i=0; i<1  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(provp1(row,&poi) != 0)
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


    if(provp1(row,&poi) != 0)
      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
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
    cur.read_cursor(&row);


    if(provp1(row,&poi) != 0)
      continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;
  polen(spis_menu.spis_str.ravno(kom),strsql,sizeof(strsql),1,'|');
  tabel=atoi(strsql);
  polen(spis_menu.spis_str.ravno(kom),fio,2,'|');
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     *tbn=tabel;
     return(1);

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp2_5.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
     
  case ENTER : /*Проосмотр выбранного*/
      *tbn=tabel;
      return(0);
          

  case FK4:       /*Поиск*/
    VV.VVOD_delete();
    poi.clear();
    VV.VVOD_SPISOK_add_MD(gettext("Фамилия................"));
    VV.VVOD_SPISOK_add_MD(gettext("Идентификационный код.."));
    VV.VVOD_SPISOK_add_data(64);
    VV.VVOD_SPISOK_add_data(11);
    
fk4:;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F10",gettext("выход"),NULL);

    kom1=VV.vvod(0,1,1,-1,-1);

    switch(kom1)
     {
      case FK10:
      case ESC:
         break;                 
      case FK2:
      case PLU:
        poi.fio.new_plus(VV.VVOD_SPISOK_return_data(0));
        poi.inn.new_plus(VV.VVOD_SPISOK_return_data(1));
        break;
      default:
        goto fk4;
     }   
    clear();
    if(kolstr > 0)
      goto naz; 
    else
      goto lll;
          
  case FK5:       /*Распечатать*/
   dirtar1_rs(&cur,&poi);
   clear();
   goto naz;

  case FK6: /*Смена режима просмотра*/
    poi.mp++;
    if(poi.mp == 3)
     poi.mp=0;
    clear();
    if(kolstr > 0)
      goto naz;
    else
      goto lll;


  case FK9: //Смена сортировки
    msor++;
    if(msor == 2)
     msor=0;
    clear();
    goto naz1;

   default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

 }  
return(1);
}

/*********************/
/*Шапка режима работы*/
/*********************/
void		sapptab1(class dirtab1_poi *poi)
{
int		Y,X;
move(0,0);

if(poi->mp == 0)
 {
  attron(A_BLINK);
  printw(gettext("Просмотр всех табельных номеров"));
  attroff(A_BLINK);
 }
if(poi->mp == 1)
 printw(gettext("Просмотр только работающих"));
if(poi->mp == 2)
 {
  attron(A_BLINK);
  printw(gettext("Просмотр только уволенных"));
  attroff(A_BLINK);
 }
short d,m,g;
poltekdat(&d,&m,&g);
printw(" %s %d.%d.%d%s",gettext("Текущая дата"),d,m,g,
gettext("г."));

if(poi->pod != 0)
  printw(" %s %d",gettext("Подразделение"),poi->pod);


move(1,0);

printw("    1                     2");
move(Y=2,X=COLS-20);
printw(gettext("Расшифровка полей:"));

move(++Y,X);
printw(gettext("1-табельный ном."));

move(++Y,X);
printw(gettext("2-фамилия"));


move(++Y,X);
printw(gettext("S-совместитель"));

  

if(poi->fio.getdlinna() > 1 || poi->inn.getdlinna() > 1)
 {
  attron(A_BLINK);
  move(Y+=2,X);
  printw("%s:",gettext("Поиск"));
  if(poi->fio.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s",poi->fio.ravno());
   }
  if(poi->inn.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s",poi->inn.ravno());
   }
  attroff(A_BLINK);
 }
helstr(LINES-1,0,"F1",gettext("помощь"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("увол."),
"F7","*",
"F9",gettext("сорт."),
"F10",gettext("выход"),NULL);

}

/**********/
/*Проверка*/
/**********/
int provp1(SQL_str roww,class dirtab1_poi *poi)
{

if(poi->mp == 2 && roww[2][0] == '0')
  return(1);
  
if(poi->mp == 1 && roww[2][0] != '0')
  return(2);

if(proverka(poi->fio.ravno(),roww[1],4,0) != 0)
  return(3); 

if(proverka(poi->inn.ravno(),roww[3],4,0) != 0)
 return(4);




  
return(0);

}
/**********************************/
/*Распечатка списка*/
/****************************/

void dirtar1_rs(class SQLCURSOR *cur,class dirtab1_poi *poi)
{
struct  tm      *bf;
time_t          tmm;
char imaf[64];
FILE *ff;

GDITE();
sprintf(imaf,"pod%d.lst",poi->pod);
if((ff = fopen(imaf,"w")) == NULL)
 {
    error_op_nfil(imaf,errno,"");
  return;
 }
time(&tmm);
bf=localtime(&tmm);

fprintf(ff,"%s\n\n%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка списка работников"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);
char strsql[512];
if(poi->pod != 0)
 {
   SQL_str row1;
   SQLCURSOR cur1;
   class iceb_tu_str naim_podr("");
   
   sprintf(strsql,"select naik from Podr where kod=%d",poi->pod);
   if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    naim_podr.new_plus(row1[0]);

   fprintf(ff,"%s:%d %s\n",gettext("Подразделение"),poi->pod,naim_podr.ravno());
 }
fprintf(ff,"\
-----------------------------------------------------\n");
fprintf(ff,gettext(" Т/н |   Фамилия, Имя, Отчество\n"));
fprintf(ff,"\
-----------------------------------------------------\n");
int i=0;
SQL_str row;
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
   if(provp1(row,poi) != 0)
     continue;
   i++;
   fprintf(ff,"%-5s %s\n",row[0],row[1]);
 }
fprintf(ff,"\
-----------------------------------------------------\n\
%s - %d\n",gettext("Общее количество"),i);
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка списка работников"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);
iceb_t_rabfil(&oth,"");

}
