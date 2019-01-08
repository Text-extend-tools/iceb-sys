/*$Id: dirmat1.c,v 5.103 2013/09/26 09:43:54 sasa Exp $*/
/*28.03.2018	26.01.1999	Белых А.И.	dirmat1.c
Ввод и корректировка нормативно-справочной информации
*/
#include	<errno.h>
#include	"buhl.h"

void shmt1(const char *tz,class iceb_tu_str *naimm,short mk,const char *nm);
void 		vmat1(SQL_str,const char*,short,const char*);
void ud_spiskom(const char *tabl);
void dirmat1_rs(class SQLCURSOR *cur,const char *tz,const char *naimm,const char *nm,const char *ignor,short mk);
int provdirmat1(const char *nm,const char *nm1,const char *ignor,const char *kod);

int dirmat1(const char *tz,//Имя таблицы
class iceb_tu_str *kod,  //Код записи
class iceb_tu_str *nai,  //Наименование
int kls,     //Максимальное количество строк
int x,int y, //Координаты вывода меню
short mk,    //0-ввод и корректировка  
	     //1-выбор нужного с корректировкой 
	     //2-чистый выбор
int *poz,    //Номер строки в курсоре на которую нужно вернуться при возврате в меню
int *komv,   //Номер строки в меню на которую нужно установить подсветку строки
const char *nm,   //Эталон поиска
const char *ignor) //Список кодов которые игнорируются при просмотре
{
int             K;
int		prc;
int             i=0;
SQL_str         row;
char		strsql[1024];
int		kom;
int		kolstr;
int    	        pozz,pozz1;
class iceb_tu_str naimm("");
class iceb_tu_str vidop("");
class iceb_tu_str bros("");
char		imaf[64];
short		mspz; /*метка соответствия первой записи*/
char		metprov[64];
int		metkasort=0; //0-сортировка по кодам 1-по наименованию
class iceb_tu_str kodr("");
class iceb_tu_str nair("");
class iceb_tu_str fondop("");
class iceb_t_mstr spis_menu(0);
char  stroka[2048];
VVOD		VVOD1(2);
VVOD		SOOB(1);
VVOD		DANET(1);
class iceb_tu_str naim_poi(nm);
class iceb_tu_str zagolovok_menu("");

if(mk == 2)
 zagolovok_menu.new_plus(gettext("Выберите нужное"));

pozz=*poz;
kom=*komv;
 
naz1:;

GDITE();

i=0;
if(y > 0)
  i=y;

if(kls <= 0)
 kls=LINES-i-5;

if(mk == 2)
 {
  if(kls > LINES-i-3-2)
   kls = LINES-i-3-2;

 }
else
 {
  if(kls > LINES-i-3)
   kls = LINES-i-3;
 }


K=0;
class SQLCURSOR cur;
move(0,0);

if(metkasort == 0)
  sprintf(strsql,"select * from %s order by kod asc",tz);
if(metkasort == 1)
  sprintf(strsql,"select * from %s order by naik asc",tz);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {  
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
  
if(kolstr == 0)
 {

lll:;
  shmt1(tz,&naimm,mk,naim_poi.ravno());
  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == ENTER || K == SFK2 || K == FK3 || K == FK5)
      goto lll;
  if(K == KEY_RESIZE)
   {
    clear();
    goto naz1;
   }      
  goto vper;

 }  

naz:;
GDITE();
if(mk != 2)
 clear();
 
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;


cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i <= kls-1;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(provdirmat1(naim_poi.ravno(),row[1],ignor,row[0]) != 0)
    continue;
  
  memset(metprov,'\0',sizeof(metprov));
  memset(stroka,'\0',sizeof(stroka));  
  
  sprintf(stroka,"%s|%s",row[0],row[1]);
  
  if(mk < 2)
    sprintf(stroka,"%-*s|%s",iceb_tu_kolbait(6,row[0]),row[0],row[1]);

  int kbkod=iceb_tu_kolbait(6,row[0]);
  


  if(SRAV(tz,"Gnali",0) == 0  && mk < 2)
    sprintf(stroka,"%-*s|%-*s|%s",
    kbkod,row[0],
    iceb_tu_kolbait(20,row[1]),row[1],
    row[2]);



  i++;
  spis_menu.spis_str.plus(stroka);

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 } 
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


