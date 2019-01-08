/*31.08.2017	10.06.2004	Белых 	dirlg.c
Просмотр записей с льготами
*/
#include <iceb.h>
#include <buhg.h>
#include <errno.h>
class poi
{
 public:
 class iceb_tu_str nomd;  // Номер дома
 class iceb_tu_str nomk;   // Номер квартиры
 class iceb_tu_str data;   // Дата
 short mp;       // 0 - Не искать, 1 - Искать

 char zapros[512]; 

 poi()
 {
  nomd.new_plus("");
  nomk.new_plus("");
  data.new_plus("");
  mp=0;
  
 }
 
};

void dirlg_ud(char*,char *nk,short m, short g);
void dirlg_poi(class poi*);
void dirlgs(class poi*);
int dirlg_v(const char*,const char*,short,short);
void dirlg_rs(SQLCURSOR *cur,class poi *data);
extern SQL_baza	bd;

void dirlg()
{
class poi qw;
int pozz=0,pozz1=0,K=0;
char strsql[300];
int kolstr;
SQL_str row;
short		mspz=0; /*метка соответствия первой записи*/
int i=0;
short dz,mz,gz;
int kom=0;
short           x=0,y=2;
int prc=0;
char nomkvrt[20];
char nomd[20];
class iceb_t_mstr spis_menu(0);

SQLCURSOR curr;
VVOD VV(0);

VVOD SOOB(1);

naz1:;

GDITE();
clear();
short           kls=LINES-5;
if(qw.mp == 0)
 sprintf(strsql,"select * from Kuslg order by dt desc,nd asc,nk asc");
else 
 strcpy(strsql,qw.zapros); 

class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
if(kolstr == 0)
 {

lll:;

  dirlgs(&qw);

  SOOB.VVOD_delete();

  SOOB.VVOD_SPISOK_add_MD("Не найдено ни одной записи !");
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


cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;

  pozz1++;
  rsdat(&dz,&mz,&gz,row[2],2);
  
  sprintf(strsql,"%-4s|%-4s|%02d.%02d|%5.5g|%5.5g|%5.5g|%5.5g|%5.5g",
  row[0],row[1],mz,gz,atof(row[3]),atof(row[4]),atof(row[5]),atof(row[6]),atof(row[7]));

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


dirlgs(&qw);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);

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

  POLE(spis_menu.spis_str.ravno(kom),nomd,1,'|');
  POLE(spis_menu.spis_str.ravno(kom),nomkvrt,2,'|');
  POLE(spis_menu.spis_str.ravno(kom),strsql,3,'|');
  rsdat1(&mz,&gz,strsql);

 }

vper:;

switch(K)
 {  

  case FK1: //Просмотр документации
    GDITE();

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK2:  
      dirlg_v("","",0,0);
      goto naz1; 

  case PLU:
  case ENTER:
  case SFK2:  
      dirlg_v(nomd,nomkvrt,mz,gz);
      goto naz1;

  case FK3:  
   dirlg_ud(nomd,nomkvrt, mz, gz);
   goto naz1;

  case FK4:  //Поиск
    dirlg_poi(&qw);
    goto naz1; 

  case FK5://Печать
    dirlg_rs(&cur,&qw);
    clear();
    goto naz;

  case FK10:
  case ESC:
    return;
    
 }

}
/*****************************/
/*Шапка на экран*/
/***********************/

