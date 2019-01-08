/* $Id: dirmatr.c,v 5.113 2013/11/24 08:23:03 sasa Exp $ */
/*06.12.2018    02.11.1993      Белых А.И.       dirmatr.c
Просмотр ввод и корректировка кодов и наименований материалов
*/
#include        <errno.h>
#include        <ctype.h>
#include        <sys/stat.h>
#include        "buhg.h"
#include "mugetdpp.h"

class dirmatr_prov
 {
  public:
   class iceb_tu_str kgrm;
   class iceb_tu_str naimpoi;
   short mpro;   /*Метка просмотра списка 0-обычный 1-помеченные записи*/
   int regim;
   int sklpoi;
   int metkasklada; //0-все 1-только материалы карточки которых есть на складе
   double kolih;
   short metka_mat_s_ost; //0-всё 1-только материалы с остатками
   short most; //Метка остатка 0-пказывать без остатка 1- с остатками реальными 2-с остатками по документам в том числе и по неподтверждённым
   short dt,mt,gt;
  
  dirmatr_prov()
   {
    kgrm.new_plus("");
    naimpoi.new_plus("");
    mpro=0;    
    regim=0;
    sklpoi=0;
    metkasklada=0;
    kolih=0.;
    metka_mat_s_ost=0;
    most=0;
    poltekdat(&dt,&mt,&gt);
   }
 }; 

void dirmatr_vvod(int mk,class iceb_tu_str *kodmv);

void shmt3(short valuta,double kursue,const char *naigrup,int metkapros,int kolstr,class dirmatr_prov *prov,int dlinna_nazv);
int		provdirm(char*,char*,char*,char*,char*,short,char*,int,int,char*,int);
void		udnmat(SQLCURSOR*,long);
void 		otmmat(const char*,long*,short,SQLCURSOR*);
extern int      readscan();
void            dirmatrzag(void);
void	        zamkm(int);
int dirmatr_prov(SQL_str row,class dirmatr_prov *poi);
void dirmatr_rs(class SQLCURSOR *cur,class dirmatr_prov *prov);
int dirmatr_provkor(int nkod,int skod);
void	kormat(int,int);
int dirmatr_vmost();
void dirmatr_csv(class SQLCURSOR *cur,class dirmatr_prov *prov);
void dirmatr_f9k(class SQLCURSOR *curz,class dirmatr_prov *prov,class iceb_tu_str *naigrup);
void dirmatr_rsdpp(int kolstr,class SQLCURSOR *cur,class dirmatr_prov *prov);

extern double   okrcn;  /*Округление цены*/
extern double   kursue;/*Курс условной единицы*/
char		*maskmat; /*Массив кодов отмеченных материалов*/
extern short	startgod;
extern short metka_pros_mat_s_ost; //0-показывать все материалы 1-только имеющие остаток (для расходных документов)

int dirmatr(int *kgr, //Код группы
int *kod,  //Код материалла
class iceb_tu_str *nai, //Наименование
int mk,  //0-ввод 1-выбор
int skl, //Склад
const char *grup,int regim,int metka_pros_mat) //0-ничего 1-смотреть переменную metka_pros_mat_s_ost
{
short           x=0,y=4;
int             K;
int             kom,kom1;
int             i,prc;
int             i1;
double          cena,krat,fas;
short		valuta; /*0-гривня 1-пересчитанная по курсу УЕ*/
class iceb_tu_str naigrup("");
class iceb_tu_str mom(""); /*Метка отмеченного материалла*/
class iceb_tu_str obr("");
long		pozz=0,pozz1=0;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str ei("");
class iceb_tu_str strih("");
short		mspz; /*метка соответствия первой записи*/
class iceb_tu_str kodmv(""); /*Код вновь введенного материалла*/
long		dlmas;
class iceb_tu_str datn("");
int		metkapros=0; //0-обычный режим просмотра 1-по артикулам и штрихкодам
short metka_sort=0; /*0-сортировка по наименованиям 1- по кодам*/
class iceb_tu_str vstr("");
class iceb_tu_str nomz("");
short dlinna_nazv=0;
short max_dlinna_nazv=0;
class iceb_t_mstr spis_menu(0);
class dirmatr_prov prov;
prov.regim=regim;
prov.sklpoi=skl;

if(metka_pros_mat == 1)
 {
  prov.most=prov.metka_mat_s_ost=metka_pros_mat_s_ost;
 }
pozz=pozz1=0;
dlmas=0;


//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);
class VVOD VV(0);
class VVOD MENU(3);

prov.kgrm.new_plus(grup);
valuta=0;
prov.naimpoi.new_plus(nai->ravno());
kom=0;

naz1:;
GDITE();
clear();
short           kls=LINES-1-6;

if(prov.regim != 0)
  kls=LINES-12;

K=0;
if(metka_sort == 0)
  sprintf(strsql,"select * from Material order by naimat asc");
else
  sprintf(strsql,"select * from Material order by kodm asc");
 
SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
lll:;
  shmt3(valuta,kursue,naigrup.ravno(),metkapros,kolstr,&prov,30);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == SFK9 || K == ENTER || K == SFK3)
     goto lll;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;
 }

if(prov.naimpoi.ravno()[0] == '\0' && kodmv.getdlinna() > 1 && kolstr > kls  && \
pozz < kolstr-kls)
 {
  pozz=poivcur(kodmv.ravno(),0,&cur);
  kodmv.new_plus("");
 }

naz:;
GDITE();
clear();

if(prov.regim != 0)
 {
  VVOD SOOBN(1);
  SOOBN.VVOD_SPISOK_add_MD(gettext("С помощью клавиш \"стрелка вверх\", \"стрелка вниз\" выберите"));
  SOOBN.VVOD_SPISOK_add_MD(gettext("товар, который вам нужен и нажмите клавишу \"Enter\""));
  soobsh(&SOOBN,LINES-6,0,1);
 }
 
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);


for(i=0; i < kls ;) /*Вычисляем максимальную длинну названия*/
 {
  
  if(cur.read_cursor(&row) == 0)
    break;


  if(dirmatr_prov(row,&prov) != 0)
    continue;

  if(iceb_tu_strlen(row[2]) > max_dlinna_nazv)
   max_dlinna_nazv=iceb_tu_strlen(row[2]);
  i++;
 }

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();

dlinna_nazv=COLS-52;
if(max_dlinna_nazv < dlinna_nazv)
 dlinna_nazv=max_dlinna_nazv;
if(dlinna_nazv < 30)
 dlinna_nazv=30;
