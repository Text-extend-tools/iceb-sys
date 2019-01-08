/*$Id: iceb_t_menudpr.c,v 5.5 2013/05/17 14:56:28 sasa Exp $*/
/*02.07.2015	09.04.2009	Белых А.И.	iceb_t_dprog.c
Создание меню для запуска дополнительных программ
*/
#include <pwd.h>
#include <errno.h>
#include "buhl.h"


int  iceb_t_menudpr(int metka) /*0-проверить есть ли настройки 1- вывести меню*/
{
class SPISOK naimstr;
class SPISOK prog;

FILE *ff;
char strok[1024];
class iceb_tu_str putnakat;
struct passwd *rek_login;
rek_login=getpwuid(getuid());

putnakat.new_plus(rek_login->pw_dir);

putnakat.plus(G_DIR_SEPARATOR_S,ICEB_KAT_ALX);
putnakat.plus("/menu_dpr.alx");
//putnakat.plus(G_DIR_SEPARATOR_S,".iceB/menu_dpr.alx");



if((ff = fopen(putnakat.ravno(),"r")) == NULL)
 {
   if(errno != ENOENT)
     {
      error_op_nfil(putnakat.ravno(),errno,"");
      return(1);
     }
  if(errno == ENOENT)  /*Файл не существует*/
   return(1);
 }

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  if(strok[0] == '#')
   continue;
  if(polen(strok,&naimstr,1,'|') != 0)
   continue;
  polen(strok,&prog,2,'|');
  
 }
fclose(ff);

if(naimstr.kolih() == 0)
   return(1);
else
 if(metka == 0)
   return(0);  

int voz=0;

if(naimstr.kolih() > 1)
 {
  int kod_kl=0;
  voz=dirmasiv(&naimstr,-1,-1,0,"",1,&kod_kl);
  if(voz < 0 || kod_kl != ENTER)
   return(0);
 }

//iceb_t_system(prog.ravno(voz),0);
runsystem(prog.ravno(voz));

return(0);
}


