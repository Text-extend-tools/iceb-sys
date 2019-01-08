/*$Id: dirdovs.c,v 5.20 2014/06/30 06:36:34 sasa Exp $*/
/*13.07.2015	01.04.2009	Белых А.И.	dirdovs.c
Просмотр списка доверенностей
*/
#include "buhg.h"

class dirdovs_poi
 {
  public:
   class iceb_tu_str nomdov;
   class iceb_tu_str komu;
   class iceb_tu_str post;
   int metpoi;
   int metka_isp;
   class iceb_tu_str datan;
   class iceb_tu_str datak;
         
   dirdovs_poi()
    {
     datan.new_plus("");
     datak.new_plus("");
     clear();
     metka_isp=0;
    }
   void clear()
    {
     nomdov.new_plus("");
     komu.new_plus("");
     post.new_plus("");
     metpoi=0;
     
    }
 };
 
void dirdovs_sap(class dirdovs_poi *poi);
int dirdovs_poi(SQL_str row,class dirdovs_poi *poi);
int dirdovs_mpoi(class dirdovs_poi *poi);
void dirdovs_rsd(class dirdovs_poi *poi,class SQLCURSOR *cur);

extern int  kol_strok_na_liste; //Количесво строк на стандартном листе
extern int  kol_strok_na_liste_l; //Количесво строк на стандартном листе в ориентации ландшафт


void dirdovs(short dnp,short mnp,short gnp) //Дата начала поиска
{
class dirdovs_poi poi;

int prc=0;
int i=0;
int kom=0;
short mspz=0; /*метка соответствия первой записи*/
int pozz=0,pozz1=0;
int K=0;
short           x=0,y=2;
char strsql[1024];
class iceb_t_mstr spis_menu(0);
short dt,mt,gt;
class iceb_tu_str datdov("");
class iceb_tu_str nomdov("");
class iceb_tu_str bros("");
poltekdat(&dt,&mt,&gt);
short startgodud=0;
short metka_sort_dat=0; /*0-по возростанию дат 1- по убыванию дат*/

if(iceb_t_poldan("Стартовый год",strsql,"nastud.alx") == 0)
 startgodud=atoi(strsql);

if(dnp == 0 && startgodud != 0)
 {
  dnp=1; mnp=1; gnp=startgodud;
 }

if(dnp != 0)
 poi.metpoi=1;
if(dnp != 0)
  poi.datan.plus_data(dnp,mnp,gnp);

//Создаем класс меню
VVOD SOOB(1);
VVOD VV(0);
VVOD MENU(3);
VVOD DANET(1);

naz1:;

clear();
GDITE();

short           kls=LINES-5;

if(poi.datan.getdlinna() <= 1)
 poi.datan.new_plus_data(1,1,gt);

sprintf(strsql,"select * from Uddok where datd >= '%04d-01-01'",poi.datan.ravno_god());

if(poi.datan.ravno()[0] != '\0')
  sprintf(strsql,"select * from Uddok where datd >= '%s'",poi.datan.ravno_sqldata());

if(poi.datan.ravno()[0] != '\0')
 if(poi.datak.ravno()[0] != '\0')
  sprintf(strsql,"select * from Uddok where datd >= '%s' and datd <= '%s'",poi.datan.ravno_sqldata(),poi.datak.ravno_sqldata());

if(metka_sort_dat == 0)
 strcat(strsql," order by datd asc");
else
 strcat(strsql," order by datd desc");

SQLCURSOR cur;
SQL_str row;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;

  dirdovs_sap(&poi);

  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одного документа !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);
  
  if(K == FK5 || K == SFK2 || K == SFK3 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;
     
  goto vper;
 }

naz:;
GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;

char metka_isp[2];
cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
/*
  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  refresh();
*/
  pozz1++;
  if(dirdovs_poi(row,&poi) != 0)
   continue;


  if(poi.metka_isp == 1) /*только не использованые*/
   if(row[6][0] != '\0')
    continue;

  if(poi.metka_isp == 2) /*Только использованые*/
   if(row[6][0] == '\0')
    continue;

  memset(metka_isp,'\0',sizeof(metka_isp));
  
  if(row[6][0] == '\0')
   metka_isp[0]='*';

  bros.new_plus(iceb_tu_datzap(row[2])); /*только так*/

  sprintf(strsql,"%1s|%s|%-*s|%s|%-*.*s|%s",
  metka_isp,
  iceb_tu_datzap(row[0]),
  iceb_tu_kolbait(8,row[1]),row[1],
  bros.ravno(),
  iceb_tu_kolbait(20,row[3]),iceb_tu_kolbait(20,row[3]),row[3],
  row[4]);

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

if(i == 0)
 {
  if(kolstr != 0 && pozz != 0)
   {
    //Если записи есть но не подходят по условиям поиска
    pozz--;
    goto naz;
   }
  goto lll;
 }
dirdovs_sap(&poi);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-22);


if(kom == -7) //Изменение размера экрана
  goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz  ; )
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
    if(dirdovs_poi(row,&poi) != 0)
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
    if(dirdovs_poi(row,&poi) != 0)
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
    if(dirdovs_poi(row,&poi) != 0)
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

  polen(str_vib.ravno(),&datdov,2,'|');
  polen(str_vib.ravno(),&nomdov,3,'|');

 }