shmt1(tz,&naimm,mk,naim_poi.ravno());
prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,zagolovok_menu.ravno(),0);


if(kom == -7) //Изменение размера экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i < kls)
   {
    beep();
    goto naz;
   }

  for(i=0; i < mspz  ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(provdirmat1(naim_poi.ravno(),row[1],ignor,row[0]) != 0)
      continue;

    i++;
   }
  kom=kls-1;
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
    if(provdirmat1(naim_poi.ravno(),row[1],ignor,row[0]) != 0)
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
  for(i=0; i < kls ; )
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

    if(provdirmat1(naim_poi.ravno(),row[1],ignor,row[0]) != 0)
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

  *komv=kom;
  *poz=pozz;

  polen(spis_menu.spis_str.ravno(kom),strsql,sizeof(strsql),1,'|');
  i=poivcur(strsql,0,&cur);

  cur.poz_cursor(i);
  cur.read_cursor(&row);
  kodr.new_plus(row[0]);
  nair.new_plus(row[1]);

  vidop.new_plus(gettext("Внешняя"));
}
vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case ENTER:/*Выход*/
     if(mk == 0)
       goto sfk2;

     kod->new_plus(kodr.ravno());
     nai->new_plus(nair.ravno());
     return(0);

  case FK1:  /*Получить подсказку*/
   GDITE();
   memset(imaf,'\0',sizeof(imaf));
   if(SRAV(tz,"Grup",0) == 0)
     sprintf(imaf,"matu4_1.txt");
   if(SRAV(tz,"Reegrup1",0) == 0)
     sprintf(imaf,"rnn2.txt");
   if(SRAV(tz,"Reegrup2",0) == 0)
     sprintf(imaf,"rnn2.txt");
   if(SRAV(tz,"Foroplat",0) == 0)
     sprintf(imaf,"matu4_7.txt");
   if(SRAV(tz,"Kateg",0) == 0)
     sprintf(imaf,"zarp3_1_2.txt");
   if(SRAV(tz,"Tabel",0) == 0)
     sprintf(imaf,"zarp3_1_6.txt");
   if(SRAV(tz,"Opltp",0) == 0 || SRAV(tz,"Opltt",0) == 0)
     sprintf(imaf,"dokum2_8.txt");


   if(SRAV(tz,"Uoshau",0) == 0)
     sprintf(imaf,"uos5_5.txt");
   if(SRAV(tz,"Uoshz",0) == 0)
     sprintf(imaf,"uos5_6.txt");
   if(SRAV(tz,"Uslgrup",0) == 0)
     sprintf(imaf,"usl3_1.txt");
   if(SRAV(tz,"Gkont",0) == 0)
     sprintf(imaf,"buhg4_7.txt");

   if(SRAV(tz,"Ukrgrup",0) == 0)
     sprintf(imaf,"ukr3_1.txt");
   if(SRAV(tz,"Ukrvkr",0) == 0)
     sprintf(imaf,"ukr3_2.txt");

   if(SRAV(tz,"Uplpodr",0) == 0)
     sprintf(imaf,"upl3_1.txt");


   if(imaf[0] == '\0')
    {
     SOOB.VVOD_delete();
     SOOB.VVOD_SPISOK_add_MD(gettext("Не задано имя файла с документацией !"));
     SOOB.VVOD_SPISOK_add_MD(gettext("       Сообщите разработчику !"));
     soobshw(&SOOB,stdscr,-1,-1,0,1);

     clear();
     if(kolstr > 0)
       goto naz;
     else   
      {
       goto naz1;
      }     
    }

   iceb_t_pdoc(imaf);
   clear();
   if(kolstr > 0)
     goto naz;
   else   
     goto lll;
     
  case FK2:  /*Ввести запись*/
  case PLU:

    vmat1(row,tz,0,vidop.ravno());
    goto naz1;
    break;

  case SFK2:   /*Корректировать запись*/

