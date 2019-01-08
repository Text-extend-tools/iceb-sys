/* $Id: udpld.c,v 5.16 2013/05/17 14:56:13 sasa Exp $ */
/*19.09.2010	06.05.1998	Белых А.И.	udpld.c
Удаление платежного документа
Если вернули 0 удалена
             1 нет
*/ 
#include        "buhg.h"


int udpld(const char *tabl,short d,short m,short g,
const char *npp, //Номер документа
short mu, //*0-удалить все 1-только документ
const char *kodop,
int metkasoob, //0 - без сообщения 1 с сообщением
int tipz)
{
char		strsql[512];

if(mu == 0)
 {
  if(SRAV(tabl,"Pltp",0) == 0)
    if(udprgr(ICEB_MP_PPOR,d,m,g,npp,0,tipz) != 0)
     return(1);
  if(SRAV(tabl,"Pltt",0) == 0)
    if(udprgr(ICEB_MP_PTRE,d,m,g,npp,0,tipz) != 0)
     return(1);
 }

sprintf(strsql,"delete from %s where datd='%04d-%02d-%02d' and nomd='%s'",tabl,g,m,d,npp);
/*
printw("\n%s\n",strsql);
OSTANOV();
*/
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   if(metkasoob == 1)
    {
     iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
    }
   return(1);
  }
 else
  {
   msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
   return(1);
  }
 }
/*Удаляем комментарии*/ 
sprintf(strsql,"delete from %sz where datd='%04d-%02d-%02d' and nomd='%s'",tabl,g,m,d,npp);

sql_zapis(strsql,1,0);


if(mu == 1)
 return(0);
 
/*Удаляем подтверждающие записи*/
memset(strsql,'\0',sizeof(strsql));

if(SRAV(tabl,"Pltp",0) == 0)
  sprintf(strsql,"delete from Pzpd where tp=0 and datd='%04d-%02d-%02d' and nomd='%s'",g,m,d,npp);

if(SRAV(tabl,"Pltt",0) == 0)
  sprintf(strsql,"delete from Pzpd where tp=1 and datd='%04d-%02d-%02d' and nomd='%s'",g,m,d,npp);

if(strsql[0] != '\0')
  sql_zapis(strsql,1,0);


return(0);
}