vper:;
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch(K)
 {  
  case ENTER:
  
     dirdov(&datdov,&nomdov);
     goto naz1;

  case FK10:
  case ESC:
    return;

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("udov1_4.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK3: /*Переключатель просмотра*/
    poi.metka_isp++;
    if(poi.metka_isp >= 3)
     poi.metka_isp=0;
    goto naz1;

  case FK4: /*поиск*/
    dirdovs_mpoi(&poi);
    goto naz1;   

  case FK5: /*распечатать*/
    dirdovs_rsd(&poi,&cur);
    goto naz1;   


  case TAB: /*включить/выключить сортировку в порядке убывания дат*/
    metka_sort_dat++;
    if(metka_sort_dat >= 2)
     metka_sort_dat=0;
    goto naz1;   

           
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }


}
/*************/
/*Шапка*/
/**************/
void dirdovs_sap(class dirdovs_poi *poi)
{
int		i1;
int		X;
clear();
if(poi->metka_isp == 0)
  printw(gettext("Просмотр документов"));
if(poi->metka_isp == 1)
  printw(gettext("Список не использованных документов"));
if(poi->metka_isp == 2)
  printw(gettext("Список использованных документов"));
printw(" %s:%d%s",gettext("Стартовый год"),poi->datan.ravno_god(),
gettext("г."));

move(1,0);
printw("      1          2        3             4               5");

move(i1=2,X=COLS-18);
printw(gettext("Расшифровка полей:"));
move(++i1,X);
printw("1 %s",gettext("дата документа"));
move(++i1,X);
printw("2 %s",gettext("номер документа"));
move(++i1,X);
printw("3 %s",gettext("действителен до"));
move(++i1,X);
printw("4 %s",gettext("коме"));
move(++i1,X);
printw("5 %s",gettext("поставщик"));
                                   
if(poi->metpoi == 1)
 {
  move(i1+=2,X=COLS-18);
  attron(A_BLINK);
  printw(gettext("Поиск:"));

  if(poi->datan.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s:%s",gettext("Дата н."),poi->datan.ravno());
   }
  if(poi->datak.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s:%s",gettext("Дата к."),poi->datak.ravno());
   }
  if(poi->nomdov.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s:%s",gettext("Н.д."),poi->nomdov.ravno());
   }

  if(poi->komu.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s:%s",gettext("Кому"),poi->komu.ravno());
   }

  if(poi->post.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s:%s",gettext("Пост."),poi->post.ravno());
   }

  attroff(A_BLINK);
 }

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F3",gettext("исполь-ные"),
"F4",gettext("поиск"),
"F5",gettext("распечатать"),
"F10",gettext("выход"),NULL);
 
}
/**********************************/
/*Проверка на соответствие поиска*/
/*********************************/
int dirdovs_poi(SQL_str row,class dirdovs_poi *poi)
{
if(poi->metka_isp == 1) /*только не использованые*/
 if(row[6][0] != '\0')
  return(1);

if(poi->metka_isp == 2) /*Только использованые*/
 if(row[6][0] == '\0')
  return(1);

if(poi->metpoi == 0 )
 return(0);
if(proverka(poi->komu.ravno(),row[3],4,0) != 0)
 return(1);
if(proverka(poi->post.ravno(),row[4],4,0) != 0)
 return(1);
if(proverka(poi->nomdov.ravno(),row[1],0,0) != 0)
 return(1);

return(0);

}
/***************************************/
/*Меню для ввода реквизитов поиска*/
/************************************/

int dirdovs_mpoi(class dirdovs_poi *poi)
{
VVOD VV(0);


VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска........"));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.....(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Кому................(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Поставщик...........(,,).."));//4

VV.VVOD_SPISOK_add_data(poi->datan.ravno(),11);
VV.VVOD_SPISOK_add_data(poi->datak.ravno(),11);
VV.VVOD_SPISOK_add_data(poi->nomdov.ravno(),100);
VV.VVOD_SPISOK_add_data(poi->komu.ravno(),100);
VV.VVOD_SPISOK_add_data(poi->post.ravno(),100);

naz:;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
//"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kl=VV.vvod(0,1,1,-1,-1);

switch(kl)
 {
  case FK10:
  case ESC:
    poi->metpoi=0;
    break;
    
  case FK2:
  case PLU:
    GDITE();
    poi->datan.new_plus(VV.data_ravno(0));
    poi->datak.new_plus(VV.data_ravno(1));
    poi->nomdov.new_plus(VV.data_ravno(2));
    if(poi->datan.prov_dat() != 0)
     {
      iceb_t_soob(gettext("Не правильно введена дата начала!"));
      goto naz;
     }
    if(poi->datak.prov_dat() != 0)
     {
      iceb_t_soob(gettext("Не правильно введена дата конца!"));
      goto naz;
     }
    poi->nomdov.new_plus(VV.data_ravno(2));
    poi->komu.new_plus(VV.data_ravno(3));
    poi->post.new_plus(VV.data_ravno(4));

    poi->metpoi=1;
    break;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

 }

