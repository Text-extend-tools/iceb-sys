/* $Id: zapvkr.c,v 5.17 2013/09/26 09:43:45 sasa Exp $ */
/*19.06.2004    26.09.1997      Белых А.И.      zapvkr.c
Запись в карточку материалла
*/
#include	"buhg.h"


int zapvkr(short dp,short mp,short gp,const char *nn,int skl,
int nk,short d,short m,short g,int tp,double kolih,double cena)
{
char		strsql[512];

sprintf(strsql,"insert into Zkart \
values (%d,%d,'%s','%04d-%02d-%02d','%04d-%02d-%02d',%d,%.10g,%.10g,%d,%ld)",
skl,nk,nn,gp,mp,dp,g,m,d,tp,kolih,cena,iceb_t_getuid(),time(NULL));

//printw("strsqk=%s\n",strsql);
//OSTANOV();
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }
return(0);
}
