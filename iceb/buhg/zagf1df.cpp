/*$Id: zagf1df.c,v 5.7 2013/08/13 05:50:01 sasa Exp $*/
/*28.09.2014	02.08.2011	Белых А.И.	zagf1df.c
Заголовок формы 1дф
*/

#include "buhg.h"


void	zagf1df(int kolrh, //Количество работников
int kolrs,//Количество работающих по договорам подряда
short kvrt,const char *kod,short TYP,
int *klst,int vidd, //1-вилучення
short dn,short mn,short gn,
short dk,short mk,short gk,
int metka_oth, //0-звітний 1-новий звітний 2-уточнюючий
FILE *ff1,FILE *ff2)
{
class iceb_tu_str stroka("");
time_t          tmm;
time(&tmm);

iceb_t_zagolov(gettext("Расчёт справки за квартал (форма N1ДФ)"),dn,mn,gn,dk,mk,gk,ff2);

fprintf(ff2,"%s:%d\n",gettext("Квартал"),kvrt);


int kkk=0;
spkvd(&kkk,ff2);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str adres("");
/*Узнаём адрес*/
sprintf(strsql,"select adres from Kontragent where kodkon='00'");
if(readkey(strsql,&row_alx,&cur_alx) == 1)
 adres.new_plus(row_alx[0]);

sprintf(strsql,"select str from Alx where fil='zarsf1df_1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsf1df.alx");
  iceb_t_soob(strsql);
  return;
 }

class iceb_tu_str kodni("");
class iceb_tu_str naim_ni("");
iceb_t_poldan("Код налоговой инспекции",&kodni,"zarsdf1df.alx");

iceb_t_poldan("Название налоговой инспекции",&naim_ni,"zarsdf1df.alx");

char period[128];
memset(period,'\0',sizeof(period));

if(kvrt == 1)
  sprintf(period,"I квартал %dр.",gn);
if(kvrt == 2)
  sprintf(period,"II квартал %dр.",gn);
if(kvrt == 3)
  sprintf(period,"III квартал %dр.",gn);
if(kvrt == 4)
  sprintf(period,"IV квартал %dр.",gn);

int nomstr=0;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  stroka.new_plus(row_alx[0]);
  switch(nomstr)
   {
    case 7:
     iceb_tu_vstav(&stroka,kod,1,27,2);
     break;

    case 10:
      if(metka_oth == 0) //звітний
       iceb_tu_vstav(&stroka,"X",88,94,1);
     break;

    case 12:
      if(TYP == 0) //юридическое лицо
       iceb_tu_vstav(&stroka,"X",2,3,1);
      if(metka_oth == 1) //новий звітний
       iceb_tu_vstav(&stroka,"X",87,94,1);
     break;

    case 14:
      if(TYP == 1) //физическое лицо
       iceb_tu_vstav(&stroka,"X",2,3,1);
      if(metka_oth == 2) //уточнюючий
       iceb_tu_vstav(&stroka,"X",87,94,1);
     break;

    case 19:
     iceb_tu_vstav(&stroka,iceb_t_get_pnk("00",0),16,114,1);
     break;

    case 22:
     iceb_tu_vstav(&stroka,adres.ravno(),17,114,1);
     break;    

    case 25:
     iceb_tu_vstav(&stroka,naim_ni.ravno(),27,114,1);
     break;

    case 26:
     iceb_tu_vstav(&stroka,kodni.ravno(),1,20,2);
     break;     
    case 28:
     iceb_tu_vstav(&stroka,period,37,106,1);
     break;
     
    case 31:
     if(kolrh != 0)
      {  
       iceb_tu_vstav(&stroka,kolrh,44,53,1);
      }
     break;   

    case 33:
     if(kolrs != 0)
      { 
       iceb_tu_vstav(&stroka,kolrs,44,53,1);
      }
     break;   
   }


  fprintf(ff1,"%s",stroka.ravno());  
  *klst+=1;
 }
 

}
