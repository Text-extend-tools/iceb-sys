/*$Id: prsh1.c,v 5.8 2013-04-07 10:22:54 sasa Exp $*/
/*09.03.2016	16.10.2000	Белых А.И.	prsh1.c
Проверка счета

Если вернули 0 - все впорядке
	     1 - нет
*/
#include	"buhl.h"


int prsh1(const char *shet)
{
OPSHET shetv;
return(prsh1(shet,&shetv));
}
/**************************/
int prsh1(const char *shet,OPSHET *shetv)
{
int		kl;
char		bros[512];

if((kl=prsh(shet,shetv)) == 0)
 {
  sprintf(bros,gettext("Нет счета %s в плане счетов !"),shet);
  iceb_t_soob(bros);
  return(1);
 }
if(kl == 2)
 {
  sprintf(bros,gettext("Счет %s имеет субсчета ! Введите нужный субсчет"),shet);
  iceb_t_soob(bros);
  return(2);
 }
if(kl == 4)
 {
  sprintf(bros,"%s %s %s!",gettext("Счет"),shet,gettext("заблокирован в плане счетов"));
  iceb_t_soob(bros);
  return(4);
 }
return(0);

}
