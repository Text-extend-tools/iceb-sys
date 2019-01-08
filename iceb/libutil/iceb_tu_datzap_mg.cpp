/*$Id: iceb_tu_datzap_mg.c,v 5.3 2013/05/17 14:56:40 sasa Exp $*/
/*02.11.2011	02.11.2011	Белых А.И.	iceb_tu_datzap_mg.c
Получение символьной строки с датой
*/
#include  "util.h"

const char *iceb_tu_datzap_mg(const char *data)
{
short d=0,m=0,g=0;
static char stroka[32];
memset(stroka,'\0',sizeof(stroka));
if(data[0] == '\0')
 return(stroka);

rsdat(&d,&m,&g,data,2);
if(d != 0)
 sprintf(stroka,"%02d.%04d",m,g);
return(stroka);

}
