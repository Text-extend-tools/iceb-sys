/*$Id: iceb_t_progforlst.c,v 5.4 2013-04-07 10:22:54 sasa Exp $*/
/*02.07.2015	20.03.2010	Белых А.И.	iceb_t_progforlst.c
Чтение и выбор настройки для выполнения программы
Возвращаем 0- программа выполнена
           1- нет
*/
#include <pwd.h>
#include <errno.h>
#include	"buhl.h"



int iceb_t_progforlst(const char *imaf)
{
class SPISOK str_menu;
class SPISOK prog;

char strok[1024];
class iceb_tu_str bros("");
struct passwd *rek_login;

rek_login=getpwuid(getuid());

class iceb_tu_str imaf_nast;

imaf_nast.plus(rek_login->pw_dir);
imaf_nast.plus(G_DIR_SEPARATOR_S,ICEB_KAT_ALX);
imaf_nast.plus(G_DIR_SEPARATOR_S,"progforlst.alx");

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
  if(polen(strok,&bros,1,'|') != 0)
   continue;
  if(bros.ravno()[0] == '\0')
   continue;

  str_menu.plus(bros.ravno());

  polen(strok,&bros,2,'|');
  prog.plus(bros.ravno());
 }
fclose(ff);

if(str_menu.kolih() == 0)
 {
  iceb_t_soob(gettext("Не найдено настроек !"));
  return(1);
 }

int voz=0;
if(str_menu.kolih() > 1)
 {
  int kod_kl=0;
  voz=dirmasiv(&str_menu,-1,-1,0,gettext("Выберите нужное и нажмите Enter"),1,&kod_kl);
  if(voz < 0 || kod_kl != ENTER)
   return(1);
 }

sprintf(strok,"%s %s",prog.ravno(voz),imaf);

int kodv=runsystem(strok);
if(kodv != 0)
 {
  sprintf(strok,"%s %d!\n%s",gettext("Ошибка выполнения программы"),kodv,strok);
  iceb_t_soob(strok);
 } 
return(kodv);
}



