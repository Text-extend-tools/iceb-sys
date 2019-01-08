/*$Id: iceb_t_spis_komp.c,v 5.14 2013/05/17 14:56:29 sasa Exp $*/
/*02.07.2015	13.12.2006	Белых А.И.	iceb_t_spis_komp.c
Подпрограмма для передачи файла на компъютер работающий под Windows
Если вернули 0 - файл передан 
             1 - нет
*/
#include <pwd.h>
#include <errno.h>
#include	"buhl.h"


int iceb_t_vperecod(const char *imaf);


int iceb_t_spis_komp(const char *imaf)
{
char bros[1024];
char strok[1024];
class SPISOK str_nast;
class SPISOK naim_komp;
int metka_perecod=0; /*0-как есть 1-cp866 2-cp1251*/
char imaf_copy[56];

struct passwd *rek_login;

if((metka_perecod=iceb_t_vperecod(imaf))  < 0)
 return(1);



rek_login=getpwuid(getuid());

class iceb_tu_str imaf_nast;

imaf_nast.plus(rek_login->pw_dir);
imaf_nast.plus(G_DIR_SEPARATOR_S,ICEB_KAT_ALX);
imaf_nast.plus(G_DIR_SEPARATOR_S,"spis_komp.alx");

FILE *ff;

if((ff = fopen(imaf_nast.ravno(),"r")) == NULL)
 {
  error_op_nfil(imaf_nast.ravno(),errno,"");
  return(1);
 }

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  if(strok[0] == '#')
   continue;
  if(polen(strok,bros,sizeof(bros),1,'|') != 0)
   continue;
  if(bros[0] == '\0')
   continue;
  naim_komp.plus(bros);
  str_nast.plus(strok);   
 }
fclose(ff);
class iceb_tu_str ima_mah;
class iceb_tu_str ima_kat;
class iceb_tu_str parol;
class iceb_tu_str prog;

if(naim_komp.kolih() == 0)
 {
  iceb_t_soob(gettext("Не найдено настроек !"));
  return(1);
 }

int voz=0;
if(naim_komp.kolih() > 1)
 {
  int kod_kl=0;
  voz=dirmasiv(&naim_komp,-1,-1,0,gettext("Выберите куда копировать"),1,&kod_kl);
  if(voz < 0 || kod_kl != ENTER)
   return(1);
 }


if(metka_perecod > 0)
 {
  sprintf(imaf_copy,"iceb_t_spis_komp%d.tmp",getpid());
  iceb_t_cp(imaf,imaf_copy,0);
  perecod(metka_perecod,imaf);
 } 

if(pole2(str_nast.ravno(voz),'|') > 2)
 {

  polen(str_nast.ravno(voz),&ima_mah,2,'|');   
  polen(str_nast.ravno(voz),&ima_kat,3,'|');   
  polen(str_nast.ravno(voz),&parol,4,'|');   
  polen(str_nast.ravno(voz),&prog,5,'|');   

  sprintf(bros,"%s %s %s %s %s %s 2>&1",prog.ravno(),ima_mah.ravno(),imaf,imaf,ima_kat.ravno(),parol.ravno());
 }
else /*программа (скрипт) которая получает имя файла для передачи по сети*/
 {
  
  polen(str_nast.ravno(voz),&prog,2,'|');   
  if(strstrm(prog.ravno(),"ICEBFIL") == 1)
   {
    iceb_tu_zvstr(&prog,"ICEBFIL",imaf);
    strncpy(bros,prog.ravno(),sizeof(bros));
   }
  else
    sprintf(bros,"%s %s",prog.ravno(),imaf);  
  
 }
//int i=system(bros);
int i=iceb_t_system(bros,0);
if(i != 0)
 {
  strcat(bros,"\n");
  sprintf(strok,gettext("Передача файла не произошла ! Код ошибки %d"),i);
  strcat(bros,strok);
  iceb_t_soob(bros);
 }
if(i == 0)
 {
  iceb_t_soob(gettext("Передача файла произошла успешно"));
 }     

if(metka_perecod > 0)
 rename(imaf_copy,imaf);
return(i);
}

