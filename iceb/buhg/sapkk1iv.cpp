/* $Id: sapkk1iv.c,v 5.6 2013/05/17 14:56:11 sasa Exp $ */
/*12.11.2004    01.02.1994      Белых А.И.      sapkk1iv.c
Счетчик строк
*/
#include	"buhg.h"
extern int kol_strok_na_liste;

void		sapkk1iv(int *kolstrlist,int *kollist,FILE *ff1)
{

*kolstrlist+=1;
if(*kolstrlist <= kol_strok_na_liste)
 return;

*kollist+=1;
fprintf(ff1,"\f");
*kolstrlist=1;
sapkkiv(kolstrlist,*kollist,ff1);

}
