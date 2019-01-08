/*$Id: iceb_tu_sqlfiltr.c,v 5.3 2013/05/17 14:56:41 sasa Exp $*/
/*25.03.2011	13.03.2010	Белых А.И.	iceb_tu_sqlfiltr.cxx
Фильт для sql запроса
*/
#include "util.h"

const char *iceb_tu_sqlfiltr(const char *stroka)
{

int dlinna=strlen(stroka);
static char *new_str=NULL;
if(new_str != NULL)
 delete [] new_str;

if((new_str=new char[dlinna*2+1]) == NULL)
 {
  printf("%s-не могу выделить память!\n",__FUNCTION__);
  return(stroka);
 }
int nomer_to=0;
int nom=0;
for(nom=0; nom < dlinna; nom++)
 {
  if(stroka[nom] == '\'')
    new_str[nomer_to++]='\'';
  new_str[nomer_to++]=stroka[nom];

 }
new_str[nom]='\0';


return(new_str);
}
