/*$Id: zarnrn.c,v 5.15 2013/08/13 05:50:02 sasa Exp $*/
/*01.06.2012	12.09.2008	Белых А.И.	zarnrn.c
Корректировка процентов расчёта начислений по конкретному табельному номеру
*/
#include <errno.h>
#include "buhg.h"

class zarnrn_zap
 {
  public:
   class iceb_tu_str pole2; /*Коды исходные*/
   class iceb_tu_str pole3; /*Код выходной*/
   class iceb_tu_str pole4; /*Процент*/
   class iceb_tu_str pole5; /*Тариф*/
   class iceb_tu_str pole6; /*Месяц*/
   class iceb_tu_str pole7; /*исключающий код*/
   void clear()
    {
     pole2.new_plus("");
     pole3.new_plus("");
     pole4.new_plus("");
     pole5.new_plus("");
     pole6.new_plus("");
     pole7.new_plus("");
    }  
 };
 
int zarnrn_vvod(int,class zarnrn_zap *zap);
int zarnrn_zap(int tabnom,class SPISOK *sp_nastr);
int zarnrn_proc(class zarnrn_zap *zap);
void zarnrn_rs(int tabnom,const char *fio,class SPISOK *sp_nastr);
int zarnrn_tarif(class zarnrn_zap *zap);
int zarnrn_knah(class zarnrn_zap *zap);

extern char *imabaz;

void zarnrn(int tabnom)
{
class zarnrn_zap zap;
int metka_kor=0; /*0-корректировок данных небыло 1-было*/
int kom=0;
char strsql[512];
GDITE();

if(iceb_t_prn() != 0)
 return;
 
/*Проверяем не работает ли кто в режиме редактирования файла*/
sprintf(strsql,"%s%s",imabaz,"zarproc.alx");
class icebt_sql_flag flag;

if(flag.flag_on(strsql) != 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С файлом уже работает другой оператор !"));
  sprintf(strsql,"%s","zarproc.alx");
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

flag.flag_off();

/*перечитываем тарифную таблицу*/
zartarroz1();

char flag_edit[128]; /*флаг, который выствляется для невозможности войти в режим редактирования*/
sprintf(flag_edit,"%szarnrn_edit",imabaz);
class icebt_sql_flag flagedit(flag_edit);

clear();



VVOD DANET(1);

class SPISOK sp_nastr; /*Список настроек на табельный номер*/

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='zarproc.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarproc.alx");
  iceb_t_soob(strsql);
  return;
 }


SQL_str row;
class SQLCURSOR cur;

char stroka[1024];
char stroka_menu[1024];
class iceb_tu_str naim_nah("");
int tbn=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&tbn,1,'|') != 0)
   continue;
  if(tabnom != tbn)
   continue;
  sp_nastr.plus(row_alx[0]);   
  
 
 }
class iceb_tu_str fio("");
/*Узнаём фамилию*/
sprintf(strsql,"select fio from Kartb where tabn=%d",tabnom);
if(readkey(strsql,&row,&cur) == 1)
 fio.new_plus(row[0]);

naz1:;
class SPISOK sp_menu;
char tarif[128];

for(int ii=0 ; ii < sp_nastr.kolih(); ii++)
 {
  polen(sp_nastr.ravno(ii),&zap.pole2,2,'|');
  polen(sp_nastr.ravno(ii),&zap.pole3,3,'|');
  polen(sp_nastr.ravno(ii),&zap.pole4,4,'|');
  polen(sp_nastr.ravno(ii),&zap.pole5,5,'|');
  polen(sp_nastr.ravno(ii),&zap.pole6,6,'|');
  polen(sp_nastr.ravno(ii),&zap.pole7,7,'|');

  naim_nah.new_plus("");
  /*Узнаём наименование начисления*/
  sprintf(strsql,"select naik from Nash where kod=%d",zap.pole3.ravno_atoi());
  if(readkey(strsql,&row,&cur) == 1)
   naim_nah.new_plus(row[0]); 
  int nomstr=0;
  int nomkol=0;
  
  if(polen(zap.pole5.ravno(),&nomstr,1,'/') == 0)
    polen(zap.pole5.ravno(),&nomkol,2,'/') ;
    
  memset(tarif,'\0',sizeof(tarif));
  if(nomstr == 0)
   strncpy(tarif,zap.pole5.ravno(),sizeof(tarif));
  else
   {
    double ztar=zartarroz1(nomstr,nomkol,0,0,0);
    sprintf(tarif,"%s/%.6g",zap.pole5.ravno(),ztar);
   }  
      
  sprintf(stroka_menu,"%3d|%-*.*s|%5.f|%5s|%s",
  zap.pole3.ravno_atoi(),
  iceb_tu_kolbait(20,naim_nah.ravno()),
  iceb_tu_kolbait(20,naim_nah.ravno()),
  naim_nah.ravno(),
  zap.pole4.ravno_atof(),
  tarif,
  zap.pole2.ravno());
  
  sp_menu.plus(stroka_menu);
 
 }

