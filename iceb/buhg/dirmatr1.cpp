/* $Id: dirmatr1.c,v 5.55 2013/05/17 14:55:58 sasa Exp $ */
/*13.07.2015    02.11.1993      Белых А.И.       dirmatr1.c
Просмотр ввод и корректировка кодов и наименований материалов
*/
#include        <errno.h>
#include        <ctype.h>
#include        "buhg.h"

void            shmt3(const char*,short,short,double,const char[],const char[],int,int,int,int);
int provdirm(const char kgrm[],const char kgrm1[],const char nm[],const char nm1[],const char ei[],short mpro,const char kodm[],int regim,int skl,const char *artikul,int metkasklada);
void		otmmat(const char*,long*,short,SQLCURSOR*);

extern double	okrcn;  /*Округление цены*/
extern double   kursue;/*Курс условной единицы*/
char		*maskmat; /*Массив кодов отмеченных материалов*/
extern int      readscan();
extern short	startgod;

int dirmatr1(int *kgr, //Код группы
int *kod,  //Код материалла
class iceb_tu_str *nai,  //Наименование
int skl,
const char *grup,int regim,
int metkasklada, //0-все 1-только материалы карточки которых есть на складе
int metka_kc) //0-с нулевой ценой не выбирать 1-выбирать с любой ценой
{
class iceb_tu_str vstr("");
class spis_oth oth;
double		kolih;
short           kls=0;
short           x=0,y=4;
struct  tm      *bf;
time_t          tmm;
int             K;
int           kom,kom1;
int             i,prc;
int             i1;
class iceb_tu_str nm(nai->ravno());
char            imaf[64];
FILE            *ff;
double          cena=0.,krat=0.,fas=0.;
unsigned short  km,ki,kn;
short		mpro;   /*Метка просмотра списка 0-обычный 1-цены 2-помеченные записи*/
#define		M_CEN 	6
short		valuta; /*0-гривня 1-пересчитанная по курсу УЕ*/
class iceb_tu_str naigrup("");
class iceb_tu_str kgrm(grup);
long		dlmas;
class iceb_tu_str mom(""); /*Метка отмеченного материалла*/
class iceb_tu_str obr("");
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
char		bros[1024];
int		most; /*Метка остатка*/
class iceb_tu_str strih("");
short		mspz; /*метка соответствия первой записи*/
static class iceb_tu_str datn("");
int		sklpoi=skl;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

SQLCURSOR curr;
most=0;
//ei[0]='\0';
pozz=pozz1=0;
dlmas=0;
VVOD VVOD1(2);
VVOD VV(0);
VVOD DANET(1);
VVOD MENU(3);
  
valuta=mpro=0;
kom=0;

naz1:;
clear();
if(regim == 0)
  kls=LINES-1-6;
if(regim != 0)
  kls=LINES-12;


K=0;
sprintf(strsql,"select * from Material order by kodm asc");

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }   
if(kolstr == 0)
 {
lll:;
//  readscan(NULL); /*Сброс сканерного ввода*/
  shmt3(nm.ravno(),mpro,valuta,kursue,kgrm.ravno(),naigrup.ravno(),most,sklpoi,regim,metkasklada);

   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
   K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 )
     goto naz1;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;
 }

naz:;
GDITE();

