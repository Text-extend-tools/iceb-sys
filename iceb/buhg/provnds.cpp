/*$Id: provnds.c,v 5.6 2013/05/17 14:56:08 sasa Exp $*/
/*21.10.2002	21.10.2002	Белых А.И.	provnds.c
Проверка НДС
Если вернули 0 подходит условиям поиска
             1 нет
*/
#include        <math.h>
#include        "buhg.h"


int provnds(const char *nds,SQL_str row)
{

if(nds[0] == '\0')
 return(0);
 
//Карточки с нулевым НДС
if(nds[0] == '0' && atof(row[9]) != 0.)
 return(1);

//Карточки с НДС нужного значения
if(nds[0] != '+' && nds[0] != '0' && (fabs(atof(row[9]) - atof(nds)) > 0.009 || atoi(row[3]) != 0))
 return(1);

//Карточки в цене которых включено НДС нужного значения
if(nds[0] == '+' && nds[1] != '\0' && (fabs(atof(row[9]) - atof(nds)) > 0.009 || atoi(row[3]) != 1) )
 return(1);

//Карточки в цене которых НДС любого значения
if(nds[0] == '+' && nds[1] == '\0' &&  atoi(row[3]) != 1 )
 return(1);

return(0);

}
