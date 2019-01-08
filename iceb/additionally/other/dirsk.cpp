/*01.09.2017	10.06.2004	Белых 	dirsk.c
Просмотр списка квартир
*/
#include <iceb.h>
#include <buhg.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


class pois
{
 public:
 char nomd[5];   // Номер дома
 char nom[10];   // Номер квартиры
 char fio[100];  // Фамилия имя отчество
 char kol[5];    // количество жильцов
 char sq[10];    // площадь вкартиры
 char sqk[10];   // площадь кухни
 char nv[2];     // Наличие ванны
 short mp;       // 0 - Не искать, 1 - Искать
 char zapros[300]; 

 pois()
 {
  memset (zapros,'\0',sizeof(zapros));
  memset (nomd,'\0',sizeof(nomd));
  memset (nom,'\0',sizeof(nom)); 
  memset (fio,'\0',sizeof(fio));
  memset (kol,'\0',sizeof(kol));
  memset (sq,'\0',sizeof(sq));
  memset (sqk,'\0',sizeof(sqk));
  memset (nv,'\0',sizeof(nv));

  mp=0;
  
 }
 
};

int karta(const char *nd,const char *nkv);
void udall(char *nd, char *nk);
void poisk(class pois*);
void dirpmds(class pois*);
void pechat(SQLCURSOR *cur,class pois *data);
extern SQL_baza	bd;

void dirsk()
{
class iceb_t_mstr spis_menu(0);
pois qw;
int pozz=0,pozz1=0,K=0;
char strsql[512];
int kolstr;
SQL_str row;
short		mspz=0; /*метка соответствия первой записи*/
int i=0;
short dz,mz,gz;
int kom=0;
short           x=0,y=2;
int prc=0;
char nomkvrt[64];
char nomdom[64];

SQLCURSOR curr;
VVOD VV(0);

VVOD SOOB(1);

naz1:;

GDITE();
clear();
short           kls=LINES-5;
if(qw.mp==0)
 sprintf(strsql,"select * from Kuskvart order by nd asc,nk asc");
else 
 strcpy(strsql,qw.zapros); 
SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
if(kolstr == 0)
 {

lll:;

  dirpmds(&qw);

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
char priznak[2];
char metka_radio[2];

spis_menu.spis_str.free_class();
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  rsdat(&dz,&mz,&gz,row[1],2);
  strcpy(priznak,"+");
  if (row[6][0]=='0')
   strcpy (priznak,"-");

  strcpy(metka_radio,"-");
  if (row[9][0]=='1')
   strcpy (metka_radio,"+");

  sprintf(strsql,"%-*s|%-*s|%-*.*s|%*s|%s|%s|%s|%s",
  iceb_tu_kolbait(5,row[0]),
  row[0],
  iceb_tu_kolbait(4,row[1]),
  row[1],
  iceb_tu_kolbait(20,row[2]),
  iceb_tu_kolbait(20,row[2]),
  row[2],
  iceb_tu_kolbait(2,row[3]),
  row[3],
  row[4],row[5],priznak,metka_radio);

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


dirpmds(&qw);

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

  POLE(spis_menu.spis_str.ravno(kom),nomdom,1,'|');
  POLE(spis_menu.spis_str.ravno(kom),nomkvrt,2,'|');

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
      karta("","");   
      goto naz1; 
 
  case PLU:
  case ENTER:
  case SFK2:  
     karta(nomdom,nomkvrt);
      goto naz1;

  case FK3:  
   udall(nomdom,nomkvrt);
   goto naz1;
 
  case FK4:  //Поиск
    poisk(&qw);
   goto naz1; 
 
  case FK5://Печать
    pechat(&cur,&qw);
    goto naz;
 
  case FK10:
  case ESC:
    return;
    
 }

}
/*****************************/
/*Шапка на экран*/
/***********************/