if(regim != 0)
 {
  VVOD SOOBN(1);
  SOOBN.VVOD_SPISOK_add_MD(gettext("С помощью клавиш \"стрелка вверх\", \"стрелка вниз\" выберите"));
  SOOBN.VVOD_SPISOK_add_MD(gettext("товар, который вам нужен и нажмите клавишу \"Enter\""));
  soobsh(&SOOBN,LINES-6,0,1);
 }
 
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
  if(provdirm(kgrm.ravno(),row[1],nm.ravno(),row[2],row[4],mpro,row[0],regim,sklpoi,row[14],metkasklada) != 0)
    continue;

     
  mom.new_plus("");

  if(maskmat != NULL)
   {
    sprintf(strsql,"%s",row[0]);
    if(pole1(maskmat,strsql,',',0,&i1) == 0 || SRAV(strsql,maskmat,0) == 0)
      mom.plus("*");
   }
  if(atof(row[12]) == 1)
   mom.plus("#");
         
  cena=atof(row[6]);
  krat=atof(row[7]);
  fas=atof(row[8]);
  if(most == 1)
   {
    kolih=ostdok1(dt,mt,gt,sklpoi,atoi(row[0]));
    sprintf(bros,"%-4s|%-2s|%-*.*s|%-*s|%8.15g|%4.5g|%-4.3g|%.10g%s",
    row[0],row[1],
    iceb_tu_kolbait(30,row[2]),iceb_tu_kolbait(30,row[2]),row[2],
    iceb_tu_kolbait(3,row[4]),row[4],
    cena,krat,fas,kolih,mom.ravno());
   }
  else        
    sprintf(bros,"%-4s|%-2s|%-*.*s|%-*s|%8.15g|%4.5g|%-4.3g%s",
    row[0],row[1],
    iceb_tu_kolbait(30,row[2]),iceb_tu_kolbait(30,row[2]),row[2],
    iceb_tu_kolbait(3,row[4]),row[4],
    cena,krat,fas,mom.ravno());
  i++;
  spis_menu.spis_str.plus(bros);

  if(pozz1 == 1)
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

shmt3(nm.ravno(),mpro,valuta,kursue,kgrm.ravno(),naigrup.ravno(),most,sklpoi,regim,metkasklada);

strih.new_plus("");
prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


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

   if(provdirm(kgrm.ravno(),row[1],nm.ravno(),row[2],row[4],mpro,row[0],regim,sklpoi,row[14],metkasklada) != 0)
     continue;
    i++;
   }
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  GDITE();
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

    if(provdirm(kgrm.ravno(),row[1],nm.ravno(),row[2],row[4],mpro,row[0],regim,sklpoi,row[14],metkasklada) != 0)
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
/*  
  printw("\n вперед pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
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
    cur.read_cursor(&row);

    if(provdirm(kgrm.ravno(),row[1],nm.ravno(),row[2],row[4],mpro,row[0],regim,sklpoi,row[14],metkasklada) != 0)
      continue;
    i++;
   }
  goto naz;
 }

/*Читаем материал*/
if(kom == -6) /*Прочитали сканерный ввод*/
 {
  sprintf(strsql,"select kodm from Material \
where strihkod='%s'",strih.ravno());

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найден материал со штрих-кодом %s !"),strih.ravno());
    iceb_t_soob(strsql);
    goto naz;
   }
  strcpy(strsql,row1[0]);
  pozz=poivcur(strsql,0,&cur);
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  
  if(pozz+kom >= kolstr)
   goto naz;

  POLE(str_vib.ravno(),strsql,1,'|');

  i=poivcur(strsql,0,&cur);
  cur.poz_cursor(i);
  cur.read_cursor(&row);

  *kod=atoi(row[0]);

  *kgr=atoi(row[1]);
