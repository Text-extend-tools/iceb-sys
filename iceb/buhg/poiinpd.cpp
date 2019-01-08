/* $Id: poiinpd.c,v 5.12 2013-04-07 10:22:43 sasa Exp $ */
/*14.02.2013	28.07.1999	Белых А.И. 	poiinpd.c
Поиск переменных данных на инвентарный номер
Если вернули 0 нашли настройку
             3 - нет ни одной записи

*/
#include	"buhg.h"
#include "poiinpd.h"

/*****************************************/

int poiinpd(int in, //Инвентарный номер
short m,short g, //Дата
class poiinpdw_data *data)
{
SQL_str         row;
char		strsql[512];
data->clear();

sprintf(strsql,"select * from Uosinp where innom=%d \
and (god < %d or (god = %d and mes <= %d)) order by god desc,mes desc limit 1",
in,g,g,m);
class SQLCURSOR cur;
//printw("%s-%s\n",__FUNCTION__,strsql);
if(readkey(strsql,&row,&cur) <= 0)
 return(3);
//printw("%s\n",__FUNCTION__);

data->mz=atoi(row[1]);
data->gz=atoi(row[2]);
data->shetu.new_plus(row[3]);
data->hzt.new_plus(row[4]);
data->hau.new_plus(row[5]);
data->hna.new_plus(row[6]);
data->popkf=atof(row[7]);
data->soso=atoi(row[8]);
data->nomz.new_plus(row[9]);
data->hnaby.new_plus(row[12]);
data->popkfby=atof(row[13]);

//printw("%s-%s\n",__FUNCTION__,data->shetu.ravno());
//OSTANOV();

return(0);

}
