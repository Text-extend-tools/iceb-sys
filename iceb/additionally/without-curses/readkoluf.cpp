/* $Id: readkolu.c,v 5.13 2013/05/17 14:56:09 sasa Exp $ */
/*09.11.2008	28.09.1997	Белых А.И.	readkolu.c
Получение количества подтвержденного по документа для услуг
*/
#include	"bezcurses.h"

double readkolu(short d,short m,short g,int podr,const char *nomd,
int tipz,int metka,int kodzap,int nomzap)
{
double		kol;
SQL_str         row;
char		strsql[512];
long		kolstr;

/*Читаем количество реально выданное-полученное*/
sprintf(strsql,"select kolih from Usldokum2 where datd='%04d-%02d-%02d' and \
nomd='%s' and podr=%d and tp=%d and metka=%d and kodzap=%d and nz=%d",
g,m,d,nomd,podr,tipz,metka,kodzap,nomzap);

//printw("\nreadkolu-strsql=%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printf("%s\n",sql_error(&bd));
  return(0.);
 }

if(kolstr == 0)
  return(0.);
 
kol=0.;
while(cur.read_cursor(&row) != 0)
 {
/*   printw("kol=%s\n",row[0]);*/
   kol+=atof(row[0]);
 } 
/*
printw("итог kol=%.2f\n",kol);
OSTANOV();
*/
return(kol);

}
