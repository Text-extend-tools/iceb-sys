/*31.08.2017	10.06.2004	Белых А.И.	karta.c 
Ввод новой квартиры
*/

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <iceb.h>
#include <buhg.h>



int karta(const char *nd,const char *nkv)
{
char nomd[10];  // Номер дома
char nom[10];   // Номер квартиры
char fio[512];  // Фамилия имя отчество
char kol[5];    // количество жильцов
char sq[10];    // площаь вкартиры
char sqk[10];   // площадь кухни
char nv[2];     // Наличие ванны
char rt[2];     // Наличие радиоточки
char kontr[64]; //Код контрагента
char npk[10]; //Неотапливаемая площадь квартиры
char mao[2];  //Метка автономного отопления
char nsv[2]; //наличие счётчика воды
char naim[512];
SQLCURSOR cur;
SQL_str   row;
time_t vrem;

char zapros[512];
memset (zapros,'\0',sizeof(zapros));
memset (nomd, '\0',sizeof(nomd));
memset (nom,'\0',sizeof(nom));
memset (fio,'\0',sizeof(fio));
memset (kol,'\0',sizeof(kol));
memset (sq,'\0',sizeof(sq));
memset (sqk,'\0',sizeof(sqk));
memset (nv,'\0',sizeof(nv));
memset (rt,'\0',sizeof(rt));
memset(kontr,'\0',sizeof(kontr));
memset(npk,'\0',sizeof(npk));
memset(mao,'\0',sizeof(mao));
memset(nsv,'\0',sizeof(nsv));

VVOD  VV(0);
if(nkv[0] == '\0')
VV.VVOD_SPISOK_add_ZAG("Ввод новой квартиры");

if (nkv[0] != '\0')
 {
  sprintf(zapros,"\
SELECT * from  Kuskvart where nd=%s and nk='%s'",nd, nkv);

  if(sql_readkey(&bd,zapros,&row,&cur) < 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD("Квартира не найдена !!!");
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return (1);
   }
  
  strncpy(nomd,row[0],sizeof(nomd)-1);
  strncpy(nom,row[1],sizeof(nom)-1);
  strncpy(fio,row[2],sizeof(fio)-1);
  strncpy(kol,row[3],sizeof(kol)-1);
  strncpy(sq,row[4],sizeof(sq)-1);
  strncpy(sqk,row[5],sizeof(sqk)-1);
  if(row[6][0]=='0' )
     strcpy(nv,"-");
  if (row[6][0]=='1') 
     strcpy(nv,"+"); 
  sprintf(zapros,"Номер дома:%s",nomd);
  VV.VVOD_SPISOK_add_ZAG(zapros);
  sprintf(zapros,"Номер квартиры:%s",nom);
  VV.VVOD_SPISOK_add_ZAG(zapros);
  vrem=atol(row[8]);
  int ktoi=atoi(row[7]);
  
  if(row[9][0]=='0' )
     strcpy(rt,"-");
  if (row[9][0]=='1') 
     strcpy(rt,"+"); 
  strncpy(kontr,row[10],sizeof(kontr)-1);
  strncpy(npk,row[11],sizeof(npk)-1);

  if(row[12][0]=='0' )
     strcpy(mao,"-");
  if (row[12][0]=='1') 
     strcpy(mao,"+"); 

  if(row[13][0]=='0' )
     strcpy(nsv,"-");
  if (row[13][0]=='1') 
     strcpy(nsv,"+"); 
  kzvz(ktoi,vrem,&VV);  
 }



clear();
if(nd[0] == '\0')
 {

  VV.VVOD_SPISOK_add_MD("Номер дома.......................");
  VV.VVOD_SPISOK_add_MD("Номер квартиры...................");
 }
VV.VVOD_SPISOK_add_MD("Фамилия Имя Отчество.............");//0
VV.VVOD_SPISOK_add_MD("Количество Жильцов...............");//1
VV.VVOD_SPISOK_add_MD("Площадь квартиры отапливаемая....");//2
VV.VVOD_SPISOK_add_MD("Площадь кухни....................");//3
VV.VVOD_SPISOK_add_MD("Наличие ванны..........(+/-).....");//4
VV.VVOD_SPISOK_add_MD("Наличие радиоточки........(+/-)..");//5
VV.VVOD_SPISOK_add_MD("Код контрагента..................");//6
VV.VVOD_SPISOK_add_MD("Площадь квартиры неотапливаемая..");//7
VV.VVOD_SPISOK_add_MD("Автономное отопление......(+/-)..");//8
VV.VVOD_SPISOK_add_MD("Наличие счётчика воды.....(+/-)..");//9

if(nd[0] == '\0')
 {
  VV.VVOD_SPISOK_add_data(nomd,sizeof(nomd));
  VV.VVOD_SPISOK_add_data(nom,sizeof(nom));
 }
VV.VVOD_SPISOK_add_data(fio,sizeof(fio));
VV.VVOD_SPISOK_add_data(kol,sizeof(kol));
VV.VVOD_SPISOK_add_data(sq,sizeof(sq));
VV.VVOD_SPISOK_add_data(sqk,sizeof(sqk));
VV.VVOD_SPISOK_add_data(nv,sizeof(nv));
VV.VVOD_SPISOK_add_data(rt,sizeof(rt));
VV.VVOD_SPISOK_add_data(kontr,sizeof(kontr));
VV.VVOD_SPISOK_add_data(npk,sizeof(npk));
VV.VVOD_SPISOK_add_data(mao,sizeof(mao));
VV.VVOD_SPISOK_add_data(nsv,sizeof(nsv));


naz:;

helstr(LINES-1,0,
"F1","Помощь",
"F2/+","Записать",
"F3","Контрагент",
"F10","Выход",NULL);


int kom=VV.vvod(0,1,1,-1,-1);

if(nd[0] == '\0')
 {
  strncpy(nomd,VV.VVOD_SPISOK_return_data(0),sizeof(nomd)-1);
  strncpy(nom,VV.VVOD_SPISOK_return_data(1),sizeof(nom)-1);
  strncpy(fio,VV.VVOD_SPISOK_return_data(2),sizeof(fio)-1);
  strncpy(kol,VV.VVOD_SPISOK_return_data(3),sizeof(kol)-1);
  strncpy(sq,VV.VVOD_SPISOK_return_data(4),sizeof(sq)-1);
  strncpy(sqk,VV.VVOD_SPISOK_return_data(5),sizeof(sqk)-1);
  strncpy(nv,VV.VVOD_SPISOK_return_data(6),sizeof(nv)-1);
  strncpy(rt,VV.VVOD_SPISOK_return_data(7),sizeof(rt)-1);
  strncpy(kontr,VV.VVOD_SPISOK_return_data(8),sizeof(kontr)-1);
  strncpy(npk,VV.VVOD_SPISOK_return_data(9),sizeof(npk)-1);
  strncpy(mao,VV.VVOD_SPISOK_return_data(10),sizeof(mao)-1);
  strncpy(nsv,VV.VVOD_SPISOK_return_data(11),sizeof(mao)-1);
 }
else
 {
  strncpy(fio,VV.VVOD_SPISOK_return_data(0),sizeof(fio)-1);
  strncpy(kol,VV.VVOD_SPISOK_return_data(1),sizeof(kol)-1);
  strncpy(sq,VV.VVOD_SPISOK_return_data(2),sizeof(sq)-1);
  strncpy(sqk,VV.VVOD_SPISOK_return_data(3),sizeof(sqk)-1);
  strncpy(nv,VV.VVOD_SPISOK_return_data(4),sizeof(nv)-1);
  strncpy(rt,VV.VVOD_SPISOK_return_data(5),sizeof(rt)-1);
  strncpy(kontr,VV.VVOD_SPISOK_return_data(6),sizeof(kontr)-1);
  strncpy(npk,VV.VVOD_SPISOK_return_data(7),sizeof(npk)-1);
  strncpy(mao,VV.VVOD_SPISOK_return_data(8),sizeof(mao)-1);
  strncpy(nsv,VV.VVOD_SPISOK_return_data(9),sizeof(mao)-1);
 } 

switch(kom)
 {
  case FK1:
    goto naz;

  case FK2:
    break;

  case FK3:
    vibrek("Kontragent",kontr,sizeof(kontr),naim);
    if(nd[0] == '\0')
     VV.VVOD_SPISOK_zapis_data(8,kontr);
    else
     VV.VVOD_SPISOK_zapis_data(6,kontr);
    goto naz;
    
  case FK10:
     return(0);


 }

if (nkv[0]=='\0')
{
 sprintf(zapros,"select * from Kuskvart where nd='%s' and nk='%s'",nomd, nom);
 if(sql_readkey(&bd,zapros) >= 1)
  {
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD("Номер квартиры уже введен в базу !");
   soobshw(&SOOB,stdscr,-1,-1,0,1);
   goto naz;
  }
}

if (nv[0] != '+' && nv[0] != '-')
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Не правильно введен признак наличия ванны !!");
  SOOB.VVOD_SPISOK_add_MD("Введите '+' или '-' !");
  SOOB.VVOD_SPISOK_add_MD(nv);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }

if (nsv[0] != '+' && nsv[0] != '-')
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Не правильно введен признак наличия счётчика воды !!");
  SOOB.VVOD_SPISOK_add_MD("Введите '+' или '-' !");
  SOOB.VVOD_SPISOK_add_MD(nv);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }

if (rt[0] != '+' && rt[0] != '-')
 {
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD("Не правильно введен признак наличия радиоточки !!");
   SOOB.VVOD_SPISOK_add_MD("Введите '+' или '-' !");
   soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }
if(kontr[0] != '\0')
 {
  sprintf(zapros,"select kodkon from Kontragent where kodkon='%s'",kontr);
  if(readkey(zapros) != 1)
   {
    VVOD SOOB(1);
    sprintf(zapros,"Не найден код контрагента %s !",kontr);
    SOOB.VVOD_SPISOK_add_MD(zapros);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    goto naz;
   }
 }

int perem=0;
if(nv[0] == '+')
 perem=1;

int radio=0;
if(rt[0] == '+')
 radio=1;

int metka_ao=0;

if(mao[0] == '+')
 metka_ao=1;

int metka_nsv=0;
if(nsv[0] == '+')
 metka_nsv=1; 

time(&vrem);

if(nkv[0]=='\0') 
{
 sprintf(zapros,"\
INSERT INTO  Kuskvart \
values ('%s','%s','%s',%d,%.2f,%.2f,%d,%d,%ld,%d,'%s',%.2f,%d,%d)",
 nomd, nom,fio,atoi(kol),atof(sq),atof(sqk),perem,iceb_t_getuid(),vrem,radio,kontr,atof(npk),metka_ao,metka_nsv);
}
else
 sprintf(zapros,"\
update  Kuskvart \
set \
fio='%s',\
kj=%d,\
hsq=%.2f,\
csq=%.2f,\
van=%d,\
ktoi=%d,\
vrem=%ld,\
rt=%d, \
kk='%s',\
np=%.2f,\
mo=%d,\
sv=%d \
where nk = '%s' and nd='%s'", 
fio,atoi(kol),atof(sq),atof(sqk),perem,iceb_t_getuid(),vrem,radio,kontr,
atof(npk),metka_ao,metka_nsv,
nkv, nd);

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
return(0);
}
