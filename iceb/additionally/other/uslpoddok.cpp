/*$Id: uslpoddok.c,v 5.4 2011-01-13 13:43:24 sasa1 Exp $*/
/*07.04.2009	07.04.2009	Белых А.И.	uslpoddok.c
Подпрограмма подтверждения записей в документе учёта услуг
*/
#include <math.h>
#include "buhg.h"

void uslpoddok(short dd,short md,short gd,
short d1,short m1,short g1,
const char *nomdok,
int podr,
int tipz,
int metka_soob) /*0-выводить сообщения 1-нет*/
{
char strsql[512];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,nz from \
Usldokum1 where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gd,md,dd,podr,nomdok,tipz);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  if(metka_soob == 0)
    iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }
int nz=0;
int metka=0;
int kodzap=0;
double cena=0.;
double kol1=0.,kol2=0.;
char naimat[512];
while(cur.read_cursor(&row) != 0)
 {
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kol1=atof(row[2]);
  cena=atof(row[3]);
  nz=atoi(row[6]);  
  /*Узнаем наименование материалла*/
  if(metka == 0)
   sprintf(strsql,"select naimat from Material where kodm=%s",
   row[1]);
  if(metka == 1)
   sprintf(strsql,"select naius from Uslugi where kodus=%s",
   row[1]);

  if(readkey(strsql,&row1,&cur1) != 1)
    {
     if(metka_soob == 0)
      {    
       sprintf(strsql,"Не найден код %s !",row[1]);
       iceb_t_soob(strsql);
      }     
    }
  strncpy(naimat,row1[0],sizeof(naimat)-1);
  if(metka_soob == 0)
   printw("%-4s %-20.20s %10.10g - ",
   row[1],naimat,kol1);
  refresh();


  /*Читаем в карточке количество реально выданное-полученное*/

  kol2=readkolu(dd,md,gd,podr,nomdok,tipz,metka,kodzap,nz);

//  printw("kol1=%f kol2=%f\n",kol1,kol2);
  //Количество может быть отрицательным
  if(fabs(kol1) > fabs(kol2))
   {
    if(metka_soob == 0)
     printw(gettext("подтверждена\n"));
    zappodu(tipz,dd,md,gd,d1,m1,g1,nomdok,metka,kodzap,kol1-kol2,cena,row[4],row[5],podr,nz);
   }
  else
    if(metka_soob == 0)
     printw(gettext("подтверждена раньше\n"));


 }

}
