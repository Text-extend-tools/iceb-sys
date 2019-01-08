/*$Id: iceb_t_tmptab.c,v 5.4 2013/05/17 14:56:29 sasa Exp $*/
/*03.03.2010	03.03.2010	Белых А.И.	iceb_t_tmptab.c
Cоздание и удаление временных таблиц
*/
#include "buhl.h"

/****************************/
/*Конструктор объекта       */
/****************************/
iceb_t_tmptab::iceb_t_tmptab()
{
metka=0; /*таблица не создана*/

}
/****************************/
/*Деструктор объекта       */
/****************************/
iceb_t_tmptab::~iceb_t_tmptab()
{
drop_tab();
}
/*******************/
/*cоздаём талблицу*/
/******************/
int iceb_t_tmptab::create_tab(const char *ima_tabl,const char *zapros)
{
if(metka == 1)
 drop_tab();

ima_tab.new_plus(ima_tabl);
 
int voz=sql_zapis(zapros,0,0);
if(voz == 0)
 metka=1;
return(voz);
}

/*******************/
/*Удаляем таблицу*/
/******************/
int iceb_t_tmptab::drop_tab()
{
char strsql[512];
if(metka == 0 || ima_tab.getdlinna() <= 1)
 return(0);

sprintf(strsql,"drop table %s",ima_tab.ravno());
int voz=sql_zapis(strsql,0,0);
if(voz == 0)
 metka=0;
return(voz); 
}
