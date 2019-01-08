/*$Id: rasttn.c,v 5.2 2014/01/31 12:11:33 sasa Exp $*/
/*29.01.2014	28.09.2013	Белых А.И.	rasttn.c
Распечатка товарно-транспортной накладной
*/
#include "buhg.h"

int rasttn1(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth);
int rasttn2(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth);


int rasttn(short dd,short md,short gd,const char *nomdok,int sklad,class spis_oth *oth)
{
int voz=0;
if(sravmydat(dd,md,gd,14,1,2014) < 0)
 voz=rasttn1(dd,md,gd,nomdok,sklad,oth);
else
 voz=rasttn2(dd,md,gd,nomdok,sklad,oth);
 


return(voz);

}

