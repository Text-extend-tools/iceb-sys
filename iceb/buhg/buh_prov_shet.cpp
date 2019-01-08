/*$Id: buh_prov_shet.c,v 5.5 2013-04-18 09:26:03 sasa Exp $*/
/*17.04.2013	06.02.2008	Белых А.И.	buh_prov_shet.c
Проверка счетов по строке настройки
Если вернули 0- подходит 
             1- нет
*/
#include "buhg.h"


int buh_prov_shet(char *shet,char *shet_kor,class SPISOK *nastr)
{
class iceb_tu_str sh("");
class iceb_tu_str shk("");

for(int ii=0; ii < nastr->kolih(); ii++)
 {
  polen(nastr->ravno(ii),&sh,2,'|');
  polen(nastr->ravno(ii),&shk,3,'|');
  
  if(proverka(sh.ravno(),shet,1,0) != 0)
   continue;    
  
  if(proverka(shk.ravno(),shet_kor,1,0) != 0)
   continue;    
  
  return(0);
 }
return(1);

}