for(i=0; i < kls ;)
 {
  
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;

  if(dirmatr_prov(row,&prov) != 0)
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
  memset(strsql,'\0',sizeof(strsql));
  if(prov.most != 0 || prov.metka_mat_s_ost == 1)
   {
    if(metkapros == 0)
     sprintf(strsql,"%-5s|%-2s|%-*.*s|%-*s|%8.15g|%4.5g|%4.4g|%.10g%s",
     row[0],row[1],
     iceb_tu_kolbait(dlinna_nazv,row[2]),iceb_tu_kolbait(dlinna_nazv,row[2]),row[2],
     iceb_tu_kolbait(3,row[4]),row[4],
     cena,krat,fas,prov.kolih,mom.ravno());
    if(metkapros == 1)
     sprintf(strsql,"%-5s|%-2s|%-*.*s|%-10s|%-10s|%.10g%s",
     row[0],row[1],
     iceb_tu_kolbait(dlinna_nazv,row[2]),iceb_tu_kolbait(dlinna_nazv,row[2]),row[2],
     row[3],
     row[14],prov.kolih,mom.ravno());
   }
  else        
   {
    if(metkapros == 0)
     sprintf(strsql,"%-5s|%-2s|%-*.*s|%-*s|%8.15g|%4.5g|%.4g%s",
     row[0],row[1],
     iceb_tu_kolbait(dlinna_nazv,row[2]),iceb_tu_kolbait(dlinna_nazv,row[2]),row[2],
     iceb_tu_kolbait(3,row[4]),row[4],
     cena,krat,fas,mom.ravno());
    if(metkapros == 1)
     sprintf(strsql,"%-5s|%-2s|%-*.*s|%-10s|%-10s|%s",
     row[0],row[1],
     iceb_tu_kolbait(dlinna_nazv,row[2]),iceb_tu_kolbait(dlinna_nazv,row[2]),row[2],
     row[3],row[14],mom.ravno());
   }

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
  if(kolstr > 0 && pozz > 0 )
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }
if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


