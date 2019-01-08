/* $Id: rozprod.c,v 5.62 2013/11/05 10:50:49 sasa Exp $ */
/*15.07.2015	01.02.1999	Белых А.И.	rozprod.c
Розничная продажа
*/
#include <ctype.h>
#ifdef HAVE_TORGDEV
#include	<libdev.h>
#endif
#include	"buhg.h"

void vvodm(short dp,short mp,short gp,short kassa,int nomp,int skl,char shtrih[],const char *grup,int regim);
void            saproz(short,short,short,short,int,double,double,int,double);
void		tovshek(SQLCURSOR*);

//extern char     *scale_port;
extern short	konost;   /*Контроль остатка 0-включен 1-выключен*/

void		rozprod(short dp,short mp,short gp, //Дата продажи
short kassa, //Номер рабочего места
int nomp, //Номер продажи
int skl, //Склад
const char *grup, //Группы для склада
int regim, //режим работы подсказок
KASSA kasr) //кассовый регистратор
{
/************************
#ifdef HAVE_TORGDEV
check_record    *shek;
#else
char 		*shek;
#endif
**********************/
class iceb_tu_str vstr("");
SQL_str         row,row1;
int		i,prc;
char		strsql[1024];
double		osm; /*общая сумма продаж*/
double		stp; /*Сумма текущей продажи*/
double		summa=0.; /*Сумма одной записи*/
int		kom;
int		K;
short		kls;
short		x=0,y=5;
long		pozz=0;
long		kolstr,kolpr=0;
double		kolih,cena;
double		suma=0.;
double		bb, b1, b2;
class iceb_tu_str bros("");
int		ktoi;
time_t		vrem;
char		shtrih[56];
class iceb_t_mstr spis_menu(0);
int		vidnal=0;
class iceb_tu_str naim("");
int             vozvr=0;
double		sum_nal=0.; //наличные
double          sum_tara=0.; //тара
double          sum_pchek=0.; //платежный чек
double          sum_cred=0.;  //кредит
int     	nomerpp=0;  //Номер последней продажи
int		np=0;
double          sum_pol=0.; //полученная сумма денег от покупателя в оплату покупки
double		sum_sd=0.;  //сумма сдачи
//int		Model=0;
short		metkapr=0;
SQLCURSOR curr;
VVOD SOOBN(1);
VVOD DANET(1);
VVOD VVOD1(2);

kom=np=nomerpp=0;
  
memset(shtrih,'\0',sizeof(shtrih));

naz2:;
sum_pol=0.;

osm=stp=0.;

clear();
/*Определяем сумму проданого и номер следующей продажи*/

sprintf(strsql,"select nomer,kolih,cena  from Roznica \
where datrp='%04d-%02d-%02d' and kassa=%d order by nomer asc",
gp,mp,dp,kassa);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }  
kolih=cena=0;
i=0;
while(cur.read_cursor(&row) != 0)
 {

  i=atoi(row[0]);
  if(i == np)
   break;    

  i++;
  b1=atof(row[1]);  
  b2=atof(row[2]);  
  bb=b1*b2;  
  bb=okrug(bb,0.01);
  osm+=bb;  
 }


if(nomp == 0)
 np=i;
else
 np=nomp;
pozz=0;   
if(np == 0)
  np=1;

naz1:;

clear();
GDITE();

kls=LINES-8;
if(regim == 1)
  kls=LINES-17;
if(regim == 2)
  kls=LINES-16;
if(regim == 3)
  kls=LINES-15;

K=0;

move(0,0);

sprintf(strsql,"select * from Roznica \
where datrp='%04d-%02d-%02d' and kassa=%d and nomer=%d",
gp,mp,dp,kassa,np);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }  

