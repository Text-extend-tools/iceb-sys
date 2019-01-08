/* $Id: udkar.c,v 5.8 2013/05/17 14:56:13 sasa Exp $ */
/*10.10.2011    27.12.1994      Белых А.И.      udkar.c
Удаление карточки
Если вернули 0 - карточка удалена
	     1 - нет
*/
#include	"buhg.h"


short           udkar(int skl,int nk)
{
char		strsql[512];
long		kolstr;
/*
printw("udkar-skl=%d nk=%d\n",skl,nk);
OSTANOV();
*/
/*Если в карточке больше нет записей то карточку тоже надо удалить*/
sprintf(strsql,"select sklad from Zkart where sklad=%d and nomk=%d",
skl,nk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  /*Проверяем есть ли не подтвержденные ссылки на эту карточку*/
  /*Если более одной записи значит ссылка есть*/
  sprintf(strsql,"select datd from Dokummat1 where sklad=%d and nomkar=%d",
  skl,nk);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }
/*
  printw("strsqk=%s\nudkart-kolstr=%d\n",strsql,kolstr);
  OSTANOV();
*/
  if(kolstr == 1)
   {
    sprintf(strsql,"delete from Kart where sklad=%d and nomk=%d",
    skl,nk);

    sql_zapis(strsql,1,0);
    return(0);
   }
  else
    return(1);
  
 }
return(1);
}
