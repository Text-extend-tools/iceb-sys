/*$Id: razuz1.c,v 5.16 2013-04-07 10:22:44 sasa Exp $*/
/*19.03.2013	18.01.2000	Белых А.И.	razuz1.c
Определение входимости первичных деталей в изделие
Возвращаем количество наименований деталей в изделии
*/
#include        "buhg.h"

int razuz1(class iceb_t_razuz_data *data)
{
SQL_str         row;
char		strsql[512];
int		kolstr,kolstr1;
int		kodizo;
int		koldd;
int		koddet;
int		poz,i;
double		koliz;
int koluz=data->kod_uz.kolih();
/*
printw("\nrazuz1-Код изделия: %d\n",data->kod_izd);
refresh();
*/

poz=koldd=0;
kodizo=data->kod_izd;
koliz=1.;
class SQLCURSOR cur;
class SQLCURSOR cur1;

for(;;)
 {
  sprintf(strsql,"select kodd,kolih,kz,ei from Specif where kodi=%d",kodizo);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(0);
   }

  if(kolstr == 0 )
   {
    printw("%s-%s !!!\n%s\n",__FUNCTION__,gettext("Нет ни одной записи в изделии"),strsql);
    OSTANOV();
    return(0);
   }

  while(cur.read_cursor(&row) != 0)
   {

  //  printw("%s %s %s\n",row[0],row[1],row[2]);
  //  refresh();

    /*Проверяем не является ли входящий материал изделием*/
    sprintf(strsql,"select kodi from Specif where kodi=%s limit 1",row[0]);
    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      printw("%s\n",sql_error(&bd));
      OSTANOV();
     }

    koddet=0;
    if(kolstr1 == 0)
      koddet=atoi(row[0]);
    else
      continue;

    /*Проверяем есть ли деталь в массиве*/
    if((i=data->kod_det.find(koddet)) == -1)
     {
      koldd++;
      data->kod_det.plus(koddet,-1);
     }    

    sprintf(strsql,"%d|%s",koddet,row[3]);
    if((i=data->kod_det_ei.find(strsql)) < 0)
     {
      data->kod_det_ei.plus(strsql); 
      data->metka_mu.plus(atoi(row[2]),-1);
     }
    data->kolih_det_ei.plus(atof(row[1])*koliz,i);

   }

  /*
  printw("poz=%d koluz=%d\n",poz,koluz);
  refresh();
  */
  /*Обрабатываем узлы*/
  if(poz < koluz)
   {
    kodizo=data->kod_uz.ravno(poz);
    koliz=data->kolih_uz.ravno(poz);
  /*
    printw("Узел:%dколичество %d\n",kodizo,koliz);
    refresh();
  */
    poz++;
   }
  else
   break;
 }
//OSTANOV();
return(koldd);

}
