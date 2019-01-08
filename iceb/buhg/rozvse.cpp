/* $Id: rozvse.c,v 5.28 2013/08/13 05:49:54 sasa Exp $ */
/*15.07.2015	02.02.1999	Белых А.И.	rozvse.c
Распечатка всех продаж для конкретного рабочего места
или для всех рабочих мест
*/
#include	<errno.h>
#include	"buhg.h"


void rozvse(short dp,short mp,short gp,int kassa,const char *fam)
{
struct  tm      *bf;
time_t		vrem;
SQL_str         row,row1;
long		kolstr;
char		strsql[512];
double		bb;
int		np,npz;
double		it,itp,itk,ito;
char		imaf[64];
FILE		*ff;
int		kas,kasz;
class iceb_tu_str naim("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
short		dn,mn,gn;
short		dk,mk,gk;
short		d,m,g;
int		kom=0;

if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d.%d",dp,mp,gp);
  dat1.new_plus(strsql);
  dat2.new_plus(strsql);
 }

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка продаж за период"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }


if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

GDITE();

if(kassa != 0)
  sprintf(strsql,"select * from Roznica \
where datrp >= '%d-%02d-%02d' and datrp<= '%d-%02d-%02d' and kassa=%d order by datrp,nomer asc",
gn,mn,dn,gk,mk,dk,kassa);

if(kassa == 0)
  sprintf(strsql,"select * from Roznica \
where datrp >= '%d-%02d-%02d' and datrp<= '%d-%02d-%02d' order by kassa,datrp,nomer asc",
gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
//printw("\n%s\n",strsql);
//OSTANOV();

sprintf(imaf,"vse%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));
fprintf(ff,"%s %d.%d.%d => %d.%d.%d\n",gettext("Распечатка списка продаж за период"),
dn,mn,gn,dk,mk,gk);
if(kassa != 0)
  fprintf(ff,"%s%d\n",gettext("По рабочему месту N"),kassa);
else
  fprintf(ff,gettext("По всем рабочим местам.\n"));

time(&vrem);
bf=localtime(&vrem);
fprintf(ff,"%s %d.%d.%d  %d:%d\n",gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" Дата     |  Наименование товара         |Е и|Количество|  Цена    |  Сумма   |К.мт|N м|N пр|Время| Кто |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");

clear();
printw("%s %d.%d.%d\n",gettext("Распечатка списка продаж за"),dp,mp,gp);

GDITE();
kas=kasz=npz=np=0;
ito=itk=itp=it=0;
SQLCURSOR cur1;
while((cur.read_cursor(&row)) != 0)
 {
  np=atoi(row[2]);
  kas=atoi(row[1]);
  if(npz != 0 && npz != np)
   {
    fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(67,gettext("Итого за покупку:")),
    gettext("Итого за покупку:"),itp);
    itp=0.;
   }   

  if(kasz != 0 && kasz != kas)
   {
    sprintf(strsql,"%s %d:",gettext("Итого по месту"),kasz);
    fprintf(ff,"%*s %10.2f\n\n",iceb_tu_kolbait(67,strsql),strsql,itk);
    itk=0.;
   }   
  npz=np;
  kasz=kas;
  naim.new_plus("");
  if(row[3][0] == '1')
   {
    /*Читаем наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
       naim.new_plus(row1[0]);
   }
  else
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naiust from Uslugi where kodus=%s",row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
       naim.new_plus(row1[0]);
   }

  rsdat(&d,&m,&g,row[0],2);      
  bb=atof(row[6])*atof(row[7]);  
  bb=okrug(bb,0.01);

  it+=bb;
  itp+=bb;
  itk+=bb;
  ito+=bb;
  vrem=atol(row[11]);
  
//  printw("%s %ld\n",row[11],vrem);
//  OSTANOV();
  
  bf=localtime(&vrem);

  fprintf(ff,"%02d.%02d.%d %-*.*s %-*s %10.10g %10s %10.2f\
 %-4s %-4s %-3s %02d:%02d %s\n",
  d,m,g,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(3,row[5]),row[5],
  atof(row[6]),row[7],bb,
  row[3],row[1],row[2],bf->tm_hour,bf->tm_min,row[10]);

 }

fprintf(ff,"%67s %10.2f\n",gettext("Итого за покупку:"),itp);

sprintf(strsql,"%s %d:",gettext("Итого по месту"),kasz);
fprintf(ff,"%67s %10.2f\n\n",strsql,itk);

if(kassa == 0)
  fprintf(ff,"%66s: %10.2f\n\n",gettext("Общий итог"),ito);

podpis(ff);

fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/


fclose(ff);        


class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка всех продаж"));

iceb_t_ustpeh(imaf,1);

iceb_t_rabfil(&oth,"");
}
