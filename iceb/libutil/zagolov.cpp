/*$Id: zagolov.c,v 5.15 2013/08/13 05:50:33 sasa Exp $*/
/*06.02.2010	18.06.2001	Белых А.И.	zagolov.c
Заголовок распечаток
*/
#include        <time.h>
#include	"util.h"

/***************
void zagolov(const char *naim,const char *datan,
const char *datak,const char *organ,FILE *ff)
{
time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);
short dn,mn,gn;
short dk,mk,gk;
rsdat(&dn,&mn,&gn,datan,1);
rsdat(&dk,&mk,&gk,datan,1);

zagolov(naim,bf,dn,mn,gn,dk,mk,gk,organ,ff);

}
******************/
/****************************/
void zagolov(const char *naim,short dn,short mn,short gn,
short dk,short mk,short gk,const char *organ,FILE *ff)
{
time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

zagolov(naim,bf,dn,mn,gn,dk,mk,gk,organ,ff);

}
/****************************/

void zagolov(const char *naim,struct tm *bf,short dn,short mn,short gn,
short dk,short mk,short gk,const char *organ,FILE *ff)
{
if(ff == NULL)
 return;
 
if(organ[0] != '\0')
 fprintf(ff,"%s\n\n",organ);

fprintf(ff,"%s\n",naim);

if(dn != 0)
  fprintf(ff,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
  gettext("Период с"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));
else
 {
  if(dk != 0)
   fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата"),
   dk,mk,gk,gettext("г."));
  if(dk == 0 && mk != 0)
   fprintf(ff,"%s:%02d.%d%s\n",gettext("Дата"),
   mk,gk,gettext("г."));
  if(dk == 0 && mk == 0 && gk != 0)
   fprintf(ff,"%s:%d%s\n",gettext("Дата"),
   gk,gettext("г."));
 }  
fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

}
/****************************/

void zagolov(const char *naim,const char *dat_start,
const char *dat_end,const char *organ,FILE *ff)
{
if(ff == NULL)
 return;

time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);
 
if(organ[0] != '\0')
 fprintf(ff,"%s\n\n",organ);

fprintf(ff,"%s\n",naim);

if(dat_start[0] != '\0')
  fprintf(ff,"%s %s %s %s %s %s\n",
  gettext("Период с"),
  dat_start,
  gettext("г."),
  gettext("по"),
  dat_end,
  gettext("г."));
else
 {
  if(dat_end[0] != '\0')
   fprintf(ff,"%s:%s %s\n",gettext("Дата"),
   dat_end,gettext("г."));

 }  
fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

}
