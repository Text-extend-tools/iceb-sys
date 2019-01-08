/*$Id:$*/
/*16.04.2017	18.04.2015	Белых А.И.	zagf1df_csv.c
Загрузка формы 1ДФ из файла в формате cvs
*/

#include        <sys/stat.h>
#include "buhg.h"

int zagf1df_csv(const char *nomd,int kolst)
{
class iceb_tu_str stroka("");
class iceb_tu_str inn("");
class iceb_tu_str fio("");
class iceb_tu_str kodstr("");
class iceb_tu_str priz("");
class iceb_tu_str lgota("");
class iceb_tu_str vidz("");
class iceb_tu_str bros("");
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
short g1=0,m1=0,d1=0;
short g2=0,m2=0,d2=0;

struct stat work;
double sfr=0.,svs=0.,sfrv=0.,svsv=0.; /*военный сбор*/
double sumad=0.,suman=0.,sumadn=0.,sumann=0.;

const char *imaf_csv={"f1df.csv"};

if(stat(imaf_csv,&work) != 0)
 {
  sprintf(strsql,"%s: %s !",gettext("Не найден файл"),imaf_csv);
  iceb_t_soob(strsql);
  return(1);
 }

clear();

if(kolst != 0)
 {
  VVOD DANET(1);
  DANET.VVOD_SPISOK_add_MD(gettext("Удалить уже введенные данные ?"));
  if(danet(&DANET,2,stdscr) == 1)
   {
    sprintf(strsql,"delete from F8dr1 where nomd='%s'",nomd);
    if(sql_zapis(strsql,0,0) != 0)
     return(1);
     
    kolst=0;    

    sprintf(strsql,"delete from F1dfvs where nomd='%s'",nomd);
    sql_zapis(strsql,0,0);
   }
 }

class iceb_fopen fil;
if(fil.start(imaf_csv,"r") != 0)
 return(1);
int kolih=0;
int razmer=0;
while(iceb_tu_fgets(&stroka,fil.ff) == 0)
 {
  razmer+=strlen(stroka.ravno());

  strzag(LINES-1,0,work.st_size,razmer);
  printw("%s",stroka.ravno());

  if(stroka.ravno()[0] == '#')
   continue;

  if(polen(stroka.ravno(),&kodstr,1,'|') != 0)
   continue;

  if(SRAV(kodstr.ravno(),"KR",0) == 0)
   {
    polen(stroka.ravno(),&kolih,2,'|');
    sprintf(strsql,"update F8dr set kr=%d where nomd='%s'",kolih,nomd);
    sql_zapis(strsql,1,0);
    continue; 
   }
  if(SRAV(kodstr.ravno(),"KRDP",0) == 0)
   {
    polen(stroka.ravno(),&kolih,2,'|');
    sprintf(strsql,"update F8dr set ks=%d where nomd='%s'",kolih,nomd);
    sql_zapis(strsql,1,0);
    continue; 
   }

  fio.new_plus(""); /*Если из главной книги то с фамилией, если из зарплаты то без фамилии*/


  if(SRAV(kodstr.ravno(),"VS",0) == 0) /*военный сбор*/
   {
    /*военный сбор грузится только из зарплаты*/
    polen(stroka.ravno(),&inn,2,'|');
    polen(stroka.ravno(),&sfr,3,'|');
    polen(stroka.ravno(),&sfrv,4,'|');
    polen(stroka.ravno(),&svs,5,'|');
    polen(stroka.ravno(),&svsv,6,'|');
    
    /*Делаем запись в таблицу для военного сбора*/
    sprintf(strsql,"insert into F1dfvs \
values ('%s','%s','%s',%.2f,%.2f,%.2f,%.2f,%d,%ld)",
    nomd,inn.ravno(),fio.ravno_filtr(),sfr,svs,sfrv,svsv,iceb_t_getuid(),time(NULL));

  zpvs:;

    if(sql_zap(&bd,strsql) != 0)
     {
      if(kolst != 0 && sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
       {
        printw("Повторяющаяся запись. Увеличивем значения в уже введенной !\n");
        sprintf(strsql,"update F1dfvs set sfr=sfr+%.2f, svs=svs+%.2f,\
  sfrv=sfrv+%.2f, svsv=svsv+%.2f where nomd='%s' and inn='%s'",
        sfr,svs,sfrv,svsv,nomd,inn.ravno());
        goto zpvs;
       }
      msql_error(&bd,"Ошибка ввода записи !",strsql);
     }
    continue;
   }

  if(SRAV(kodstr.ravno(),"PN",0) == 0) /*подоходный налог*/
   {

    polen(stroka.ravno(),&inn,2,'|');
    /*Проверяем наличие в базе индентификационного кода*/
    //У частных предпринимателей нет карточки
    
    sprintf(strsql,"select inn from Kartb where inn='%s'",inn.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) < 1)
     {
      sprintf(strsql,"select naikon from Kontragent where kod='%s'",inn.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur) < 1)
       {
        VVOD SOOB(1);
        sprintf(strsql,"%s %s !",
        gettext("Не найден индентификационный код"),inn.ravno());
        SOOB.VVOD_SPISOK_add_MD(strsql);
        SOOB.VVOD_SPISOK_add_MD(gettext("Запись не загружена !"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        continue;
       }
      fio.new_plus(row[0]);
     }

    polen(stroka.ravno(),&sumadn,3,'|');
    polen(stroka.ravno(),&sumad,4,'|');
    polen(stroka.ravno(),&sumann,5,'|');
    polen(stroka.ravno(),&suman,6,'|');
    polen(stroka.ravno(),&priz,7,'|');
    polen(stroka.ravno(),&bros,8,'|');
    rsdat(&d1,&m1,&g1,bros.ravno(),1);
    polen(stroka.ravno(),&bros,9,'|');
    rsdat(&d2,&m2,&g2,bros.ravno(),1);
    polen(stroka.ravno(),&lgota,10,'|');
    polen(stroka.ravno(),&vidz,11,'|');
    if(vidz.getdlinna() <= 1)
     vidz.new_plus("0");        


    /*Делаем запись в базу*/
    sprintf(strsql,"insert into F8dr1 \
  values ('%s','%s',%.2f,%.2f,'%s','%04d-%d-%d','%04d-%d-%d','%s',%d,%ld,'%s',%.2f,%.2f,%s)",
    nomd,inn.ravno(),sumad,suman,priz.ravno(),g1,m1,d1,g2,m2,d2,lgota.ravno(),iceb_t_getuid(),time(NULL),fio.ravno_filtr(),sumadn,sumann,vidz.ravno());

  zp:;

    if(sql_zap(&bd,strsql) != 0)
     {
      if(kolst != 0 && sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
       {
        printw("Повторяющаяся запись. Увеличивем значения в уже введенной !\n");
        sprintf(strsql,"update F8dr1 set sumad=sumad+%.2f, suman=suman+%.2f,\
  sumadn=sumadn+%.2f, sumann=sumann+%.2f where nomd='%s' and inn='%s' and priz='%s' and lgota='%s'",
        sumad,suman,sumadn,sumann,nomd,inn.ravno(),priz.ravno(),lgota.ravno());
        goto zp;
       }
      msql_error(&bd,"Ошибка ввода записи !",strsql);
     }

   }
 }
fil.end();

sprintf(strsql,"%s !",gettext("Загрузка завершена"));
iceb_t_soob(strsql);

unlink(imaf_csv);

return(0);
}