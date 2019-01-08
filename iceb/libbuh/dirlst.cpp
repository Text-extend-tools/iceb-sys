/*$Id: dirlst.c,v 5.16 2014/04/30 06:12:32 sasa Exp $*/
/*23.05.2018    15.01.1993      Белых А.И.  dirlst.c
Подпрограмма работы с файлами листингов
распечатка файлов на АЦПУ
редактирование и просмотр в редакторе
удаление
*/
#include	"buhl.h"

void dirlst(const char *tfima) //Трафарет имени файла
{
class SPISOK	mfa; /*Массив с именами файлов*/
class spis_oth oth;
char bros[1024];
if(dirfajl(tfima,2,&oth.spis_imaf) == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного файла !"));
  return;
 }

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 {
  sprintf(bros,"%s %d",gettext("Файл"),nom+1);
  oth.spis_naim.plus(bros);
  oth.spis_ms.plus(oth.spis_imaf.ravno(nom));
 }   
iceb_t_rabfil(&oth,"");
}
