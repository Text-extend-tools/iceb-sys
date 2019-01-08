/* $Id: sumdok.c,v 5.18 2014/07/31 07:09:17 sasa Exp $ */
/*14.11.2013	05.02.1999	Белых А.И.	sumdok.c
Вычисление данных по документу
*/
#include	"buhg.h"

extern short	vtara; /*Код группы возвратная тара*/
extern double	okrcn;
extern double   okrg1;

void sumdok(short d,short m,short g,
int skl,
const char *nn,
SQLCURSOR *cur,
double *suma, //Сумма по документа
double *sumabn, //Сумма без НДС*/
double *ves,  //Вес по накладной
int *mu,      //Метка услуг
int lnds, //Метка НДС*/
double *kolmat, //Количество материалов в документе
double *sumsnds,
float pnds,
int *max_dlina_nazv)
{
long		kolstr;
char		strsql[512];
SQL_str         row,row1;
double		kolih;
double		cena;
double		bb;
int		kodm,kgrm;
float		fas;
class iceb_tu_str ei("");
short		vtr;
short		mnds; /*1-НДС в том числе*/


//printw("sumdok\n");

*sumsnds=*kolmat=*suma=*sumabn=*ves=0.;
*mu=0;
*max_dlina_nazv=0;

fas=kgrm=0;
SQLCURSOR cur1;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  kodm=atoi(row[4]);
  kolih=atof(row[5]);
  *kolmat=*kolmat+kolih;
  cena=atof(row[6]);
//  Для вложенного налога
  if(lnds == 0 || lnds == 4)
    bb=cena+cena*pnds/100.;
  else
    bb=cena;

  bb=okrug(bb,okrg1);
  bb=bb*kolih;
  *sumsnds+=okrug(bb,okrg1);

  vtr=atoi(row[8]);
  ei.new_plus(row[7]);
  mnds=atoi(row[10]);
  bb=cena*kolih;
  bb=okrug(bb,okrg1);
  
  fas=0.;
  kgrm=0;

  sprintf(strsql,"select naimat,fasv,kodgr from Material where kodm=%d",kodm);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    //printw("\nstrsql=%s\n",strsql);
    printw("%s-%s %d !\n",__FUNCTION__,gettext("Не найден код материалла"),kodm);
    OSTANOV();        
   }
  else
   {

    fas=atof(row1[1]);
    kgrm=atoi(row1[2]);
    if(*max_dlina_nazv < iceb_tu_strlen(row1[0]))
     *max_dlina_nazv=iceb_tu_strlen(row1[0]);
   }
  if(vtr == 1 || (kgrm == vtara && vtara != 0) || (lnds > 0 && lnds < 4) || mnds == 1)
   {
    *sumabn+=bb;
   }
  else
   {
    *suma+=bb;
   }
  /*Читаем карточку материалла*/
  if(atoi(row[3]) != 0)
   {
    fas=0.;
    sprintf(strsql,"select fas from Kart where sklad=%s and nomk=%s",
    row[1],row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw("%sk-",__FUNCTION__);
      printw(gettext("Не нашли карточки %d склад %d !"),
      atoi(row[3]),atoi(row[1]));
      printw("\n");
      OSTANOV();
     }
    else
      fas=atof(row1[0]);

   }
  
  if(fas > 0.00001 )
   {
    *ves=*ves+kolih*fas;
   }
  else 
    if(SRAV(ei.ravno(),"Кг",0) == 0 || SRAV(ei.ravno(),"кг",0) == 0 || SRAV(ei.ravno(),"КГ",0) == 0)
     {
      *ves=*ves+kolih;
     }
  
 }

//printw("\nsumsnd=%f\n",*sumsnds);
//OSTANOV();

/*Читаем услуги если они есть*/
//printw("читаем услуги\n");

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kolih,cena from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",g,m,d,skl,nn);
/*printw("\nstrsql=%s\n",strsql);*/

if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
  return;

while(cur1.read_cursor(&row) != 0)
 {
  *mu=*mu+1;
  kolih=atof(row[0]);
  cena=atof(row[1]);
  cena=okrug(cena,okrcn);
  
  if(kolih > 0)
    bb=kolih*cena;
  else
    bb=cena;
  bb=okrug(bb,okrg1);
  *suma+=bb;

 } 

}
