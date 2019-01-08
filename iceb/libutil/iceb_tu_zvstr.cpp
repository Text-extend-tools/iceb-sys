/*$Id: iceb_tu_zvstr.c,v 5.8 2013/05/17 14:56:41 sasa Exp $*/
/*28.07.2011	15.04.2009	Белых А.И.	iceb_tu_zvstr.c
Замена в строке одной цепочки символов другой
*/
#include "util.h"

int iceb_tu_zvstr(class iceb_tu_str *stroka,const char *obr,const char *zamena)
{

if(zamena[0] == '\0')
 return(1);
if(obr[0] == '\0')
 return(1);

if(stroka->getdlinna() <= 1)
 return(1); 

int metka_zam=0;
int dlina=stroka->getdlinna()+1;
char *stroka1=new char[dlina];
memset(stroka1,'\0',sizeof(dlina));
class iceb_tu_str strout;

int dlina_prov=strlen(obr)+1;
 
char *prov=new char[dlina_prov];
int nomer_str=0;

for(int ii=0; ii < stroka->getdlinna(); ii++)
 {
  if(stroka->ravno()[ii] == obr[0])
   {
    memset(prov,'\0',dlina_prov);
    strncpy(prov,&stroka->ravno()[ii],dlina_prov-1);
        
    if(SRAV(prov,obr,0) == 0)
     {
      metka_zam++;
      stroka1[nomer_str]='\0'; /*обязательно закрыть нулевым байтом*/
   
      strout.plus(stroka1);
      strout.plus(zamena);      
      memset(stroka1,'\0',dlina);
      nomer_str=0;
      ii+=strlen(obr)-1; /*пропускаем цепочку символов которую надо заменить на новый образец уменьшаем на один так как ii увеличится в цикле на 1 */
      continue;   /*делаем для того чтобы проверить сразу за образцом иначе следующий байт присвоится в stroka1 без проверки*/ 
     }
   }
  stroka1[nomer_str++]=stroka->ravno()[ii];
 }

if(metka_zam == 0)
 {
  delete [] prov;
  delete [] stroka1;
  return(0);
 }

strout.plus(stroka1);

delete [] stroka1;
delete [] prov;
stroka->new_plus(strout.ravno());
return(0);
}
