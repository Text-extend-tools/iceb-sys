/*$Id: sprmatpom.c,v 5.29 2013/09/26 09:43:41 sasa Exp $*/
/*19.02.2013	02.02.2003	Белых А.И.	sprmatpom.c
Получение отчёта о выплаченной материальной помощи
*/
#include        <errno.h>
#include        "buhg.h"

void	sprmatpoms(FILE*);

extern short    *kodmp;   /*Коды материальной помощи*/

int sprmatpom(short mn,short gn,short mk,short gk,
const char *podr,const char *kategr,const char *tabnom,class spis_oth *oth)
{
char		strsql[512];
char		bros[512];
int		kolstr=0,kolstr1=0;
SQL_str		row,row1;
time_t		tmmn=0;
char		imaf[64];
FILE		*ff=NULL;
short		d,m,g;
class iceb_tu_str tabnomz("");
class iceb_tu_str fio("");
double		suma=0.;
double		sumatab=0.;
double		itogo=0.;
class iceb_tu_str naimn("");
double		sumano=0.;
double		sumanoi=0.;
SQLCURSOR curr;
time(&tmmn);

clear();
printw("%s\n",gettext("Распечатка списка работников, которым начислена материальная помощь."));
printw("%d.%d => %d.%d\n",mn,gn,mk,gk);
GDITE();

if(kodmp == NULL)
 {
  iceb_t_soob(gettext("Не введены коды благотворительной помощи !"));
  return(1);
 }
if(kodmp[0] == 0)
 {
  iceb_t_soob(gettext("Не введены коды благотворительной помощи !"));
  return(1);
 }


sprintf(strsql,"select datz,tabn,knah,suma,shet,kom,podr from Zarp where \
datz >= '%d-%d-1' and datz <= '%d-%d-31' and prn='1' and suma <> 0. and (",gn,mn,gk,mk);

for(int i1=1; i1 <= kodmp[0] ; i1++)
 {
  if(i1 == 1)
   sprintf(bros,"knah=%d",kodmp[i1]);
  else
   sprintf(bros," or  knah=%d",kodmp[i1]);
  strcat(strsql,bros);
 }

strcat(strsql,") order by tabn,datz asc");
//printw("%s\n",strsql);

SQLCURSOR cur;



if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
 

sprintf(imaf,"matp%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

d=1; m=mk; g=gk;
dpm(&d,&m,&g,5);

iceb_t_zagolov(gettext("Распечатка списка работников, которым начислена материальная помощь."),1,mn,gn,d,mk,gk,ff);

short	knah=0;
kolstr1=0;
short mesz=0;
int metkapropuska=0;
class iceb_tu_str kateg("");
double  matpom[kodmp[0]]; //Массив сумм мат-помощи по кодам на работника за месяц
double  imatpom[kodmp[0]]; //Массив сумм мат-помощи по кодам на работника
double  inmp[kodmp[0]]; //Массив сумм необлагаемой мат-помощи по кодам всего
memset(&matpom,'\0',sizeof(matpom));
memset(&imatpom,'\0',sizeof(imatpom));
memset(&inmp,'\0',sizeof(inmp));
double bb=0;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
//OSTANOV();  
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(tabnom,row[1],0,0) != 0)
    continue;

  if(proverka(podr,row[6],0,0) != 0)
    continue;

  if(SRAV(tabnomz.ravno(),row[1],0) != 0)
   {

    //Читаем карточку табельного номера
    sprintf(strsql,"select fio,kateg from Kartb where tabn=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     {
      fio.new_plus(row1[0]);
      kateg.new_plus(row1[1]);
     }
    else
     {
      fio.new_plus("");
      kateg.new_plus("");
     }
    metkapropuska=0;
    if(proverka(kategr,kateg.ravno(),0,0) != 0)
     {
      metkapropuska=1;
      continue;
     }

    if(tabnomz.ravno()[0] != '\0')
     {
      bb=0.;
      
      fprintf(ff,"%51s:%10.2f %10.2f\n",gettext("Итого"),sumatab,sumano);
      sumanoi+=sumano;
      if(bb != 0.)
       for(int i1=0; i1 < kodmp[0] ; i1++)
        {
         if(matpom[i1] == 0.)
          continue;
         if(matpom[i1] > bb)
          {
           inmp[i1]+=bb;
           bb=0.;
           break;
          }
         else
          {
           inmp[i1]+=matpom[i1]; 
           bb-=matpom[i1];
          }
        }
     }

    tabnomz.new_plus(row[1]);
    mesz=0;
    sumano=0.;
    sumatab=0.;


    fprintf(ff,"\n%5s %s\n",row[1],fio.ravno());

    sprmatpoms(ff);

    memset(&matpom,'\0',sizeof(matpom));

   }

  if(metkapropuska == 1)
   continue;
   
  //Читаем наименование начисления
  sprintf(strsql,"select naik from Nash where kod=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    naimn.new_plus(row1[0]);
  else
    naimn.new_plus("");    
  rsdat(&d,&m,&g,row[0],2);
  if(mesz != m)
   {
    if(mesz != 0)
     {
      bb=0.;

      sumanoi+=sumano;
      if(bb != 0.)
       for(int i1=0; i1 < kodmp[0] ; i1++)
        {
         if(matpom[i1] == 0.)
          continue;
         if(matpom[i1] > bb)
          {
           inmp[i1]+=bb;
           bb=0.;
           break;
          }
         else
          {
           inmp[i1]+=matpom[i1]; 
           bb-=matpom[i1];
          }
        }
      memset(&matpom,'\0',sizeof(matpom));
     }
    mesz=m;
   }

  suma=atof(row[3]);
  itogo+=suma;
  sumatab+=suma;
  knah=atoi(row[2]);

  for(int i1=1; i1 <= kodmp[0] ; i1++)
   if(knah == kodmp[i1])
    {
     matpom[i1-1]+=suma;
     imatpom[i1-1]+=suma;
    }                 

  fprintf(ff,"%3s %-*.*s %02d.%02d.%d %-5s %10.2f %10s %s\n",
  row[2],
  iceb_tu_kolbait(30,naimn.ravno()),iceb_tu_kolbait(30,naimn.ravno()),naimn.ravno(),
  d,m,g,row[4],suma," ",row[5]);  
 }
bb=0.;
if(bb != 0.)
 for(int i1=0; i1 < kodmp[0] ; i1++)
  {
   if(matpom[i1] == 0.)
    continue;
   if(matpom[i1] > bb)
    {
     inmp[i1]+=bb;
     bb=0.;
     break;
    }
   else
    {
     inmp[i1]+=matpom[i1]; 
     bb-=matpom[i1];
    }
  }

fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_tu_kolbait(51,gettext("Итого")),gettext("Итого"),sumatab,sumano);
sumanoi+=sumano;

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_tu_kolbait(51,gettext("Общий итог")),gettext("Общий итог"),itogo,sumanoi);
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(51,gettext("Разница")),gettext("Разница"),itogo-sumanoi);

if(kodmp[0] > 1)
 {
  fprintf(ff,"\n");
  for(int i1=1; i1 <= kodmp[0] ; i1++)
   {
    sprintf(strsql,"select naik from Nash where kod=%d",kodmp[i1]);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     naimn.new_plus(row[0]);    
    else
     naimn.new_plus("");
    fprintf(ff,"%d %-*.*s %10.2f %10.2f %10.2f\n",
    kodmp[i1],
    iceb_tu_kolbait(30,naimn.ravno()),iceb_tu_kolbait(30,naimn.ravno()),naimn.ravno(),
    imatpom[i1-1],inmp[i1-1],imatpom[i1-1]-inmp[i1-1]);
   }                 
 }

podpis(ff);

fclose(ff);
printw_vr(tmmn);

OSTANOV();
  
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка списка работников, которым начислена материальная помощь"));
iceb_t_ustpeh(imaf,1);
return(0);
}

/********/
/*Шапка*/
/********/

void	sprmatpoms(FILE *ff)
{

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код|        Наименование          |  Дата    | Счёт|  Сумма   |Не обл.сум| Коментарий\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");

}
