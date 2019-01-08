/*$Id:$*/
/*04.02.2015	04.02.2015	Белых А.И.	impusldok_xml_r.c
Импорт полученных налоговых накладных в формате XML
*/
#include        <sys/stat.h>
#include "buhg.h"

int impusldok_xml_m(class iceb_imp_xml_nn_r *rk);

int imulsdok_xml_r(const char *imaf)
{
struct stat work;
class iceb_tu_str stroka("");
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kodusl=0;
int kodusl_start=1;
double kolih=0.;
double cena=0.;
class iceb_tu_str eiz("");
int tipz=1;
class iceb_tu_str repl("");
class iceb_imp_xml_nn_r rk;

if(stat(imaf,&work) != 0)
 {
  repl.new_plus(gettext("Нет такого файла !"));
  repl.ps_plus(imaf);
  iceb_t_soob(repl.ravno());
  return(1);  
 }

rk.ima_tablic.new_plus("nalnakmu");
if(iceb_t_imp_xml_nn(imaf,&rk) != 0)
 {
  return(1);;
 }

if(impusldok_xml_m(&rk) != 0)
 return(1);;

clear();

/*загружаем записи в документ*/
sprintf(strsql,"select * from %s order by nz asc",rk.ima_tablic.ravno());
if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);;
 }

printw("%s\n",gettext("Загружаем содержимое документа"));

while(cur.read_cursor(&row) != 0)
 {
  printw("%2s %-*.*s %-*s %2s %10.10g %10.10g %10.10g %10.10g %10.10g %10.10g\n",
  row[0],
  iceb_tu_kolbait(20,row[1]),
  iceb_tu_kolbait(20,row[1]),
  row[1],
  iceb_tu_kolbait(5,row[2]),
  row[2],
  row[3],atof(row[4]),atof(row[5]),atof(row[6]),atof(row[7]),atof(row[8]),atof(row[9]));

  refresh();
  
  /*проверяем есть ли такое наименование в списке услуг*/
  sprintf(strsql,"select kodus from Uslugi where naius='%s'",sql_escape_string(row[1],0));
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    kodusl=atoi(row1[0]);
   }
  else
   {
    class lock_tables lk("LOCK TABLES Material READ,Uslugi WRITE,icebuser READ");
    kodusl=nomkm(kodusl_start);
    sprintf(strsql,"insert into Uslugi (kodus,naius,ktoz,vrem) values(%d,'%s',%d,%ld)",kodusl,sql_escape_string(row[1],0),iceb_t_getuid(),time(NULL));

    if(sql_zapis(strsql,1,0) == 0)
     {
      kodusl_start=kodusl;
     }
    else
     continue;
   }
  
  kolih=atof(row[4]);

  if(kolih == 0)
   {
    repl.new_plus(gettext("В записи нулевое количество"));
    repl.plus(" !");
    iceb_t_soob(repl.ravno());
    continue;
   }

  eiz.new_plus(row[2]);
  cena=atof(row[5]);
  zapuvdok(tipz,rk.datdok.ravno(),rk.nomdok.ravno_filtr(),1,kodusl,kolih,cena,eiz.ravno_filtr(),rk.shetu.ravno(),rk.sklad.ravno_atoi(),"","");

 }

uslpoddok(rk.datdok.ravno(),rk.datdok.ravno(),rk.nomdok.ravno(),rk.sklad.ravno_atoi(),tipz,1); /*подтверждаем документ*/
podudok(rk.datdok.ravno(),rk.nomdok.ravno(),rk.sklad.ravno_atoi(),tipz); /*проверяем подтверждение и ставим метку подтверждённого документа*/

rk.pnds=iceb_t_pnds(rk.datdok.ravno()); /*получаем действующее значение НДС*/

prosprus(1,rk.sklad.ravno_atoi(),rk.datdok.ravno(),rk.nomdok.ravno(),tipz,rk.nds,rk.kod_op.ravno(),rk.pnds); /*составляем список проводок которые должны быть сделаны*/
avtprousl1(tipz,rk.datdok.ravno(),rk.nomdok.ravno(),NULL); /*делаем проводки*/
prosprus(0,rk.sklad.ravno_atoi(),rk.datdok.ravno(),rk.nomdok.ravno(),tipz,rk.nds,rk.kod_op.ravno(),rk.pnds); /*проверяем сделаны ли проводки и ставим метку выполненных проводок*/

iceb_t_soob(gettext("Загрузка завершена"));

return(0);
}