//  kt=atoi(row[10]);
//  vr=atoi(row[11]);
  nai->new_plus(row[2]);
  cena=atof(row[6]);
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case NOL :   /*Для выбора без привязки к карточкам*/
  case ENTER: /*Выход*/

    GDITE();
    if(metka_kc == 0)
     {
       if(cena < 0.01)
        {
         iceb_t_soob(gettext("Не введена цена на товар !"));
         goto naz;
        }
      if(row[4][0] == '\0')
       {
        iceb_t_soob(gettext("Не введена единица измерения !"));
        goto naz;
       }
     }     
    return(0);

  case PLU : /*Пометить материал по образцу*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     obr.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для отметки материалов"));

     if((kom1=vvod1(&obr,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      {
       goto naz1;
      }
     if(kom1 == ESC)
      {
       goto naz1;
      }
     otmmat(obr.ravno(),&dlmas,0,&cur);
     pozz=0;
     goto naz;

  case OTN : /*Снять отметку с материалла по образцу*/
     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     obr.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для снятия отметки с материалла"));

     if((kom1=vvod1(&obr,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      {
       pozz=0;
       goto naz;
      }
     if(kom1 == ESC)
      {
       pozz=0;
       goto naz;
      }
     otmmat(obr.ravno(),&dlmas,1,&cur);
      {
       pozz=0;
       goto naz;
      }     
  case INS : /*Пометить материал*/
    sprintf(strsql,"%s",row[0]);
    if(maskmat != NULL)
     {
      if(pole4(maskmat,strsql,',') == 0)
       {
        if(strlen(maskmat) == 0)
         {
          free(maskmat);
          maskmat=NULL;
          dlmas=0;
         }
        kom++; 
        goto naz;
       }
     } 
    sozdmas(strsql,&maskmat,&dlmas,',');
    kom++;
    goto naz;

  case UMN:  /*Переключение режима вывода на экран*/
    if(mpro == 2)
     mpro=0; 
    else
     mpro=2;

    goto naz;

  case DEL:  /*Переключение режима вывода на экран*/

/*
    if(mpro == 1)
     mpro=0; 
    else
     mpro=1;

  */
  
    goto naz;

  case '8':  /*Переключение режима вывода на экран цены*/
/*
    if(valuta == 1)
     valuta=0; 
    else
     valuta=1;

*/
    goto naz;

  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
     
  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(provud("Material",row[0],1) != 0)
        goto naz;
        
      sprintf(strsql,"delete from Material where kodm = %s",row[0]);
/*      printw("\nstrsql=%s\n",strsql);*/
      sql_zapis(strsql,0,0); 
      clear();
      goto naz1;
     }
    goto naz;


  case FK4:       /*Отобрать по образцу*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     nm.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

     vvod1(&nm,112,&VVOD1,NULL,stdscr,-1,-1);

     GDITE();

     clear();
     kom=pozz=0;
     if(kolstr == 0)
      goto lll;
     else
       goto naz;

  case SFK4:    //Включение/выключение просмотра материалов карточки которых есть на складе

    metkasklada++;
    if(metkasklada > 1)
     metkasklada=0;

    if(skl == 0 && metkasklada == 1)
     {
      helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
      nm.new_plus("");
            
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код склада"));
      vstr.new_plus("");
      if(vvod1(&vstr,10,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       {
        metkasklada=0;
        goto naz1;
       }
      sklpoi=vstr.ravno_atoi();

      GDITE();

     }
    pozz=0;
    goto naz1;

  case FK5:       /*Распечатать*/
        
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать прайс лист"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список отмеченных материалов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ценники на отмеченные товары"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    switch (kom1)
     {
      case  4 :
      case -1 :
       goto naz;

      case  1 :
        praisl();
        goto naz;

      case  2 :
       clear();
       move(0,0);
       printw(gettext("Список отмеченных материалов:\n"));
       rasmaskm(maskmat);
       goto naz;

      case  3 :
       cennikm(maskmat);
       goto naz;

     }

    clear();
    time(&tmm);
    bf=localtime(&tmm);
    printw(gettext("Готовим распечатку списка материалов.\n"));
    GDITE();

    sprintf(imaf,"mat.lst");
    if((ff = fopen(imaf,"w")) == NULL)
     {
      error_op_nfil(imaf,errno,"");
       goto naz;
     }
    startfil(ff);

    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    fprintf(ff,"\x0F");
    fprintf(ff,"%s\n\n%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
    iceb_t_get_pnk("00",0),
    gettext("Распечатка списка товаров и материалов"),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);

    fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");

    fprintf(ff,gettext("Код мат.|Груп.|  Н а и м е н о в а н и е               |К.т.|Цена р. |Е.из|Кратност|Фасовка \n"));

    fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");

  km=ki=kn=0;
  cur.poz_cursor(0);
  while(cur.read_cursor(&row) != 0)
   {
    km++;

    fprintf(ff,"%8s %5s %-40s %4d %8.8g %-4s %8.8g %8.8g\n",
    row[0],row[1],row[2],atoi(row[7]),atof(row[6]),row[4],atof(row[7]),atof(row[8]));
   
   }
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,"\x1B\x50"); /*10-знаков*/
  fprintf(ff,"\x12");
  fclose(ff);

  move(15,0);
  printw("%s %s\n",gettext("Распечатка выгружена в файл"),imaf);
  OSTANOV();
  //rabfil(gettext("Распечатка списка товаров и материалов"),imaf);
  oth.clear();
  oth.spis_imaf.plus(imaf);
  oth.spis_naim.plus(gettext("Распечатка списка товаров и материалов"));
  iceb_t_rabfil(&oth,"");
  cur.poz_cursor(pozz);
  goto naz;


  case FK6:       /*Стать на номер */
    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код материалла"));
    vstr.new_plus("");
    if((kom1=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(kom1 == ESC)
      goto naz;
    if(strsql[0] == '\0')
      goto naz;

    nm.new_plus("");
    pozz=0;
    kom=0;
    pozz=poivcur(vstr.ravno(),0,&cur);
    if(pozz == kolstr)
     {
      sprintf(bros,"%s %s !",gettext("Не найден код материалла"),strsql);
      iceb_t_soob(bros);
      pozz=0;
     }
    clear();
    goto naz;
        
  case FK7:  /*Включение режима просмотра с остатками*/
    most++;
    if(most > 1)
     most=0;
    goto naz;

  case FK8:       /*Просмотр двіженія по матеріалу*/

    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_MD(gettext("Склад........."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала..."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата остатка.."));
  
    VV.VVOD_SPISOK_add_data(5);
    if(datn.ravno()[0] == '\0')
     sprintf(strsql,"1.1.%d",startgod);
    else
     strcpy(strsql,datn.ravno());
    VV.VVOD_SPISOK_add_data(strsql,11);
           
    sprintf(strsql,"%d.%d.%d",dt,mt,gt);
    VV.VVOD_SPISOK_add_data(strsql,11);

sfff8:;

  
    helstr(LINES-1,0,"F2/+",gettext("расчитать"),
    "F10",gettext("выход"),NULL);

    kom1=VV.vvod(0,1,1,-1,-1);

    datn.new_plus(VV.VVOD_SPISOK_return_data(1));

    switch(kom1)
     {
      case FK10:
      case ESC:
        break;
      case FK2:
      case PLU:
        
        short dn,mn,gn;
        short dk,mk,gk;
        
        i=(int)ATOFM(VV.VVOD_SPISOK_return_data(0));
        if(rsdat(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(1),1) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата начала !"));
          goto sfff8;      
         }

        if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(2),1) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата остатка !"));
          goto sfff8;      
         }
        dvmat(dn,mn,gn,dk,mk,gk,i,*kod);
        break;
      default:
       goto sfff8;      
     }
    goto naz;

    
  case FK9:       /*Просмотр по группе*/
    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
    kgrm.new_plus("");
     
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование группы материалла"));
    if((i=vvod1(&kgrm,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
     {
     if(kolstr == 0)
      goto lll;
     else
       goto naz;
     }
    if(i == FK10)
     {
     if(kolstr == 0)
      goto lll;
     else
       goto naz;
     }

    if(kgrm.ravno()[0] == '\0' || isdigit(kgrm.ravno()[0]) == 0)
      if(vibrek("Grup",&kgrm,&naigrup) != 0)
       {
        if(kolstr == 0)
         goto lll;
        else
          goto naz;
       }

    /*Читаем код группы материалла*/
    sprintf(strsql,"select * from Grup where kod=%s",
    kgrm.ravno());

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,gettext("Нет группы материалла %s !"),kgrm.ravno());
      iceb_t_soob(strsql);
      if(kolstr == 0)
       goto lll;
      else
        goto naz;
     }
   naigrup.new_plus(row1[1]);
   pozz=kom=0;
   if(kolstr == 0)
    goto lll;
   else
     goto naz;

  case SFK9:       /*Отмена просмотра по группе*/
    kgrm.new_plus("");
    naigrup.new_plus("");
    goto naz1;

  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }

}

