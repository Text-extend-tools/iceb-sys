/*$Id: iceb_t_vperecod.c,v 5.7 2013/05/17 14:56:29 sasa Exp $*/
/*21.02.2010	15.02.2010	Белых А.И. 	iceb_t_vperecod.c
Определение нужной перекодировки по расширению файла до передачи на WINDOWS машину
-1 - не смогли определить нужно перекодировать или нет
 0 - без перекодировки 
 1 - CP866 
 2 - WINDOWS-1251
*/
#include "buhl.h"
int iceb_t_vperecod_menu();


int iceb_t_vperecod(const char *imaf)
{
class iceb_tu_str rashir("");

int kolpol=pole2(imaf,'.');

if(kolpol == 0)
 return(iceb_t_vperecod_menu());
 
if(polen(imaf,&rashir,kolpol,'.') != 0)
 return(iceb_t_vperecod_menu());

if(SRAV("DBF",rashir.ravno(),0) == 0 || SRAV("dbf",rashir.ravno(),0) == 0)
 {
  return(0);
 }
if(SRAV("PFZ",rashir.ravno(),0) == 0 || SRAV("pfz",rashir.ravno(),0) == 0)
 {
  return(0);
 }
  /*Форма 1дф имеет такие расширения файла являясь dbf файлом*/
if(SRAV("1",rashir.ravno(),0) == 0 || SRAV("2",rashir.ravno(),0) == 0 ||\
   SRAV("3",rashir.ravno(),0) == 0 || SRAV("4",rashir.ravno(),0) == 0)
 {
  return(0);
 }

if(SRAV("TXT",rashir.ravno(),0) == 0 || SRAV("txt",rashir.ravno(),0) == 0)
 {
  return(2);
 }
if(SRAV("LST",rashir.ravno(),0) == 0 || SRAV("lst",rashir.ravno(),0) == 0)
 {
  return(2);
 }
if(SRAV("XML",rashir.ravno(),0) == 0 || SRAV("xml",rashir.ravno(),0) == 0)
 {
  if(iceb_t_poldan_vkl("Перекодировка XML файлов при копировании на Windows машину","nastsys.alx") != 0)
   return(0);
  return(2);
 }

return(iceb_t_vperecod_menu());


}
/***************************/
/*меню выбора*/
/****************************/
int iceb_t_vperecod_menu()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Виберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Записать как есть"));
MENU.VVOD_SPISOK_add_MD(gettext("Записать перекодированным в WINDOWS кодировку"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch(kom)
 {
  case -1:
  case 2:
    return(-1);
  case 0:
    return(0);
  case 1:
    return(2);        
 }

return(-1);
}