return(0);

}
/****************************************/
void dirdovs_hap(int nom_list,int *kolstr,FILE *ff)
{
*kolstr=4;
fprintf(ff,"%130s %s N%d\n","",gettext("Страница"),nom_list);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Дата док. |Номер док.|Действ. до|   Кому выдана      |    Поставщик       |          Дата и номер наряда           |Отметка об использовании |Подпись\n"));
/*
1234567890 1234567890 1234567890 12345678901234567890 12345678901234567890 1234567890123456789012345678901234567890 1234567890123456789012345 
*/

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}



/*********************************/
/********************************/
void dirdovs_hp(class dirdovs_poi *poi,FILE *ff)
{
iceb_t_zagolov(gettext("Реестр доверенностей"),poi->datan.ravno(),poi->datak.ravno(),ff);

if(poi->metka_isp == 1)
  fprintf(ff,"%s\n",gettext("Список не использованных документов"));
if(poi->metka_isp == 2)
  fprintf(ff,"%s\n",gettext("Список использованных документов"));

if(poi->komu.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Кому"),poi->komu.ravno());
if(poi->post.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Поставщик"),poi->post.ravno());
if(poi->nomdov.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Номер документа"),poi->nomdov.ravno());
int kolstr;
dirdovs_hap(1,&kolstr,ff);

}


/*********************/
/*распечатка*/
/*****************/

void dirdovs_rsd(class dirdovs_poi *poi,class SQLCURSOR *cur)
{
char strsql[1024];
SQL_str row,row1;
char imaf1[64];
char imaf2[64];

class SQLCURSOR cur1;
class iceb_tu_str datainom("");

sprintf(imaf1,"rdov%d.lst",getpid());
class iceb_fopen fil1;
if(fil1.start(imaf1,"w") != 0)
 return;

dirdovs_hp(poi,fil1.ff);

sprintf(imaf2,"rdov2%d.lst",getpid());
class iceb_fopen fil2;
if(fil2.start(imaf2,"w") != 0)
 return;

dirdovs_hp(poi,fil2.ff);

cur->poz_cursor(0);
class iceb_tu_str data_dd("");
while(cur->read_cursor(&row) != 0)
 {
  if(dirdovs_poi(row,poi) != 0)
   continue;
  
  data_dd.new_plus(iceb_tu_datzap(row[2]));
  
  datainom.new_plus(iceb_tu_datzap(row[7]));
  datainom.plus(" ",row[8]);
  
  fprintf(fil1.ff,"%-10s|%-*s|%-10s|%-*.*s|%-*.*s|%-*.*s|%-*s|\n",
  iceb_tu_datzap(row[0]),
  iceb_tu_kolbait(10,row[1]),
  row[1],
  data_dd.ravno(),
  iceb_tu_kolbait(20,row[3]),
  iceb_tu_kolbait(20,row[3]),
  row[3],
  iceb_tu_kolbait(20,row[4]),
  iceb_tu_kolbait(20,row[4]),
  row[4],
  iceb_tu_kolbait(40,datainom.ravno()),
  iceb_tu_kolbait(40,datainom.ravno()),
  datainom.ravno(),
  iceb_tu_kolbait(25,row[6]),
  row[6]);  

  sprintf(strsql,"select zapis,ei,kol from Uddok1 where datd='%s' and nomd='%s'",row[0],row[1]);
  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return;
   }

  while(cur1.read_cursor(&row1) != 0)
    fprintf(fil1.ff,"%1s %s %s %s\n","",row1[0],row1[1],row1[2]);     

fprintf(fil1.ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  fprintf(fil2.ff,"%-10s|%-*s|%-10s|%-*.*s|%-*.*s|%-*.*s|%-*s|\n",
  iceb_tu_datzap(row[0]),
  iceb_tu_kolbait(10,row[1]),
  row[1],
  data_dd.ravno(),
  iceb_tu_kolbait(20,row[3]),
  iceb_tu_kolbait(20,row[3]),
  row[3],
  iceb_tu_kolbait(20,row[4]),
  iceb_tu_kolbait(20,row[4]),
  row[4],
  iceb_tu_kolbait(40,datainom.ravno()),
  iceb_tu_kolbait(40,datainom.ravno()),
  datainom.ravno(),
  iceb_tu_kolbait(25,row[6]),
  row[6]);  

fprintf(fil2.ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

 }
fprintf(fil1.ff,"ICEB_LST_END\n\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(fil2.ff,"ICEB_LST_END\n\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

podpis(fil1.ff);
fil1.end();

podpis(fil2.ff);
fil2.end();

class spis_oth oth;   
oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Реестр доверенностей с содержимым доверенностей"));
oth.spis_imaf.plus(imaf2);
oth.spis_naim.plus(gettext("Реестр доверенностей"));

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 {
  int orient=iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);
  iceb_t_rnl(oth.spis_imaf.ravno(nom),orient,"-",&dirdovs_hap); /*разбиваем на листы со вставкой шапки*/
 }
iceb_t_rabfil(&oth,"");

}
