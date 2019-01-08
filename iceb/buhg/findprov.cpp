/* $Id: findprov.c,v 5.6 2013/05/17 14:56:02 sasa Exp $ */
/*26.04.1999	26.04.1999	Белых А.И.	findprov.c
Поиск проводки в курсоре и выдача номера записи
Если вернули -1 значит не нашли
*/
#include        "buhg.h"

int		findprov(SQLCURSOR *cur,short d,short m,short g,
char sh[],char shk[],time_t vrem,char kto[],double deb,double kre,short val)
{
short		d1,m1,g1;
SQL_str         row;
int		pozz;
double		deb1,kre1;


cur->poz_cursor(0);
pozz=0;

while(cur->read_cursor(&row) != 0)
 {
  rsdat(&d1,&m1,&g1,row[1],2);
  
  deb1=atof(row[9]);
  deb1=okrug(deb1,0.01);
  kre1=atof(row[10]);
  kre1=okrug(kre1,0.01);
  
  if(d == d1 && m == m1 && g == g1 &&
  SRAV(sh,row[2],0) == 0 && SRAV(shk,row[3],0) == 0 &&
  vrem == atol(row[12]) && SRAV(kto,row[5],0) == 0 &&
  deb == deb1 && kre  == kre1 && val == atoi(row[0]))
   {
    return(pozz);        
   }
  pozz++;
 }
return(-1);
}