void dirlgs(class poi *data)
{

move (0,0);
printw("Перегляд списку пільг");

move(1,0);
printw("    1    2      3     4     5     6     7     8");
int nomer=0;
move (nomer++,COLS-20);
printw ("Расшифровка полей");
move (nomer++,COLS-20);
printw ("1-Номер дома");
move (nomer++,COLS-20);
printw ("2-Номер квартиры");
move (nomer++,COLS-20);
printw ("3-Дата записи");
move (nomer++,COLS-20);
printw ("4-утримання будинку");
move (nomer++,COLS-20);
printw ("5-опалення");
move (nomer++,COLS-20);
printw ("6-елеткроенергія");
move (nomer++,COLS-20);
printw ("7-вода");
move (nomer++,COLS-20);
printw ("8-газ");

if(data->mp!=0)
{
nomer+=4;



move (nomer++,COLS-20);
printw ("Пошук по:");



if(data->nomd.ravno()[0]!='\0')
{
 move (nomer++,COLS-20);
 printw ("Номер дома:%s",data->nomd.ravno());
}
if(data->nomk.ravno()[0]!='\0')
{
 move (nomer++,COLS-20);
 printw ("Номер квартіри:%s",data->nomk.ravno() );
}
if(data->data.ravno()[0]!='\0')
{
 move (nomer++,COLS-20);
 printw ("Дата запису:%s",data->data.ravno());
}

 
}



helstr(LINES-1,0,
"F1","Помощь",
"F2/+","Ввод",
"SF2","Корректировка",
"F3","Удаление",
"F4","Поиск",
"F5","Печать",
"F10","Выход",NULL);

}

/*******************************/
/*Поиск*/
/**********************/


void dirlg_poi(class poi *data)
{
VVOD  VV(0);
VV.VVOD_SPISOK_add_ZAG("Поиск льгот");

VV.VVOD_SPISOK_add_MD("Номер дома..............");
VV.VVOD_SPISOK_add_MD("Номер квартиры..........");
VV.VVOD_SPISOK_add_MD("Дата....................");

VV.VVOD_SPISOK_add_data(data->nomd.ravno(),10);
VV.VVOD_SPISOK_add_data(data->nomk.ravno(),10);
VV.VVOD_SPISOK_add_data(data->data.ravno(),8);


naz:;

helstr(LINES-1,0,
"F1","Помощь",
"F2/+","Найти",
"F10","Выход",NULL);


int kom=VV.vvod(0,1,1,-1,-1);

data->nomd.new_plus(VV.VVOD_SPISOK_return_data(0));
data->nomk.new_plus(VV.VVOD_SPISOK_return_data(1));
data->data.new_plus(VV.VVOD_SPISOK_return_data(2));


switch(kom)
 {
  case FK1:
    goto naz;

  case FK2:
   data->mp=1;
    break;

  case FK10:
     data->mp=0;
     return;
 }


short m=0,g=0;

if(data->data.ravno()[0]!='\0')
if(rsdat1(&m,&g,data->data.ravno()) != 0)
 {
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD("Не правильно введена дата!");
   soobshw(&SOOB,stdscr,-1,-1,0,1);
 goto naz;
 }

char strsql[200];
memset(strsql,'\0',sizeof(strsql));
sprintf(data->zapros,"select * from Kuslg where ");
short mt=0;


if(data->nomd.ravno()[0] != '\0')
 {
  sprintf(strsql,"nd='%s'",data->nomd.ravno());
  strcat(data->zapros,strsql);  
  mt++;
 }

if(data->nomk.ravno()[0] != '\0')
 {
  if (mt==0)
  sprintf(strsql,"nk=%s",data->nomk.ravno());
  else
  sprintf(strsql," and nk=%s",data->nomk.ravno());
  strcat(data->zapros,strsql);  
  mt++;
 }

if(data->data.ravno()[0] != '\0')
 {
 if (mt==0)
  sprintf(strsql,"data >='%d-%d-01' and data <='%d-%d-31'",g,m,g,m);
  else
  sprintf(strsql," and data='%d-%d-01' and data <='%d-%d-31'",g,m,g,m);
  strcat(data->zapros,strsql);  
  mt++;
 }
strcat(data->zapros," order by dt desc,nd asc,nk asc");

}


/***********************************/
/*Удаление*/
/*******************************************/


void dirlg_ud(char *nd,char *nk,short m, short g)
{


VVOD DANET(1);


DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
 return;



short d=1;
char zapros[300];
sprintf(zapros,"delete from Kuslg where nd='%s' and nk='%s' and dt='%d-%d-%d'",nd,nk,g,m,d);

if(sql_zap(&bd,zapros) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));

  }
 else
  msql_error(&bd,gettext("Ошибка удаления записи !"),zapros);
 }
}


/**************************************************/
/*Печать*/
/*********************************/


