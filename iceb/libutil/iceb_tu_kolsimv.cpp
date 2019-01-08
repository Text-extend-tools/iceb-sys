/*$Id: iceb_tu_kolsimv.c,v 5.4 2013/05/17 14:56:41 sasa Exp $*/
/*12.09.2011	05.09.2011	Белых А.И.	iceb_tu_kolsimv.c
Определение сколько байт занимают символы во входной строке которые могут целиком поместитьтся в массив заданного размера
Возвращает количество символов
*/
#include "util.h"
int iceb_tu_kolsimv(int kolbait_str,const char *stroka)
{
int voz=0;
int kol_bait=0;
int kolih_simv=iceb_tu_strlen(stroka);
//printw("%s-stroka=%s kolih_simv=%d kolbait_str=%d\n",__FUNCTION__,stroka,kolih_simv,kolbait_str);
for(int nom=1; nom <= kolih_simv; nom++)
 {
  kol_bait=iceb_tu_kolbait(nom,stroka);
  if(kol_bait >= kolbait_str-1)
   break;
  voz++;
 }
return(voz);

}
