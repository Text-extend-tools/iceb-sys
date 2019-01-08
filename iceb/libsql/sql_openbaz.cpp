/* $Id: sql_openbaz.c,v 5.10 2013/05/17 14:56:37 sasa Exp $ */
/*12.06.2009	26.01.1999	Белых А.И.	sql_openbaz.c
Открыть SQL базу данных
Если вернули 0 - база открыта
*/
#include        <nl_types.h>
//#include        <scr.h>
#include	"libsql.h"

extern nl_catd	fils; /*Указатель на базу сообщений*/

int sql_openbaz(SQL_baza *bd,const char *imabaz,const char *host,const char *kto,const char *parol)
{

if(mysql_init(bd) == NULL)
  printf("\n%s\n",mysql_error(bd));

if(mysql_real_connect(bd,host,kto,parol,imabaz,0,NULL,0) == NULL)
 { 
//printw(catgets(fils,3,58,"Связываемся с демоном базы.\n"));
//  printw("%s\n",mysql_error(bd));
//  OSTANOV();

  return(1);

 } 

//printf(catgets(fils,3,59,"Открываем базу данных.\n"));
if(imabaz != NULL)
if(imabaz[0] != '\0')
if(mysql_select_db(bd,imabaz) < 0)
 { 
//printw(catgets(fils,3,59,"Открываем базу данных.\n"));
//  printw("%s\n",mysql_error(bd));
//  OSTANOV();
  return(1);
 } 

return(0);

}
