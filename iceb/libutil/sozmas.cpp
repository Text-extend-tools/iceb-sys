/* $Id: sozmas.c,v 5.14 2013/09/26 09:44:11 sasa Exp $ */
/*22.07.2013	07.12.1999	Белых А.И.	sozmas.c
Выделить память и зарядить данные
В нулевом елементе количество кодов
*/
#include        "util.h"

void sozmas(short **kod,char *st1,FILE *ff)
{
short		i,i1;
class iceb_tu_str str("");
class iceb_tu_str bros("");

if(polen(st1,&str,2,'|') != 0)
  str.new_plus(st1);
class iceb_tu_str spisok_kodov("");

iceb_tu_fplus(str.ravno(),&spisok_kodov,ff);

if(spisok_kodov.getdlinna() <= 1)
 return;  

i=pole2(spisok_kodov.ravno(),',');
if(i == 0 && spisok_kodov.getdlinna() > 1)
  i=1;

if((*kod=new short[i+1]) == NULL)
 {
  printf("%s-%s kod !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return;
 }

(*kod)[0]=i;

for(i1=1; i1 < *kod[0]+1; i1++)
 {
  if(i == 1)
   bros.new_plus(spisok_kodov.ravno());
  else        
    polen(spisok_kodov.ravno(),&bros,i1,',');
  (*kod)[i1]=(short)bros.ravno_atoi();
 }

}
/*************************************/

void sozmas(short **kod,char *st1)
{
short		i,i1;
class iceb_tu_str str("");
class iceb_tu_str bros("");

if(polen(st1,&str,2,'|') != 0)
  str.new_plus(st1);

if(str.ravno()[0] == '\0')
 return;  

i=pole2(str.ravno(),',');
if(i == 0 && str.ravno()[0] != '\0')
  i=1;

if((*kod=new short[i+1]) == NULL)
 {
  printf("%s-%s kod !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return;
 }

(*kod)[0]=i;

for(i1=1; i1 < *kod[0]+1; i1++)
 {
  if(i == 1)
   bros.new_plus(str.ravno());
  else        
    polen(str.ravno(),&bros,i1,',');
  (*kod)[i1]=(short)bros.ravno_atoi();
 }

}