void dirlg_rs(SQLCURSOR *cur,class poi *data)
{
int summa=0;
short dz,mz,gz;
SQL_str row;

FILE *ff;
char imaf[100];
sprintf(imaf,"kuslg%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,0);
  return;
 }


iceb_t_zagolov("Список пільг",ff);

if(data->mp!=0)
{
 fprintf (ff,"Поиск по:\n");
 if(data->nomd.ravno()[0]!='\0')
  {
   fprintf (ff,"Номеру дома:%s\n",data->nomd.ravno());
  }
 if(data->nomk.ravno()[0]!='\0')
  {
   fprintf (ff,"Номеру квартиры:%s\n",data->nomk.ravno() );
  }
 if(data->data.ravno()[0]!='\0')
  {
   fprintf (ff,"Дате:%s\n",data->data.ravno());
  }
}
#define MAS 5
double itogo[MAS];
memset(itogo,'\0',sizeof(itogo));

fprintf(ff,"\
-----------------------------------------------------------------------------\n\
N Дома |N кв |  Дата |Утрим.буд.| Опалення |Електроен.|   Вода   |  Газ     |\n\
-----------------------------------------------------------------------------\n");
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  summa+=atoi(row[3]);
  rsdat(&dz,&mz,&gz,row[2],2);
  fprintf(ff, "%7s|%5s|%02d.%04d|%10s|%10s|%10s|%10s|%10s|\n",row[0],row[1],mz,gz,row[3],row[4],row[5],row[6],row[7]);
  for(int nom=0; nom < MAS; nom++)
    itogo[nom]+=atof(row[nom+3]);
  
 } 
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,"%*s|",iceb_tu_kolbait(21,"Итого"),"Итого");
for(int nom=0; nom < MAS; nom++)
  fprintf(ff,"%10.2f|",itogo[nom]);
fprintf(ff,"\n");
podpis(ff);


fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus("Распечатка списка показаний счётчика");
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
/*******************************************/
/*ввод и корректировка*/
/************************************/
int dirlg_v(const char *nd,const char *nkv,short mz,short gz)
{
class iceb_tu_str nomd("");  // Номер дома
class iceb_tu_str nomk("");   // Номер квартиры
class iceb_tu_str data("");   // дата

class iceb_tu_str lub(""); /*Льгота на утримання будинку*/
class iceb_tu_str lop(""); /*Льгота на опалення*/
class iceb_tu_str lee(""); /*Льгота на електро-енергию*/
class iceb_tu_str lvd(""); /*Льгота на воду*/
class iceb_tu_str lgz(""); /*Льгота на газ*/

SQLCURSOR cur;
SQL_str   row;

char zapros[512];
memset (zapros,'\0',sizeof(zapros));


VVOD  VV(0);
if (nkv[0] =='\0')
 VV.VVOD_SPISOK_add_ZAG("Ввод данных за електроенергию");

if (nkv[0] != '\0')
 {
  nomd.new_plus(nd);
  
  VV.VVOD_SPISOK_add_ZAG("Корректировка данных счётчика");
  nomk.new_plus(nkv);

  sprintf(zapros,"%d.%d",mz,gz);
  data.new_plus(zapros);

  sprintf(zapros,"Квартира %s",nkv);
  VV.VVOD_SPISOK_add_ZAG(zapros);

  sprintf(zapros,"Дата записи %d.%d",mz,gz);
  VV.VVOD_SPISOK_add_ZAG(zapros);


  sprintf(zapros,"\
SELECT * from  Kuslg where nd='%s' and nk='%s' and dt='%d-%02d-%02d'",nd,nkv,gz,mz,1);

  if(readkey(zapros,&row,&cur) < 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD("Не найдена запись для корректировки !!!");
    SOOB.VVOD_SPISOK_add_MD(zapros);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return (1);
   }
  lub.new_plus(row[3]);  
  lop.new_plus(row[4]);  
  lee.new_plus(row[5]);  
  lvd.new_plus(row[6]);  
  lgz.new_plus(row[7]);  

  kzvz(row[8],row[9],&VV);  
 }




if (nkv[0] == '\0')
 {
  
VV.VVOD_SPISOK_add_MD("Номер дома.........");
VV.VVOD_SPISOK_add_MD("Номер квартиры.....");
VV.VVOD_SPISOK_add_MD("Дата......(м.г)....");
 }
VV.VVOD_SPISOK_add_MD("Утримання будику...");
VV.VVOD_SPISOK_add_MD("Опалення...........");
VV.VVOD_SPISOK_add_MD("Електроенергія.....");
VV.VVOD_SPISOK_add_MD("Вода...............");
VV.VVOD_SPISOK_add_MD("Газ................");


if (nkv[0] == '\0')
 {
    
  VV.VVOD_SPISOK_add_data(nomd.ravno(),10);
  VV.VVOD_SPISOK_add_data(nomk.ravno(),10);
  VV.VVOD_SPISOK_add_data(data.ravno(),8);
 }
VV.VVOD_SPISOK_add_data(lub.ravno(),10);
VV.VVOD_SPISOK_add_data(lop.ravno(),10);
VV.VVOD_SPISOK_add_data(lee.ravno(),10);
VV.VVOD_SPISOK_add_data(lvd.ravno(),10);
VV.VVOD_SPISOK_add_data(lgz.ravno(),10);



naz:;

helstr(LINES-1,0,
//"F1","Помощь",
"F2/+","Записать",
"F10","Выход",NULL);


int kom=VV.vvod(0,1,1,-1,-1);

if (nkv[0] == '\0')
 {
  nomd.new_plus(VV.VVOD_SPISOK_return_data(0));
  nomk.new_plus(VV.VVOD_SPISOK_return_data(1));
  data.new_plus(VV.VVOD_SPISOK_return_data(2));
  lub.new_plus(VV.data_ravno(3));
  lop.new_plus(VV.data_ravno(4));
  lee.new_plus(VV.data_ravno(5));
  lvd.new_plus(VV.data_ravno(6));
  lgz.new_plus(VV.data_ravno(7));
 }
else
 {
  lub.new_plus(VV.data_ravno(0));
  lop.new_plus(VV.data_ravno(1));
  lee.new_plus(VV.data_ravno(2));
  lvd.new_plus(VV.data_ravno(3));
  lgz.new_plus(VV.data_ravno(4));
 } 

switch(kom)
 {
  case FK1:
    goto naz;

  case FK2:
    break;

  case FK10:
     return(0);

  default:
   goto naz;
 }

sprintf(zapros,"select * from Kuskvart where nd='%s' and nk=%d",nomd.ravno(),nomk.ravno_atoi());
if(sql_readkey(&bd,zapros) != 1)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Такой квартиры в этом доме нету!");
  SOOB.VVOD_SPISOK_add_MD(zapros);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }


short d=1,m=0,g=0;
if(rsdat1(&m,&g,data.ravno()) != 0)
 {
  iceb_t_soob("Не правильно введена дата!");
  VV.VVOD_SPISOK_zapis_data(0,nkv);
  goto naz;
 }

if(nkv[0]=='\0') 
 {
  sprintf(zapros,"\
INSERT INTO Kuslg \
values ('%s','%s','%d-%d-%d',%.2f,%.2f,%.2f,%.2f,%.2f,%d,%ld)",
  nomd.ravno(),
  nomk.ravno(),
  g,m,d,
  lub.ravno_atof(),
  lop.ravno_atof(),
  lee.ravno_atof(),
  lvd.ravno_atof(),
  lgz.ravno_atof(),
  iceb_t_getuid(),
  time(NULL));
 }
else
  sprintf(zapros,"\
update Kuslg \
set \
ub=%.2f,\
op=%.2f,\
ee=%.2f,\
vd=%.2f,\
gz=%.2f,\
ktoi=%d,\
vrem=%ld \
where nk = '%s' and dt='%d-%d-%d' and nd='%s'", 
  lub.ravno_atof(),
  lop.ravno_atof(),
  lee.ravno_atof(),
  lvd.ravno_atof(),
  lgz.ravno_atof(),
  iceb_t_getuid(),
  time(NULL),
  nkv, gz, mz, 1,nd);

if(sql_zap(&bd,zapros) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return(1);
  }
 else
  {
   msql_error(&bd,"Ошибка записи !",zapros);
   return(1);
  }  
 }

return(0);

}
