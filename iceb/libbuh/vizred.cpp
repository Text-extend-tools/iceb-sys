/*$Id: vizred.c,v 5.10 2013-04-07 10:22:55 sasa Exp $*/
/*08.08.2010	27.09.1999	Белых А.И.	vizred.c
Запуск текстового редактора
*/
#include <errno.h>
#include "buhl.h"

int vizred(const char *imaf,const char *editor)
{
/*Проверяем возможность открытия файла*/
FILE *ff;

if((ff=fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,__FUNCTION__);
  return(1);
 }
fclose(ff);



class iceb_tu_str redaktor;
if(editor == NULL || editor[0] == '\0')
 {
  if(iceb_t_poldan("Редактор",&redaktor,"nastsys.alx") != 0)
    return(1);
 }
else
 redaktor.new_plus(editor);
 
class iceb_tu_str faj;
int voz;

clear();
refresh();

faj.plus(redaktor.ravno());
faj.plus(" ",imaf);

voz=runsystem(faj.ravno());

if(voz == 0)
  clear();
return(voz);

}
/************************************/
int vizred(const char *imaf)
{
return(vizred(imaf,NULL));
}