int kolih_str_menu=sp_menu.kolih();

naz:;

clear(); 

flagedit.flag_on(); /*Всё время его устанавливаем, так как он может быть выствлен другим оператором и снят им-же*/

mvprintw(0,0,"%d %s\n",tabnom,fio.ravno());
mvprintw(1,0,"   1          2              3     4       5");
int i1,X;
move(i1=2,X=COLS-21);
printw("%s:",gettext("Расшифровка полей"));
move(++i1,X);
printw("1 %s",gettext("код начис."));
move(++i1,X);
printw("2 %s",gettext("наименование"));
move(++i1,X);
printw("3 %s",gettext("процент"));
move(++i1,X);
printw("4 %s",gettext("тариф"));
move(++i1,X);
printw("5 %s",gettext("входящие нач."));


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("процент"),
"F5",gettext("распеч."),
"F6",gettext("тариф"),
"F7",gettext("начис."),
"F10",gettext("выход"),NULL);

int kod_kl=0;
kom=dirmasiv(&sp_menu,2,0,kom,"",1,&kod_kl);
if(kom >= sp_nastr.kolih())
  goto naz;

switch (kod_kl)
 {
  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("zarp2_14.txt");
    
    goto naz;
    
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     if(metka_kor != 0)
      {
       DANET.VVOD_delete();
       DANET.VVOD_SPISOK_add_MD(gettext("Сберечь выполненные корректировки?"));

       if(danet(&DANET,2,stdscr) == 1)
        zarnrn_zap(tabnom,&sp_nastr);
        
      }
     break;

  case SFK2:
  case ENTER:
    if(kolih_str_menu == 0)
     goto naz;
    polen(sp_nastr.ravno(kom),&zap.pole2,2,'|');
    polen(sp_nastr.ravno(kom),&zap.pole3,3,'|');
    polen(sp_nastr.ravno(kom),&zap.pole4,4,'|');
    polen(sp_nastr.ravno(kom),&zap.pole5,5,'|');
    polen(sp_nastr.ravno(kom),&zap.pole6,6,'|');
    polen(sp_nastr.ravno(kom),&zap.pole7,7,'|');
    
    if(zarnrn_vvod(1,&zap) == 0)
     {
      sprintf(stroka,"%-11d|%-12s|%-12s|%-7s|%-12s|%-7s|%s\n",
      tabnom,
      zap.pole2.ravno(),
      zap.pole3.ravno(),
      zap.pole4.ravno(),
      zap.pole5.ravno(),
      zap.pole6.ravno(),
      zap.pole7.ravno());
      sp_nastr.new_plus(kom,stroka);     
      metka_kor=1;
     }
    goto naz1;

  case FK2:
  case PLU:
    zap.clear();    
    if(zarnrn_vvod(0,&zap) == 0)
     {
      sprintf(stroka,"%-11d|%-12s|%-12s|%-7s|%-12s|%-7s|%s\n",
      tabnom,
      zap.pole2.ravno(),
      zap.pole3.ravno(),
      zap.pole4.ravno(),
      zap.pole5.ravno(),
      zap.pole6.ravno(),
      zap.pole7.ravno());
      
      sp_nastr.plus(stroka);
      metka_kor=1;      
     }

    goto naz1;

  case FK3:
    if(kolih_str_menu == 0)
     goto naz;
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить? Вы уверены?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;
    
    sp_nastr.del(kom);
    metka_kor=1;    
    goto naz1;

  case FK4:
    if(kolih_str_menu == 0)
     goto naz;
    polen(sp_nastr.ravno(kom),&zap.pole2,2,'|');
    polen(sp_nastr.ravno(kom),&zap.pole3,3,'|');
    polen(sp_nastr.ravno(kom),&zap.pole4,4,'|');
    polen(sp_nastr.ravno(kom),&zap.pole5,5,'|');
    polen(sp_nastr.ravno(kom),&zap.pole6,6,'|');
    polen(sp_nastr.ravno(kom),&zap.pole7,7,'|');
    if(zarnrn_proc(&zap) == 0)
     {
      sprintf(stroka,"%-11d|%-12s|%-12s|%-7s|%-12s|%-7s|%s\n",
      tabnom,
      zap.pole2.ravno(),
      zap.pole3.ravno(),
      zap.pole4.ravno(),
      zap.pole5.ravno(),
      zap.pole6.ravno(),
      zap.pole7.ravno());
      
      sp_nastr.new_plus(kom,stroka);     
      metka_kor=1;      
     }
    goto naz1;

  case FK5:
    if(kolih_str_menu == 0)
     goto naz;
    zarnrn_rs(tabnom,fio.ravno(),&sp_nastr);
    goto naz;

  case FK6:
    polen(sp_nastr.ravno(kom),&zap.pole2,2,'|');
    polen(sp_nastr.ravno(kom),&zap.pole3,3,'|');
    polen(sp_nastr.ravno(kom),&zap.pole4,4,'|');
    polen(sp_nastr.ravno(kom),&zap.pole5,5,'|');
    polen(sp_nastr.ravno(kom),&zap.pole6,6,'|');
    polen(sp_nastr.ravno(kom),&zap.pole7,7,'|');
    if(zarnrn_tarif(&zap) == 0)
     {
      sprintf(stroka,"%-11d|%-12s|%-12s|%-7s|%-12s|%-7s|%s\n",
      tabnom,
      zap.pole2.ravno(),
      zap.pole3.ravno(),
      zap.pole4.ravno(),
      zap.pole5.ravno(),
      zap.pole6.ravno(),
      zap.pole7.ravno());
      
      sp_nastr.new_plus(kom,stroka);     
      metka_kor=1;      
     }
    goto naz1;

  case FK7:
    polen(sp_nastr.ravno(kom),&zap.pole2,2,'|');
    polen(sp_nastr.ravno(kom),&zap.pole3,3,'|');
    polen(sp_nastr.ravno(kom),&zap.pole4,4,'|');
    polen(sp_nastr.ravno(kom),&zap.pole5,5,'|');
    polen(sp_nastr.ravno(kom),&zap.pole6,6,'|');
    polen(sp_nastr.ravno(kom),&zap.pole7,7,'|');
    if(zarnrn_knah(&zap) == 0)
     {
      sprintf(stroka,"%-11d|%-12s|%-12s|%-7s|%-12s|%-7s|%s\n",
      tabnom,
      zap.pole2.ravno(),
      zap.pole3.ravno(),
      zap.pole4.ravno(),
      zap.pole5.ravno(),
      zap.pole6.ravno(),
      zap.pole7.ravno());
      
      sp_nastr.new_plus(kom,stroka);     
      metka_kor=1;      
     }
    goto naz1;
     
       
  default:
   goto naz;
 }




}
/*****************************/
/*Ввод и корректировка записи*/
/*******************************/

