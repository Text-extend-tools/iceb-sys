/* $Id: zaravpr.c,v 5.112 2013-01-02 10:38:37 sasa Exp $ */
/*22.12.2012	23.09.1998	Белых А.И.	zaravpr.c
Выполнение проводок для заработной платы
*/
#include "buhg.h"


void zaravpr_old(int tbn,short mp,short gp,FILE *ff_prot);
void avtprozar(int tbn,short mp,short gp,FILE *ff_prot);

void zaravpr(int tbn,short mp,short gp,FILE *ff_prot)
{
int metka_prov=0;
class iceb_tu_str stroka("");
if(iceb_t_poldan("Автоматическое выполнение проводок с использованием таблицы",&stroka,"zarnast.alx") == 0)
 if(SRAV(stroka.ravno(),"Вкл",1) == 0)
  metka_prov=1;


/*Проверка на разрешение выполнения проводок внутри подпрограмм*/
if(metka_prov == 1)  
  zaravpr_old(tbn,mp,gp,ff_prot);
else
 avtprozar(tbn,mp,gp,ff_prot);
 
}