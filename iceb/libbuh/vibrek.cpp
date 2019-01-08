/*$Id: vibrek.c,v 1.31 2013/09/26 09:43:55 sasa Exp $*/
/*28.03.2018	24.11.2001	Белых А.И. 	vibrek.c
Выбор реквизита для заполнения в меню.
Если выбрали возвращаем 0
             не выбрали 1
*/
#include	"buhl.h"


int vibrek(const char *tabl,class iceb_tu_str *kodrec)
{
class iceb_tu_str naim("");
return(vibrek(tabl,kodrec,&naim));
}
/********************************************/
int vibrek(const char *tabl,char *kodrec,int sizekod,char *naim)
{
class iceb_tu_str kodrec1;
class iceb_tu_str naim1("");
kodrec1.plus(kodrec);
int voz=vibrek(tabl,&kodrec1,&naim1);
if(voz == 0)
 {
  memset(kodrec,'\0',sizeof(sizekod));
  strncpy(kodrec,kodrec1.ravno(),sizekod-1);
  strcpy(naim,naim1.ravno());
 }
return(voz);
}
/********************************************/
int vibrek(const char *tabl,class iceb_tu_str *kodrec,class iceb_tu_str *naim)
{
int		poz,komv;
int		metkatab=0;
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");
clear();

poz=komv=0;

if(SRAV(tabl,"Grup",0) == 0 || \
SRAV(tabl,"Reegrup1",0) == 0 || SRAV(tabl,"Reegrup2",0) == 0 || \
SRAV(tabl,"Uospod",0) == 0 || \
SRAV(tabl,"Uosgrup",0) == 0 || SRAV(tabl,"Uosgrup1",0) == 0 || \
SRAV(tabl,"Uoshau",0) == 0 || SRAV(tabl,"Uoshz",0) == 0 || \
SRAV(tabl,"Uosol",0) == 0 || SRAV(tabl,"Uslgrup",0) == 0 || \
SRAV(tabl,"Gkont",0) == 0 || SRAV(tabl,"Foroplat",0) == 0 || \
SRAV(tabl,"Kateg",0) == 0 || \
SRAV(tabl,"Gnali",0) == 0 || \
SRAV(tabl,"Tabel",0) == 0 || SRAV(tabl,"Zarsoc",1) == 0 || \
SRAV(tabl,"Opltp",0) == 0 || SRAV(tabl,"Opltt",0) == 0 || \
SRAV(tabl,"Oznvb",0) == 0 || SRAV(tabl,"Ksn",0) == 0 || \
SRAV(tabl,"Ukrgrup",0) == 0 || \
SRAV(tabl,"Ukrvkr",0) == 0 || SRAV(tabl,"Zargrupp",0) == 0 || SRAV(tabl,"Uplpodr",0) == 0)
 {
  class iceb_tu_str naimn("");  
  metkatab++;
  if(dirmat1(tabl,&kodn,&naimn,LINES-5,1,2,1,&poz,&komv,"","\0") == 0)
   {
    kodrec->z_plus(kodn.ravno());
    if(naim != NULL)
      naim->new_plus(naimn.ravno());
    clear();
    return(0);
   }
 }
if(SRAV(tabl,"Plansh",0) == 0)
 {
  class iceb_tu_str naimn("");  
  metkatab++;
  if(dirshet(&kodn,&naimn,1) == 0)
   {
    kodrec->z_plus(kodn.ravno());
    if(naim != NULL)
      naim->new_plus(naimn.ravno());
    clear();
    return(0);
  }
 }

if(SRAV(tabl,"Edizmer",0) == 0)
 {
  class iceb_tu_str naimn("");  
  metkatab++;
  if(iceb_tl_ei(1,&kodn,&naimn) == 0)
   {
    kodrec->z_plus(kodn.ravno());
    if(naim != NULL)
      naim->new_plus(naimn.ravno());
    clear();
    return(0);
  }
 }


if(SRAV(tabl,"Kontragent",0) == 0)
 {
  class iceb_tu_str naimn("");  
  metkatab++;
  if(dirkontr(&kodn,&naimn,1) == 0)
   {
    kodrec->z_plus(kodn.ravno());
    if(naim != NULL)
      naim->new_plus(naimn.ravno());
    clear();
    return(0);
   }
 }

if(SRAV(tabl,"Uplmt",0) == 0)
 {
  metkatab++;
  if(dirmt(&kodn,1) == 0)
   {
    kodrec->z_plus(kodn.ravno());
    if(naim != NULL)
      naim->new_plus(naimn.ravno());
    clear();
    return(0);
  }
 }

if(SRAV(tabl,"Glksubbal",0) == 0)
 {
  metkatab++;
  if(dirsubbal(1,&kodn,&naimn) == 0)
   {
    kodrec->z_plus(kodn.ravno());
    if(naim != NULL)
      naim->new_plus(naimn.ravno());
    clear();
    return(0);
  }
 }

if(metkatab == 0)
 {
  char bros[512];
  sprintf(bros,"%s-%s %s !",__FUNCTION__,gettext("Не известная таблица"),tabl);
  iceb_t_soob(bros);
 }

clear();
return(1);

}
