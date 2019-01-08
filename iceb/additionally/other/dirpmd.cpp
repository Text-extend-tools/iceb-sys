/*31.08.2017	10.06.2004	Белых 	dirpmd.c
Просмотр записей с показаниями счетчика
*/
#include <iceb.h>
#include <buhg.h>
#include <errno.h>
class poi
{
 public:
 char nomd[10];  // Номер дома
 char nom[10];   // Номер квартиры
 char data[8];   // Дата
 char ps[20];    // Показания счётчика
 short mp;       // 0 - Не искать, 1 - Искать
 char zapros[300]; 
 poi()
 {
    memset(nomd,0,sizeof(nomd));
  memset(nom,0,sizeof(nom));
  memset(data,0,sizeof(data));
  memset(ps,0,sizeof(ps));
  mp=0;
  
 }
 
};

void udal(char*,char *nk,short m, short g);
void poisk(class poi*);
void dirpmds(class poi*);
int energy(const char*,const char*,short,short);
void pechat(SQLCURSOR *cur,class poi *data);
extern SQL_baza	bd;

void dirpmd()
{
poi qw;
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
if(qw.mp==0)
 sprintf(strsql,"select * from Kuspm order by data desc");
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
spis_menu.spis_str.free_class();
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  rsdat(&dz,&mz,&gz,row[2],2);
  
  sprintf(strsql,"%-4s|%-4s|%02d.%02d|%-5s|%s",
  row[0],row[1],mz,gz,row[3],row[6]);

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
      energy("","",0,0);
      goto naz1; 
  case PLU:
  case ENTER:
  case SFK2:  
      energy(nomd,nomkvrt,mz,gz);
      goto naz1;

  case FK3:  
   udal(nomd,nomkvrt, mz, gz);
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

void dirpmds(class poi *data)
{

move (0,0);
printw("Просмотр списка показаний счётчика");

move(1,0);
printw("    1    2      3     4     5");
int nomer=0;
move (nomer++,COLS-30);
printw ("Расшифровка полей.");
move (nomer++,COLS-30);
printw ("1 - Номер дома.");
move (nomer++,COLS-30);
printw ("2 - Номер квартиры.");
move (nomer++,COLS-30);
printw ("3 - Дата записи.");
move (nomer++,COLS-30);
printw ("4 - Показания эл.-счётчика.");
move (nomer++,COLS-30);
printw ("5 - Показания счётчика воды.");

if(data->mp!=0)
{
nomer+=4;



move (nomer++,COLS-25);
printw ("Поиск по:");



if(data->nomd[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Номеру дома:%s",data->nomd);
}
if(data->nom[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Номеру квартиры :%s",data->nom );
}
if(data->data[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Дате записи: %s",data->data);
}
if(data->ps[0]!='\0')
{
 move (nomer++,COLS-25);
 printw ("Показанию счётчика:%s",data->ps);
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


void poisk(class poi *data)
{
VVOD  VV(0);
VV.VVOD_SPISOK_add_ZAG("Поиск квартиры.");

VV.VVOD_SPISOK_add_MD("Номер дома..............");
VV.VVOD_SPISOK_add_MD("Номер квартиры..........");
VV.VVOD_SPISOK_add_MD("Дата....................");
VV.VVOD_SPISOK_add_MD("Показания счётчика......");

VV.VVOD_SPISOK_add_data(data->nomd,sizeof(data->nomd));
VV.VVOD_SPISOK_add_data(data->nom,sizeof(data->nom));
VV.VVOD_SPISOK_add_data(data->data,sizeof(data->data));
VV.VVOD_SPISOK_add_data(data->ps,sizeof(data->ps));


naz:;

helstr(LINES-1,0,
"F1","Помощь",
"F2/+","Найти",
"F10","Выход",NULL);


int kom=VV.vvod(0,1,1,-1,-1);

strncpy(data->nomd,VV.VVOD_SPISOK_return_data(0),sizeof(data->nomd)-1);
strncpy(data->nom,VV.VVOD_SPISOK_return_data(1),sizeof(data->nom)-1);
strncpy(data->data,VV.VVOD_SPISOK_return_data(2),sizeof(data->data)-1);
strncpy(data->ps,VV.VVOD_SPISOK_return_data(3),sizeof(data->ps)-1);


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

if(data->data[0]!='\0')
if(rsdat1(&m,&g,data->data) != 0)
 {
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD("Не правильно введена дата!");
   soobshw(&SOOB,stdscr,-1,-1,0,1);
 goto naz;
 }

char strsql[200];
memset(strsql,'\0',sizeof(strsql));
sprintf(data->zapros,"select * from Kuspm where ");
short mt=0;


if(data->nomd[0] != '\0')
 {
  sprintf(strsql,"nd='%s'",data->nomd);
  strcat(data->zapros,strsql);  
  mt++;
 }
if(data->nom[0] != '\0')
 {
  if (mt==0)
  sprintf(strsql,"nk=%s",data->nom);
  else
  sprintf(strsql," and nk=%s",data->nom);
  strcat(data->zapros,strsql);  
  mt++;
 }

if(data->data[0] != '\0')
 {
 if (mt==0)
  sprintf(strsql,"data >='%d-%d-01' and data <='%d-%d-31'",g,m,g,m);
  else
  sprintf(strsql," and data='%d-%d-01' and data <='%d-%d-31'",g,m,g,m);
  strcat(data->zapros,strsql);  
  mt++;
 }


if(data->ps[0] != '\0')
 {
 if (mt==0)
  sprintf(strsql,"ps=%s",data->ps);
  else
  sprintf(strsql," and ps=%s",data->ps);
  strcat(data->zapros,strsql);  
  mt++;
 }
strcat(data->zapros," order by data desc");
}


/***********************************/
/*Удаление*/
/*******************************************/


void udal(char *nd,char *nk,short m, short g)
{


VVOD DANET(1);


DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
 return;



short d=1;
char zapros[300];
sprintf(zapros,"delete from Kuspm where nd='%s' and nk='%s' and data='%d-%d-%d'",nd,nk,g,m,d);

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
}


/**************************************************/
/*Печать*/
/*********************************/


void pechat(SQLCURSOR *cur,class poi *data)
{
int summa=0;
short dz,mz,gz;
SQL_str row;

FILE *ff;
char imaf[100];
sprintf(imaf,"kuspm%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,0);
  return;
 }
startfil(ff);


fprintf(ff,"Распечатка списка показаний счётчика\n");

if(data->mp!=0)
{
  fprintf (ff,"Поиск по:\n");
  if(data->nomd[0]!='\0')
  {
   fprintf (ff,"Номеру дома:%s\n",data->nomd);
  }
  if(data->nom[0]!='\0')
  {
   fprintf (ff,"Номеру квартиры:%s\n",data->nom );
  }
  if(data->data[0]!='\0')
  {
   fprintf (ff,"Дате записи:%s\n",data->data);
  }
  if(data->ps[0]!='\0')
  {
   fprintf (ff,"Показанию счётчика:%s\n",data->ps);
  }
}


fprintf(ff,"\
---------------------------------------\n\
N Дома |N кв |  Дата |  Показ счётч.  |\n\
---------------------------------------\n");
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  summa+=atoi(row[3]);
  rsdat(&dz,&mz,&gz,row[2],2);
  fprintf(ff, "%7s %5s %02d.%04d %-17s \n",row[0],row[1],mz,gz,row[3]);
 } 
fprintf(ff,"\
----------------------------------\n");
fprintf(ff,"%14s %17d","Итого",summa);
podpis(ff);


fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus("Распечатка списка показаний счётчика");
iceb_t_rabfil(&oth,"");
}