stp=0.;
if(kolstr == 0)
 {

lll:;

//  readscan(NULL);
  saproz(dp,mp,gp,kassa,np,osm,stp,skl,sum_pol);
  
  SOOBN.VVOD_delete();

  switch(regim)
   {
    case 0:
      SOOBN.VVOD_SPISOK_add_MD(gettext("       Теперь вы можете выполнить продажу товаров."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("Вы можете выбрать товар с помощю сканера, или нажать F2 для"));
      SOOBN.VVOD_SPISOK_add_MD(gettext("            визуального выбора нужного товара."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("                      Желаем успеха."));
      break;

    case 1:
      SOOBN.VVOD_SPISOK_add_MD(gettext("             Возьмите в правую (левую) руку сканер."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("Совместите зону ввода сканера с етикеткой, на которой нанесен штрих-код."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("          Указательным пальцем нажмите круглу кнопку."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("Если на екране ничего не изменилось, это означает, что у Вас не получилось."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("           Попробуйте еще раз, более внимательно..."));
      break;

    case 2:
      SOOBN.VVOD_SPISOK_add_MD(gettext("              Поставте товар на весы."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("            Внимание! Вес не больше 15 кг."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("Для выбора товара нажмите клавишу F2 на клавиатуре."));
      break;

    case 3:
      SOOBN.VVOD_SPISOK_add_MD(gettext("В этом случае сканер и електронные весы не используются."));
      SOOBN.VVOD_SPISOK_add_MD(gettext("Выбор и ввод данных осуществляется с помощю клавиатуры."));
      SOOBN.VVOD_SPISOK_add_MD(gettext(" Для выбора товара нажмите клавишу F2 на клавиатуре."));
      break;

    default:
      SOOBN.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
      break;
   }
  soobsh(&SOOBN,-1,-1,1);

  K=getch();

/******************
  if(readscan(shtrih) <= 0) 
   {
    K=getch();
   }
  else
    K=FK2;
****************/

  if(K == FK3 || K == FK5 || K == FK4 || K == SFK5 || K == ENTER)
      goto naz1;
  
  goto vper;

 }  
else
 {
  /*Определяем сумму текущей продажи*/
  while(cur.read_cursor(&row) != 0)
   {
   
    b1 = atof(row[6]);
    b2 = atof(row[7]);
    bb=b1*b2;  
/*    printw("rows: %f * %f = %f\n",atof(row[8]),atof(row[9]),atof(row[8])*atof(row[9]) );
    printw("%s %s %f\n",row[8],row[9],bb);
*/
    bb=okrug(bb,0.01);
/*    printw("**%f %f\n",bb,stp);*/
    stp+=bb;  
/*    printw("***%f\n",stp);*/

   }
  
 }

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;


naz:;

GDITE();

if(regim == 1)
 {
  SOOBN.VVOD_delete();
  SOOBN.VVOD_SPISOK_add_MD(gettext("У Вас получилось! На экране Вы видите наименование товаров, "));
  SOOBN.VVOD_SPISOK_add_MD(gettext("количество, цену и сумму. Дальше можно:"));
  SOOBN.VVOD_SPISOK_add_MD(gettext("1.Ввести другой, прочитав данные с другой етикетки."));
  SOOBN.VVOD_SPISOK_add_MD(gettext("2.Увеличить количество товара, повторив чтение данных с той же етикетки,"));
  SOOBN.VVOD_SPISOK_add_MD(gettext("  или нажав клавишу F4 ввести количество с клавиатуры."));
  SOOBN.VVOD_SPISOK_add_MD(gettext("3.Распечатать кассовый чек, нажав клавишу F5."));
  SOOBN.VVOD_SPISOK_add_MD(gettext("4.Закончить работу, нажав клавишу  F10."));
  soobsh(&SOOBN,LINES-10,-1,0);
 }
if(regim == 2)
 {
  SOOBN.VVOD_delete();
  SOOBN.VVOD_SPISOK_add_MD(gettext("У Вас получилось! На экране Вы видите наименование товаров, "));
  SOOBN.VVOD_SPISOK_add_MD(gettext("количество, цену и сумму. Дальше можно:"));
  SOOBN.VVOD_SPISOK_add_MD(gettext("1.Взвесить другой товар. ( Установить товар на весы, нажать клавишу F2,"));
  SOOBN.VVOD_SPISOK_add_MD(gettext("  выбрать наименование товара и нажать клавишу \"Enter\")"));
  SOOBN.VVOD_SPISOK_add_MD(gettext("2.Распечатать касовый чек, нажав клавишу F5."));
  SOOBN.VVOD_SPISOK_add_MD(gettext("3.Закончить работу, нажав клавишу F10."));

  soobsh(&SOOBN,LINES-9,-1,0);
 }

if(regim == 3)
 {
  SOOBN.VVOD_delete();
  SOOBN.VVOD_SPISOK_add_MD(gettext("Дальше можете:"));
  SOOBN.VVOD_SPISOK_add_MD(gettext("1.Выбрать другой товар, нажав клавишу F2."));
  SOOBN.VVOD_SPISOK_add_MD(gettext("2.Изменить количество введеного товара, нажав клавишу F4."));
  SOOBN.VVOD_SPISOK_add_MD(gettext("3.Распечатать касовый чек, нажав клавишу F5."));
  SOOBN.VVOD_SPISOK_add_MD(gettext("4.Закончить работу, нажав клавишу F10."));
  soobsh(&SOOBN,LINES-8,-1,0);
 }

/*
printw("**pozz=%d\n",pozz);
OSTANOV();
*/
cur.poz_cursor(pozz);
kolpr=pozz;

spis_menu.spis_str.free_class();
for(i=0; i<=kls-1;)
 {

  if(cur.read_cursor(&row) == 0)
    break;

  kolpr++;  
  
  /*Читаем наименование материалла*/
   sprintf(strsql,"select * from Material where kodm=%s",row[4]);
   readkey(strsql,&row1,&curr);
/*  
  printw("\nrow1[0]=%s row1[1]=%s row1[2]=%s\n",
  row1[0],row1[1],row1[2]);
  OSTANOV();
*/  
  summa=atof(row[6])*atof(row[7]);  
  if(metkapr == 0)
   sprintf(strsql,"%4s|%-30.30s|%-3s|%8.8g|%8s|%8.2f",
   row[4],row1[2],row[5],atof(row[6]),row[7],summa);
  else
   sprintf(strsql,"%4s|%-30.*s",
   row[4],COLS-10,row1[2]);

  spis_menu.spis_str.plus(strsql);
  i++;
    
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


saproz(dp,mp,gp,kassa,np,osm,stp,skl,sum_pol);
prc=kolpr*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -6) /*Сканерный ввод*/
  K=FK2;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i == kls)
    pozz++;
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  if(pozz != 0)
    pozz--;
  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i == kls)
    pozz+=kls;
  else
    beep();
/*
  printw("\n вперед pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  pozz-=kls;
  if(pozz < 0)  
   {
    beep();
    pozz=0;
   }
/*
  printw("\n назад pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;
 }

if(kom >= 0)
 {
  cur.poz_cursor(pozz+kom);
  cur.read_cursor(&row);
 }

vper:;

switch (K)
 {
  case DEL: //Переключение режима просмотра
    metkapr++;
    if(metkapr == 2)
     metkapr=0;
    goto naz1;

  case FK10: /*Выход*/
  case ESC : /*Выход*/
    if(kolstr > 0)
     {
      i=nomerpp;
      if(i == 0)
       i=np;      
      
      sprintf(strsql,"delete from Roznica \
where datrp='%d-%02d-%02d' and kassa=%d and nomer=%d",
      gp,mp,dp,kassa,i);
      
      if(sql_zapis(strsql,0,0) != 0)
       goto naz1;
     }
    break;


  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("rozn1.txt");
   goto naz1;

  case FK2: /*Ввод*/
    
    if(np < nomerpp)
     {
      iceb_t_soob(gettext("Ввод невозможен, так как уже распечатан чек !"));
      goto naz;
     }
    vvodm(dp,mp,gp,kassa,np,skl,shtrih,grup,regim);
    memset(shtrih,'\0',sizeof(shtrih));
    
    goto naz1;

  case FK3: /*Удаление записи*/

    if(np < nomerpp)
     {
      iceb_t_soob(gettext("Удаление невозможно, так как уже распечатан чек !"));
      goto naz;
     }

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();
      sprintf(strsql,"delete from Roznica \
where datrp='%d-%02d-%02d' and kassa=%d and nomer=%d \
and metka=%s and kodm=%s",
gp,mp,dp,kassa,np,row[3],row[4]);

      sql_zapis(strsql,0,0);
     }

     goto naz1;

  case ENTER: 
  case FK4: /*Корректировать количество*/

    if(np < nomerpp)
     {
      iceb_t_soob(gettext("Корректировка невозможна, так как уже распечатан чек !"));
      goto naz;
     }

    helstr(LINES-1,0,"F10",gettext("выход"),NULL);
    

    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));

    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите количество"));
    vstr.new_plus("");
    if((i=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(i == ESC)
      goto naz;

    GDITE();

    ktoi=iceb_t_getuid();
    time(&vrem);
    kolih=vstr.ravno_atof();
    if(kolih <= 0.)
     goto naz1;
      
    sprintf(strsql,"update Roznica \
set \
kolih = %.10g,\
ktoi  = %d,\
vrem  = %ld \
where datrp='%d-%02d-%02d' and kassa=%d and nomer=%d \
and metka=%s and kodm=%s",
kolih,ktoi,vrem,
gp,mp,dp,kassa,np,row[3],row[4]);

/*    
    printw("\nstrsql=%s\n",strsql);
    OSTANOV();
*/    
    sql_zapis(strsql,0,0);

    goto naz1;

  case FK5: /*Выбить чек перейти на следующюю продажу*/

    DANET.VVOD_delete();

    DANET.VVOD_SPISOK_add_MD(gettext("Распечатать чек ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

    if(np < nomerpp)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Чек уже распечатан !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }

    SOOBN.VVOD_delete();
#ifdef HAVE_TORGDEV
    Model = iceb_model(kasr.handle);
    if (Model & M_MARIA) 
       SOOBN.VVOD_SPISOK_add_MD(gettext("Печатаем чек на кассовом регистраторе."));
    else if (Model & M_AMC100F) 
       SOOBN.VVOD_SPISOK_add_MD(gettext("Нажмите клавиши ФЦ-ВВ для печати чека или СБ для отмены."));
    soobsh(&SOOBN,-1,-1,0);
#endif

    GDITE();
    
    stp=0.;
    kom=0;

    cur.poz_cursor(0);
    summa=0.;
//    shek=NULL;
    while(cur.read_cursor(&row) != 0)
     {
      vidnal=1;
     /*Читаем наименование материалла*/
      sprintf(strsql,"select naimat,nds from Material where kodm=%s",
      row[4]);
      if(readkey(strsql,&row1,&curr) == 1)
       {
        naim.new_plus(row[4]);
        naim.plus(" ",row1[0]);
        if(row1[1][0] == '0')
          vidnal=1;
        if(row1[1][1] == '1')
          vidnal=2;
       }
      else
        naim.new_plus("");
        
      b1=atof(row[7]);
      cena=okrug(b1,0.01);

      b2=atof(row[6]);
      kolih=okrug(b2,0.001);

      bb=b1*b2;
      suma=okrug(bb,0.01);

      summa+=bb;

      sprintf(strsql,"FSTART: ADD_CHECK\n\
%s\n\
%.2f\n\
%.2f\n\
%.6g\n\
%d\n\
FEND:\n",
     naim.ravno(),suma,cena,kolih,vidnal);  
     if (iceb_cmd(kasr, strsql)) 
       goto naz1;
    }
    sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
    kasr.prodavec,kasr.parol,1,gettext("К оплате"));  
    iceb_cmd(kasr, strsql);

    sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%.2f\n\
FEND:\n",
    kasr.prodavec,kasr.parol,2,summa);  

    iceb_cmd(kasr, strsql);

    vozvr=0;
//    if(tipz == 1)
//     vozvr=1;
    /*************************
    sum_tara - Тара           |
    sum_pchek- Платежный чек  |=> Безналичная форма оплаты
    sum_cred - Кредит         |
    --------------------
    sum_nal  - Наличные

    Если сумма безналичной оплаты поркрывает общий результат операций по 
    чеку, то все движение средств представляет собой безналичную оплату.
    В противном случае недостающая до покрытия часть средств считается
    наличной.
    *************************/

    sum_nal=summa;
    sum_tara=sum_pchek=sum_cred=0.;
    sprintf(strsql,"FSTART: MARIA_PRCHECK\n\
%s\n\
%s\n\
%d\n\
%d\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
FEND:\n",
    kasr.prodavec,kasr.parol,skl,vozvr,summa,sum_tara,sum_pchek,sum_cred,sum_nal);


    if(iceb_cmd(kasr, strsql)) 
       goto naz1;

    np++;
    nomerpp=np;
    goto naz2;

  case SFK5: //Распечатать на принтере товарный чек
    tovshek(&cur);
    goto naz;
         
  case FK6: /*Назад на предыдущюю продажу*/
    stp=0.;
    kom=0;
    if(nomerpp == 0)
      nomerpp=np;
    np--;
    if(np < 1)
     np=1;
     
    goto naz2;

  case SFK6: /*вперед на следующюю продажу*/
    stp=0.;
    kom=0;

    if(nomerpp == 0)
      nomerpp=np;

    if(nomerpp == np) //Если мы на последней продаже то вперед уже нельзя
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Это последняя продажа !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      if(kolstr == 0)
       goto lll;
       
      goto naz;
     }           

    np++;

    goto naz2;

  case FK7: //Вывод на экран суммы

    GDITE();

    sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
      kasr.prodavec,kasr.parol,1,gettext("К оплате"));  
    iceb_cmd(kasr, strsql);

    sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%.2f\n\
FEND:\n",
      kasr.prodavec,kasr.parol,2,summa);  
    iceb_cmd(kasr, strsql);

    bigsuma(stp,gettext("К оплате"));
    goto naz1;

  case FK8: //Ввод полученной суммы денег для определения сдачи

    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_ZAG(gettext("Вычисление суммы сдачи."));

    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите полученную сумму."));
    vstr.new_plus("");
    if((i=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz1;
    if(i == ESC)
      goto naz1;

    sum_pol=vstr.ravno_atoi();
    sum_pol=okrug(sum_pol,0.01);
    sum_sd=sum_pol-stp;
    sum_sd=okrug(sum_sd,0.01);
    if(sum_sd > 0.)
      bros.new_plus(gettext("Сумма сдачи"));
    else
      bros.new_plus(gettext("Не хватает"));
    bigsuma(sum_sd,bros.ravno());

    sprintf(strsql,"FSTART: AMC_SETCASH\n\
%.2f\n\
FEND:\n",sum_pol);
    
    iceb_cmd(kasr, strsql);

    goto naz1;

  case SFK10: //Для отладки программы
    np++;
    goto naz2;
  
  default:
   goto naz1;    
 }


}

/*****************/
/*Ввод материалла*/
/******************/

void vvodm(short dp,short mp,short gp, //Дата
short kassa, //Номер рабочего места
int nomp, //Номер продажи
int skl,  //Склад
char shtrih[], //штрих-код
const char *grup,
int regim) //Режим вывода подсказок
{
char		strsql[1024];
class iceb_tu_str bros("");
class iceb_tu_str bros1("");
int		i;
SQL_str         row,row1;
double		kolih;
class iceb_tu_str naim("");
int             kgrm,kodm;
int		ktoi;
time_t		vrem;
class iceb_tu_str ei(""),cenas("");
class iceb_tu_str ndss("");
double		bb;
double		ostatok=0.;
class iceb_tu_str kolihch("");
short           metkaiz=0; //0-материал 1-изделие
class iceb_tu_str vstr("");
SQLCURSOR curr;
if(shtrih[0] == '\0')
 {
  helstr(LINES-1,0,"F10",gettext("выход"),NULL);


  VVOD VVOD1(2);
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование товара"));

  if((i=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(i == ESC)
    return;

  kodm=vstr.ravno_atoi();

  if(vstr.ravno()[0] == '\0' || isdigit(vstr.ravno()[0]) == 0)
   {
    clear();
    
    if(polen(vstr.ravno(),&bros1,2,'+') != 0)
      bros1.new_plus(vstr.ravno());
    kodm=0;
    if(isdigit(bros1.ravno()[0]) != 0)
       kodm=bros1.ravno_atoi();
    naim.new_plus(bros1.ravno());
    if(kodm != 0)
      naim.new_plus(kodm);
    
    if((i=dirmatr1(&kgrm,&kodm,&naim,skl,grup,regim,1,0)) != 0)
     {
      clear();
      return;
     }
/*
   printw("kodm=%d\n",kodm);
   OSTANOV();
*/
   }
 }
else
 {
  bros.new_plus(shtrih);
  i=-1;
 }
  /*Читаем материал*/
/*
printw("bros=%s\n",bros);
OSTANOV();
*/
if(i != -1)
  sprintf(strsql,"select * from Material \
where kodm=%d",kodm);

  /*Читаем материал*/
if(i == -1)
  sprintf(strsql,"select * from Material \
where strihkod='%s'",bros.ravno());

/*clear();*/


if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  iceb_t_soob(gettext("Не найден материал !"));
  return;
 }

if(row[4][0] == '\0')
 {
  iceb_t_soob(gettext("Для этого товара не введена единица измерения !"));
  return;
 }
//printw("1111111111\n");
//refresh();
if(atof(row[6]) == 0.)
 {
  iceb_t_soob(gettext("Для этого товара не введена цена !"));

  return;
 }


kodm=atoi(row[0]);
naim.new_plus(row[2]);
ei.new_plus(row[4]);
cenas.new_plus(row[6]);
ndss.new_plus(row[12]);
kolih=1.0;

//Проверяем на принадлежность к изделию

sprintf(strsql,"select kodi from Specif where kodi='%d' limit 1",kodm);
if(sql_readkey(&bd,strsql,&row,&curr) > 0)
  metkaiz=1;


if(SRAV(ei.ravno(),"Кг",0) == 0 || SRAV(ei.ravno(),"КГ",0) == 0 ||
 SRAV(ei.ravno(),"кг",0) == 0 || i != -1 )
 {
  clear();
  nazz:;

//  if(regim != 2 || strlen(scale_port) == 0)
  if(regim != 2 )
   {
    ostatok=ostdok1(dp,mp,gp,skl,kodm);

vv:;    
/*************
    if(strlen(scale_port) != 0)
      helstr(LINES-1,0,"F2",gettext("взять вес"),
      "F10",gettext("выход"),NULL);
    else
*************/
      helstr(LINES-1,0,"F10",gettext("выход"),NULL);
    

    VVOD VVOD1(2);

    sprintf(strsql,"%s: %s",
    gettext("Товар"),naim.ravno());
    VVOD1.VVOD_SPISOK_add_ZAG(strsql);

    sprintf(strsql,"%s: %s",
    gettext("Единица измерения"),ei.ravno());
    VVOD1.VVOD_SPISOK_add_ZAG(strsql);

    sprintf(strsql,"%s: %s",
    gettext("Цена за единицу"),cenas.ravno());
    VVOD1.VVOD_SPISOK_add_ZAG(strsql);

    sprintf(strsql,"%s:%.10g",gettext("Остаток"),ostatok);
    VVOD1.VVOD_SPISOK_add_ZAG(strsql);
    if(metkaiz == 1)
     {
      sprintf(strsql,"%s",gettext("Изделие"));
      VVOD1.VVOD_SPISOK_add_ZAG(strsql);
     }    

    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите количество"));
    kolihch.new_plus("");
    if((i=vvod1(&kolihch,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      return;
    if(i == ESC)
      return;
/***********************
    if(i == FK2 && strlen(scale_port) == 0)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Весы не поключены к вашему рабочему месту."));
      SOOB.VVOD_SPISOK_add_MD(gettext("     Введите вес с клавиатуры."));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto nazz;
     }
*********************/
   }
/********************
  if(regim == 2 && strlen(scale_port) != 0)
   {
    VVOD SOOBN(1);
    SOOBN.VVOD_SPISOK_add_MD(gettext("Подождите. Выполняется чтение данных с электронных весов."));
    soobsh(&SOOBN,-1,-1,0);

    i=FK2;    
   }   
*****************/
  if(i == FK2)
   {
    GDITE();
//    bb=0.;
    bb=kolihch.ravno_atof();        
/*********************
    if((i=readves(&bb)) <= 0) 
     {
      if(i == -1)
       {
        bb=kolihch.ravno_atof();        
       }
      else
       { 
        beep();
        move(20,0);
        printw("%s %d !",
        gettext("Ошибка чтения веса"),i);
        OSTANOV();
        clearstr(20,0); 
        goto nazz;  
       }
     } 
***********************/
    sprintf(strsql,"%.10g",bb);
    kolihch.new_plus(strsql);
   } 

  kolih=kolihch.ravno_atof();
  if(kolih <= 0.)
   goto nazz;

  if(konost == 0 && metkaiz == 0)
   {
    ostatok=ostdok1(dp,mp,gp,skl,kodm);
    if(ostatok-kolih < -0.0000001)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Отрицательный остаток."));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto vv;
     }
   }      
 }
 
GDITE();
ktoi=iceb_t_getuid();
time(&vrem);

/* Проверяем если материал введён то прибавляем количество*/

sprintf(strsql,"select kolih from Roznica \
where datrp='%d-%02d-%02d' and kassa=%d and nomer=%d \
and metka=1 and kodm=%d",
gp,mp,dp,kassa,nomp,kodm);

if((i=sql_readkey(&bd,strsql,&row1,&curr)) == 1)
 {
//  printw("Корректировка количества. %d\n",i);
//  refresh();
  kolih+=atof(row1[0]);
  sprintf(strsql,"update Roznica \
set \
kolih=%.10g,\
ktoi=%d,\
vrem=%ld \
where datrp='%d-%02d-%02d' and kassa=%d and nomer=%d \
and metka=1 and kodm=%d",
kolih,ktoi,vrem,
gp,mp,dp,kassa,nomp,kodm);
 }
else
 {
/*
  printw("Новая запись %d.\n",i);
  refresh();
*/
  sprintf(strsql,"insert into Roznica \
values ('%d-%02d-%02d',%d,%d,1,%d,'%s',%.10g,%s,%s,%d,%d,%ld)",
  gp,mp,dp,kassa,nomp,kodm,ei.ravno(),kolih,cenas.ravno(),ndss.ravno(),skl,ktoi,vrem);
 }

sql_zapis(strsql,1,0);

}

/************/
/*Шапка меню*/
/************/
void		saproz(short dp,short mp,short gp, //Дата продажи
short kassa, //Номер рабочего места
int nomp, //Номер продажи
double osm, //Общая сумма продаж
double stp, //Сумма текущей продажи
int skl, //Склад
double sum_pol) //Полученная от покупателя сумма
{
extern class iceb_tu_str fio;

move(0,0);

attron(A_BOLD); /*Повышенная яркость*/
attron(COLOR_PAIR(COLOR_YELLOW*COLORS+COLOR_BLUE));

printw(gettext("\
                              Розничная торговля.                              "));

attroff(COLOR_PAIR(COLOR_YELLOW*COLORS+COLOR_BLUE));

move(1,0);
printw("%s %8.2f %s%d.%d.%d %s%d %s:%.2f",
gettext("Общая сумма продаж   :"),osm,
gettext("Дата:"),dp,mp,gp,
gettext("Отдел:"),skl,
gettext("Получено"),sum_pol);

move(2,0);
printw("%s %8d %s %d %s:%.2f",
gettext("Номер текущей продажи:"),nomp,
gettext("Номер рабочего места:"),kassa,
gettext("Сумма сдачи"),sum_pol-stp);

move(3,0);
printw("%s %8.2f %s %s\n",
gettext("Сумма текущей продажи:"),stp,
gettext("Кассир:"),fio.ravno());

attroff(A_BOLD); /*Повышенная яркость*/

attron(COLOR_PAIR(COLOR_BLACK*COLORS+COLOR_GREEN));
printw(gettext("\
Код.м.    Наименование товара       Ед.изм. Колич.   Цена      Сумма           \n"));

attroff(COLOR_PAIR(COLOR_BLACK*COLORS+COLOR_GREEN));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("выб.товара"),
"F3",gettext("удалить"),
"F4",gettext("корре."),
"F5",gettext("чек"),
"F6",gettext("пред. продажа"),
"F7",gettext("сумма"),
"F8",gettext("пол."),
"F10",gettext("выход"),NULL);

}
