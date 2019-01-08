/* $Id: matudd.c,v 5.21 2013-04-07 10:22:43 sasa Exp $ */
/*19.10.2018    25.01.1994      Белых А.И.      matudd.c
Удаляем в карточке запись или во всех карточках с указанным номером
документа
Если в документе удалены все записи удаляем сам документа

Если запись на приход то необходимо проверить
какой остаток будет в карточке если запись удалить
Если остаток отрицательный то были проведены расходы с учетом
этого материалла
Если запись на расход то удаляем без проверок
Если вернули 0 - накладная удалена полностью
	     1 - удалена не полностью
*/
#include	"buhg.h"

int            udmk10(int,short,short,short,const char*,int,int);

int matudd(short mu, //0-записи во всех карточках 1-в конкретной карточке
short mprv, //*0-проверять на остаток 1-нет
short mun, //*0-Удалять накладную 1 - нет
int tipz,
short d,short m,short g,const char *nomdok,int skl,int nk,int kodm)
{
class ostatokt ost;
double          kol;
char		strsql[512];
long		kolstr;
long		kolust=0;
SQL_str         row,row1;
int		kodmm;

/*
printw("\nРежим удаления %d.%d.%d\n",d,m,g);
refresh();
*/
if(mprv == 0)
 GDITE();
kolust=0;

sprintf(strsql,"select nomkar,kodm from Dokummat1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
g,m,d,skl,nomdok);

if(mu == 1)
 sprintf(strsql,"select nomkar,kodm from Dokummat1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
 g,m,d,skl,nomdok,kodm,nk);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

/*
  printw("\nstrsql=%s\nkolstr=%ld\n",strsql,kolstr);
  OSTANOV();
*/
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  nk=atoi(row[0]);
  kodmm=atoi(row[1]);
  if(nk == 0)
    goto vper1;
    
  if(tipz == 1 && mprv == 0 && nk != 0)
   {

    ostkar(1,1,g,31,m,g,skl,nk,&ost);

    if((kol=readkolk(skl,nk,d,m,g,nomdok)) == 0)
      goto vpr;

    if(ost.ostg[3]-kol < -0.000000001)
     {
      class iceb_tu_str naimat("");
      sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
        naimat.new_plus(row1[0]);
       
      VVOD SOOB(1);

      sprintf(strsql,"%s-%s %s",__FUNCTION__,row[1],naimat.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);

      sprintf(strsql,gettext("Склад %d, карточка %d - невозможно удалить приход !"),
      skl,nk);
      SOOB.VVOD_SPISOK_add_MD(strsql);

      sprintf(strsql,gettext("Документ %s дата %d.%d.%dг."),
      nomdok,d,m,g);
      SOOB.VVOD_SPISOK_add_MD(strsql);

      sprintf(strsql,"%s %f",gettext("Отрицательный остаток."),ost.ostg[3]-kol);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      return(1);
     }

    if(udmk10(tipz,d,m,g,nomdok,skl,nk) != 0)
      return(1);
vpr:;
    udkar(skl,nk);

   }
  else
   {
    if(udmk10(tipz,d,m,g,nomdok,skl,nk) != 0)
      return(1);
    udkar(skl,nk);
   }

  if(tipz == 1 && mprv == 1)
   {
    udkar(skl,nk);
   }


vper1:;

  sprintf(strsql,"delete from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
  g,m,d,skl,nomdok,kodmm,nk);
/*
  printw("\nstrsql=%s\nkolust=%ld\n",strsql,kolust);
  OSTANOV();
*/
  if(sql_zapis(strsql,1,0) == 0)
   {
    kolust++;

    sprintf(strsql,"delete from Dokummat4 where datd='%d-%02d-%02d' and skl=%d and nomd='%s' and km=%d and nk=%d",
    g,m,d,skl,nomdok,kodmm,nk);
    sql_zapis(strsql,1,0);
   }    
 }

/* Если в накладной удалена последняя запись
 то нужно удалить и саму накладную*/

if(mun == 0 && kolust == kolstr)
 {
  matud(nomdok,d,m,g,skl,tipz);
 } 

return(0);
}
/****************************/
/*Если вернули 1 доступ к базе толко на чтение*/
int udmk10(int tipz,short d,short m,short g,const char *nomdok,int skl,int nk)
{
char		strsql[512];

sprintf(strsql,"delete from Zkart where datd='%d-%02d-%02d' and nomd='%s' and sklad=%d and nomk=%d",
g,m,d,nomdok,skl,nk);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return(1); 
  }
 else
   msql_error(&bd,"",strsql);
 }
return(0);
}