/**********************/
/* Шапка              */
/**********************/
void            shmt3(const char nm[],short mpro,short valuta,double kursue,
const char kgrm[],const char naigrup[],int most,int skl,int regim,int metkasklada)
{
attroff(A_REVERSE);  /*Выключить инверсию*/
attroff(A_UNDERLINE);  /*Выключить подчеркивание*/
move(0,0);
clrtoeol();  /*Очистить стороку*/
printw(gettext("Ввод и корректировка материалов и товаров"));
if(skl != 0)
 printw(" %s %d",gettext("Склад"),skl);


if(mpro == 2)
 {
  attron(A_BLINK);
  printw(gettext("     Только отмеченные материалы"));
  attroff(A_BLINK);
 }
if(valuta == 1)
 {
  attron(A_BLINK);
  printw(gettext("     Курс У.Е.=%.6g"),kursue);
  attroff(A_BLINK);
 }

clearstr(1,0);
if(kgrm[0] != '\0')
 {
  attron(A_BLINK);
  printw(gettext("Поиск групы - %s %s"),kgrm,naigrup);
  attroff(A_BLINK);
 }
if(regim == 2)
 {
  attron(A_BLINK);
  printw(gettext("Отобраны только весовые товары и материалы"));
  attroff(A_BLINK);
 }
if(most == 1)
 {
  if(skl != 0)
    printw(" %s:%d",gettext("Остатки по складу"),skl);
  if(skl == 0)
    printw(" %s",gettext("Остатки по всем складам"));
 }
if(skl != 0 && metkasklada == 1)
 {
  printw(" %s:%d",gettext("Только материалы на складе"),skl);
 }

if(nm[0] != '\0')
 {
  clearstr(2,0);
  attron(A_BLINK);
  printw(" %s:%s",gettext("Найдено по образцу"),nm);
  attroff(A_BLINK);
 }

move(3,0);
if(most == 0)
 printw(gettext("Код м. Группа    Наименование           Ед.из. Цена. Крат.Фас-ка."));
else
 printw(gettext("Код м. Група.    Наименование           Ед.из. Цена. Крат.Фас-ка.Остаток."));

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("код"),
"F7",gettext("остат."),
"F9",gettext("груп."),
"F10",gettext("выход"),NULL);

}
/*******************************/
/*Распечатка массива материалов*/
/*******************************/

