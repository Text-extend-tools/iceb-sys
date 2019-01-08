/* $Id: prcn.c,v 5.5 2013/08/13 05:49:51 sasa Exp $ */
/*23.05.2013	30.10.1997	Белых А.И.	prcn.c
Преобразование цены так чтобы дробная часть выводилась как
надо. Либо с 2мя нолями либо со всеми значащими цифрами после
запятой.
*/
#include	<util.h>

extern double		okrcn;  /*Округление цены*/

char            *prcn(double cn)
//double          cn; /*Число для преобразования*/
{
static char     str[1024]; /*Символьная строка с числом*/
class iceb_tu_str br("");

/*printw("\ncn=%.10f okr=%.10f\n",cn,okr);*/
sprintf(str,"%.10g",cn);
polen(str,&br,2,'.');
if(strlen(br.ravno()) < 2)
  sprintf(str,"%.2f",cn);
 
return(str);
}
