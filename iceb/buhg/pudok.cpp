/* $Id: pudok.c,v 5.12 2013/05/17 14:56:08 sasa Exp $ */
/*23.04.2008    05.07.1996      Белых А.И.      pudok.c
Проверка удаления всего документа
Если вернули 0-можно удалять
	     1-нельзя
*/
#include        "buhg.h"


int pudok(int podd,short d,short m,short g,const char *ndk,short tz)
{
short           voz;
long            in;
long		kolstr;
SQL_str         row;
char		strsql[512];

/*Смотрим заблокированы ли проводки к этому документа*/
if(iceb_t_pbp(3,d,m,g,ndk,podd,tz,gettext("Удалить документ невозможно!")) != 0)
 return(1);

sprintf(strsql,"select innom from Uosdok1 where datd = '%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tipz=%d",
g,m,d,podd,ndk,tz);


//printw("\n%s\n",strsql);
//OSTANOV();

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  return(0);
 }
voz=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  in=atol(row[0]);
  if(puzap(in,podd,d,m,g,ndk) != 0)
   {
    voz++;
    break;
   }
 }
//printw("pudok-voz=%d\n",voz);
//OSTANOV();

return(voz);
}