shmt3(valuta,kursue,naigrup.ravno(),metkapros,kolstr,&prov,dlinna_nazv);

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

    if(dirmatr_prov(row,&prov) != 0)
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

    if(dirmatr_prov(row,&prov) != 0)
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

    if(dirmatr_prov(row,&prov) != 0)
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

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
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

  polen(str_vib.ravno(),strsql,sizeof(strsql),1,'|');

  i=poivcur(strsql,0,&cur);
  cur.poz_cursor(i);
  cur.read_cursor(&row);

  *kod=atoi(row[0]);

  *kgr=atoi(row[1]);
  nai->new_plus(row[2]);

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

     
  case NOL :   /*Для выбора без привязки к карточкам*/
     return(NOL);
  case ENTER: /*Выход*/
     if(mk == 0)
       goto sfk2;
     return(0);

  case PLU : /*Пометить материал по образцу*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     obr.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для отметки материалов"));

     if((kom1=vvod1(&obr,56,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       goto naz1;
     if(kom1 == ESC)
       goto naz1;

     otmmat(obr.ravno(),&dlmas,0,&cur);
     pozz=0;
     goto naz;

  case OTN : /*Снять отметку с материалла по образцу*/
     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     obr.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для снятия отметки с материалла"));

     if((kom1=vvod1(&obr,56,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
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
    if(prov.mpro == 2)
     prov.mpro=0; 
    else
     prov.mpro=2;

    goto naz;

  case DEL:  //Переключение режимов просмотра
    metkapros++;
    if(metkapros == 2)
     metkapros=0; 

    goto naz;


  case SFK2:   /*Корректировать запись*/


sfk2:;    
    kodmv.new_plus(*kod);
    dirmatr_vvod(1,&kodmv);
    
    goto naz1;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu4_3.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
     
  case FK2:  /*Ввести запись*/
    dirmatr_vvod(0,&kodmv);
    prov.naimpoi.new_plus("");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(provud("Material",row[0],1) != 0)
        goto naz;
        
      sprintf(strsql,"delete from Material where kodm = %s",row[0]);
/*      printw("\nstrsql=%s\n",strsql);*/
      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
       
      clear();
      kodmv.new_plus("");
      if(pozz == kolstr-1)
       pozz=0;
      goto naz1;
     }
    goto naz;

  case SFK3:       /*Удалить не используемые записи*/
   udnmat(&cur,kolstr);
   goto naz1;

  case FK4:       /*Отобрать по образцу*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     prov.naimpoi.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

     if(vvod1(&prov.naimpoi,40,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       prov.naimpoi.new_plus("");

     GDITE();

     clear();
     kom=pozz=0;
     if(kolstr == 0)
       goto lll;
     else
       goto naz;

  case SFK4:    //Включение/выключение просмотра материалов карточки которых есть на складе

    prov.metkasklada++;
    if(prov.metkasklada > 1)
     prov.metkasklada=0;

    if(skl == 0)
     {
      helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
      
      prov.naimpoi.new_plus("");
      
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код склада"));
      vstr.new_plus("");
      if(vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       {
        prov.metkasklada=0;
        goto naz1;
       }
      prov.sklpoi=vstr.ravno_atoi();
      GDITE();
     }

    pozz=0;
    goto naz1;

  case FK5:       /*Распечатать*/
        
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список материалов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать прайс лист"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список отмеченных материалов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ценники на отмеченные товары"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список материалов для загрузки в другую базу"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список материалов с датами последнего получения"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    switch (kom1)
     {
      case  6 :
      case -1 :
       goto naz;

      case  0 :
        dirmatr_rs(&cur,&prov);
        break;

      case  1 :
        praisl();
        break;

      case  2 :
        clear();
        printw(gettext("Список отмеченных материалов:\n"));
        rasmaskm(maskmat);
        break;

      case  3 :
        cennikm(maskmat);
        break;

      case 4:
        dirmatr_csv(&cur,&prov);
        break;

      case 5:
        dirmatr_rsdpp(kolstr,&cur,&prov);
        break;
     }

    goto naz;

  case SFK5:   /*переключение режима сортировки*/
     metka_sort++;
     if(metka_sort > 1)
      metka_sort=0;
     goto naz1;

  case FK6:       /*Стать на номер */

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    vstr.new_plus("");
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код материалла"));

    if((kom1=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(kom1 == ESC)
      goto naz;
    if(strsql[0] == '\0')
      goto naz;
      
    prov.naimpoi.new_plus("");
    pozz=0;
    kom=0;
    pozz=poivcur(vstr.ravno(),0,&cur);
    if(pozz == kolstr)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),vstr.ravno());
      iceb_t_soob(strsql);
      pozz=0;
     }
    clear();
    goto naz;

  case SFK6: //Заменить один код материалла на другой
    zamkm(*kod);
    clear();
    goto naz1;
        
  case FK7:  /*Включение/выключение режима просмотра с остатками*/

    prov.most=dirmatr_vmost();

    goto naz;

  case SFK7:  //Включение/выключение режима просмотра только материалов с остатками
    if(prov.most == 0)
     {
      iceb_t_soob(gettext("Не включен режим показа с остатками (F7)"));
      goto naz;
     }
    pozz=0;
    kom=0;
    prov.metka_mat_s_ost++;
    if(prov.metka_mat_s_ost > 1)
     prov.metka_mat_s_ost=0;
    goto naz;

  case FK8:       /*Просмотр карточек*/
    clear();
    nomz.new_plus("");
    vmat(0,"\0",prov.dt,prov.mt,prov.gt,0,"\0",0,*kod,0,1,"\0","\0",0,"",&nomz,0.);
    clear();
    goto naz;

  case SFK8:       /*Просмотр движения по материалу*/

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
           
    sprintf(strsql,"%d.%d.%d",prov.dt,prov.mt,prov.gt);
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
   
    dirmatr_f9k(&cur,&prov,&naigrup);
    pozz=0;
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
void shmt3(short valuta,double kursue,
const char *naigrup,int metkapros,int kolstr,class dirmatr_prov *prov,int dlinna_nazv)
{
attroff(A_REVERSE);  /*Выключить инверсию*/
attroff(A_UNDERLINE);  /*Выключить подчеркивание*/
move(0,0);
clrtoeol();  /*Очистить стороку*/
printw(gettext("Ввод и корректировка материалов и товаров"));
if(prov->sklpoi != 0)
 printw(" %s %d",gettext("Склад"),prov->sklpoi);
if(prov->mpro == 2)
 {
//  attron(A_BLINK);
  printw(gettext(" Только отмеченные материалы"));
//  attroff(A_BLINK);
 }
if(valuta == 1)
 {
//  attron(A_BLINK);
  printw(gettext(" Курс У.Е.=%.6g"),kursue);
//  attroff(A_BLINK);
 }

printw(" %d",kolstr);

clearstr(1,0);
if(prov->kgrm.getdlinna() > 1)
 {
//  attron(A_BLINK);
  printw(gettext("Поиск групп - %s %s"),prov->kgrm.ravno(),naigrup);
//  attroff(A_BLINK);
 }

if(prov->regim == 2)
 {
//  attron(A_BLINK);
  printw(gettext("Отобраны только весовые товары и материалы"));
//  attroff(A_BLINK);
 }

if(prov->most != 0)
 {
  if(prov->sklpoi != 0)
    printw(" %s:%d",gettext("Остатки по складу"),prov->sklpoi);
  if(prov->sklpoi == 0)
    printw(" %s",gettext("Остатки по всем складам"));
 }
if(prov->sklpoi != 0 && prov->metkasklada == 1)
 {
  printw(" %s:%d",gettext("Только материалы на складе"),prov->sklpoi);
 }

if(prov->metka_mat_s_ost == 1)
 {
 printw(" %s",gettext("Только с остатками !"));
 }
if(prov->naimpoi.ravno()[0] != '\0')
 {
  clearstr(2,0);
//  attron(A_BLINK);
  printw("%s:%s",gettext("Найдено по образцу"),prov->naimpoi.ravno());
//  attroff(A_BLINK);
 }
move(3,0);
if(prov->most == 0)
 {
  if(metkapros == 0)
//   printw(gettext("Код м. Группа    Наименование           Ед.из. Цена. Крат.Фас-ка"));
   printw(gettext("Код м. Группа    Наименование           %*sЕд.из. Цена. Крат.Фас-ка"),dlinna_nazv-28,"");
  if(metkapros == 1)
   printw(gettext("Код м. Группа    Наименование            %*sШтрих код  Артикул"),dlinna_nazv-28,"");
 }
else
 {
  if(metkapros == 0)
   printw(gettext("Код м. Группа    Наименование           %*sЕд.из. Цена. Крат.Фас-ка.Остаток"),dlinna_nazv-28,"");
  if(metkapros == 1)
   printw(gettext("Код м. Группа    Наименование           %*sШтрих код  Артикул  Остаток"),dlinna_nazv-28,"");
 }
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("код"),
"F7",gettext("остат."),
"F8",gettext("карт."),
"F9",gettext("меню"),
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
time_t		tmm;
struct  tm      *bf;
SQL_str         row;
SQLCURSOR       cur;
char		strsql[1025];

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

  sprintf(strsql,"select * from Material where kodm=%s",kodm.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
    {
     printw("%s %s !\n",gettext("Не нашли код материала"),kodm.ravno());
     OSTANOV();
     continue;
    }
   
  printw("%4s %s\n",row[0],row[0]);
  fprintf(ff,"%8s %5s %-*s %4d %8.8g %-4s %8.8g %8.8g\n",
  row[0],
  row[1],
  iceb_tu_kolbait(40,row[2]),
  row[2],
  atoi(row[7]),atof(row[6]),row[4],atof(row[7]),atof(row[8]));

 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------\n");

podpis(ff);


fclose(ff);

OSTANOV();
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка отмеченных материалов"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");

}

/*******************************/
/*Отметка материалов по образцу*/
/*******************************/
void otmmat(const char *obr,long *dlmas,
short mt, //0-отметить 1-снять отметку*
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
int dirmatr_prov(SQL_str row,class dirmatr_prov *prov)
{
int		i1;

if(proverka(prov->kgrm.ravno(),row[1],0,0) != 0)
 return(1);

if(prov->naimpoi.ravno()[0] != '\0')
 if(strstrm(row[2],prov->naimpoi.ravno()) == 0)
  if(strstrm(row[14],prov->naimpoi.ravno()) == 0)
   return(2);

if(prov->mpro == 2 && (maskmat != NULL && pole1(maskmat,row[0],',',0,&i1) != 0))
  return(3);

if(prov->regim == 2 && (SRAV("Кг",row[4],0) != 0 && 
SRAV("кг",row[4],0) != 0 && SRAV("КГ",row[4],0) != 0))
  return(4);


if(prov->sklpoi != 0 && prov->metkasklada == 1)
 {
  char strsql[512];
  sprintf(strsql,"select kodm from Kart where sklad=%d and kodm=%s limit 1",prov->sklpoi,row[0]);
  if(sql_readkey(&bd,strsql) <= 0)
    return(5);  
 }


if(prov->most == 1)
  prov->kolih=ostkar1(atoi(row[0]),prov->sklpoi,prov->dt,prov->mt,prov->gt);
if(prov->most == 2)
  prov->kolih=ostdok1(prov->dt,prov->mt,prov->gt,prov->sklpoi,atoi(row[0]));

if(prov->metka_mat_s_ost == 1 && prov->kolih <= 0.00000001) 
  return(1);
 
return(0);
}
/************************************************/
/*Проверка и удаление не используемых материалов*/
/************************************************/

void 	udnmat(SQLCURSOR *cur,long kolstr)
{
int		koludk;
SQL_str         row;
SQLCURSOR       cur1;
long		kolstr1;
char		strsql[512];

VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Удалить не используемые материалы ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
  return;


clear();
cur->poz_cursor(0);
koludk=kolstr1=0;
while(cur->read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  printw("%-5s %-*.*s",
  row[0],
  iceb_tu_kolbait(40,row[2]),
  iceb_tu_kolbait(40,row[2]),
  row[2]);
  
  if(provud("Material",row[0],0) != 0)
   {
    printw(" %s\n",gettext("используется"));
    continue;
   }
  koludk++;
  printw(" %s\n",gettext("удален"));

  sprintf(strsql,"delete from Material where kodm = %s",row[0]);
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
    {
     iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
     return;
    }
   else
     msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
   }
 }
printw("%s: %d\n",gettext("Количество удаленных материалов"),koludk);

class iceb_tu_str imatab("Material");
printw(gettext("Оптимизация таблицы %s !\n"),imatab.ravno());
GDITE();

int		metkazap=0;

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur1);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printw("VERSION:%s\n",row[0]);
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab.ravno());

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
 }
else
  sql_readkey(&bd,strsql);

printw("%s\n",gettext("Оптимизацию таблицы закончено"));

OSTANOV();

}
/*******************************/
/*Сщздание списка данных меню ввода материалла*/
/*********************************************/

void   sozvmat3(VVOD *VV)
{
VV->VVOD_SPISOK_add_data(20);//0
VV->VVOD_SPISOK_add_data(60);//1
VV->VVOD_SPISOK_add_data(100);//2
VV->VVOD_SPISOK_add_data(20);//3
VV->VVOD_SPISOK_add_data(30);//4
VV->VVOD_SPISOK_add_data(10);//5
VV->VVOD_SPISOK_add_data(10);//6
VV->VVOD_SPISOK_add_data(10);//7
VV->VVOD_SPISOK_add_data(24);//8
VV->VVOD_SPISOK_add_data(30);//9
VV->VVOD_SPISOK_add_data(20);//10
VV->VVOD_SPISOK_add_data(20);//11

}
/*************************************/
/*Замена одного кода материалла другим*/
/*************************************/

