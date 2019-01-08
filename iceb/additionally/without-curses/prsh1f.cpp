/*$Id: prsh1.c,v 5.8 2013-04-07 10:22:54 sasa Exp $*/
/*28.03.2013	16.10.2000	Белых А.И.	prsh1f.c
Проверка счета

Если вернули 0 - все впорядке
	     1 - нет
*/
#include	"bezcurses.h"


int prsh1(const char *shet,OPSHET *shetv)
{
int		kl;

if((kl=prsh(shet,shetv)) == 0)
 {
  printf("%s %s %s!\n",gettext("Нет счета"),shet,gettext("в плане счетов"));
  return(1);
 }
if(kl == 2)
 {
  printf("%s %s %s! %s!\n",gettext("Счет"),shet,gettext("имеет субсчета"),gettext("Введите нужный субсчёт"));
  return(2);
 }
return(0);

}