int zarnrn_vvod(int metka, /*0-ввод новой записи 1-корректировка*/
class zarnrn_zap *zap)
{


VVOD VV(0);
if(metka == 0)
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
if(metka == 1)
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));


VV.VVOD_SPISOK_add_MD(gettext("Код начисления...."));//0
VV.VVOD_SPISOK_add_MD(gettext("Процент..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Коды для расчёта.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Тариф............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Месяц............."));//4
VV.VVOD_SPISOK_add_MD(gettext("Исключающий код..."));//5

VV.VVOD_SPISOK_add_data(zap->pole3.ravno(),50);
VV.VVOD_SPISOK_add_data(zap->pole4.ravno(),10);
VV.VVOD_SPISOK_add_data(zap->pole2.ravno(),500);
VV.VVOD_SPISOK_add_data(zap->pole5.ravno(),50);
VV.VVOD_SPISOK_add_data(zap->pole6.ravno(),10);
VV.VVOD_SPISOK_add_data(zap->pole7.ravno(),10);


naz:;

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK10:
  case ESC:
     return(1);

  case FK2:
  case PLU:
    GDITE();
    zap->pole3.new_plus(VV.data_ravno(0));
    zap->pole4.new_plus(VV.data_ravno(1));
    zap->pole2.new_plus(VV.data_ravno(2));
    zap->pole5.new_plus(VV.data_ravno(3));
    zap->pole6.new_plus(VV.data_ravno(4));
    zap->pole7.new_plus(VV.data_ravno(5));

    return(0);
  
  default:
   goto naz;
 }
 