void zamkm(int kodm) //Код материалла который нужно заменить на другой и удалить после замены
{
char		strsql[1024];
int		kom1=0;
class iceb_tu_str kodmz("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD VVOD1(2);

sprintf(strsql,"%s:%d",gettext("Код материалла"),kodm);
VVOD1.VVOD_SPISOK_add_ZAG(strsql);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Введите код материалла на который его надо заменить"));
clearstr(LINES-1,0);

naz:;

if((kom1=vvod1(&kodmz,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom1 == ESC)
  return;

//Проверяем есть ли такой код в списке материалов
sprintf(strsql,"select kodm from Material where kodm=%s",kodmz.ravno());
if(sql_readkey(&bd,strsql) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),kodmz.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }
if(dirmatr_provkor(kodmz.ravno_atoi(),kodm) != 0)
 return;
 
VVOD PAROL(2);
PAROL.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
strsql[0]='\0';
vparol(&PAROL,strsql,10,stdscr,-1,-1,1);


if(atoi(strsql) != gt+mt+dt)
 {
  iceb_t_soob(gettext("Не верно введён пароль !"));
  goto naz;
 }

kormat(kodmz.ravno_atoi(),kodm);

sprintf(strsql,"%d",kodm);
if(provud("Material",strsql,1) != 0)
  return;
  
sprintf(strsql,"delete from Material where kodm = %d",kodm);
sql_zapis(strsql,1,0);

}
/**********************************************/
void dirmatr_rs_hap(int nom_str,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr+=4;
fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nom_str);
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("Код мат.|Груп.|  Н а и м е н о в а н и е               |К.т.|Цена р. |Е.из|Кратност|Фасовка \n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");

}
/***************************/
/*Распечатка списка материалов*/
/*****************************/
void dirmatr_rs(class SQLCURSOR *cur,class dirmatr_prov *prov)
{
struct  tm      *bf;
time_t          tmm;
time(&tmm);
bf=localtime(&tmm);
printw("%s\n",gettext("Готовим распечатку списка материалов"));
GDITE();
FILE *ff;
char imaf[56];

sprintf(imaf,"mat%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


fprintf(ff,"%s\n\n%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка списка товаров и материалов"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

dirmatr_rs_hap(1,NULL,ff);

SQL_str row;
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  if(dirmatr_prov(row,prov) != 0)
    continue;
  
  
  fprintf(ff,"%8s %5s %-*.*s %4d %8.8g %-*s %8.8g %8.8g\n",
  row[0],row[1],
  iceb_tu_kolbait(40,row[2]),
  iceb_tu_kolbait(40,row[2]),
  row[2],
  atoi(row[7]),atof(row[6]),
  iceb_tu_kolbait(4,row[4]),row[4],
  atof(row[7]),atof(row[8]));

  if(iceb_tu_strlen(row[2]) > 40)
   fprintf(ff,"%14s %s\n","",iceb_tu_adrsimv(40,row[2]));
   
    
 }
fprintf(ff,"ICEB_LST_END\n\
------------------------------------------------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка списка товаров и материалов"));

int orient=iceb_t_ustpeh(imaf,3);
iceb_t_rnl(imaf,orient,NULL,&dirmatr_rs_hap);

iceb_t_rabfil(&oth,"");
}
/************************************/
/*проверка возможности корректировки*/
/************************************/
int dirmatr_provkor(int nkod,int skod)
{
char strsql[512];

/*Проверяем есть ли изделие по старому коду и входит ли в него код на который хотим заменить*/
sprintf(strsql,"select kodi from Specif where kodi=%d and kodd=%d limit 1",skod,nkod);
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Specif\n%s\n%s",gettext("Таблица"),
  gettext("Коды взаимно связаны!"),
  gettext("Корректировка не возможна!"));
  iceb_t_soob(strsql);
  return(1);
 }
/*Проверяем есть ли издели по коду на который меняем и входит ли в него старый код*/
sprintf(strsql,"select kodi from Specif where kodi=%d and kodd=%d limit 1",nkod,skod);
if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Specif\n%s\n%s",gettext("Таблица"),
  gettext("Коды взаимно связаны!"),
  gettext("Корректировка не возможна!"));
  iceb_t_soob(strsql);
  return(1);
 }
return(0);
}
/******************************/
/*выбор с каким остатком показывать*/
/***********************************/
int dirmatr_vmost()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Реальный остаток на карточках"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Остаток по всем документам (в том числе и по неподтверждённым)"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

for(;;)
 {

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 2:
    case -1 :
      return(0);

    case 0 :
      return(1);

    case 1 :
      return(2);

   }
 }
return(0);

}
/*******/
/*Ввод */
/*******/
void dirmatr_vvod(int mk,//0- ввод нового 1 - корректировка
class iceb_tu_str *kodmv) //Код материалла
{
struct  tm      *bf;
int             K;
int             N;
class iceb_tu_str naimm("");
class iceb_tu_str naimmz(""); /*Запомненное наименование*/
class iceb_tu_str kodmz("");
class iceb_tu_str kodgg("");
char		bros[1024];
time_t		vrem;
char		strsql[1024];
SQL_str         row;
SQLCURSOR       cur;
int		poz,komv;
short		metkor; //0- не корректирваеть код материалла 1-да
int kodg=0;
int kt=0;
static int kod_mat_pred=1;
time_t vr=0;
float nds=0.; //0-цена с НДС 1-цена без НДС
class iceb_tu_str ei("");
N=1;
class VVOD VV(0);
class VVOD DANET(1);

VV.VVOD_SPISOK_add_data(20);//0
VV.VVOD_SPISOK_add_data(60);//1
VV.VVOD_SPISOK_add_data(100);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(30);//4
VV.VVOD_SPISOK_add_data(10);//5
VV.VVOD_SPISOK_add_data(10);//6
VV.VVOD_SPISOK_add_data(10);//7
VV.VVOD_SPISOK_add_data(24);//8
VV.VVOD_SPISOK_add_data(30);//9
VV.VVOD_SPISOK_add_data(20);//10
VV.VVOD_SPISOK_add_data(20);//11
VV.VVOD_SPISOK_add_data(32);//12

if(mk == 1) /*Корректировка*/
 {
  sprintf(strsql,"select * from Material where kodm=%d",kodmv->ravno_atoi());
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return;
   }
  VV.data_plus(0,kodmv->ravno());
  kodg=atoi(row[1]);
  kt=atoi(row[10]);  
  vr=atol(row[11]);
  nds=atoi(row[12]);
  ei.new_plus(row[4]);
  VV.data_plus(2,row[2]);
  VV.data_plus(3,row[6]);
  VV.data_plus(5,row[7]);
  VV.data_plus(6,row[9]);
  VV.data_plus(7,row[8]);
  VV.data_plus(8,row[3]);
  VV.data_plus(10,row[14]);
  VV.data_plus(11,row[5]);
  VV.data_plus(12,row[15]);
    
