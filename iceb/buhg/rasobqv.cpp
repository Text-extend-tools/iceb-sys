/* $Id: rasobqv.c,v 5.38 2013/09/26 09:43:39 sasa Exp $ */
/*15.07.2015	14.10.1998	Белых А.И.	rasobqv.c
Распечатка объявки для сдачи налички в банк
*/
#include        <errno.h>
#include        "buhg.h"


void rasobqv(const char *dt0, //Номер объявки
const char *dt1, //Сумма
const char *dt2, //Дата
const char *dt3, //Наименование операции
const char *fam) //Фамилия
{
char           strok[1024];
char strsql[512];
class iceb_tu_str bros("");
SQLCURSOR       cur;
SQL_str         row;
class iceb_tu_str rassh(""); //Свой расчётный счет
FILE            *ff1;
char            imaf[64];
int metka_pn=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
iceb_t_poldan("Печать номера объявки",&bros,"nastdok.alx");
if(SRAV(bros.ravno(),"Вкл",1) != 0)
  metka_pn=1;



/*Читаем свои реквизиты*/
sprintf(strsql,"select naikon,nomsh,naiban,kod from Kontragent where kodkon='00'");

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_t_soob(gettext("Не найден код контрагента 00 !"));
  return;
 }
rassh.new_plus(row[1]);

sprintf(strsql,"select str from Alx where fil='gotivka3.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"gotivka3.alx");
  iceb_t_soob(strsql);
  return;
 }

sprintf(imaf,"got%d.lst",getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
int  shetshik=0;
short d,m,g;
rsdat(&d,&m,&g,dt2,0);
class iceb_tu_str mescc("");

mesc(m,1,&mescc);
char data1[64];
sprintf(data1,"%d %s %dр.",d,mescc.ravno(),g);
memset(strok,'\0',sizeof(strok));
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  memset(strok,'\0',sizeof(strok));
  strncpy(strok,row_alx[0],sizeof(strok)-1);
  shetshik++;
  switch (shetshik)
   {
    case 1: //номер объявки
     if(metka_pn == 0)
       iceb_tu_vstav(strok,dt0,54,63,1,sizeof(strok));
     break;

    case 2: //дата
    case 37: //дата

     iceb_tu_vstav(strok,data1,40,63,1,sizeof(strok));
     break;
    
    case 9: //название валюты номер счета сумма
    case 44: //название валюты номер счета сумма
     iceb_tu_vstav(strok,"UAN",3,9,1,sizeof(strok));
     iceb_tu_vstav(strok,rassh.ravno(),22,43,1,sizeof(strok));
     iceb_tu_vstav(strok,dt1,43,52,1,sizeof(strok));
     break;

    case 11: //общая сумма
    case 46: //общая сумма
     iceb_tu_vstav(strok,dt1,43,52,1,sizeof(strok));
     break;
    
    case 13: //плательщик
    case 48: //плательщик
//     iceb_tu_vstav(strok,naimork,8,74,1,sizeof(strok));
     break;

    case 20: //Сумма буквами
    case 55: //Сумма буквами
     preobr(ATOFM(dt1),&bros,0);
     iceb_tu_vstav(strok,bros.ravno(),14,74,1,sizeof(strok));
     break;

    case 22: //Назначение платежа
    case 57: //Назначение платежа
     iceb_tu_vstav(strok,dt3,20,74,1,sizeof(strok));
     break;

   }

  fprintf(ff1,"%s",strok);
  memset(strok,'\0',sizeof(strok));
 }

fclose(ff1);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Документ для сдачи налички в банк"));
iceb_t_ustpeh(imaf,0);
iceb_t_rabfil(&oth,"");

}