void dirpmds(class pois *data)
{

move (0,0);
printw("Просмотр списка квартир");

move(1,0);
printw("   1      2           3            4    5    6  7 8");
int nomer=0;
move (nomer++,COLS-25);
printw ("Расшифровка полей.");
move (nomer++,COLS-25);
printw ("1 - Номер дома.");
move (nomer++,COLS-25);
printw ("2 - Номер квартиры.");
move (nomer++,COLS-25);
printw ("3 - ФИО жильца.");
move (nomer++,COLS-25);
printw ("4 - Количество жильцов.");
move (nomer++,COLS-25);
printw ("5 - Площадь квартиры.");
move (nomer++,COLS-25);
printw ("6 - Площадь кухни.");
move (nomer++,COLS-25);
printw ("7 - Наличие ванны.");
move (nomer++,COLS-25);
printw ("8 - Наличие радио.");




if(data->mp!=0)
{
nomer+=4;



move (nomer++,COLS-25);
printw ("Поиск по:");
if(data->nomd[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Номеру дома:%s",data->nomd );
}
if(data->nom[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Номеру квартиры: %s",data->nom);
}
if(data->fio[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("ФИО жильца:%s",data->fio);
}

if(data->kol[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Количеству жильцов:%s",data->kol);
}

if(data->sq[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Площади квартиры:%s",data->sq);
}

if(data->sqk[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Площади кухни:%s",data->sqk);
}

if(data->nv[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Наличию ванны:%s",data->nv);
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


void poisk(class pois *data)
{
VVOD  VV(0);
VV.VVOD_SPISOK_add_ZAG("Поиск квартиры.");

VV.VVOD_SPISOK_add_MD("Номер дома..............");
VV.VVOD_SPISOK_add_MD("Номер квартиры..........");
VV.VVOD_SPISOK_add_MD("ФИО.....................");
VV.VVOD_SPISOK_add_MD("Количество жильцов......");
VV.VVOD_SPISOK_add_MD("Площадь квартиры........");
VV.VVOD_SPISOK_add_MD("Площадь кухни...........");
VV.VVOD_SPISOK_add_MD("Наличие ванны...........");

VV.VVOD_SPISOK_add_data(data->nomd,sizeof(data->nomd));
VV.VVOD_SPISOK_add_data(data->nom,sizeof(data->nom));
VV.VVOD_SPISOK_add_data(data->fio,sizeof(data->fio));
VV.VVOD_SPISOK_add_data(data->kol,sizeof(data->kol));
VV.VVOD_SPISOK_add_data(data->sq,sizeof(data->sq));
VV.VVOD_SPISOK_add_data(data->sqk,sizeof(data->sqk));
VV.VVOD_SPISOK_add_data(data->nv,sizeof(data->nv));

naz:;

helstr(LINES-1,0,
"F1","Помощь",
"F2/+","Найти",
"F10","Выход",NULL);


int kom=VV.vvod(0,1,1,-1,-1);


strncpy(data->nomd,VV.VVOD_SPISOK_return_data(0),sizeof(data->nomd)-1);
strncpy(data->nom,VV.VVOD_SPISOK_return_data(1),sizeof(data->nom)-1);
strncpy(data->fio,VV.VVOD_SPISOK_return_data(2),sizeof(data->fio)-1);
strncpy(data->kol,VV.VVOD_SPISOK_return_data(3),sizeof(data->kol)-1);
strncpy(data->sq,VV.VVOD_SPISOK_return_data(4),sizeof(data->sq)-1);
strncpy(data->sqk,VV.VVOD_SPISOK_return_data(5),sizeof(data->sqk)-1);
strncpy(data->nv,VV.VVOD_SPISOK_return_data(6),sizeof(data->nv)-1);

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



char strsql[512];
memset(strsql,'\0',sizeof(strsql));
sprintf(data->zapros,"select * from Kuskvart where ");
short mt=0;

if(data->nomd[0] != '\0')
 {
  sprintf(strsql," nd=%s",data->nomd);
  strcat(data->zapros,strsql);  
  mt++;
 }


if(data->nom[0] != '\0')
 {
  if(mt == 0)
    sprintf(strsql," nk=%s",data->nom);
  else
    sprintf(strsql," and nk=%s",data->nom);
  strcat(data->zapros,strsql);  
  mt++;
 }

if(data->fio[0] != '\0')
 {
  if(mt == 0)
  sprintf(strsql," fio=%s",data->fio);
  else
  sprintf(strsql," and fio=%s",data->fio);
  strcat(data->zapros,strsql);  
  mt++;
 }


if(data->kol[0] != '\0')
 {
 if(mt == 0) 
 sprintf(strsql," kj=%s",data->kol); 
 else 
 sprintf(strsql," and kj=%s",data->kol);  
  strcat(data->zapros,strsql);  
  mt++;
 }


if(data->sq[0] != '\0')
 {
 if(mt == 0) 
  sprintf(strsql," hsq=%s",data->sq);
else
 sprintf(strsql," and hsq=%s",data->sq); 
  strcat(data->zapros,strsql);  
  mt++;
 }


if(data->sqk[0] != '\0')
 {
  if(mt == 0) 
   sprintf(strsql," csq=%s",data->sqk);
  else
  sprintf(strsql," and csq=%s",data->sqk);
  strcat(data->zapros,strsql);  
  mt++;
 }

if(data->nv[0] != '\0')
 {
 if(mt == 0) 
  sprintf(strsql," van=%s",data->nv);
  else
   sprintf(strsql," and van=%s",data->nv);
  strcat(data->zapros,strsql);  
  mt++;
 }

strcat(data->zapros," order by nd asc,nk asc");

}


/***********************************/
/*Удаление*/
/*******************************************/


void udall(char *nd,char *nk)
{
VVOD DANET(1);


DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
 return;



char zapros[512];
sprintf(zapros,"delete from Kuskvart where nd=%s and nk='%s'",nd,nk);

if(sql_zap(&bd,zapros) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD(gettext("У вас нет полномочий для выполнения этой операции !"));
   soobshw(&SOOB,stdscr,-1,-1,0,1);

  }
 else
  msql_error(&bd,gettext("Ошибка удаления записи !"),zapros);
 }
sprintf(zapros,"delete from Kuspm where nk='%s'",nk);
sql_zapis(zapros,1,0);
sprintf(zapros,"delete from Kuslg where nk='%s'",nk);
sql_zapis(zapros,0,0);
}


/**************************************************/
/*Печать*/
/*********************************/


void pechat(SQLCURSOR *cur,class pois *data)
{
int summa=0;
short dz,mz,gz;
SQL_str row;

FILE *ff;
char imaf[512];
sprintf(imaf,"kuspm%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
startfil(ff);



fprintf(ff,"Распечатка списка квартир.\n");

if(data->mp!=0)
{
fprintf (ff,"Поиск по:\n");
if(data->nomd[0]!='\0')
{
 fprintf (ff,"Номеру дома:%s\n",data->nomd);
}
if(data->nom[0]!='\0')
{
 fprintf (ff,"Номеру квартиры:%s\n",data->nom);
}
if(data->fio[0]!='\0')
{
 fprintf (ff,"ФИО жильца:%s\n",data->fio);
}

if(data->kol[0]!='\0')
{
 fprintf (ff,"Количеству жильцов:%s\n",data->kol);
}

if(data->sq[0]!='\0')
{
 fprintf (ff,"Площади квартиры:%s\n",data->sq);
}

if(data->sqk[0]!='\0')
{
 fprintf (ff,"Площади кухни:%s\n",data->sqk);
}
if(data->nv[0]!='\0')
{
 fprintf (ff,"Наличию ванны:%s\n",data->nv);
}
}

char priznak[2];

fprintf(ff,"\
--------------------------------------------------------------------------------------\n\
N Дома | N кв |   Фамилия Имя Отчество Жильца     |К/ж|Площадь кв.|Площадь кух.|Ванна|\n\
--------------------------------------------------------------------------------------\n");
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  summa+=atoi(row[2]);
  rsdat(&dz,&mz,&gz,row[1],2);
  strcpy(priznak,"+");
  if (row[6][0]=='0')
   strcpy (priznak,"-");
  fprintf(ff,"%*s %*s %-*.*s %3s %11s %12s %4s\n",
  iceb_tu_kolbait(7,row[0]),
  row[0],
  iceb_tu_kolbait(6,row[1]),
  row[1],
  iceb_tu_kolbait(34,row[2]),
  iceb_tu_kolbait(34,row[2]),
  row[2],
  row[3],row[4],row[5],priznak);
  
 } 
fprintf(ff,"\
--------------------------------------------------------------------------------------\n");
podpis(ff);


fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus("Распечатка списка квартир");

}
