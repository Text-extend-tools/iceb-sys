/*$Id: rsdatp.c,v 5.6 2013/05/17 14:56:32 sasa Exp $*/
/*06.10.2014	20.10.2000	Белых А.И.	rsdatp.c
Проверка правильности ввода дат начала и конца поиска
с расшифровкой
Если вернули 0-порядок
             1-нет
*/

#include	"buhl.h"

/*******************************************/
int rsdatp(short *dn,short *mn,short *gn,const char *datn, //Дата начала
short *dk,short *mk,short *gk,const char *datk) //Дата конца
{

if(rsdat(dn,mn,gn,datn,0) != 0)
 { 
  iceb_t_soob(gettext("Не верно ведена дата начала !"));
  return(1);
 } 

if(datk[0] == '\0')
 {
  *dk=*dn; *mk=*mn; *gk=*gn;
  dpm(dk,mk,gk,5); 
 } 
else   
 if(rsdat(dk,mk,gk,datk,0) != 0)
  { 
   iceb_t_soob(gettext("Не верно ведена дата конца !"));
   return(1);
  } 

if(sravmydat(*dn,*mn,*gn,*dk,*mk,*gk) > 0)
 {
  iceb_t_soob(gettext("Дата начала не может быть больше даты конца !"));
  return(1);
 }

return(0);

}
/********************************/
int rsdatp(const char *datan,class iceb_tu_str *datak)
{
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
if(rsdatp(&dn,&mn,&gn,datan,&dk,&mk,&gk,datak->ravno()) != 0)
 return(1);
if(datak->getdlinna() <= 1)
 {
  char bros[64];
  sprintf(bros,"%02d.%02d.%d",dk,mk,gk);
  datak->new_plus(bros);
 } 
return(0);
}