return(1);
}
/*****************************/
/*Запись в файл*/
/**********************/
int zarnrn_zap(int tabnom,class SPISOK *sp_nastr)
{
GDITE();
char stroka[1024];

/*Выставляем флаг записи в файл*/
sprintf(stroka,"%szarnrn_zap",imabaz);
class icebt_sql_flag flag;

while(flag.flag_on(stroka) != 0)
 sleep(1);

FILE *ff_tmp;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='zarproc.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarproc.alx");
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(stroka,"zarnrn%d.tmp",getpid());
class iceb_tu_str imaf_tmp(strsql); /*из текущего каталога не перенести файл, так как нужно указывать полный путь*/
//imafn(stroka,&imaf_tmp);

if((ff_tmp = fopen(imaf_tmp.ravno(),"w")) == NULL)
 {
  error_op_nfil(imaf_tmp.ravno(),errno,gettext("Информация не сохранена!"));
  return(1);
 }

int tbn;
int metka=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  printw("%s",row_alx[0]);
  if(row_alx[0][0] == '#')
   {
    fprintf(ff_tmp,"%s",row_alx[0]);
    continue;
   }
  if(polen(row_alx[0],&tbn,1,'|') != 0)
   {
    fprintf(ff_tmp,"%s",row_alx[0]);
    continue;
   }
  if(tbn != tabnom)
   {
    fprintf(ff_tmp,"%s",row_alx[0]);
    continue;
   }
  if(metka == 0) /*записываем*/
   {
    metka=1;
    for(int ii=0; ii < sp_nastr->kolih();ii++)
     fprintf(ff_tmp,"%s",sp_nastr->ravno(ii));
   }
  
 }     

if(metka == 0)
    for(int ii=0; ii < sp_nastr->kolih();ii++)
     fprintf(ff_tmp,"%s",sp_nastr->ravno(ii));

fclose(ff_tmp);

if(rename(imaf_tmp.ravno(),"zarproc.alx") != 0)
 {
  VVOD SOOB(1);

  sprintf(stroka,"Переименовываем %s -> %s",imaf_tmp.ravno(),"zarproc.alx");
  SOOB.VVOD_SPISOK_add_MD(stroka);

  sprintf(stroka,"%s %d",strerror(errno),errno);
  SOOB.VVOD_SPISOK_add_MD(stroka);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
 }
iceb_t_alxin("zarproc.alx");

return(0);
}
/*************************/
/*ввод процента*/
/**********************/
int zarnrn_proc(class zarnrn_zap *zap)
{
VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите процент"));
helstr(LINES-1,0,"Enter",gettext("ввод"),
"F10",gettext("выход"),NULL);

if(vvod1(&zap->pole4,20,&VVOD1,NULL,stdscr,-1,-1) == FK10)
 {
  return(1);
 }
return(0);


}
/*************************/
/*ввод тарифа*/
/**********************/
int zarnrn_tarif(class zarnrn_zap *zap)
{
VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите тариф"));
helstr(LINES-1,0,"Enter",gettext("ввод"),
"F10",gettext("выход"),NULL);

if(vvod1(&zap->pole5,50,&VVOD1,NULL,stdscr,-1,-1) == FK10)
 {
  return(1);
 }
return(0);


}
/*************************/
/*ввод кодов начисления*/
/**********************/
int zarnrn_knah(class zarnrn_zap *zap)
{
VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите коды начислений"));
helstr(LINES-1,0,"Enter",gettext("ввод"),
"F10",gettext("выход"),NULL);

if(vvod1(&zap->pole2,500,&VVOD1,NULL,stdscr,-1,-1) == FK10)
 {
  return(1);
 }
return(0);


}
/***************************/
/*печать*/
/*******************/

void zarnrn_rs(int tabnom,const char *fio,class SPISOK *sp_nastr)
{

FILE *ff;
char imaf[56];
sprintf(imaf,"zarnrn%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
startfil(ff);

iceb_t_zagolov(gettext("Настройки для автоматического расчёта начислений"),0,0,0,0,0,0,ff);
fprintf(ff,"%d %s\n",tabnom,fio);
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,"\
Табельный  |Код исходный|Код выходной|Процент|   Тариф    |Месяц  |Исключающий\n\
   номер   |            |            |       |            |       |  код\n");
fprintf(ff,"\
-----------------------------------------------------------------------------\n");

for(int ii=0; ii < sp_nastr->kolih(); ii++)
 {
  fprintf(ff,"%s",sp_nastr->ravno(ii));
 
 }

podpis(ff);
fclose(ff);

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Настройки для автоматического расчёта начислений"));
iceb_t_rabfil(&oth,"");


}


