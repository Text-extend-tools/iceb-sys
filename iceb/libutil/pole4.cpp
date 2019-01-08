/*$Id: pole4.c,v 5.9 2013/09/26 09:44:09 sasa Exp $*/
/*13.02.2017	10.11.1998	Белых А.И.	pole4.c
Выбрасывание из динамически открытого массива образца
Если вернули 0- убрали
             1 -  не было образца
             
*/
#include	"util.h"

short pole4(char *mas,const char *obr,char raz)
{
int		np;
class iceb_tu_str masz;
int		rmas;
int		i;
short		kp;
class iceb_tu_str pol("");
short		met;
char		br[2];

if(pole1(mas,obr,raz,0,&np) != 0)
 return(1);
 
br[0]=raz;
br[1]='\0';

kp=pole2(mas,raz);
rmas=strlen(mas)+1;

masz.new_plus(mas);
/*printw("mas=%s\n",mas);*/

memset(mas,'\0',rmas);

met=0;
for(i=0;i<kp;i++)
 {
  if(i == np)
    continue;


  polen(masz.ravno(),&pol,i+1,raz);
/*  printw("i=%d pol=%s mas=%s\n",i,pol.ravno(),mas);*/
  if(met == 0)
   {
    sprintf(mas,"%s",pol.ravno());
    met++;
    continue;
   } 

  if(met == 1)
   { 
    strcat(mas,br);
    strcat(mas,pol.ravno());
   }   
  
 }

return(0);
}