sfk2:;
    vmat1(row,tz,1,vidop.ravno());
    goto naz1;

  case SFK3: //Удалить по списку не используемые
    DANET.VVOD_delete_ZAG();
    DANET.VVOD_SPISOK_add_ZAG(gettext("Удалить неиспользуемые записи ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz1;
     
    ud_spiskom(tz);

    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete_ZAG();
    DANET.VVOD_SPISOK_add_ZAG(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      GDITE();
      if(provud(tz,row[0],1) != 0)
       {
        clear();
        goto naz;
       }

      if(SRAV(tz,"Kateg",0) == 0 ||\
      SRAV(tz,"Gnali",0) == 0 ||\
      SRAV(tz,"Tabel",0) == 0 ||\
      SRAV(tz,"Gkont",0) == 0 ||\
      SRAV(tz,"Uplpodr",0) == 0)
        sprintf(strsql,"delete from %s where kod = %s",tz,row[0]);
      else
        sprintf(strsql,"delete from %s where kod = '%s'",tz,row[0]);

//      printw("\nstrsql=%s\n",strsql);
//      OSTANOV();

      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         SOOB.VVOD_delete();
         SOOB.VVOD_SPISOK_add_MD(gettext("У вас нет полномочий для выполнения этой операции !"));
         soobshw(&SOOB,stdscr,-1,-1,0,1);

        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
      clear();
      goto naz1;
     }
    clear();
    goto naz;

  case FK4:       /*Поиск*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));
     
     naim_poi.new_plus("");
     
     vvod1(&naim_poi,10,&VVOD1,NULL,stdscr,-1,-1);

     GDITE();

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:       /*Распечатать*/
   clear();
   refresh();
   GDITE();
   dirmat1_rs(&cur,tz,naimm.ravno(),naim_poi.ravno(),ignor,mk);

   clear();

   cur.poz_cursor(pozz);
   goto naz;

  case FK6:    //Сортировка
   metkasort++;
   if(metkasort > 1)
     metkasort=0;
     
   goto naz1;

   default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }
}


