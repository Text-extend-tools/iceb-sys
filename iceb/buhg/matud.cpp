/* $Id: matud.c,v 5.10 2013/05/17 14:56:05 sasa Exp $ */
/*26.02.2017    16.04.1997      Белых А.И.      matud.c
Удаление документа
1- удаляем записи в таблице списания материалов на изготовление деталей (только для расходных документов)
2- удаляем услуги
3- удаляем список оплат к документу
4-удаляем все записи в накладной
5-удаляем дополнительные записи к шапке документа
6-удаляем шапку накладной
*/
#include        <math.h>
#include	"buhg.h"


void matud(const char *nomdok,short d,short m,short g,int skl,int tipz)
{
char		strsql[512];

if(tipz == 2)
 {
  /*удаляем записи в таблице списания материалов на детали*/
  sprintf(strsql,"delete from Dokummat4 where datd='%04d-%02d-%02d' and skl=%d and nomd='%s'",
  g,m,d,skl,nomdok);
  if(sql_zapis(strsql,1,0) != 0)
   return;
 }
 
//Удаляем услуги
sprintf(strsql,"delete from Dokummat3 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
g,m,d,skl,nomdok);
if(sql_zapis(strsql,1,0) != 0)
 return;

//Удаляем список оплат к документу
sprintf(strsql,"delete from Opldok where ps=1 and datd='%04d-%02d-%02d' and pd=%d and nomd='%s'",
g,m,d,skl,nomdok);

if(sql_zapis(strsql,1,0) != 0)
 return;

/*Удаляем записи в накладной*/

sprintf(strsql,"delete from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and tipz=%d",
g,m,d,skl,nomdok,tipz);

if(sql_zapis(strsql,1,0) != 0)
 return;


sprintf(strsql,"delete from Dokummat2 where god=%d and nomd='%s' and \
sklad=%d",
g,nomdok,skl);

if(sql_zapis(strsql,1,0) != 0)
 return;

/*Удаляем накладную*/

sprintf(strsql,"delete from Dokummat where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and tip=%d",
g,m,d,skl,nomdok,tipz);

if(sql_zapis(strsql,1,0) != 0)
 return;

}