void		rasmaskm(char mas[])
{
FILE		*ff;
char		imaf[64];
short		kp;
short		i;
class iceb_tu_str kodm("");
time_t   	tmm;
struct  tm      *bf;
SQL_str         row;
char		strsql[200];
SQLCURSOR curr;

if(mas == NULL)
 return;
sprintf(imaf,"rmm%d.lst",getpid());

kp=pole2(mas,',');
if(kp == 0)
 kp=1;
time(&tmm);
bf=localtime(&tmm);

if((ff = fopen(imaf,"w")) == NULL)
     {
      error_op_nfil(imaf,errno,"");
      return;
     }
startfil(ff);
fprintf(ff,"\x1B\x4D");
fprintf(ff,"\x0F");
fprintf(ff,"%s\n\n%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка отмеченных материалов"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);
fprintf(ff,"-------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("Код мат.|Груп.|  Н а и м е н о в а н и е               |К.т.|Цена р. |Е.из|Кратност|Фасовка \n"));
fprintf(ff,"-------------------------------------------------------------------------------------------------------------\n");

for(i=0; i< kp; i++)
 {
  if(i == 0 && polen(mas,&kodm,i+1,',') != 0)
     kodm.new_plus(mas);
  if(i != 0)
     polen(mas,&kodm,i+1,',');

  sprintf(strsql,"select * from Material where kodm=%s",
  kodm.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
    {
     printw("%s %s !\n",gettext("Не найден код материала"),kodm.ravno());
     OSTANOV();
     continue;
    }
   
  printw("%4s %s\n",row[0],row[0]);
  fprintf(ff,"%8s %5s %-*s %4d %8.8g %-*s %8.8g %8.8g\n",
  row[0],row[1],
  iceb_tu_kolbait(40,row[2]),row[2],
  atoi(row[7]),atof(row[6]),
  iceb_tu_kolbait(4,row[4]),row[4],
  atof(row[7]),atof(row[8]));

 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\x1B\x50");
fprintf(ff,"\x12");

fclose(ff);
printw("%s %s\n",gettext("Распечатка выгружена в файл"),imaf);

OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка отмеченных материалов"));
iceb_t_rabfil(&oth,"");
}

/*******************************/
/*Отметка материалов по образцу*/
/*******************************/
void		otmmat(const char obr[],long *dlmas,
short mt, //0-отметить 1-снять отметку
SQLCURSOR *cur)
{
long		ddd;
int		i;
SQL_str         row;
char		strsql[512];

ddd=*dlmas;
if(mt == 1 && maskmat == NULL)
  return;

GDITE();
if(mt == 1 && obr[0] == '\0')
 {
  free(maskmat); 
  maskmat=NULL;
  *dlmas=0;
  return;
 }

if(maskmat != NULL)
  memset(maskmat,'\0',ddd);

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {

  if(obr[0] == '\0' || strstrm(row[2],obr) != 0)
   {
    sprintf(strsql,"%s",row[0]);
    if(mt == 0)
     { 
      if(maskmat != NULL)
       if(pole1(maskmat,strsql,',',0,&i) == 0)
         continue;
      sozdmas(strsql,&maskmat,&ddd,',');
     }
    if(mt == 1)
     {
      if(pole4(maskmat,strsql,',') == 0)
       {
        if(strlen(maskmat) == 0)
         {
          free(maskmat);
          maskmat=NULL;
          *dlmas=0;
          return;
         }
       }

     }
   }

 }

}

/*************************************************/
/*Проверка строки на соотвествие заданному поиску*/
/*************************************************/
int provdirm(const char kgrm[],const char kgrm1[],const char nm[],const char nm1[],
const char ei[],
short mpro,  //Метка просмотра списка 0-обычный 1-цены 2-помеченные записи
const char kodm[],int regim,
int skl, //Код склада
const char *artikul,
int metkasklada) //0-все 1-только матеиалы карточки которых есть на складе
{
int		i1;

if(kgrm[0] != '\0')
 if(pole1(kgrm,kgrm1,',',0,&i1) != 0 && SRAV(kgrm,kgrm1,0) != 0)
   return(1);
   
if(nm[0] != '\0' && strstrm(nm1,nm) == 0)
 if(strstrm(artikul,nm) == 0)
  return(2);

if(mpro == 2 && (maskmat != NULL && pole1(maskmat,kodm,',',0,&i1) != 0))
  return(3);

if(regim == 2 && (SRAV("Кг",ei,0) != 0 && 
SRAV("кг",ei,0) != 0 && SRAV("КГ",ei,0) != 0))
  return(4);


if(skl != 0 && metkasklada == 1)
 {
  char strsql[512];
  sprintf(strsql,"select kodm from Kart where sklad=%d and kodm=%s limit 1",skl,kodm);
  if(sql_readkey(&bd,strsql) <= 0)
    return(5);  
 }

return(0);
}