/*******************/
void shmt1(const char *tz,class iceb_tu_str *naimm,
short mk,  //0-ввод 1-выбор
const char *nm)
{
move(0,0);


if(SRAV(tz,"Grup",0) == 0)
 naimm->new_plus(gettext("групп материалов"));
if(SRAV(tz,"Uslgrup",0) == 0)
 naimm->new_plus(gettext("групп услуг"));

if(SRAV(tz,"Reegrup1",0) == 0)
 naimm->new_plus(gettext("групп полученых документов"));
if(SRAV(tz,"Reegrup2",0) == 0)
 naimm->new_plus(gettext("групп виданых документов"));
if(SRAV(tz,"Foroplat",0) == 0)
 naimm->new_plus(gettext("форм оплаты"));

if(SRAV(tz,"Tabel",0) == 0)
 naimm->new_plus(gettext("видов табеля"));
if(SRAV(tz,"Gnali",0) == 0)
 naimm->new_plus(gettext("городов налоговых инспекций"));
if(SRAV(tz,"Kateg",0) == 0)
 naimm->new_plus(gettext("категорий"));


if(SRAV(tz,"Gkont",0) == 0 )
 naimm->new_plus(gettext("групп контрагентов"));


if(SRAV(tz,"Zargrupp",0) == 0)
 naimm->new_plus(gettext("групп подразделений"));


if(SRAV(tz,"Opltp",0) == 0)
 naimm->new_plus(gettext("операций платежных поручений"));

if(SRAV(tz,"Opltt",0) == 0)
 naimm->new_plus(gettext("операций платежных требований"));

if(SRAV(tz,"Ksn",0) == 0)
 naimm->new_plus(gettext("особ, которые сдают наличку"));

if(SRAV(tz,"Oznvb",0) == 0)
 naimm->new_plus(gettext("операций здачи налички"));


if(SRAV(tz,"Uoshau",0) == 0)
 naimm->new_plus(gettext("шифров аналитического учета"));

if(SRAV(tz,"Uoshz",0) == 0)
 naimm->new_plus(gettext("шифров затрат"));


if(SRAV(tz,"Ukrvkr",0) == 0)
 naimm->new_plus(gettext("видов затрат"));

if(SRAV(tz,"Ukrgrup",0) == 0)
 naimm->new_plus(gettext("групп коман-ных расходов"));

if(mk < 2)
  printw(gettext("Ввод и корректировка %s"),naimm->ravno());
if(nm[0] != '\0')
 {
  attron(A_BLINK);
  printw(" %s %s",gettext("Поиск:"),nm);
  attroff(A_BLINK);
 }
if(mk == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(mk < 2)
 helstr(LINES-1,0,"F1",gettext("помощь"),
 "F2/+",gettext("запись"),
 "Sh+F2",gettext("корректировка"),
 "F3",gettext("удалить"),
 "F4",gettext("поиск"),
 "F5",gettext("печать"),
 "F6",gettext("сорт."),
 "F10",gettext("выход"),NULL);
if(mk == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);
}

/**************************/
/*Проверка на соответствие*/
/***************************/
int provdirmat1(const char *nm,const char *nm1,const char *ignor,const char *kod)
{
int		i1;

if(nm[0] != '\0' && strstrm(nm1,nm) == 0)
 return(1);

if(ignor[0] != '\0')
 if(pole1(ignor,kod,',',0,&i1) == 0 || SRAV(ignor,kod,0) == 0)
  return(2);
return(0);
}
/**********************************/
/*Удаление складов без остатков*/
/********************************/

void ud_spiskom(const char *tabl)
{
int kolstr;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

clear();
GDITE();

sprintf(strsql,"select * from %s",tabl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
int kolstr1=0;
int kolsklud=0;
while(cur.read_cursor(&row) != 0)
 {
  
  printw("%5s %-40s ",row[0],row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(provud(tabl,row[0],0) != 0)
   {
    printw("\n");
    continue;
   }

  if(SRAV(tabl,"Kateg",0) == 0 ||\
  SRAV(tabl,"Gnali",0) == 0 ||\
  SRAV(tabl,"Tabel",0) == 0 ||\
  SRAV(tabl,"Gkont",0) == 0 ||\
  SRAV(tabl,"Uplpodr",0) == 0)
    sprintf(strsql,"delete from %s where kod=%s",tabl,row[0]);
  else
    sprintf(strsql,"delete from %s where kod='%s'",tabl,row[0]);
    
  if(sql_zapis(strsql,1,0) != 0)
   continue;
   
  kolsklud++;  
  printw("%s\n",gettext("Удален"));
  
 }

VVOD		SOOB(1);
sprintf(strsql,"%s %d",gettext("Количество удалённых записей"),kolsklud);
iceb_t_soob(strsql);


}

/************************************/
/*Роздрук списку*/
/********************/
void dirmat1_rs(class SQLCURSOR *cur,const char *tz,const char *naimm,const char *nm,const char *ignor,short mk)
{

FILE *ff;
char imaf[64];

sprintf(imaf,"%s%d.lst",tz,getpid());
if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }
 
GDITE();
struct  tm      *bf;
time_t tmm;
time(&tmm);
bf=localtime(&tmm);

fprintf(ff,"%s\n\n%s %s\n\
%s %d.%d.%d %s: %d:%d\n",
 iceb_t_get_pnk("00",0),
 gettext("Распечатка"),
 naimm,
 gettext("По состоянию на"),
 bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
 gettext("Время"),
 bf->tm_hour,bf->tm_min);


fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,gettext(" Код |           Наименование\n"));
fprintf(ff,"-----------------------------------------------------\n");
SQL_str row;
int i=0;
class iceb_tu_str bros("");
char strsql[512];

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(provdirmat1(nm,row[1],ignor,row[0]) != 0)
     continue;
   i++;

  fprintf(ff,"%-*s %-*s %-*s",
  iceb_tu_kolbait(5,row[0]),row[0],
  iceb_tu_kolbait(40,row[1]),row[1],
  iceb_tu_kolbait(6,row[2]),row[2]);



   fprintf(ff,"\n");

 }

fprintf(ff,"\
-----------------------------------------------------\n\
%s - %d\n",gettext("Количество записей"),i);
podpis(ff);
fclose(ff);

sprintf(strsql,gettext("Распечатка списка %s"),naimm);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(strsql);
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");

}