  sprintf(strsql,"select naik from Edizmer where kod='%s'",ei.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   VV.data_plus(4,row[0]);

  sprintf(strsql,"select naik from Grup where kod=%d",kodg);
  if(readkey(strsql,&row,&cur) == 1)
   VV.data_plus(1,row[0]);
 }
else
 {
  sprintf(strsql,"%d",nomkm(kod_mat_pred));
  VV.data_plus(0,strsql);
 }
kodmz.new_plus(VV.VVOD_SPISOK_return_data(0));
kodgg.new_plus(kodg);
naimmz.new_plus(VV.VVOD_SPISOK_return_data(2));


VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка товаров и материалов"));
if(kt != 0)
 {
  clearstr(1,0);
  VV.VVOD_SPISOK_add_ZAG(iceb_t_kszap(kt));
 }
if(vr != 0)
 {
  bf=localtime(&vr);
  sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


VV.VVOD_SPISOK_add_MD(gettext("Код материалла......."));//0
VV.VVOD_SPISOK_add_MD(gettext("Группа..............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Наименование........."));//2
VV.VVOD_SPISOK_add_MD(gettext("Цена реализации......"));//3
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения...."));//4
VV.VVOD_SPISOK_add_MD(gettext("Кратность............"));//5
VV.VVOD_SPISOK_add_MD(gettext("Код тары............."));//6
VV.VVOD_SPISOK_add_MD(gettext("Фасовка.............."));//7
VV.VVOD_SPISOK_add_MD(gettext("Штрих-код............"));//8
VV.VVOD_SPISOK_add_MD(gettext("Цена введена........."));//9
VV.VVOD_SPISOK_add_MD(gettext("Артикул.............."));//10
VV.VVOD_SPISOK_add_MD(gettext("Критический остаток.."));//11
VV.VVOD_SPISOK_add_MD(gettext("Код УКТ ВЭД.........."));//12  Украинская класификация товаров внешне экономической деятельности


naz:;
clear();

if(nds == 0.)
  VV.VVOD_SPISOK_zapis_data(9,gettext("C НДС"));
if(nds == 1.)
  VV.VVOD_SPISOK_zapis_data(9,gettext("Без НДС"));
  


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("найти код"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,1);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K ==  KEY_RESIZE)
    goto naz;

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 1)   /*Группа материалла*/
     {

      VV.VVOD_SPISOK_zapis_data(N,"");
      naimm.new_plus("");
      kodgg.new_plus("");
      poz=komv=0;
      if(dirmat1("Grup",&kodgg,&naimm,-1,-1,-1,2,&poz,&komv,"","\0") != 0)
       {
	kodgg.new_plus(kodg);
       }
      else
       {
        VV.VVOD_SPISOK_zapis_data(N,naimm.ravno());
       }
      N++;
      goto naz;
     }

    if(N == 4)  /*Единица измерения*/
     {
      VV.VVOD_SPISOK_zapis_data(N,"");
      attroff(VV.VVOD_return_cs(iceb_CFV));
      naimm.new_plus("");
      if(iceb_tl_ei(2,&ei,&naimm) != 0)
       ei.new_plus("");
      VV.VVOD_SPISOK_zapis_data(N,naimm.ravno());
      N++;
      goto naz;
     }

    if(N == 9)  /*Цена с НДС или нет*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      nds++;
      if(nds > 1)
       nds=0;
      goto naz;
     }

    VV.vgetstr(N,stdscr);


    if(N == 0)  /*Код материалла*/
     {

      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        if(SRAV(kodmz.ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
         {
          sprintf(strsql,"select naimat from Material where kodm=%s",
          VV.VVOD_SPISOK_return_data(N));
          if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
           {
            attroff(VV.VVOD_return_cs(iceb_CFV));
            VVOD SOOB(1);
            sprintf(strsql,gettext("Код материалла %s уже введён в список !"),VV.VVOD_SPISOK_return_data(N));
            SOOB.VVOD_SPISOK_add_MD(strsql);
            sprintf(strsql,gettext("Это: %s"),row[0]);
            SOOB.VVOD_SPISOK_add_MD(strsql);
            soobshw(&SOOB,stdscr,-1,-1,0,1);
            VV.data_plus(N,"");
            goto naz;
           }

          sprintf(strsql,"select naius from Uslugi where kodus=%s",
          VV.VVOD_SPISOK_return_data(N));
          if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
           {
            attroff(VV.VVOD_return_cs(iceb_CFV));
            VVOD SOOB(1);
            sprintf(strsql,gettext("Код %s уже имеет услуга !"),VV.VVOD_SPISOK_return_data(N));
            SOOB.VVOD_SPISOK_add_MD(strsql);
            sprintf(strsql,gettext("Это: %s"),row[0]);
            SOOB.VVOD_SPISOK_add_MD(strsql);
            SOOB.VVOD_SPISOK_add_MD(gettext("Нумерация материалов и услуг сквозная"));
            soobshw(&SOOB,stdscr,-1,-1,0,1);
            VV.data_plus(N,"");
            goto naz;
           }
         }
      }
     }

    if(N == 2)  /*Наименование материалла*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      /*Проверяем наименование*/
      if(SRAV(naimmz.ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        strcpy(bros,VV.VVOD_SPISOK_return_data(N));
        sqlfiltr(bros,sizeof(bros));
        sprintf(strsql,"select kodm from Material where naimat='%s'",
        bros);
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          sprintf(strsql,"%s:%s",gettext("Такое наименование уже введено. Код"),row[0]);
          iceb_t_soob(strsql);
          VV.data_plus(N,"");
          goto naz;
         }
        }
      naimm.new_plus(VV.VVOD_SPISOK_return_data(N));

     }

    if(N == 8)  /*Штрих код*/
     {
      sprintf(strsql,"select kodm,naimat from Material where strihkod='%s'",
      VV.VVOD_SPISOK_return_data(N));

      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
      if(sql_readkey(&bd,strsql,&row,&cur) == 1)
       {
        if(SRAV(VV.VVOD_SPISOK_return_data(0),row[0],0) != 0)
         {         
          attroff(VV.VVOD_return_cs(iceb_CFV));
          VVOD SOOB(1);
          sprintf(strsql,gettext("Прочитали штрих-код %s"),VV.VVOD_SPISOK_return_data(8));
          SOOB.VVOD_SPISOK_add_MD(strsql);
          sprintf(strsql,gettext("Этот штрих-код введён для материалла %s %s"),row[0],row[1]);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          VV.data_plus(N,"");
          goto naz;
         }
       }

     }
    if(N == 12)  /*Код Украинской класификации товаров внешне экономической деятельности*/
     {
      if(VV.data_ravno(N)[0] != '\0')
       if(iceb_tu_strlen(VV.data_ravno(N)) != 10)
        iceb_t_soob(gettext("Код должен имень 10 знаков!"));
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
    N=-1;
    break;
   }

  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    GDITE();
    iceb_t_pdoc("matu4_3_1.txt");
    clear();
    goto naz;
   }

   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || ATOFM(VV.VVOD_SPISOK_return_data(0)) == 0.)
      {
       iceb_t_soob(gettext("Не введён код материалла !"));
       goto naz;
      }
     if(ATOFM(VV.VVOD_SPISOK_return_data(0)) < 0)
      {
       iceb_t_soob(gettext("Код не может быть отрицательным !"));
       goto naz;
      }
     if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введена группа материалла !"));
       goto naz;
      }

     if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введено наименование материалла !"));
       goto naz;
      }

     if(VV.data_ravno(12)[0] != '\0')
      if(iceb_tu_strlen(VV.data_ravno(12)) != 10)
       {
        iceb_t_soob(gettext("Код УКТ ВЭД должен иметь 10 знаков!")); /*предупреждение*/
       }

     /*Проверяем штрих-код*/
     if(VV.VVOD_SPISOK_return_data(8)[0] != '\0')
      {
       sprintf(strsql,"select kodm,naimat from Material where strihkod='%s'",
       VV.VVOD_SPISOK_return_data(8));
       if(sql_readkey(&bd,strsql,&row,&cur) == 1)
        {
         if(SRAV(kodmz.ravno(),row[0],0) != 0)
          {         
           sprintf(strsql,gettext("Этот штрих-код введён для материалла %s %s"),row[0],row[1]);
           iceb_t_soob(strsql);
           goto naz;
          }
        }
      }

     /*Проверяем код тары*/
     if(VV.VVOD_SPISOK_return_data(6)[0] != '\0' && ATOFM(VV.VVOD_SPISOK_return_data(6)) != 0)
      {
       sprintf(strsql,"select kodm,naimat from Material where kodm=%s",
       VV.VVOD_SPISOK_return_data(6));
       if(sql_readkey(&bd,strsql,&row,&cur) != 1)
         {
          sprintf(strsql,gettext("Не найден код тары %s !"),VV.VVOD_SPISOK_return_data(6));
          iceb_t_soob(strsql);
          goto naz;
         }
       }

      if(mk == 0)
       {
        sprintf(strsql,"select naimat from Material where kodm=%s",
        VV.VVOD_SPISOK_return_data(0));
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          VVOD SOOB(1);
          sprintf(strsql,gettext("Код материалла %s уже введён в список !"),VV.VVOD_SPISOK_return_data(0));
          SOOB.VVOD_SPISOK_add_MD(strsql);
          sprintf(strsql,gettext("Это: %s"),row[0]);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          goto naz;
         }

        sprintf(strsql,"select naius from Uslugi where kodus=%s",
        VV.VVOD_SPISOK_return_data(0));
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          VVOD SOOB(1);
          sprintf(strsql,gettext("Код %s уже имеет услуга !"),VV.VVOD_SPISOK_return_data(0));
          SOOB.VVOD_SPISOK_add_MD(strsql);
          sprintf(strsql,gettext("Это: %s"),row[0]);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          SOOB.VVOD_SPISOK_add_MD(gettext("Нумерация материалов и услуг сквозная"));
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          goto naz;
         }
      }
     metkor=0;
     if(mk == 1)
      {
       if(SRAV(kodmz.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
        {
         if(dirmatr_provkor(VV.data_ravno_atoi(0),kodmz.ravno_atoi()) != 0)
          goto naz;

         DANET.VVOD_delete();
         DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код материалла ? Вы уверены ?"));

         if(danet(&DANET,2,stdscr) == 1)
           metkor=1;
         else
           goto naz;
        }
       if(SRAV(naimmz.ravno(),VV.data_ravno(2),0) != 0)
        {
         //проверяем разрешено ли оператору корректировать наименование
         class iceb_tu_str spisok_log;
         if(iceb_t_poldan("Разрешено корректировать наименование материалла",&spisok_log,"matnast.alx") == 0)
          {
           if(spisok_log.getdlinna() > 1)
            {
              if(proverka(spisok_log.ravno(),iceb_tu_getlogin(),0,0) != 0)
               {
                iceb_t_soob(gettext("Вам запрещена корректировка наименования материалла !"));
                goto naz;
               }
            }
          }
        }
      }
     time(&vrem);
     if(VV.VVOD_SPISOK_return_data(3)[0] == '\0')
       VV.data_plus(3,"");
     if(VV.VVOD_SPISOK_return_data(5)[0] == '\0')
       VV.data_plus(5,"");
     if(VV.VVOD_SPISOK_return_data(6)[0] == '\0')
       VV.data_plus(6,"");
     if(VV.VVOD_SPISOK_return_data(7)[0] == '\0')
       VV.data_plus(7,"");



     /*Новая запись*/
     if(mk == 0)       
      {
       kodmv->new_plus(VV.VVOD_SPISOK_return_data(0));
       sprintf(strsql,"insert into Material \
values (%d,%s,'%s','%s','%s',%.10g,%.10g,%.10g,%.10g,%d,%d,%ld,%.6g,%d,'%s','%s')",
       VV.data_ravno_atoi(0),
       kodgg.ravno(),
       VV.data_ravno_filtr(2),
       VV.VVOD_SPISOK_return_data(8),
       ei.ravno(),
       VV.data_ravno_atof(11), //Критический остаток
       VV.data_ravno_atof(3),
       VV.data_ravno_atof(5),
       VV.data_ravno_atof(7),
       VV.data_ravno_atoi(6),
       iceb_t_getuid(),vrem,nds,0,
       VV.VVOD_SPISOK_return_data(10),
       VV.data_ravno_filtr(12));

      }
     /*Корректировка записи*/
     if(mk == 1)       
      {
       sprintf(strsql,"update Material \
set \
kodm = %d,\
kodgr= %s,\
naimat='%s',\
strihkod='%s',\
ei='%s',\
kriost=%.10g,\
cenapr=%.10g,\
krat=%.10g,\
fasv=%.10g,\
kodt=%d,\
ktoz=%d,\
vrem=%ld,\
nds=%.2f, \
msp=%d,\
artikul='%s',\
ku='%s' \
where kodm = %s",
       VV.data_ravno_atoi(0),kodgg.ravno(),VV.data_ravno_filtr(2),
       VV.VVOD_SPISOK_return_data(8),
       ei.ravno(),
       ATOFM(VV.VVOD_SPISOK_return_data(11)),
       ATOFM(VV.VVOD_SPISOK_return_data(3)),
       ATOFM(VV.VVOD_SPISOK_return_data(5)),ATOFM(VV.VVOD_SPISOK_return_data(7)),
       VV.data_ravno_atoi(6),iceb_t_getuid(),vrem,nds,0,
       VV.VVOD_SPISOK_return_data(10),
       VV.data_ravno_filtr(12),
       kodmz.ravno());
      } 
/*
     printw("\n%s\n",strsql);
     refresh();

     OSTANOV(); 
*/

     if(sql_zap(&bd,strsql) != 0)
      {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
         goto naz;
        }
       else
        {
         msql_error(&bd,gettext("Ошибка записи !"),strsql);
         goto naz;
        }
      }

     if(mk == 0)
        kod_mat_pred=VV.data_ravno_atoi(0);

     if(metkor == 1)
         kormat(VV.data_ravno_atoi(0),kodmz.ravno_atoi());
      
     break;
    }

  if(K == FK4) /*Взять новый код*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.data_plus(0,nomkm(kod_mat_pred));
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
refresh();
}
/*********************************/
/*Выгрузка списка материалов в формате csv*/
/*******************************************/
void dirmatr_csv(class SQLCURSOR *cur,class dirmatr_prov *prov)
{
SQL_str row;
char imaf[64];

sprintf(imaf,"sm%d.csv",getpid());

class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return;


fprintf(fil.ff,"#%s\n#%s %s %s\n",
gettext("Экспорт списка материалов из материального учёта"),
gettext("Выгружено из базы"),
iceb_t_get_namebase(),
iceb_t_get_pnk("00",1));

time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

fprintf(fil.ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(fil.ff,"#Расшифровка полей записи:\n\
#1  Код материалла\n\
#2  Код группы материалла\n\
#3  Наименование материалла\n\
#4  Штрих-код\n\
#5  Единица измерения\n\
#6  Критический остаток\n\
#7  Цена реализации\n\
#8  Кратность\n\
#9  Фасовка\n\
#10 Код тары\n\
#11 1-цена введена без НДС 0-цена с НДС\n\
#12 Метка скоропортящегося материалла 1-скоропортяцийся 0-нет\n\
#13 артикул\n\
#14 код товара согласно УКТ ЗЕД\n");

cur->poz_cursor(0);


while(cur->read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr2,++kolstr1);
  if(dirmatr_prov(row,prov) != 0)
    continue;
/****
strcpy(strsql,"CREATE TABLE Material (\
kodm int not NULL primary key default 0,\
kodgr smallint not null default 0,\
naimat varchar(100) not null default '',\
strihkod varchar(50) not null default '',\
ei char(8) not null default '',\
kriost double not null default 0,\
cenapr double (15,6) not null default 0,\
krat double not null default 0,\
fasv double not null default 0,\
kodt int not null default 0,\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
nds  real not null default 0,\
msp smallint not null default 0,\
artikul varchar(20) not null default '',\
ku CHAR(32) NOT NULL DEFAULT '',\
unique (naimat),\
index (strihkod)) ENGINE = MYISAM");

0  kodm      Код материалла
1  kodgr     Код группы материалла
2  naimat    Наименование материалла
3  strihkod  Штрих-код
4  ei        Единица измерения
5  kriost    Критический остаток
6  cenapr    Цена реализации
7  krat      Кратность
8  fasv      Фасовка
9  kodt      Код тары
10 ktoz      Кто записал
11 vrem      Время записи
12 nds       1-цена введена без НДС 0-цена с НДС
13 msp       Метка скоропортящегося материалла
14 artikul   артикул
15 ku        код товара согласно УКТ ЗЕД
*/
  fprintf(fil.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[12],row[13],row[14],row[15]);
 }

fil.end();
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список материалов для загрузки в базу"));

iceb_t_rabfil(&oth,"");

}
/***************************************************/
/*Импорт материалов из файла*/
/********************************/
void dirmatr_imp()
{
char strsql[2048];
struct stat work;
int kom=0;
static class iceb_tu_str imafim("");

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт материалов из файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла с материалами"));

naz:;

clear();

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F10",gettext("выход"),NULL);

if((kom=vvod1(&imafim,COLS-6,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("matu4_3_2.txt");
    clear();
    goto naz;

  case ENTER:
    if(imafim.ravno()[0] == '\0')
     {
      dirkatl(&imafim);
      goto naz;
     }
    break;

  case FK3:
   GDITE();
   if(stat(imafim.ravno(),&work) != 0)
    {
     sprintf(strsql,"%s !",gettext("Нет такого файла"));
     iceb_t_soob(strsql);
     goto naz;
    }
    vizred(imafim.ravno());
    goto naz;

  default:
    goto naz;
 }

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }

clear();

char strok[2048];

class iceb_fopen filimp;
if(filimp.start(imafim.ravno(),"r") != 0)
 return;

char imaftmp[64];
sprintf(imaftmp,"imp%d.tmp",getpid());

class iceb_fopen filtmp;
if(filtmp.start(imaftmp,"w") != 0)
 return;

printw("%s\n\n",gettext("Загружаем записи из файла"));
int razmer=0;
int i=0;
#define KOLKOL 14
class iceb_tu_str rek[KOLKOL];
SQL_str row;
class SQLCURSOR cur;
int ktoi=iceb_t_getuid();
int kolihoh=0;
int nom=0;

while(fgets(strok,sizeof(strok),filimp.ff) != NULL)
 {
  if(strok[0] == '#')
   continue;
  
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каректки меняем на пробел
   strok[i-2]=' ';

  printw("%s",strok);
  strzag(LINES-1,0,work.st_size,razmer);

  for(nom=0; nom < KOLKOL; nom++)
   if(polen(strok,&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filtmp.ff,"%s",strok);
     fprintf(filtmp.ff,"#%s %d\n",gettext("Не найдено поле"),nom+1);
     kolihoh++;
     break;     
    }

  if(nom < KOLKOL)
   continue;

  /*Проверяем наименование материала*/
  sprintf(strsql,"select naimat from Material where naimat='%s'",rek[2].ravno_filtr());
  if(readkey(strsql,&row,&cur) != 0)
   {
    fprintf(filtmp.ff,"%s",strok);
    fprintf(filtmp.ff,"#%s %s:%s\n",gettext("Такое наименование уже есть"),gettext("Код материала"),row[0]);
    printw("%s %s:%s\n",gettext("Такое наименование уже есть"),gettext("Код материала"),row[0]);
    continue;
   }

  /*проверяем единицу измерения*/
  if(rek[4].getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Edizmer where kod='%s'",rek[4].ravno_filtr());
    if(readkey(strsql) != 1)
     rek[4].new_plus("");
   }

  /*проверяем код группы*/
  if(rek[1].getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Grup where kod=%d",rek[1].ravno_atoi());
    if(readkey(strsql) != 1)
     rek[1].new_plus("");
   }

  class lock_tables kkk("LOCK TABLES Material WRITE,Uslugi READ");
  /*проверяем код материала*/
  if(rek[0].ravno_atoi() > 0)
   {
    sprintf(strsql,"select naimat from Material where kodm='%d'",rek[0].ravno_atoi());
    if(readkey(strsql) >= 1)
     {
      rek[0].new_plus(nomkm());
     }
    else
     {
      sprintf(strsql,"select kodus from Uslugi where kodus=%d",rek[0].ravno_atoi());
      if(readkey(strsql) >= 1)
       {
        rek[0].new_plus(nomkm());
       }
     }
   }
  else
   rek[0].new_plus(nomkm());  



/****
strcpy(strsql,"CREATE TABLE Material (\
kodm int not NULL primary key default 0,\
kodgr smallint not null default 0,\
naimat varchar(100) not null default '',\
strihkod varchar(50) not null default '',\
ei char(8) not null default '',\
kriost double not null default 0,\
cenapr double (15,6) not null default 0,\
krat double not null default 0,\
fasv double not null default 0,\
kodt int not null default 0,\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
nds  real not null default 0,\
msp smallint not null default 0,\
artikul varchar(20) not null default '',\
ku CHAR(32) NOT NULL DEFAULT '',\
unique (naimat),\
index (strihkod)) ENGINE = MYISAM");

0  kodm      Код материалла
1  kodgr     Код группы материалла
2  naimat    Наименование материалла
3  strihkod  Штрих-код
4  ei        Единица измерения
5  kriost    Критический остаток
6  cenapr    Цена реализации
7  krat      Кратность
8  fasv      Фасовка
9  kodt      Код тары
10 ktoz      Кто записал
11 vrem      Время записи
12 nds       1-цена введена без НДС 0-цена с НДС
13 msp       Метка скоропортящегося материалла
14 artikul   артикул
15 ku        код товара согласно УКТ ЗЕД
*/
  sprintf(strsql,"insert into Material (kodm,kodgr,naimat,strihkod,ei,kriost,cenapr,krat,fasv,kodt,ktoz,vrem,nds,msp,artikul,ku) \
values (%d,%d,'%s','%s','%s',%.10g,%.10g,%.10g,%.10g,%d,%d,%ld,%d,%d,'%s','%s')",
  rek[0].ravno_atoi(),
  rek[1].ravno_atoi(),
  rek[2].ravno_filtr(),
  rek[3].ravno_filtr(),
  rek[4].ravno_filtr(),
  rek[5].ravno_atof(),
  rek[6].ravno_atof(),
  rek[7].ravno_atof(),
  rek[8].ravno_atof(),
  rek[9].ravno_atoi(),
  ktoi,
  time(NULL),
  rek[10].ravno_atoi(),
  rek[11].ravno_atoi(),
  rek[12].ravno_filtr(),
  rek[13].ravno_filtr());
  
  if(sql_zapis(strsql,1,0) != 0)
   {
    fprintf(filtmp.ff,"%s",strok);
    fprintf(filtmp.ff,"#%s\n",gettext("Ошибка записи в базу"));
    kolihoh++;
   }  
      
 }

filtmp.end();
filimp.end();

unlink(imafim.ravno());

rename(imaftmp,imafim.ravno());

if(kolihoh == 0)
 iceb_t_soob(gettext("Все записи загружены"));
else
 {
  sprintf(strsql,"%s %d",gettext("Количество не загруженых записей"),kolihoh);
  iceb_t_soob(strsql);
 }
}



/***************************************/
/*меню*/
/****************************************/
void dirmatr_f9k(class SQLCURSOR *curz,class dirmatr_prov *prov,class iceb_tu_str *naigrup)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int		kom=0,kom1=0;
int i=0;
double krat=0.;

class VVOD VV(0);
class VVOD VVOD1(2);
class VVOD MENU(3);
class VVOD DANET(1);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Поиск материалов по группе"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Переценка цен на заданный процент"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Загрузка списка материалов из файла"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

naz:;


while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 3 :
  case -1 :
    return;

  case 0 :
    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
    prov->kgrm.new_plus("");
    naigrup->new_plus("");
     
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование группы материалла"));

    if((i=vvod1(&prov->kgrm,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
     {
      return;
     }
    if(i == FK10)
     {
      return;
     }

    if(prov->kgrm.getdlinna() <= 1 || isdigit(prov->kgrm.ravno()[0]) == 0)
     {
      clear();
      prov->kgrm.new_plus("");
      int poz=0,komv=0;
      if(dirmat1("Grup",&prov->kgrm,naigrup,-1,-1,-1,1,&poz,&komv,"","\0") != 0)
       {
        return;
       }
     }
    if(prov->kgrm.ravno_atoi() > 0)
     {
      /*Читаем код группы материалла*/
      
      sprintf(strsql,"select * from Grup where kod=%s",prov->kgrm.ravno());

      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        prov->kgrm.new_plus("");
        naigrup->new_plus("");

        sprintf(strsql,gettext("Нет группы материалла %s !"),prov->kgrm.ravno());
        iceb_t_soob(strsql);
        return;
       }
          
      naigrup->new_plus(row[1]);
     }
    else 
      naigrup->new_plus("");
   return;


  case 1:

    VV.VVOD_SPISOK_add_ZAG(gettext("Переоценка всех цен на заданный процент"));

    VV.VVOD_SPISOK_add_MD(gettext("Процент.(+/-).."));
    VV.VVOD_SPISOK_add_MD(gettext("Группа (,,)...."));
    VV.VVOD_SPISOK_add_MD(gettext("Материал (,,).."));

    VV.VVOD_SPISOK_add_data(10);
    VV.VVOD_SPISOK_add_data(55);
    VV.VVOD_SPISOK_add_data(55);

sfk9:;
    
    helstr(LINES-1,0,"F2/+",gettext("переценить"),
    "F10",gettext("выход"),NULL);
    
    kom1=VV.vvod(0,1,1,-1,-1);
    switch(kom1)
     {
      case FK10:
      case ESC:
        goto naz;
      case FK2:
      case PLU:
        break;
      default:
       goto sfk9;
     }

    DANET.VVOD_delete();
    sprintf(strsql,gettext("Переценить на %s процентов ? Вы уверены ?"),VV.VVOD_SPISOK_return_data(0));
    DANET.VVOD_SPISOK_add_MD(strsql);

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;
         
    krat=ATOFM(VV.VVOD_SPISOK_return_data(0));

    curz->poz_cursor(0);
    while(curz->read_cursor(&row) != 0)
     {
      if(proverka(VV.VVOD_SPISOK_return_data(1),row[1],0,0) != 0)
       continue;
      if(proverka(VV.VVOD_SPISOK_return_data(2),row[0],0,0) != 0)
       continue;

      double cena=atof(row[6]);
      if(cena == 0.)
        continue;

      if(krat >= 0.)        
       cena=cena+cena*krat/100;
      else
       cena=cena+cena*krat/(100-krat);
       
      cena=okrug(cena,0.01);
               
      sprintf(strsql,"update Material \
set \
cenapr=%.2f,\
ktoz=%d,\
vrem=%ld \
where kodm=%s",cena,iceb_t_getuid(),time(NULL),row[0]);

      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
         return;
        }
       else
        {
         msql_error(&bd,gettext("Ошибка записи !"),strsql);
        }
       }
     }

    return;

  case 2:
    dirmatr_imp();
    break;
          
  default:
    kom=0;
    goto naz;
 }
}
/*****************************************************/
void dirmatr_rsdpp_hap(int nom_str,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%100s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код мат.|       Н а и м е н о в а н и е          |   Цена   |Един. изм.|Количество|Склад|Ном.карт.|Дата полу.|Контагент\n"));
/*
12345678 1234567890123456789012345678901234567890 1234567890 1234567890 1234567890 12345 123456789 1234567890
*/
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
}
/****************************************************/
/*Распечатка с датами последнего получения материала*/
/****************************************************/
void dirmatr_rsdpp(int kolstr,class SQLCURSOR *cur,class dirmatr_prov *prov)
{
char imaf[64];
class iceb_fopen fr;
SQL_str row;
class mugetdpp_str rk;
sprintf(imaf,"matr%d.lst",getpid());

GDITE();

if(fr.start(imaf,"w") != 0)
 return;

iceb_t_zagolov(gettext("Список материалов с датами последнего получения"),fr.ff);
 
dirmatr_rsdpp_hap(1,NULL,fr.ff);
int kolstr1=0;
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(dirmatr_prov(row,prov) != 0)
    continue;
  
  if(mugetdpp(atoi(row[0]),&rk,"","") != 0)
   continue;
  
  for(int nom=0; nom < rk.sklads.kolih(); nom++)
   {
    fprintf(fr.ff,"%8s %-*.*s %10.10g %-*s %10.10g %-5d %-9d %s %s %s\n",
    row[0],
    iceb_tu_kolbait(40,row[2]),
    iceb_tu_kolbait(40,row[2]),
    row[2],
    rk.cenas.ravno(nom),
    iceb_tu_kolbait(10,rk.eis.ravno(nom)),
    rk.eis.ravno(nom),
    rk.kolihs.ravno(nom),
    rk.sklads.ravno(nom),
    rk.nomkarts.ravno(nom),
    rk.dataps.ravno(nom),
    rk.kodkontrs.ravno(nom),
    iceb_t_get_pnk(rk.kodkontrs.ravno(nom),0));
    
    if(iceb_tu_strlen(row[2]) > 40)
     fprintf(fr.ff,"%8s %s\n","",iceb_tu_adrsimv(40,row[2]));
   }   
    
 }
fprintf(fr.ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

podpis(fr.ff);
fr.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список материалов с датами последнего получения"));

kolstr1=iceb_t_ustpeh(imaf,3);
iceb_t_rnl(imaf,kolstr1,NULL,&dirmatr_rsdpp_hap);
iceb_t_rabfil(&oth,"");
}
