/*01.01.2018	10.06.2004	Белых 	energyd.c
Ввод и корректировка показаний счётчиков
*/
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <iceb.h>
#include <buhg.h>



int energy(const char *nd,const char *nkv,short mz,short gz)
{
char nomd[10];  // Номер дома
char nom[10];   // Номер квартиры
char data[32];   // дата
char pl[20];    // показания електро-счётчика
char psdv[20];  // Показания счётчика для воды

SQLCURSOR cur;
SQL_str   row;
time_t vrem;

char zapros[512];
memset (zapros,'\0',sizeof(zapros));

memset (nomd,'\0',sizeof(nomd));
memset (nom,'\0',sizeof(nom));
memset (data,'\0',sizeof(data));
memset (pl,'\0',sizeof(pl));
memset (psdv,'\0',sizeof(psdv));

VVOD  VV(0);
if (nkv[0] =='\0')
 VV.VVOD_SPISOK_add_ZAG("Ввод данных за електроенергию");

if (nkv[0] != '\0')
 {
  strcpy(nomd,nd);
  
  VV.VVOD_SPISOK_add_ZAG("Корректировка данных счётчика");
  strcpy(nom,nkv);
  sprintf(data,"%d.%d",mz,gz);
  sprintf(zapros,"Квартира %s",nkv);
  VV.VVOD_SPISOK_add_ZAG(zapros);

  sprintf(zapros,"Дата записи %d.%d",mz,gz);
  VV.VVOD_SPISOK_add_ZAG(zapros);


  sprintf(zapros,"\
SELECT * from  Kuspm where nd='%s' and nk='%s' and data='%d-%d-%d'",nd ,nkv,gz,mz,1);

  if(sql_readkey(&bd,zapros,&row,&cur) < 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD("Не найдена запись для корректировки !!!");
    SOOB.VVOD_SPISOK_add_MD(zapros);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return (1);
   }
  
  strncpy(pl,row[3],sizeof(pl)-1);
  strncpy(psdv,row[6],sizeof(psdv)-1);

  vrem=atol(row[5]);
  int ktoi=atoi(row[4]);
  kzvz(ktoi,vrem,&VV);  
 }



clear();

if (nkv[0] == '\0')
 {
  
  VV.VVOD_SPISOK_add_MD("Номер дома...................");
  VV.VVOD_SPISOK_add_MD("Номер квартиры...............");
  VV.VVOD_SPISOK_add_MD("Дата......(м.г)..............");
 }
  VV.VVOD_SPISOK_add_MD("Показания електро-счётчика...");
  VV.VVOD_SPISOK_add_MD("Показания счётчика для воды..");


if (nkv[0] == '\0')
 {
    
  VV.VVOD_SPISOK_add_data(nomd,sizeof(nomd));
  VV.VVOD_SPISOK_add_data(nom,sizeof(nom));
  VV.VVOD_SPISOK_add_data(data,sizeof(data));
 }
VV.VVOD_SPISOK_add_data(pl,sizeof(pl));
VV.VVOD_SPISOK_add_data(psdv,sizeof(psdv));



naz:;

helstr(LINES-1,0,
"F1","Помощь",
"F2/+","Записать",
"F10","Выход",NULL);


int kom=VV.vvod(0,1,1,-1,-1);

if (nkv[0] == '\0')
 {
  
  strncpy(nomd,VV.VVOD_SPISOK_return_data(0),sizeof(nomd)-1);
  strncpy(nom,VV.VVOD_SPISOK_return_data(1),sizeof(nom)-1);
  strncpy(data,VV.VVOD_SPISOK_return_data(2),sizeof(data)-1);
  strncpy(pl,VV.VVOD_SPISOK_return_data(3),sizeof(pl)-1);
  strncpy(psdv,VV.VVOD_SPISOK_return_data(4),sizeof(psdv)-1);
 }
else
 {
  strncpy(pl,VV.VVOD_SPISOK_return_data(0),sizeof(pl)-1);
  strncpy(psdv,VV.VVOD_SPISOK_return_data(1),sizeof(psdv)-1);
 }
 
switch(kom)
 {
  case FK1:
    goto naz;

  case FK2:

    break;

  case FK10:
     return(0);


 }

sprintf(zapros,"select * from Kuskvart where nd='%s' and nk=%d",nomd, atoi(nom));
if(sql_readkey(&bd,zapros) != 1)
 {
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD("Такой квартиры в этом доме нету!");
   SOOB.VVOD_SPISOK_add_MD(zapros);
   soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }

time(&vrem);

short d,m,g;
d=1;
if(rsdat1(&m,&g,data) != 0)
 {
  iceb_t_soob("Не правильно введена дата!");
  VV.VVOD_SPISOK_zapis_data(0,nkv);
  goto naz;
 }

if(nkv[0]=='\0') 
 {
  sprintf(zapros,"\
INSERT INTO Kuspm \
values ('%s','%s','%d-%d-%d',%d,%d,%ld,%.2f)",
  nomd ,nom,g,m,d,atoi(pl),iceb_t_getuid(),vrem,ATOFM(psdv));
 }
else
  sprintf(zapros,"\
update Kuspm \
set \
ps=%d,\
ktoi=%d,\
vrem=%ld,\
voda=%.2f \
where nk = '%s' and data='%d-%d-%d' and nd='%s'", 
  atoi(pl),iceb_t_getuid(),vrem,ATOFM(psdv),
  nkv, gz, mz, 1,nd);

if(sql_zap(&bd,zapros) != 0)
 {
  if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   }
  else
   msql_error(&bd,"Ошибка записи !",zapros);
  goto naz;  
 }
return(0); 
}
