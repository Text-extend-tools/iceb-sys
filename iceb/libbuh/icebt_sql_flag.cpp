/*$Id: icebt_sql_flag.c,v 5.6 2013/05/17 14:56:30 sasa Exp $*/
/*24.08.2006	16.08.2004	Белых А.И.	icebt_sql_flag.c
Установка и снятие флага в базе данных
*/
#include "buhl.h"

//extern SQL_baza	bd;

//Конструктор
icebt_sql_flag::icebt_sql_flag(void)
{
flag.new_plus("");
metka=0;
}
//Конструктор
icebt_sql_flag::icebt_sql_flag(const char *flag_str)
{
flag.new_plus(flag_str);
metka=0;
}
//Деструктор
icebt_sql_flag::~icebt_sql_flag()
{
flag_off();
}

//Выставить флаг
int icebt_sql_flag::flag_on(const char *str_flag)
 {
  flag.new_plus(str_flag);
  return(flag_on());
 }

//Выставить флаг текущий 
int icebt_sql_flag::flag_on()
{
//printw("icebt_sql_gflag-Выстaвляем флаг=%s\n",flag.ravno());

int voz=0;
if(flag.getdlinna() <= 1)
 return(voz);
if((voz=sql_flag(&bd,flag.ravno(),0,0)) == 0)
  metka=1;
return(voz);

}

//Снять флаг
int icebt_sql_flag::flag_off()
{
//printw("icebt_sql_flag-Снимаем флаг=%s\n",flag.ravno());
if(metka == 0)
  return(0);
int voz=sql_flag(&bd,flag.ravno(),0,1);
flag.new_plus("");
return(voz);
}
