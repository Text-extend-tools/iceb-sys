/*$Id: iceb_t_menu_import.c,v 5.2 2013/05/17 14:56:28 sasa Exp $*/
/*09.02.2016	18.02.2012	Белых А.И.	iceb_t_menu_import.c
Меню для ввода имени файла для импорта данных в базу
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhl.h"

int iceb_t_menu_import(class iceb_tu_str *imaf_imp,const char *zagolov,const char *imaf_nast,const char *imaf_help)
{

struct stat work;
int kom=0;
static class iceb_tu_str imafim("");
FILE		*ff;
char            strsql[512];


imaf_imp->new_plus("");

VVOD VVOD1(2);
clear();

if(zagolov[0] != '\0')
  VVOD1.VVOD_SPISOK_add_ZAG(zagolov);
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));

naz:;

clear();

if(imaf_nast[0] != '\0')
 {
  if(imaf_help[0] != '\0')
   helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F4",gettext("очистить"),
"F5",gettext("настроить"),
"F6",gettext("выполнить"),
"F10",gettext("выход"),NULL);
  else
   helstr(LINES-1,0,
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F4",gettext("очистить"),
"F5",gettext("настроить"),
"F6",gettext("выполнить"),
"F10",gettext("выход"),NULL);
  
 }
else
 {
  if(imaf_help[0] != '\0')
   helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);
  else
   helstr(LINES-1,0,
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);
  
 }
if((kom=vvod1(&imafim,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return(1);

if(kom == ESC)
  return(1);

switch(kom)
 {
  case FK1:
    GDITE();
    if(imaf_help[0] != '\0')
      iceb_t_pdoc(imaf_help);
    clear();
    goto naz;

  case ENTER:
  case FK2:
  case PLU:
    if(imafim.ravno()[0] == '\0')
     {
      dirkatl(&imafim);
      goto naz;
     }
    break;

  case FK3:
   GDITE();
   if(imafim.getdlinna() <= 1)
    {
     iceb_t_soob(gettext("Не введено имя файла!"));
     goto naz;
    }
   if(stat(imafim.ravno(),&work) != 0)
    {
     sprintf(strsql,"%s !",gettext("Нет такого файла"));
     iceb_t_soob(strsql);
     goto naz;
    }
    vizred(imafim.ravno());
    goto naz;

  case FK4:
    imafim.new_plus("");
    goto naz;
         

 case FK5 : /*настройка для выполнения */
    if(imaf_nast[0] != '\0')
      iceb_redfil(imaf_nast,0);
    goto naz;

 case FK6 : /*Запустить выполнение команд из файла настройки*/
    if(imaf_nast[0] != '\0')
     {
      if(imafim.getdlinna() <= 1)
       {
        iceb_t_soob(gettext("Не введено имя файла!"));
        goto naz;
       }
      iceb_t_fsystem(imaf_nast,imafim.ravno());
     }
    goto naz;
    
  default:
    goto naz;
 }



clear();
move(LINES-1,0);
GDITE();

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }


if((ff = fopen(imafim.ravno(),"r")) == NULL)
 {
  error_op_nfil(imafim.ravno(),errno,"");
  goto naz;
 }

fclose(ff);
imaf_imp->new_plus(imafim.ravno());

return(0);







}
