/*$Id:$*/
/*15.02.2016	15.02.2016	Белых А.И.	iceb_t_zkvskf.c
Запись контрагента в справочник контрагентов
применяется при иморте документов в базу
Записываем контрагента и его реквизиты
Если 

Если вернули 0-такой контрагент уже есть в базе
             1-записали в базу
            -1-не смогли записать в базу

*/
#include "bezcurses.h"

int iceb_t_zkvsk(const char *strok,int *nom_kod_kontr,FILE *ff_prot)
{
char strsql[2048];
int metka_zapisi=0;
#define KOLPOL 17
class iceb_tu_str rekkon[KOLPOL];
class iceb_tu_str is("");

if(polen(strok,&is,1,'|') != 0) /*не найдено поле*/
 {
  return(-1);
 }

if(SRAV(is.ravno(),"KON",0) != 0) /*строка не та что нужно*/
 return(-1);
 
for(int nom=0; nom < KOLPOL; nom++)
 polen(strok,&rekkon[nom],nom+2,'|');

if(rekkon[1].getdlinna() <= 1)/*не введено наименование контрагента*/
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"%s",strok);
    fprintf(ff_prot,"#%s|\n",gettext("Не введено наименование контрагента"));
   }
  return(-1);
 }

/*записывается код контрагента с наименованием*/
if((metka_zapisi=iceb_t_get_kkfname(rekkon[1].ravno(),rekkon[0].ravno(),&rekkon[0],nom_kod_kontr)) < 0)
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"%s",strok);
    fprintf(ff_prot,"#%s\n",gettext("Не смогли добавить контрагента в справочкик!"));
   }
  return(-1);
 }

if(metka_zapisi == 0) /*если есть в базе то остальные реквизиты не пишем*/
 return(0);

/*проверяем есть ли такая группа в справочнике групп и если нет то обнуляем*/
if(rekkon[11].getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Gkont where kod=%d",rekkon[11].ravno_atoi());
  if(readkey(strsql) <= 0)
   rekkon[11].new_plus("");
 }

sprintf(strsql,"update Kontragent set \
naikon='%s',\
naiban='%s',\
adres='%s',\
adresb='%s',\
kod='%s',\
mfo='%s',\
nomsh='%s',\
innn='%s',\
nspnds='%s',\
telef='%s',\
grup='%s',\
ktoz=%d,\
vrem=%ld,\
na='%s',\
regnom='%s',\
pnaim='%s',\
gk='%s',\
en=%d \
where kodkon='%s'",
rekkon[1].ravno_filtr(),
rekkon[2].ravno_filtr(),
rekkon[3].ravno_filtr(),
rekkon[4].ravno_filtr(),
rekkon[5].ravno_filtr(),
rekkon[6].ravno_filtr(),
rekkon[7].ravno_filtr(),
rekkon[8].ravno_filtr(),
rekkon[9].ravno_filtr(),
rekkon[10].ravno_filtr(),
rekkon[11].ravno_filtr(),
iceb_t_getuid(),
time(NULL),
rekkon[12].ravno_filtr(),
rekkon[13].ravno_filtr(),
rekkon[14].ravno_filtr(),
rekkon[15].ravno_filtr(),
rekkon[16].ravno_atoi(),
rekkon[0].ravno_filtr());


if(sql_zapis(strsql,1,0) != 0)
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"%s",strok);
    fprintf(ff_prot,"#%s Kontragent !\n",gettext("Ошибка корректировки записи записи в таблице"));
   }
  return(-1);
 }

return(0);



}
