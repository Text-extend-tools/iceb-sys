/*$Id:$*/
/*03.01.2015	03.01.2015	Белых А.И.	impmatdok_xml_rt.c
импорт налоговых накладных в формате XML
*/
#include        <sys/stat.h>
#include "buhg.h"

int impmatdok_xml_mt(class iceb_imp_xml_nn_r *rk);

int impmatdok_xml_rt(const char *imaf)
{
struct stat work;
class iceb_tu_str stroka("");
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kodmat=0;
int kodmat_start=1;
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

if(impmatdok_xml_mt(&rk) != 0)
 return(1);;

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

  /*проверяем есть ли наименование материала в справочнике материалов*/
  sprintf(strsql,"select kodm from Material where naimat='%s'",sql_escape_string(row[1],0));
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    kodmat=atoi(row1[0]);
   }
  else
   {
    class lock_tables lk("LOCK TABLES Material WRITE,Uslugi READ,icebuser READ");

    kodmat=nomkm(kodmat_start);

    sprintf(strsql,"insert into Material (kodm,naimat,ktoz,vrem) values(%d,'%s',%d,%ld)",kodmat,sql_escape_string(row[1],0),iceb_t_getuid(),time(NULL));
    if(sql_zapis(strsql,1,0) == 0)
     {
      kodmat_start=kodmat;
     }
    else 
     continue;    
   }

  sprintf(strsql,"select kodm from Material where kodm=%d",kodmat);
  if(readkey(strsql) != 1)
   {
    repl.new_plus(gettext("Не найден код материалу"));
    repl.plus(" ");
    repl.plus(kodmat);
    repl.plus(" !");
    iceb_t_soob(repl.ravno());	
    continue;
   }

  kolih=atof(row[4]);

  if(kolih == 0)
   {
    repl.new_plus(gettext("В записи нулевое количество материалла"));
    repl.plus(" !");
    iceb_t_soob(repl.ravno());
    continue;
   }

  eiz.new_plus(row[2]);
  cena=atof(row[5]);
  
  zap_prihod(rk.datdok.ravno(),rk.nomdok.ravno(),rk.sklad.ravno_atoi(),kodmat,kolih,cena,eiz.ravno(),rk.shetu.ravno(),rk.pnds,0,rk.nds,"","");

 }

podtdok1(rk.datdok.ravno(),rk.sklad.ravno_atoi(),rk.nomdok.ravno(),tipz,rk.datdok.ravno(),1);
podvdok(rk.datdok.ravno(),rk.nomdok.ravno(),rk.sklad.ravno_atoi());


prospr(1,rk.sklad.ravno_atoi(),rk.datdok.ravno(),rk.nomdok.ravno(),tipz,rk.nds,0,"",rk.kod_op.ravno()); /*составляем список проводок которые должны быть сделаны*/
avtpromu1(tipz,rk.datdok.ravno(),rk.nomdok.ravno(),NULL); /*делаем проводки*/
prospr(0,rk.sklad.ravno_atoi(),rk.datdok.ravno(),rk.nomdok.ravno(),tipz,rk.nds,0,"",rk.kod_op.ravno()); /*проверяем сделаны проводки все или нет*/

iceb_t_soob(gettext("Загрузка завершена"));
return(0);
}
