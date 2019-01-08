/*$Id: provudvdok.c,v 5.18 2013/08/13 05:49:51 sasa Exp $*/
/*15.07.2015	23.08.2001	Белых А.И. 	provudvdok.c
Проверка возможности удаления записи в приходном документе
или всего документа
Если вернули 0 -можно удалить
             1 - нельзя
*/
#include	"buhg.h"

//extern double          ostg[4];

int provudvdok(int sklad,//Склад
short dd,short md,short gd,//Дата документа
const char *nomd,//Номер документа
int	kodmat, //Код материалла
int	nomkart)//Номер карточки
{
class ostatokt ost;
char		strsql[512];
SQL_str		row,row1;
SQLCURSOR       cur;
SQLCURSOR       cur1;
int		kolstr;
struct  tm      *bf;
time_t          tmm;
double kol=0.;
short		m,g;
class iceb_tu_str naimat("");
int		vozvrat=0;

/*
printw("\nprovudvdok-%d %d.%d.%d %s %d %d\n",
sklad,dd,md,gd,nomd,kodmat,nomkart);
OSTANOV();
*/

time(&tmm);
bf=localtime(&tmm);
//d=bf->tm_mday;
m=bf->tm_mon+1;
g=bf->tm_year+1900;

if(kodmat != 0)
 {
  ostkar(1,1,g,31,m,g,sklad,nomkart,&ost);
  if((kol=readkolk(sklad,nomkart,dd,md,gd,nomd)) == 0)
    return(0);

  if(ost.ostg[3]-kol < -0.000000001)
   {
    sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
      naimat.new_plus(row[0]);
    else
      naimat.new_plus("");

    VVOD SOOB(1);
  
    sprintf(strsql,"%d %s",kodmat,naimat.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,gettext("Склад %d, карточка %d - невозможно удалить приход !"),
    sklad,nomkart);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,gettext("Документ %s дата %d.%d.%dг."),
    nomd,dd,md,gd);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %f",gettext("Отрицательный остаток"),ost.ostg[3]-kol);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
   }
 }
else
 {
  sprintf(strsql,"select nomkar,kodm from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and tipz=1",
  gd,md,dd,sklad,nomd);
//printw("\n%s\n",strsql);
//OSTANOV();
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  if(kolstr == 0)
     return(0);  
  
  int nomer_kart=0;
  
  while(cur.read_cursor(&row) != 0)
   {
//    printw("%s %s\n",row[0],row[1]);
    
    nomer_kart=atoi(row[0]);
    ostkar(1,1,g,31,m,g,sklad,nomer_kart,&ost);
    if((kol=readkolk(sklad,nomer_kart,dd,md,gd,nomd)) == 0)
      continue;

    if(ost.ostg[3]-kol < -0.000000001)
     {
      vozvrat++;
      sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
        naimat.new_plus(row1[0]);
      else
        naimat.new_plus("");

      VVOD SOOB(1);

      sprintf(strsql,"%s %s",row[1],naimat.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);

      sprintf(strsql,gettext("Склад %d, карточка %d - невозможно удалить приход !"),
      sklad,nomer_kart);
      SOOB.VVOD_SPISOK_add_MD(strsql);

      sprintf(strsql,gettext("Документ %s дата %d.%d.%dг."),
      nomd,dd,md,gd);
      SOOB.VVOD_SPISOK_add_MD(strsql);

      sprintf(strsql,"%s %f",gettext("Отрицательный остаток"),ost.ostg[3]-kol);
      SOOB.VVOD_SPISOK_add_MD(strsql);

      soobshw(&SOOB,stdscr,-1,-1,0,1);
     }
   }

 }
//OSTANOV();
return(vozvrat);
}
