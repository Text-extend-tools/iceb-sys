/* $Id: nomkmf.c,v 5.9 2013/08/13 05:49:49 sasa Exp $ */
/*05.08.2013	09.03.2000	Белих А.И.	nomkmf.c
Автоматическое определение кода материалла и услуги
*/
#include "bezcurses.h"



/************************/
int nomkm(int nom_start)
{
int             i;
char		strsql[100];
SQL_str         row;
long		kolstr;

//printw("\n%s-nom_start=%d\n",__FUNCTION__,nom_start);
//OSTANOV();

i=nom_start;


sprintf(strsql,"select kodm from Material where kodm > %d order by kodm asc",nom_start);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);

 }

if(kolstr >= 0)
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  i++;
naz:;
  if(i != atoi(row[0]))
   {
//    printw("i=%d row[0]=%s\n",i,row[0]);
    sprintf(strsql,"select kodus from Uslugi where kodus=%d",i);
    if(readkey(strsql) <= 0)
     {
      return(i);
     }
    i++;
    goto naz;
   }
 }

//printw("\ni=%d\n",i);
//OSTANOV();

sprintf(strsql,"select kodus from Uslugi where kodus > %d order by kodus asc",nom_start);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    msql_error(&bd,__FUNCTION__,strsql);
   }
  else
   {
//    printw("kolstr=%d\n",kolstr);
//    OSTANOV();
    i++;
    return(i);
   }
 }

while(cur.read_cursor(&row) != 0)
 {
  i++;

naz1:;
  if(i != atoi(row[0]))
   {
    sprintf(strsql,"select kodm from Material where kodm=%d",i);
    if(readkey(strsql) == 0)
     {
      sprintf(strsql,"select kodus from Uslugi where kodus=%d",i);
      if(sql_readkey(&bd,strsql) <= 0)
       {
        return(i);
       }
     }
    i++;
    goto naz1;
   }
 }
/*
printw("*i=%d\n",i);
OSTANOV();
*/
i++;

return(i);

}
/**********************************/
int nomkm()
{

return(nomkm(0));
}
